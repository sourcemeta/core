#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <type_traits>
#include <unordered_set>
#include <utility>

TEST(JSON_value, general_traits) {
  EXPECT_FALSE(std::is_default_constructible<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_nothrow_destructible<sourcemeta::core::JSON>::value);
}

// BIG WARNING! Increase this number will make projects like Blaze slower,
// as it will affect cache lines when dealing with JSON documents
TEST(JSON_value, size) { EXPECT_EQ(sizeof(sourcemeta::core::JSON), 40); }

TEST(JSON_value, copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_copy_constructible<sourcemeta::core::JSON>::value);
  EXPECT_FALSE(std::is_nothrow_copy_assignable<sourcemeta::core::JSON>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_constructible<sourcemeta::core::JSON>::value);
}

TEST(JSON_value, move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::JSON>::value);
  EXPECT_TRUE(std::is_move_constructible<sourcemeta::core::JSON>::value);
}

TEST(JSON_value, copy_equivalence_assignment) {
  sourcemeta::core::JSON document{sourcemeta::core::JSON::Array{}};
  document.push_back(sourcemeta::core::JSON{1});
  sourcemeta::core::JSON object = sourcemeta::core::JSON::make_object();
  object.assign("foo", sourcemeta::core::JSON{1.2});
  document.push_back(object);
  document.push_back(sourcemeta::core::JSON{3});
  sourcemeta::core::JSON copy = document;
  EXPECT_EQ(document, copy);
}

TEST(JSON_value, from_size_t) {
  const std::size_t value{5};
  const sourcemeta::core::JSON document{value};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 5);
}

TEST(JSON_value, compare_int_less_than_int) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_int_greater_than_int) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{3};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(JSON_value, compare_int_equal_int) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{4};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_int_less_than_real) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4.3};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_int_greater_than_real) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{3.8};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(JSON_value, compare_int_equal_real) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right{4.0};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_real_less_than_int) {
  const sourcemeta::core::JSON left{3.8};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_real_greater_than_int) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{3};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(JSON_value, compare_real_equal_int) {
  const sourcemeta::core::JSON left{4.0};
  const sourcemeta::core::JSON right{4};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_real_less_than_real) {
  const sourcemeta::core::JSON left{3.8};
  const sourcemeta::core::JSON right{4.1};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_real_greater_than_real) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{3.9};
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(JSON_value, compare_real_equal_real) {
  const sourcemeta::core::JSON left{4.2};
  const sourcemeta::core::JSON right{4.2};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_false_true) {
  const sourcemeta::core::JSON left{false};
  const sourcemeta::core::JSON right{true};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_false_false) {
  const sourcemeta::core::JSON left{false};
  const sourcemeta::core::JSON right{false};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_true_true) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{true};
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_bool_int) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{0};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_bool_real) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{4.5};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_int_array) {
  const sourcemeta::core::JSON left{4};
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_array_array_same) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,2,3]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_array_array_different) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,2]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_array_array_different_same_size) {
  const sourcemeta::core::JSON left = sourcemeta::core::parse_json("[1,3,4]");
  const sourcemeta::core::JSON right = sourcemeta::core::parse_json("[1,2,3]");
  EXPECT_FALSE(left < right);
  EXPECT_TRUE(right < left);
}

TEST(JSON_value, compare_object_object_same) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":1}");
  EXPECT_FALSE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_object_object_different_same_size) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":2}");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_object_object_different) {
  const sourcemeta::core::JSON left =
      sourcemeta::core::parse_json("{\"foo\":1}");
  const sourcemeta::core::JSON right =
      sourcemeta::core::parse_json("{\"foo\":1, \"bar\":2}");
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(JSON_value, compare_int_operator_less_than_or_equal_int) {
  const sourcemeta::core::JSON left{3};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left <= right);
  EXPECT_FALSE(right <= left);
}

TEST(JSON_value, compare_int_operator_greater_than_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left > right);
  EXPECT_FALSE(right > left);
}

TEST(JSON_value, compare_int_operator_greater_than_int_equal) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{5};
  EXPECT_FALSE(left > right);
}

TEST(JSON_value, compare_int_operator_greater_than_or_equal_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left >= right);
  EXPECT_FALSE(right >= left);
}

TEST(JSON_value, compare_int_operator_not_equal_int) {
  const sourcemeta::core::JSON left{5};
  const sourcemeta::core::JSON right{4};
  EXPECT_TRUE(left != right);
}

TEST(JSON_value, set_null) {
  sourcemeta::core::JSON document{true};
  EXPECT_FALSE(document.is_null());
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{nullptr});
  EXPECT_TRUE(document.is_null());
  EXPECT_FALSE(document.is_boolean());
}

TEST(JSON_value, set_const_null) {
  sourcemeta::core::JSON document{true};
  EXPECT_FALSE(document.is_null());
  EXPECT_TRUE(document.is_boolean());
  const sourcemeta::core::JSON new_value{nullptr};
  document.into(new_value);
  EXPECT_TRUE(document.is_null());
  EXPECT_FALSE(document.is_boolean());
}

TEST(JSON_value, set_negative_integer) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4});
  EXPECT_TRUE(document.is_integer());
  EXPECT_FALSE(document.is_real());
  EXPECT_EQ(document.to_integer(), -4);
}

TEST(JSON_value, set_negative_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4.3});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
  EXPECT_EQ(document.to_real(), -4.3);
}

TEST(JSON_value, set_negative_integral_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{-4.0});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(JSON_value, set_positive_integer) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4});
  EXPECT_TRUE(document.is_integer());
  EXPECT_FALSE(document.is_real());
}

TEST(JSON_value, set_positive_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4.3});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(JSON_value, set_positive_integral_real) {
  sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  document.into(sourcemeta::core::JSON{4.0});
  EXPECT_FALSE(document.is_integer());
  EXPECT_TRUE(document.is_real());
}

TEST(JSON_value, into_string_from_boolean) {
  sourcemeta::core::JSON document{false};
  EXPECT_FALSE(document.is_string());
  document.into(sourcemeta::core::JSON{"foo"});
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
}

TEST(JSON_value, into_string_from_string) {
  sourcemeta::core::JSON document{"foo"};
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "foo");
  document.into(sourcemeta::core::JSON{"bar"});
  EXPECT_TRUE(document.is_string());
  EXPECT_EQ(document.to_string(), "bar");
}

TEST(JSON_value, to_ostream) {
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

TEST(JSON_value, class_member_initializer_list) {
  const sourcemeta::core::JSON document{5};
  EXPECT_TRUE(document.is_integer());
  const ClassMemberInitializerList container{document};
  EXPECT_TRUE(container.get().is_integer());
  EXPECT_EQ(container.get().to_integer(), 5);
  EXPECT_EQ(container.get(), document);
}

TEST(JSON_value, try_at) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("foo");
  EXPECT_TRUE(result);
  EXPECT_EQ(result->to_integer(), 5);
}

TEST(JSON_value, try_at_fail) {
  const sourcemeta::core::JSON document =
      sourcemeta::core::parse_json("{\"foo\":5}");
  EXPECT_TRUE(document.is_object());
  const auto result = document.try_at("boo");
  EXPECT_FALSE(result);
}

TEST(JSON_value, unordered_set_with_custom_hash) {
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
