#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, password_copy_constructor) {
  const sourcemeta::core::URLPatternPassword original{"secret"};
  const sourcemeta::core::URLPatternPassword copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "secret");
}

TEST(URLPattern_parse, password_move_constructor) {
  sourcemeta::core::URLPatternPassword original{"secret"};
  const sourcemeta::core::URLPatternPassword moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "secret");
}

TEST(URLPattern_parse, password_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternPassword original{"secret"};
  pattern.password = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "secret");
}

TEST(URLPattern_parse, password_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternPassword original{"secret"};
  pattern.password = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "secret");
}

TEST(URLPattern_parse, password_default_constructor) {
  const sourcemeta::core::URLPatternPassword password;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(password, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, password_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.password = "secret"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "secret");
}

TEST(URLPattern_parse, password_pass) {
  const sourcemeta::core::URLPattern pattern{.password = "pass123"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar,
                                "pass123");
}

TEST(URLPattern_parse, password_with_special_chars) {
  const sourcemeta::core::URLPattern pattern{.password = "p@ss"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "p@ss");
}

TEST(URLPattern_parse, password_name_token) {
  const sourcemeta::core::URLPattern pattern{.password = ":password"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartName,
                                "password");
}

TEST(URLPattern_parse, password_name_optional) {
  const sourcemeta::core::URLPattern pattern{.password = ":password?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartNameOptional,
                                "password");
}

TEST(URLPattern_parse, password_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.password = ":password+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartNameMultiple,
                                "password");
}

TEST(URLPattern_parse, password_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = ":password*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartNameAsterisk,
                                "password");
}

TEST(URLPattern_parse, password_name_regex) {
  const sourcemeta::core::URLPattern pattern{.password = ":pass(\\w+)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.password, URLPatternPartNameRegExp, "pass", "\\w+");
}

TEST(URLPattern_parse, password_regex) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\w+)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.password, URLPatternPartRegExp, "\\w+");
}

TEST(URLPattern_parse, password_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\w+)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.password, URLPatternPartRegExpOptional,
                                "\\w+");
}

TEST(URLPattern_parse, password_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\w+)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.password, URLPatternPartRegExpMultiple,
                                "\\w+");
}

TEST(URLPattern_parse, password_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\w+)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.password, URLPatternPartRegExpAsterisk,
                                "\\w+");
}

TEST(URLPattern_parse, password_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, password_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.password = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, password_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.password = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, password_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, password_group_simple) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartGroup);
}

TEST(URLPattern_parse, password_group_optional) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, password_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, password_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password,
                                   URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, password_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPassword, "{secret", 7);
}

TEST(URLPattern_parse, password_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPassword, "(\\w+", 4);
}

TEST(URLPattern_parse, password_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPassword, ":pass%", 5);
}
