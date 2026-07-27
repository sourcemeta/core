#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <functional>
#include <type_traits>

TEST(general_traits) {
  EXPECT_FALSE(
      std::is_default_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(std::is_nothrow_default_constructible<
               sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_nothrow_destructible<sourcemeta::core::Pointer::Token>::value);
}

TEST(copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_copy_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(std::is_nothrow_copy_constructible<
               sourcemeta::core::Pointer::Token>::value);
}

TEST(move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_move_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(std::is_nothrow_move_constructible<
              sourcemeta::core::Pointer::Token>::value);
}

TEST(character_token) {
  const sourcemeta::core::Pointer::Token token{'f'};
  EXPECT_TRUE(token.is_property());
  EXPECT_FALSE(token.is_index());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_property(), "f");
}

TEST(string_token) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_TRUE(token.is_property());
  EXPECT_FALSE(token.is_index());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_property(), "foo");
}

TEST(integer_token) {
  const sourcemeta::core::Pointer::Token token{5};
  EXPECT_TRUE(token.is_index());
  EXPECT_FALSE(token.is_property());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_index(), 5);
}

TEST(hyphen_string_token) {
  const sourcemeta::core::Pointer::Token token{"-"};
  EXPECT_FALSE(token.is_index());
  EXPECT_TRUE(token.is_hyphen());
}

TEST(hyphen_char_token) {
  const sourcemeta::core::Pointer::Token token{'-'};
  EXPECT_FALSE(token.is_index());
  EXPECT_TRUE(token.is_hyphen());
}

TEST(ordering_less_than) {
  const sourcemeta::core::Pointer::Token token_1{"foo"};
  const sourcemeta::core::Pointer::Token token_2{"bar"};
  const sourcemeta::core::Pointer::Token token_3{"baz"};
  EXPECT_TRUE(token_2 < token_1);
  EXPECT_TRUE(token_3 < token_1);
  EXPECT_TRUE(token_2 < token_3);
}

TEST(to_json_index) {
  const sourcemeta::core::Pointer::Token token{5};
  const sourcemeta::core::JSON result{token.to_json()};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 5);
}

TEST(to_json_property) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  const sourcemeta::core::JSON result{token.to_json()};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "foo");
}

TEST(equality_short_property_tokens_equal) {
  const sourcemeta::core::Pointer::Token first{"foo"};
  const sourcemeta::core::Pointer::Token second{"foo"};
  EXPECT_EQ(first, second);
  EXPECT_EQ(second, first);
}

TEST(equality_short_property_tokens_not_equal) {
  const sourcemeta::core::Pointer::Token first{"foo"};
  const sourcemeta::core::Pointer::Token second{"bar"};
  EXPECT_NE(first, second);
  EXPECT_NE(second, first);
}

TEST(equality_long_property_tokens_equal) {
  const sourcemeta::core::JSON::String property(40, 'a');
  const sourcemeta::core::Pointer::Token first{property};
  const sourcemeta::core::Pointer::Token second{property};
  EXPECT_EQ(first, second);
  EXPECT_EQ(second, first);
}

TEST(equality_property_tokens_differing_by_trailing_null_not_equal) {
  sourcemeta::core::JSON::String property_with_null{"a"};
  property_with_null.push_back('\x00');
  const sourcemeta::core::Pointer::Token with_null{property_with_null};
  const sourcemeta::core::Pointer::Token without_null{"a"};
  EXPECT_NE(with_null, without_null);
  EXPECT_NE(without_null, with_null);
}

TEST(equality_property_tokens_imperfect_hash_collision_not_equal) {
  const sourcemeta::core::JSON::String stored{
      sourcemeta::core::JSON::String(31, 'a') + "11111110" + "z"};
  const sourcemeta::core::JSON::String other{
      sourcemeta::core::JSON::String(31, 'a') + "22222220" + "z"};
  const sourcemeta::core::Pointer::Token first{stored};
  const sourcemeta::core::Pointer::Token second{other};
  EXPECT_EQ(first.property_hash(), second.property_hash());
  EXPECT_NE(first, second);
  EXPECT_NE(second, first);
}

TEST(property_hash) {
  const sourcemeta::core::Pointer::Token first{"foo"};
  const sourcemeta::core::Pointer::Token second{"bar"};
  const sourcemeta::core::Pointer::Token third{"foo"};
  EXPECT_NE(first.property_hash(), second.property_hash());
  EXPECT_NE(second.property_hash(), third.property_hash());
  EXPECT_EQ(first.property_hash(), third.property_hash());
}

TEST(property_equals_perfect_match) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_TRUE(token.property_equals(
      "foo", sourcemeta::core::JSON::Object::hash("foo")));
}

TEST(property_equals_perfect_mismatch_same_size) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_FALSE(token.property_equals(
      "bar", sourcemeta::core::JSON::Object::hash("bar")));
}

TEST(property_equals_perfect_mismatch_different_size) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_FALSE(token.property_equals(
      "foobar", sourcemeta::core::JSON::Object::hash("foobar")));
}

TEST(property_equals_character_token_match) {
  const sourcemeta::core::Pointer::Token token{'a'};
  EXPECT_TRUE(
      token.property_equals("a", sourcemeta::core::JSON::Object::hash("a")));
}

TEST(property_equals_empty_property_match) {
  const sourcemeta::core::Pointer::Token token{""};
  EXPECT_TRUE(
      token.property_equals("", sourcemeta::core::JSON::Object::hash("")));
}

