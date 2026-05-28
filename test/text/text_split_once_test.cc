#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_split_once, char_delimiter_in_middle) {
  const auto parts{sourcemeta::core::split_once("key=value", '=')};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "value");
}

TEST(Text_split_once, char_delimiter_at_start) {
  const auto parts{sourcemeta::core::split_once("=value", '=')};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "value");
}

TEST(Text_split_once, char_delimiter_at_end) {
  const auto parts{sourcemeta::core::split_once("key=", '=')};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "");
}

TEST(Text_split_once, char_delimiter_absent) {
  EXPECT_FALSE(sourcemeta::core::split_once("no separator", '=').has_value());
}

TEST(Text_split_once, char_delimiter_only_first_occurrence) {
  const auto parts{sourcemeta::core::split_once("a=b=c", '=')};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a");
  EXPECT_EQ(parts->second, "b=c");
}

TEST(Text_split_once, char_delimiter_empty_input) {
  EXPECT_FALSE(sourcemeta::core::split_once("", '=').has_value());
}

TEST(Text_split_once, char_delimiter_only_delimiter) {
  const auto parts{sourcemeta::core::split_once("=", '=')};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "");
}

TEST(Text_split_once, string_delimiter_in_middle) {
  const auto parts{sourcemeta::core::split_once("1..5", "..")};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "1");
  EXPECT_EQ(parts->second, "5");
}

TEST(Text_split_once, string_delimiter_at_start) {
  const auto parts{sourcemeta::core::split_once("..suffix", "..")};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "suffix");
}

TEST(Text_split_once, string_delimiter_at_end) {
  const auto parts{sourcemeta::core::split_once("prefix..", "..")};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "prefix");
  EXPECT_EQ(parts->second, "");
}

TEST(Text_split_once, string_delimiter_absent) {
  EXPECT_FALSE(
      sourcemeta::core::split_once("no double dots", "..").has_value());
}

TEST(Text_split_once, string_delimiter_only_first_occurrence) {
  const auto parts{sourcemeta::core::split_once("a..b..c", "..")};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a");
  EXPECT_EQ(parts->second, "b..c");
}

TEST(Text_split_once, string_delimiter_partial_match_not_treated_as_split) {
  EXPECT_FALSE(sourcemeta::core::split_once("a.b.c", "..").has_value());
}

TEST(Text_split_once, string_delimiter_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::split_once("anything", "").has_value());
}

TEST(Text_split_once, string_delimiter_longer_than_input) {
  EXPECT_FALSE(sourcemeta::core::split_once("ab", "abc").has_value());
}

TEST(Text_split_once, string_delimiter_equals_input) {
  const auto parts{sourcemeta::core::split_once("XYZ", "XYZ")};
  ASSERT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "");
}
