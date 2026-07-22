#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(delimiter_in_middle) {
  const auto parts{sourcemeta::core::rsplit_once("key=value", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "value");
}

TEST(delimiter_at_start) {
  const auto parts{sourcemeta::core::rsplit_once("=value", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "value");
}

TEST(delimiter_at_end) {
  const auto parts{sourcemeta::core::rsplit_once("key=", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "key");
  EXPECT_EQ(parts->second, "");
}

TEST(delimiter_absent) {
  EXPECT_FALSE(sourcemeta::core::rsplit_once("no separator", '=').has_value());
}

TEST(delimiter_only_last_occurrence) {
  const auto parts{sourcemeta::core::rsplit_once("a=b=c", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a=b");
  EXPECT_EQ(parts->second, "c");
}

TEST(empty_input) {
  EXPECT_FALSE(sourcemeta::core::rsplit_once("", '=').has_value());
}

TEST(only_delimiter) {
  const auto parts{sourcemeta::core::rsplit_once("=", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "");
  EXPECT_EQ(parts->second, "");
}

TEST(splits_an_acct_identifier_at_the_last_at_sign) {
  const auto parts{
      sourcemeta::core::rsplit_once("alice@corp@example.com", '@')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "alice@corp");
  EXPECT_EQ(parts->second, "example.com");
}

TEST(a_trailing_delimiter_leaves_an_empty_suffix) {
  const auto parts{sourcemeta::core::rsplit_once("a=b=", '=')};
  EXPECT_TRUE(parts.has_value());
  EXPECT_EQ(parts->first, "a=b");
  EXPECT_EQ(parts->second, "");
}
