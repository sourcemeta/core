#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <type_traits>

TEST(JSONPointer_token, general_traits) {
  EXPECT_FALSE(
      std::is_default_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(std::is_nothrow_default_constructible<
               sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_nothrow_destructible<sourcemeta::core::Pointer::Token>::value);
}

TEST(JSONPointer_token, copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_copy_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_FALSE(std::is_nothrow_copy_constructible<
               sourcemeta::core::Pointer::Token>::value);
}

TEST(JSONPointer_token, move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_move_constructible<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::Pointer::Token>::value);
  EXPECT_TRUE(std::is_nothrow_move_constructible<
              sourcemeta::core::Pointer::Token>::value);
}

TEST(JSONPointer_token, character_token) {
  const sourcemeta::core::Pointer::Token token{'f'};
  EXPECT_TRUE(token.is_property());
  EXPECT_FALSE(token.is_index());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_property(), "f");
}

TEST(JSONPointer_token, string_token) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  EXPECT_TRUE(token.is_property());
  EXPECT_FALSE(token.is_index());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_property(), "foo");
}

TEST(JSONPointer_token, integer_token) {
  const sourcemeta::core::Pointer::Token token{5};
  EXPECT_TRUE(token.is_index());
  EXPECT_FALSE(token.is_property());
  EXPECT_FALSE(token.is_hyphen());
  EXPECT_EQ(token.to_index(), 5);
}

TEST(JSONPointer_token, hyphen_string_token) {
  const sourcemeta::core::Pointer::Token token{"-"};
  EXPECT_FALSE(token.is_index());
  EXPECT_TRUE(token.is_hyphen());
}

TEST(JSONPointer_token, hyphen_char_token) {
  const sourcemeta::core::Pointer::Token token{'-'};
  EXPECT_FALSE(token.is_index());
  EXPECT_TRUE(token.is_hyphen());
}

TEST(JSONPointer_token, ordering_less_than) {
  const sourcemeta::core::Pointer::Token token_1{"foo"};
  const sourcemeta::core::Pointer::Token token_2{"bar"};
  const sourcemeta::core::Pointer::Token token_3{"baz"};
  EXPECT_TRUE(token_2 < token_1);
  EXPECT_TRUE(token_3 < token_1);
  EXPECT_TRUE(token_2 < token_3);
}

TEST(JSONPointer_token, to_json_index) {
  const sourcemeta::core::Pointer::Token token{5};
  const sourcemeta::core::JSON result{token.to_json()};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 5);
}

TEST(JSONPointer_token, to_json_property) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  const sourcemeta::core::JSON result{token.to_json()};
  EXPECT_TRUE(result.is_string());
  EXPECT_EQ(result.to_string(), "foo");
}

TEST(JSONPointer_token, property_hash) {
  const sourcemeta::core::Pointer::Token first{"foo"};
  const sourcemeta::core::Pointer::Token second{"bar"};
  const sourcemeta::core::Pointer::Token third{"foo"};
  EXPECT_NE(first.property_hash(), second.property_hash());
  EXPECT_NE(second.property_hash(), third.property_hash());
  EXPECT_EQ(first.property_hash(), third.property_hash());
}

TEST(JSONPointer_get, at_property_with_hash) {
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
