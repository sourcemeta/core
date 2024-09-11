#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <type_traits>

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";

TEST(JSONWeakPointer_pointer, general_traits) {
  EXPECT_TRUE(std::is_default_constructible<
              sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_FALSE(std::is_nothrow_default_constructible<
               sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(
      std::is_destructible<sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(std::is_nothrow_destructible<
              sourcemeta::jsontoolkit::WeakPointer>::value);
}

TEST(JSONWeakPointer_pointer, copy_traits) {
  EXPECT_TRUE(
      std::is_copy_assignable<sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(
      std::is_copy_constructible<sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_FALSE(std::is_nothrow_copy_assignable<
               sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_FALSE(std::is_nothrow_copy_constructible<
               sourcemeta::jsontoolkit::WeakPointer>::value);
}

TEST(JSONWeakPointer_pointer, move_traits) {
  EXPECT_TRUE(
      std::is_move_assignable<sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(
      std::is_move_constructible<sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(std::is_nothrow_move_assignable<
              sourcemeta::jsontoolkit::WeakPointer>::value);
  EXPECT_TRUE(std::is_nothrow_move_constructible<
              sourcemeta::jsontoolkit::WeakPointer>::value);
}

TEST(JSONWeakPointer_pointer, empty) {
  const sourcemeta::jsontoolkit::WeakPointer pointer;
  EXPECT_EQ(pointer.size(), 0);
  EXPECT_TRUE(pointer.empty());
}

TEST(JSONWeakPointer_pointer, store_a_const_ref) {
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};

  EXPECT_EQ(pointer.size(), 1);
  EXPECT_FALSE(pointer.empty());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), foo);
}

TEST(JSONWeakPointer_pointer, one_fragment_back) {
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};

  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.back().is_property());
  EXPECT_EQ(pointer.back().to_property(), foo);
}

TEST(JSONWeakPointer_pointer, multiple_fragments_mixed) {
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), 2,
                                                     std::cref(bar)};
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_FALSE(pointer.empty());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), foo);
  EXPECT_EQ(pointer.at(1).to_index(), 2);
  EXPECT_EQ(pointer.at(2).to_property(), bar);
}

TEST(JSONWeakPointer_pointer, multiple_fragments_back) {
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), 2,
                                                     std::cref(bar)};
  EXPECT_TRUE(pointer.back().is_property());
  EXPECT_EQ(pointer.back().to_property(), bar);
}

TEST(JSONWeakPointer_pointer, build_with_emplace_back) {
  sourcemeta::jsontoolkit::WeakPointer pointer;
  EXPECT_EQ(pointer.size(), 0);
  auto &result_1{pointer.emplace_back(std::cref(foo))};
  EXPECT_TRUE(result_1.is_property());
  EXPECT_EQ(result_1.to_property(), foo);

  auto &result_2{pointer.emplace_back(1)};
  EXPECT_TRUE(result_2.is_index());
  EXPECT_EQ(result_2.to_index(), 1);

  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(0).to_property(), foo);
  EXPECT_EQ(pointer.at(1).to_index(), 1);
}

TEST(JSONWeakPointer_pointer, equality_empty) {
  const sourcemeta::jsontoolkit::WeakPointer pointer_1;
  const sourcemeta::jsontoolkit::WeakPointer pointer_2;
  EXPECT_EQ(pointer_1, pointer_2);
}

TEST(JSONWeakPointer_pointer, equality_true) {
  const sourcemeta::jsontoolkit::WeakPointer pointer_1{std::cref(foo), 1,
                                                       std::cref(bar)};
  const sourcemeta::jsontoolkit::WeakPointer pointer_2{std::cref(foo), 1,
                                                       std::cref(bar)};
  EXPECT_EQ(pointer_1, pointer_2);
}