TEST(property_equals_empty_property_mismatch) {
  const sourcemeta::core::Pointer::Token token{""};
  EXPECT_FALSE(
      token.property_equals("a", sourcemeta::core::JSON::Object::hash("a")));
}

TEST(property_equals_empty_value_mismatch) {
  const sourcemeta::core::Pointer::Token token{"a"};
  EXPECT_FALSE(
      token.property_equals("", sourcemeta::core::JSON::Object::hash("")));
}

TEST(property_equals_property_with_trailing_null_not_matched_as_prefix) {
  sourcemeta::core::JSON::String property_with_null{"a"};
  property_with_null.push_back('\x00');
  const sourcemeta::core::Pointer::Token token{property_with_null};
  EXPECT_FALSE(
      token.property_equals("a", sourcemeta::core::JSON::Object::hash("a")));
}

TEST(property_equals_value_with_trailing_null_not_matched_as_prefix) {
  sourcemeta::core::JSON::String value_with_null{"a"};
  value_with_null.push_back('\x00');
  const sourcemeta::core::Pointer::Token token{"a"};
  EXPECT_FALSE(token.property_equals(
      value_with_null, sourcemeta::core::JSON::Object::hash(value_with_null)));
}

TEST(property_equals_property_with_trailing_null_match) {
  sourcemeta::core::JSON::String property_with_null{"a"};
  property_with_null.push_back('\x00');
  const sourcemeta::core::Pointer::Token token{property_with_null};
  EXPECT_TRUE(token.property_equals(
      property_with_null,
      sourcemeta::core::JSON::Object::hash(property_with_null)));
}

TEST(property_equals_longest_perfect_property_match) {
  const sourcemeta::core::JSON::String property(31, 'a');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_TRUE(token.property_equals(
      property, sourcemeta::core::JSON::Object::hash(property)));
}

TEST(property_equals_shortest_imperfect_property_match) {
  const sourcemeta::core::JSON::String property(32, 'a');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_TRUE(token.property_equals(
      property, sourcemeta::core::JSON::Object::hash(property)));
}

TEST(property_equals_perfect_property_against_imperfect_prefix_value) {
  const sourcemeta::core::JSON::String property(31, 'a');
  const sourcemeta::core::JSON::String value(32, 'a');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_FALSE(token.property_equals(
      value, sourcemeta::core::JSON::Object::hash(value)));
}

TEST(property_equals_imperfect_property_against_perfect_prefix_value) {
  const sourcemeta::core::JSON::String property(32, 'a');
  const sourcemeta::core::JSON::String value(31, 'a');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_FALSE(token.property_equals(
      value, sourcemeta::core::JSON::Object::hash(value)));
}

TEST(property_equals_long_property_match) {
  const sourcemeta::core::JSON::String property(40, 'a');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_TRUE(token.property_equals(
      property, sourcemeta::core::JSON::Object::hash(property)));
}

TEST(property_equals_long_property_mismatch) {
  const sourcemeta::core::JSON::String property(40, 'a');
  const sourcemeta::core::JSON::String value(40, 'b');
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_FALSE(token.property_equals(
      value, sourcemeta::core::JSON::Object::hash(value)));
}

TEST(property_equals_imperfect_hash_collision_rejected) {
  const sourcemeta::core::JSON::String property{
      sourcemeta::core::JSON::String(31, 'a') + "11111110" + "z"};
  const sourcemeta::core::JSON::String value{
      sourcemeta::core::JSON::String(31, 'a') + "22222220" + "z"};
  const sourcemeta::core::Pointer::Token token{property};
  EXPECT_EQ(sourcemeta::core::JSON::Object::hash(property),
            sourcemeta::core::JSON::Object::hash(value));
  EXPECT_FALSE(token.property_equals(
      value, sourcemeta::core::JSON::Object::hash(value)));
}

TEST(property_equals_weak_token_perfect_match) {
  const std::string property{"foo"};
  const sourcemeta::core::WeakPointer::Token token{std::cref(property)};
  EXPECT_TRUE(token.property_equals(
      "foo", sourcemeta::core::JSON::Object::hash("foo")));
}

TEST(property_equals_weak_token_perfect_mismatch) {
  const std::string property{"foo"};
  const sourcemeta::core::WeakPointer::Token token{std::cref(property)};
  EXPECT_FALSE(token.property_equals(
      "bar", sourcemeta::core::JSON::Object::hash("bar")));
}

TEST(property_equals_weak_token_property_with_trailing_null_not_matched) {
  std::string property_with_null{"a"};
  property_with_null.push_back('\x00');
  const sourcemeta::core::WeakPointer::Token token{
      std::cref(property_with_null)};
  EXPECT_FALSE(
      token.property_equals("a", sourcemeta::core::JSON::Object::hash("a")));
}

TEST(property_equals_weak_token_long_property_match) {
  const std::string property(40, 'a');
  const sourcemeta::core::WeakPointer::Token token{std::cref(property)};
  EXPECT_TRUE(token.property_equals(
      property, sourcemeta::core::JSON::Object::hash(property)));
}

TEST(at_property_with_hash) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "foo": 1,
    "bar": 2,
    "baz": 3
  })JSON");

  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_TRUE(token.is_property());
  const auto &result{document.at(token.to_property(), token.property_hash())};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 1);
}
