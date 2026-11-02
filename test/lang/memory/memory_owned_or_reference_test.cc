#include <sourcemeta/core/memory.h>
#include <sourcemeta/core/test.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view> // std::string_view
#include <type_traits>
#include <utility>
#include <vector>

// The reference case asserts on the address of this object, so it needs
// the storage that a view cannot provide
// NOLINTNEXTLINE(cert-err58-cpp,bugprone-throwing-static-initialization)
static const std::string CACHED{"cached"};

struct Point {
  int x{0};
  int y{0};
};

struct HostileAddress {
  int value{0};
  // The point of this type is that taking its address misbehaves
  // NOLINTNEXTLINE(google-runtime-operator)
  auto operator&() const -> const HostileAddress * { return nullptr; }
};

struct MoveOnly {
  MoveOnly() = default;
  MoveOnly(const MoveOnly &) = delete;
  auto operator=(const MoveOnly &) -> MoveOnly & = delete;
  MoveOnly(MoveOnly &&) = default;
  auto operator=(MoveOnly &&) -> MoveOnly & = default;
  ~MoveOnly() = default;
};

template <typename T>
concept can_take_ownership =
    requires(sourcemeta::core::OwnedOrReference<T> value) {
      std::move(value).to_owned();
    };

TEST(default_constructed_has_no_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result;
  EXPECT_FALSE(result.has_value());
}

TEST(nullopt_has_no_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result{std::nullopt};
  EXPECT_FALSE(result.has_value());
}

TEST(empty_optional_has_no_value) {
  std::optional<std::string> input;
  const sourcemeta::core::OwnedOrReference<std::string> result{
      std::move(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(owns_a_temporary) {
  const sourcemeta::core::OwnedOrReference<std::string> result{
      std::string{"owned"}};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "owned");
}

TEST(owns_an_optional) {
  std::optional<std::string> input{"owned"};
  const sourcemeta::core::OwnedOrReference<std::string> result{
      std::move(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "owned");
}

TEST(owns_a_moved_value) {
  std::string input{"owned"};
  const sourcemeta::core::OwnedOrReference<std::string> result{
      std::move(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "owned");
}

TEST(refers_to_an_external_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "cached");
  EXPECT_EQ(&result.value(), &CACHED);
}

TEST(refers_to_a_value_behind_a_pointer) {
  const std::string *pointer{&CACHED};
  const sourcemeta::core::OwnedOrReference<std::string> result{*pointer};
  EXPECT_EQ(&result.value(), &CACHED);
}

TEST(dereference_reads_an_owned_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result{
      std::string{"owned"}};
  EXPECT_EQ(*result, "owned");
}

TEST(dereference_reads_a_referenced_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  EXPECT_EQ(&(*result), &CACHED);
}

TEST(arrow_reaches_into_an_owned_value) {
  const sourcemeta::core::OwnedOrReference<Point> result{Point{.x = 1, .y = 2}};
  EXPECT_EQ(result->x, 1);
  EXPECT_EQ(result->y, 2);
}

TEST(arrow_reaches_into_a_referenced_value) {
  const sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  EXPECT_EQ(result->size(), 6);
}

TEST(to_owned_moves_out_of_an_owned_value) {
  sourcemeta::core::OwnedOrReference<std::vector<std::string>> result{
      std::vector<std::string>{"owned"}};
  const auto *const data{result.value().data()};
  const auto value{std::move(result).to_owned()};
  EXPECT_EQ(value.size(), 1);
  EXPECT_EQ(value.front(), "owned");
  EXPECT_EQ(value.data(), data);
}

TEST(to_owned_copies_a_referenced_value) {
  sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  const auto value{std::move(result).to_owned()};
  EXPECT_EQ(value, "cached");
  EXPECT_NE(&value, &CACHED);
  EXPECT_EQ(CACHED, "cached");
}

TEST(move_construction_transfers_an_owned_value) {
  sourcemeta::core::OwnedOrReference<std::string> result{std::string{"owned"}};
  const sourcemeta::core::OwnedOrReference<std::string> other{
      std::move(result)};
  EXPECT_TRUE(other.has_value());
  EXPECT_EQ(other.value(), "owned");
}

TEST(move_construction_transfers_a_reference) {
  sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  const sourcemeta::core::OwnedOrReference<std::string> other{
      std::move(result)};
  EXPECT_TRUE(other.has_value());
  EXPECT_EQ(&other.value(), &CACHED);
}

TEST(move_assignment_transfers_an_owned_value) {
  sourcemeta::core::OwnedOrReference<std::string> result{std::string{"owned"}};
  sourcemeta::core::OwnedOrReference<std::string> other;
  other = std::move(result);
  EXPECT_TRUE(other.has_value());
  EXPECT_EQ(other.value(), "owned");
}

TEST(move_assignment_transfers_a_reference) {
  sourcemeta::core::OwnedOrReference<std::string> result{CACHED};
  sourcemeta::core::OwnedOrReference<std::string> other;
  other = std::move(result);
  EXPECT_TRUE(other.has_value());
  EXPECT_EQ(&other.value(), &CACHED);
}

TEST(cannot_be_copied) {
  EXPECT_FALSE(std::is_copy_constructible_v<
               sourcemeta::core::OwnedOrReference<std::string>>);
  EXPECT_FALSE(std::is_copy_assignable_v<
               sourcemeta::core::OwnedOrReference<std::string>>);
}

TEST(can_be_moved) {
  EXPECT_TRUE(std::is_move_constructible_v<
              sourcemeta::core::OwnedOrReference<std::string>>);
  EXPECT_TRUE(std::is_move_assignable_v<
              sourcemeta::core::OwnedOrReference<std::string>>);
}

TEST(cannot_refer_to_a_constant_temporary) {
  EXPECT_FALSE(
      (std::is_constructible_v<sourcemeta::core::OwnedOrReference<std::string>,
                               const std::string &&>));
  EXPECT_TRUE(
      (std::is_constructible_v<sourcemeta::core::OwnedOrReference<std::string>,
                               std::string &&>));
  EXPECT_TRUE(
      (std::is_constructible_v<sourcemeta::core::OwnedOrReference<std::string>,
                               const std::string &>));
}

TEST(holds_a_move_only_value) {
  const sourcemeta::core::OwnedOrReference<MoveOnly> result{MoveOnly{}};
  EXPECT_TRUE(result.has_value());
}

TEST(cannot_take_ownership_of_a_move_only_value) {
  EXPECT_FALSE(can_take_ownership<MoveOnly>);
  EXPECT_TRUE(can_take_ownership<std::string>);
}

TEST(refers_to_a_value_that_overloads_address_of) {
  static const HostileAddress HOSTILE{.value = 7};
  const sourcemeta::core::OwnedOrReference<HostileAddress> result{HOSTILE};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().value, 7);
  EXPECT_EQ(result->value, 7);
  EXPECT_EQ(std::addressof(result.value()), std::addressof(HOSTILE));
}

TEST(ownable_accepts_object_types) {
  EXPECT_TRUE(sourcemeta::core::Ownable<std::string>);
  EXPECT_TRUE(sourcemeta::core::Ownable<Point>);
  EXPECT_TRUE(sourcemeta::core::Ownable<MoveOnly>);
}

TEST(ownable_rejects_non_object_types) {
  EXPECT_FALSE(sourcemeta::core::Ownable<void>);
  EXPECT_FALSE(sourcemeta::core::Ownable<std::string &>);
  EXPECT_FALSE(sourcemeta::core::Ownable<int()>);
}
