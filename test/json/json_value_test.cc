#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstddef>       // std::size_t
#include <cstdint>       // std::int64_t
#include <functional>    // std::reference_wrapper
#include <string>        // std::string
#include <type_traits>   // std::is_default_constructible, etc
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

TEST(general_traits) {
  EXPECT_FALSE(std::is_default_constructible<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_nothrow_destructible<sourcemeta::core::JSON>::value);
}

// BIG WARNING! Increase this number will make projects like Blaze slower,
// as it will affect cache lines when dealing with JSON documents
TEST(size) {
  // The union's largest member is std::string, whose size varies across
  // standard library implementations (24 on libc++, 32 on libstdc++).
  // The Type enum (1 byte) is padded to 8 bytes for alignment.
  EXPECT_EQ(sizeof(sourcemeta::core::JSON), sizeof(std::string) + 8);
}

TEST(copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_copy_constructible<sourcemeta::core::JSON>::value);
  EXPECT_FALSE(std::is_nothrow_copy_assignable<sourcemeta::core::JSON>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_constructible<sourcemeta::core::JSON>::value);
}

TEST(move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_move_constructible<sourcemeta::core::JSON>::value);
}

TEST(copy_equivalence_assignment) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(sourcemeta::core::JSON{1});
  sourcemeta::core::JSON object = sourcemeta::core::JSON::make_object();
  object.assign("foo", sourcemeta::core::JSON{1.2});
  document.push_back(object);
  document.push_back(sourcemeta::core::JSON{3});
  sourcemeta::core::JSON copy = document;
  EXPECT_EQ(document, copy);
}

TEST(from_size_t) {
  const std::size_t value{5};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 5);
}

TEST(compare_int_less_than_int) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_int_greater_than_int) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{3};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(compare_int_equal_int) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{4};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_int_less_than_real) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4.3};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_int_greater_than_real) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{3.8};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(compare_int_equal_real) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{4.0};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_real_less_than_int) {
  const sourcemeta::core::JSON left{3.8};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_real_greater_than_int) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{3};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(compare_real_equal_int) {
  const sourcemeta::core::JSON left{4.0};
  const sourcemeta::core::JSON right{4};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_real_less_than_real) {
  const sourcemeta::core::JSON left{3.8};
  const sourcemeta::core::JSON right{4.1};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_real_greater_than_real) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{3.9};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(compare_real_equal_real) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{4.2};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_false_true) {
  const sourcemeta::core::JSON left{false};
  const sourcemeta::core::JSON right{true};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_false_false) {
  const sourcemeta::core::JSON left{false};
  const sourcemeta::core::JSON right{false};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_true_true) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{true};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_bool_int) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{0};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_bool_real) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{4.5};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_int_array) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_array_array_same) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,2,3]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_array_array_different) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,2]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_array_array_different_same_size) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,3,4]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(compare_object_object_same) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":1}");
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_object_object_different_same_size) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":2}");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_object_object_different) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":1, \"bar\":2}");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(compare_int_operator_less_than_or_equal_int) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left <= right);
  EXPECT_FALSE(right <= left);
}

TEST(compare_int_operator_greater_than_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left > right);
  EXPECT_FALSE(right > left);
}

TEST(compare_int_operator_greater_than_int_equal) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{5};
  EXPECT_FALSE(left > right);
}

TEST(compare_int_operator_greater_than_or_equal_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left >= right);
  EXPECT_FALSE(right >= left);
}

TEST(compare_int_operator_not_equal_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left != right);
}

TEST(set_null) {
  sourcemeta::core::JSON document{true};
  EXPECT_FALSE(document.is_null());
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{nullptr});
  EXPECT_TRUE(document.is_null());
  EXPECT_FALSE(document.is_boolean());
}

