#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(char_delimiter_in_middle) {
  const auto parts{sourcemeta::core::split_once("key=value", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "value");
}

TEST(char_delimiter_at_start) {
  const auto parts{sourcemeta::core::split_once("=value", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "value");
}

TEST(char_delimiter_at_end) {
  const auto parts{sourcemeta::core::split_once("key=", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "");
}

TEST(char_delimiter_absent) {
  EXPECT_FALSE(sourcemeta::core::split_once("no separator", '=').has_value());
}

TEST(char_delimiter_only_first_occurrence) {
  const auto parts{sourcemeta::core::split_once("a=b=c", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a");
  EXPECT_EQ(parts->second, "b=c");
}

TEST(char_delimiter_empty_input) {
  EXPECT_FALSE(sourcemeta::core::split_once("", '=').has_value());
}

TEST(char_delimiter_only_delimiter) {
  const auto parts{sourcemeta::core::split_once("=", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "");
}

TEST(string_delimiter_in_middle) {
  const auto parts{sourcemeta::core::split_once("1..5", "..")};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "1");
  EXPECT_EQ(parts->second, "5");
}

TEST(string_delimiter_at_start) {
  const auto parts{sourcemeta::core::split_once("..suffix", "..")};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "suffix");
}

TEST(string_delimiter_at_end) {
  const auto parts{sourcemeta::core::split_once("prefix..", "..")};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "prefix");
  EXPECT_EQ(parts->second, "");
}

TEST(string_delimiter_absent) {
  EXPECT_FALSE(
      sourcemeta::core::split_once("no double dots", "..").has_value());
}

TEST(string_delimiter_only_first_occurrence) {
  const auto parts{sourcemeta::core::split_once("a..b..c", "..")};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a");
  EXPECT_EQ(parts->second, "b..c");
}

TEST(string_delimiter_partial_match_not_treated_as_split) {
  EXPECT_FALSE(sourcemeta::core::split_once("a.b.c", "..").has_value());
}

TEST(string_delimiter_empty_returns_nullopt) {
  EXPECT_FALSE(sourcemeta::core::split_once("anything", "").has_value());
}

TEST(string_delimiter_longer_than_input) {
  EXPECT_FALSE(sourcemeta::core::split_once("ab", "abc").has_value());
}

TEST(string_delimiter_equals_input) {
  const auto parts{sourcemeta::core::split_once("XYZ", "XYZ")};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "");
}
