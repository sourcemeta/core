#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_try_parse, valid_object) {
  const auto document{sourcemeta::core::try_parse_json("{ \"foo\": 1 }")};
  ASSERT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_object());
  EXPECT_EQ(document.value().size(), 1);
}

TEST(JSON_try_parse, valid_array) {
  const auto document{sourcemeta::core::try_parse_json("[ 1, 2, 3 ]")};
  ASSERT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_array());
  EXPECT_EQ(document.value().size(), 3);
}

TEST(JSON_try_parse, valid_string) {
  const auto document{sourcemeta::core::try_parse_json("\"hello\"")};
  ASSERT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_string());
  EXPECT_EQ(document.value().to_string(), "hello");
}

TEST(JSON_try_parse, valid_integer) {
  const auto document{sourcemeta::core::try_parse_json("42")};
  ASSERT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_integer());
  EXPECT_EQ(document.value().to_integer(), 42);
}

TEST(JSON_try_parse, valid_null) {
  const auto document{sourcemeta::core::try_parse_json("null")};
  ASSERT_TRUE(document.has_value());
  EXPECT_TRUE(document.value().is_null());
}

TEST(JSON_try_parse, matches_throwing_parse) {
  const auto via_try{sourcemeta::core::try_parse_json("[ 1, 2, 3 ]")};
  const auto via_throw{sourcemeta::core::parse_json("[ 1, 2, 3 ]")};
  ASSERT_TRUE(via_try.has_value());
  EXPECT_EQ(via_try.value(), via_throw);
}

TEST(JSON_try_parse, rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("").has_value());
}

TEST(JSON_try_parse, rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("not json").has_value());
}

TEST(JSON_try_parse, rejects_truncated_array) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("[ 1, 2,").has_value());
}

TEST(JSON_try_parse, rejects_unclosed_object) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("{ \"foo\":").has_value());
}

TEST(JSON_try_parse, rejects_invalid_number) {
  EXPECT_FALSE(sourcemeta::core::try_parse_json("01").has_value());
}