TEST(set_const_null) {
  sourcemeta::core::JSON document{true};
  EXPECT_FALSE(document.is_null());
  EXPECT_TRUE(document.is_boolean());
  const sourcemeta::core::JSON new_value{nullptr};
  document.into(new_value);
  EXPECT_TRUE(document.is_null());
  EXPECT_FALSE(document.is_boolean());
}

TEST(set_negative_integer) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4});
  EXPECT_TRUE(document.is_integer());
  EXPECT_FALSE(document.is_real());
  EXPECT_EQ(document.to_integer(), -4);
}

TEST(set_negative_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4.3});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -4.3);
}

TEST(set_negative_integral_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4.0});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(set_positive_integer) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4});
  EXPECT_TRUE(document.is_integer());
  EXPECT_FALSE(document.is_real());
}

TEST(set_positive_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4.3});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(set_positive_integral_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4.0});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(into_string_from_boolean) {
  sourcemeta::core::JSON document{false};
  EXPECT_FALSE(document.is_string());
  document.into(sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(into_string_from_string) {
  sourcemeta::core::JSON document{"foo"};
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
  document.into(sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "bar");
}

TEST(to_ostream) {
  const sourcemeta::core::JSON document{
      sourcemeta::core::JSON{1}, sourcemeta::core::JSON{2},
      sourcemeta::core::JSON{3}, sourcemeta::core::JSON{4}};
  std::ostringstream stream;
  stream << document;
#ifdef NDEBUG
  EXPECT_EQ(stream.str(), "[1,2,3,4]");
#else
  EXPECT_EQ(stream.str(), "[ 1, 2, 3, 4 ]");
#endif
}

class ClassMemberInitializerList {
public:
  ClassMemberInitializerList(sourcemeta::core::JSON document)
      : data{std::move(document)} {}
  auto get() const -> const sourcemeta::core::JSON & { return this->data; }

private:
  const sourcemeta::core::JSON data;
};

TEST(class_member_initializer_list) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_integer());
  const ClassMemberInitializerList container{document};
  EXPECT_TRUE(container.get().is_integer());
  EXPECT_EQ(container.get().to_integer(), 5);
  EXPECT_EQ(container.get(), document);
}

TEST(try_at) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("foo");
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
}

TEST(try_at_fail) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("boo");
  EXPECT_FALSE(result);
}

TEST(try_at_mutable) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("foo");
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
  result->into(sourcemeta::core::JSON{7});
  EXPECT_EQ(document.at("foo").to_integer(), 7);
}

TEST(try_at_mutable_fail) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("boo");
  EXPECT_FALSE(result);
}

TEST(try_at_mutable_string_view) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const sourcemeta::core::JSON::StringView key{"foo"};
  const auto result = document.try_at(key);
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
  result->into(sourcemeta::core::JSON{7});
  EXPECT_EQ(document.at("foo").to_integer(), 7);
}

TEST(try_at_mutable_with_hash) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("foo", document.as_object().hash("foo"));
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
  result->into(sourcemeta::core::JSON{7});
  EXPECT_EQ(document.at("foo").to_integer(), 7);
}

TEST(try_at_mutable_with_hash_fail) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("boo", document.as_object().hash("boo"));
  EXPECT_FALSE(result);
}

TEST(try_at_mutable_string_view_with_hash) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const sourcemeta::core::JSON::StringView key{"foo"};
  const auto result = document.try_at(key, document.as_object().hash(key));
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
  result->into(sourcemeta::core::JSON{7});
  EXPECT_EQ(document.at("foo").to_integer(), 7);
}

TEST(unordered_set_with_custom_hash) {
  std::unordered_set<sourcemeta::core::JSON,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.insert(sourcemeta::core::JSON{"foo"});
  value.insert(sourcemeta::core::JSON{"bar"});
  value.insert(sourcemeta::core::JSON{"baz"});
  value.insert(sourcemeta::core::JSON{"bar"});

  EXPECT_EQ(value.size(), 3);
  EXPECT_TRUE(value.contains(sourcemeta::core::JSON{"foo"}));
  EXPECT_TRUE(value.contains(sourcemeta::core::JSON{"bar"}));
  EXPECT_TRUE(value.contains(sourcemeta::core::JSON{"baz"}));
}