TEST(JSONWeakPointer_pointer, equality_false) {
  const sourcemeta::jsontoolkit::WeakPointer pointer_1{std::cref(foo), 1,
                                                       std::cref(bar)};
  const sourcemeta::jsontoolkit::WeakPointer pointer_2{std::cref(foo), 3,
                                                       std::cref(bar)};
  EXPECT_FALSE(pointer_1 == pointer_2);
}

TEST(JSONWeakPointer_pointer, pop_back_non_empty) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  pointer.pop_back();
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONWeakPointer_pointer, pop_back_empty) {
  sourcemeta::jsontoolkit::WeakPointer pointer;
  EXPECT_EQ(pointer.size(), 0);
  EXPECT_THROW(pointer.pop_back(), std::runtime_error);
}

TEST(JSONWeakPointer_pointer, ordering_less_than) {
  const sourcemeta::jsontoolkit::WeakPointer pointer_1{std::cref(foo),
                                                       std::cref(bar)};
  const sourcemeta::jsontoolkit::WeakPointer pointer_2{std::cref(foo)};
  const sourcemeta::jsontoolkit::WeakPointer pointer_3{std::cref(baz)};
  EXPECT_TRUE(pointer_2 < pointer_1);
  EXPECT_TRUE(pointer_3 < pointer_1);
  EXPECT_TRUE(pointer_3 < pointer_2);
}

TEST(JSONWeakPointer_pointer, pop_back_zero_empty) {
  sourcemeta::jsontoolkit::WeakPointer pointer{};
  pointer.pop_back(0);
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONWeakPointer_pointer, pop_back_many_subset) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  pointer.pop_back(2);
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONWeakPointer_pointer, pop_back_many_all) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  pointer.pop_back(3);
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONWeakPointer_pointer, push_back_pointer_copy) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const sourcemeta::jsontoolkit::WeakPointer other{std::cref(baz),
                                                   std::cref(qux)};
  pointer.push_back(other);
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "qux");
}

TEST(JSONWeakPointer_pointer, push_back_pointer_move) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  sourcemeta::jsontoolkit::WeakPointer other{std::cref(baz), std::cref(qux)};

  pointer.push_back(std::move(other));
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "qux");

  // TODO: we should assert that other has been moved
}

TEST(JSONWeakPointer_pointer, initial_with_one_token) {
  const sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  const sourcemeta::jsontoolkit::WeakPointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 0);
}

TEST(JSONWeakPointer_pointer, initial_with_two_tokens) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const sourcemeta::jsontoolkit::WeakPointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONWeakPointer_pointer, initial_with_three_tokens) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::jsontoolkit::WeakPointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONWeakPointer_pointer, push_back_property_copy) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  const sourcemeta::jsontoolkit::WeakPointer other{std::cref(bar)};
  pointer.push_back(other.back().to_property());
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONWeakPointer_pointer, push_back_property_move) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  pointer.push_back(std::cref(bar));
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONWeakPointer_pointer, push_back_index_copy) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  const sourcemeta::jsontoolkit::WeakPointer other{0};
  pointer.push_back(other.back().to_index());
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 0);
}

TEST(JSONWeakPointer_pointer, push_back_index_move) {
  sourcemeta::jsontoolkit::WeakPointer pointer{std::cref(foo)};
  pointer.push_back(0);
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 0);
}

TEST(JSONWeakPointer_pointer, push_back_pointer) {
  const sourcemeta::jsontoolkit::Pointer pointer{bar, baz};
  sourcemeta::jsontoolkit::WeakPointer destination{std::cref(foo)};
  destination.push_back(pointer);
  EXPECT_EQ(destination.size(), 3);
  EXPECT_TRUE(destination.at(0).is_property());
  EXPECT_EQ(destination.at(0).to_property(), "foo");
  EXPECT_TRUE(destination.at(1).is_property());
  EXPECT_EQ(destination.at(1).to_property(), "bar");
  EXPECT_TRUE(destination.at(2).is_property());
  EXPECT_EQ(destination.at(2).to_property(), "baz");
}
