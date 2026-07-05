#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

TEST(valid_object) {
  const auto document{sourcemeta::core::try_parse_json("{ \"foo\": 1 }")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_object());
  EXPECT_EQ(document.value().size(), 1);
}

TEST(valid_array) {
  const auto document{sourcemeta::core::try_parse_json("[ 1, 2, 3 ]")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_array());
  EXPECT_EQ(document.value().size(), 3);
}

TEST(valid_string) {
  const auto document{sourcemeta::core::try_parse_json("\"hello\"")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_string());
  EXPECT_EQ(document.value().to_string(), "hello");
}

TEST(valid_integer) {
  const auto document{sourcemeta::core::try_parse_json("42")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_integer());
  EXPECT_EQ(document.value().to_integer(), 42);
}

TEST(valid_null) {
  const auto document{sourcemeta::core::try_parse_json("null")};
  EXPECT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_null());
}

TEST(matches_throwing_parse) {
  const auto via_try{sourcemeta::core::try_parse_json("[ 1, 2, 3 ]")};
  const auto via_throw{sourcemeta::core::parse_json("[ 1, 2, 3 ]")};
  EXPECT_TRUE(via_try.has_value());
  EXPECT_EQ(via_try.value(), via_throw);
}

TEST(rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("").has_value());
}

TEST(rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("not json").has_value());
}

TEST(rejects_truncated_array) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("[ 1, 2,").has_value());
}

TEST(rejects_unclosed_object) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("{ \"foo\":").has_value());
}

TEST(rejects_invalid_number) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("01").has_value());
}

// RFC 8259 Section 2: a JSON text is a single value optionally surrounded by
// whitespace, so trailing content after the value must be rejected.
TEST(rejects_trailing_content) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("123abc").has_value());
}

TEST(rejects_trailing_literal) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("true false").has_value());
}