// The same numeric value in different representations is a single element, so
// the hash and the equality must agree for the container to deduplicate it
TEST(unordered_set_deduplicates_equal_numbers_across_representations) {
  std::unordered_set<sourcemeta::core::JSON,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.insert(sourcemeta::core::JSON{1000});
  value.insert(sourcemeta::core::JSON{1000.0});
  EXPECT_EQ(value.size(), 1);
}

TEST(unordered_set_deduplicates_integer_real_and_decimal) {
  std::unordered_set<sourcemeta::core::JSON,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.insert(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)});
  value.insert(sourcemeta::core::JSON{1000.0});
  value.insert(sourcemeta::core::JSON{sourcemeta::core::Decimal{"1000"}});
  EXPECT_EQ(value.size(), 1);
}

// A lookup through any representation finds an element stored under another
TEST(unordered_set_finds_equal_number_across_representations) {
  std::unordered_set<sourcemeta::core::JSON,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.insert(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)});
  EXPECT_TRUE(value.contains(sourcemeta::core::JSON{1000.0}));
  EXPECT_TRUE(value.contains(
      sourcemeta::core::JSON{sourcemeta::core::Decimal{"1000"}}));
}

TEST(unordered_set_keeps_distinct_numbers_separate) {
  std::unordered_set<sourcemeta::core::JSON,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.insert(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)});
  value.insert(sourcemeta::core::JSON{1001.0});
  value.insert(sourcemeta::core::JSON{sourcemeta::core::Decimal{"1002"}});
  EXPECT_EQ(value.size(), 3);
}

// A map keyed by JSON must treat equal numbers across representations as one
// key
TEST(unordered_map_deduplicates_equal_number_keys) {
  std::unordered_map<sourcemeta::core::JSON, int,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.emplace(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)}, 1);
  value.emplace(sourcemeta::core::JSON{1000.0}, 2);
  value.emplace(sourcemeta::core::JSON{sourcemeta::core::Decimal{"1000"}}, 3);
  EXPECT_EQ(value.size(), 1);
}

TEST(unordered_map_finds_equal_number_key_across_representations) {
  std::unordered_map<sourcemeta::core::JSON, int,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.emplace(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)}, 42);
  EXPECT_TRUE(value.contains(sourcemeta::core::JSON{1000.0}));
  EXPECT_TRUE(value.contains(
      sourcemeta::core::JSON{sourcemeta::core::Decimal{"1000"}}));
  EXPECT_EQ(value.at(sourcemeta::core::JSON{1000.0}), 42);
  EXPECT_EQ(value.at(sourcemeta::core::JSON{sourcemeta::core::Decimal{"1000"}}),
            42);
}

TEST(unordered_map_keeps_distinct_number_keys_separate) {
  std::unordered_map<sourcemeta::core::JSON, int,
                     sourcemeta::core::HashJSON<sourcemeta::core::JSON>>
      value;
  value.emplace(sourcemeta::core::JSON{static_cast<std::int64_t>(1000)}, 1);
  value.emplace(sourcemeta::core::JSON{1001.0}, 2);
  EXPECT_EQ(value.size(), 2);
}

// Equal values must hash equally, otherwise hash-based containers and the hash
// prefilter in equality checks would treat them as different
TEST(equal_integral_number_representations_hash_equally) {
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(1000)};
  const sourcemeta::core::JSON as_real{1000.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"1000"}};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_integer == as_decimal);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_integer.fast_hash(), as_real.fast_hash());
  EXPECT_EQ(as_integer.fast_hash(), as_decimal.fast_hash());
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

