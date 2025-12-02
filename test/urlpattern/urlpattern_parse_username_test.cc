#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, username_copy_constructor) {
  const sourcemeta::core::URLPatternUsername original{"admin"};
  const sourcemeta::core::URLPatternUsername copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse, username_move_constructor) {
  sourcemeta::core::URLPatternUsername original{"admin"};
  const sourcemeta::core::URLPatternUsername moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse, username_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternUsername original{"admin"};
  pattern.username = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse, username_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternUsername original{"admin"};
  pattern.username = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse, username_default_constructor) {
  const sourcemeta::core::URLPatternUsername username;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(username, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, username_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.username = "admin"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse, username_user) {
  const sourcemeta::core::URLPattern pattern{.username = "user"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "user");
}

TEST(URLPattern_parse, username_with_numbers) {
  const sourcemeta::core::URLPattern pattern{.username = "user123"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar,
                                "user123");
}

TEST(URLPattern_parse, username_name_token) {
  const sourcemeta::core::URLPattern pattern{.username = ":username"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartName,
                                "username");
}

TEST(URLPattern_parse, username_name_optional) {
  const sourcemeta::core::URLPattern pattern{.username = ":username?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartNameOptional,
                                "username");
}

TEST(URLPattern_parse, username_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.username = ":username+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartNameMultiple,
                                "username");
}

TEST(URLPattern_parse, username_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = ":username*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartNameAsterisk,
                                "username");
}

TEST(URLPattern_parse, username_name_regex) {
  const sourcemeta::core::URLPattern pattern{.username = ":user(\\w+)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.username, URLPatternPartNameRegExp, "user", "\\w+");
}

TEST(URLPattern_parse, username_regex) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\w+)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.username, URLPatternPartRegExp, "\\w+");
}

TEST(URLPattern_parse, username_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\w+)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.username, URLPatternPartRegExpOptional,
                                "\\w+");
}

TEST(URLPattern_parse, username_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\w+)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.username, URLPatternPartRegExpMultiple,
                                "\\w+");
}

TEST(URLPattern_parse, username_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\w+)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.username, URLPatternPartRegExpAsterisk,
                                "\\w+");
}

TEST(URLPattern_parse, username_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, username_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.username = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, username_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.username = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, username_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, username_group_simple) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartGroup);
}

TEST(URLPattern_parse, username_group_optional) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, username_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, username_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username,
                                   URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, username_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternUsername, "{admin", 6);
}

TEST(URLPattern_parse, username_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternUsername, "(\\w+", 4);
}

TEST(URLPattern_parse, username_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternUsername, ":user%", 5);
}