TEST(equal_zero_representations_hash_equally) {
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(0)};
  const sourcemeta::core::JSON as_real{0.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"0"}};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_integer == as_decimal);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_integer.fast_hash(), as_real.fast_hash());
  EXPECT_EQ(as_integer.fast_hash(), as_decimal.fast_hash());
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

TEST(equal_negative_integral_representations_hash_equally) {
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(-1000)};
  const sourcemeta::core::JSON as_real{-1000.0};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"-1000"}};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_integer == as_decimal);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_integer.fast_hash(), as_real.fast_hash());
  EXPECT_EQ(as_integer.fast_hash(), as_decimal.fast_hash());
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

// A power of two that is exactly representable as a real, so the integer and
// real forms are genuinely equal and must hash equally
TEST(equal_large_integral_representations_hash_equally) {
  const sourcemeta::core::JSON as_integer{
      static_cast<std::int64_t>(4503599627370496)};
  const sourcemeta::core::JSON as_real{4503599627370496.0};
  const sourcemeta::core::JSON as_decimal{
      sourcemeta::core::Decimal{"4503599627370496"}};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_integer == as_decimal);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_integer.fast_hash(), as_real.fast_hash());
  EXPECT_EQ(as_integer.fast_hash(), as_decimal.fast_hash());
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

TEST(equal_fractional_representations_hash_equally) {
  const sourcemeta::core::JSON as_real{0.25};
  const sourcemeta::core::JSON as_decimal{sourcemeta::core::Decimal{"0.25"}};
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_EQ(as_real.fast_hash(), as_decimal.fast_hash());
}

// Values that are not equal are free to hash apart and must never compare equal
TEST(distinct_numbers_are_not_equal_across_representations) {
  const sourcemeta::core::JSON integer_thousand{
      static_cast<std::int64_t>(1000)};
  const sourcemeta::core::JSON real_thousand_and_one{1001.0};
  const sourcemeta::core::JSON decimal_thousand_and_two{
      sourcemeta::core::Decimal{"1002"}};
  EXPECT_FALSE(integer_thousand == real_thousand_and_one);
  EXPECT_FALSE(integer_thousand == decimal_thousand_and_two);
  EXPECT_FALSE(real_thousand_and_one == decimal_thousand_and_two);
}

TEST(unordered_set_with_reference_wrapper) {
  const sourcemeta::core::JSON foo{"foo"};
  const sourcemeta::core::JSON bar{"bar"};
  const sourcemeta::core::JSON baz{"baz"};
  const sourcemeta::core::JSON bar_duplicate{"bar"};

  std::unordered_set<std::reference_wrapper<const sourcemeta::core::JSON>,
                     sourcemeta::core::HashJSON<
                         std::reference_wrapper<const sourcemeta::core::JSON>>,
                     sourcemeta::core::EqualJSON<
                         std::reference_wrapper<const sourcemeta::core::JSON>>>
      value;
  value.insert(std::cref(foo));
  value.insert(std::cref(bar));
  value.insert(std::cref(baz));
  value.insert(std::cref(bar_duplicate));

  EXPECT_EQ(value.size(), 3);
  EXPECT_TRUE(value.contains(std::cref(foo)));
  EXPECT_TRUE(value.contains(std::cref(bar)));
  EXPECT_TRUE(value.contains(std::cref(baz)));
  EXPECT_TRUE(value.contains(std::cref(bar_duplicate)));
}

TEST(unordered_map_with_reference_wrapper) {
  const sourcemeta::core::JSON foo{"foo"};
  const sourcemeta::core::JSON bar{"bar"};
  const sourcemeta::core::JSON bar_duplicate{"bar"};

  std::unordered_map<std::reference_wrapper<const sourcemeta::core::JSON>,
                     std::size_t,
                     sourcemeta::core::HashJSON<
                         std::reference_wrapper<const sourcemeta::core::JSON>>,
                     sourcemeta::core::EqualJSON<
                         std::reference_wrapper<const sourcemeta::core::JSON>>>
      value;
  value.emplace(std::cref(foo), 1);
  value.emplace(std::cref(bar), 2);
  value.emplace(std::cref(bar_duplicate), 3);

  EXPECT_EQ(value.size(), 2);
  EXPECT_EQ(value.at(std::cref(foo)), 1);
  EXPECT_EQ(value.at(std::cref(bar)), 2);
  EXPECT_EQ(value.at(std::cref(bar_duplicate)), 2);
}

TEST(destructs_deeply_nested_array_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 2 + 1);
  deep.append(depth, '[');
  deep.push_back('0');
  deep.append(depth, ']');
  auto document = sourcemeta::core::parse_json(deep);
  EXPECT_TRUE(document.is_array());
}

TEST(destructs_deeply_nested_object_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 6 + 1 + depth);
  for (std::size_t index = 0; index < depth; ++index) {
    deep.append("{\"x\":");
  }
  deep.push_back('0');
  deep.append(depth, '}');
  auto document = sourcemeta::core::parse_json(deep);
  EXPECT_TRUE(document.is_object());
}

TEST(copies_deeply_nested_array_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 2 + 1);
  deep.append(depth, '[');
  deep.push_back('0');
  deep.append(depth, ']');
  auto document = sourcemeta::core::parse_json(deep);
  auto copy = document;
  EXPECT_TRUE(copy.is_array());
}

TEST(copies_deeply_nested_object_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 6 + 1 + depth);
  for (std::size_t index = 0; index < depth; ++index) {
    deep.append("{\"x\":");
  }
  deep.push_back('0');
  deep.append(depth, '}');
  auto document = sourcemeta::core::parse_json(deep);
  auto copy = document;
  EXPECT_TRUE(copy.is_object());
}

TEST(copy_assigns_deeply_nested_array_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 2 + 1);
  deep.append(depth, '[');
  deep.push_back('0');
  deep.append(depth, ']');
  auto source = sourcemeta::core::parse_json(deep);
  auto target = sourcemeta::core::parse_json(deep);
  target = source;
  EXPECT_TRUE(target.is_array());
}

TEST(copy_assigns_deeply_nested_object_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 6 + 1 + depth);
  for (std::size_t index = 0; index < depth; ++index) {
    deep.append("{\"x\":");
  }
  deep.push_back('0');
  deep.append(depth, '}');
  auto source = sourcemeta::core::parse_json(deep);
  auto target = sourcemeta::core::parse_json(deep);
  target = source;
  EXPECT_TRUE(target.is_object());
}

TEST(move_assigns_deeply_nested_array_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 2 + 1);
  deep.append(depth, '[');
  deep.push_back('0');
  deep.append(depth, ']');
  auto source = sourcemeta::core::parse_json(deep);
  auto target = sourcemeta::core::parse_json(deep);
  target = std::move(source);
  EXPECT_TRUE(target.is_array());
}

TEST(move_assigns_deeply_nested_object_without_stack_overflow) {
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 6 + 1 + depth);
  for (std::size_t index = 0; index < depth; ++index) {
    deep.append("{\"x\":");
  }
  deep.push_back('0');
  deep.append(depth, '}');
  auto source = sourcemeta::core::parse_json(deep);
  auto target = sourcemeta::core::parse_json(deep);
  target = std::move(source);
  EXPECT_TRUE(target.is_object());
}

TEST(direct_list_inits_deeply_nested_array_without_stack_overflow) {
  // On GCC and MSVC, JSON x{other} selects JSON(initializer_list<JSON>) whose
  // single-element workaround invokes operator=(const JSON&)
  constexpr std::size_t depth{100000};
  std::string deep;
  deep.reserve(depth * 2 + 1);
  deep.append(depth, '[');
  deep.push_back('0');
  deep.append(depth, ']');
  auto source = sourcemeta::core::parse_json(deep);
  sourcemeta::core::JSON copy{source};
  EXPECT_TRUE(copy.is_array());
}
