#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, port_copy_constructor) {
  const sourcemeta::core::URLPatternPort original{"8080"};
  const sourcemeta::core::URLPatternPort copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse, port_move_constructor) {
  sourcemeta::core::URLPatternPort original{"8080"};
  const sourcemeta::core::URLPatternPort moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse, port_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternPort original{"8080"};
  pattern.port = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse, port_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternPort original{"8080"};
  pattern.port = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse, port_default_constructor) {
  const sourcemeta::core::URLPatternPort port;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(port, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, port_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.port = "8080"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse, port_80) {
  const sourcemeta::core::URLPattern pattern{.port = "80"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "80");
}

TEST(URLPattern_parse, port_443) {
  const sourcemeta::core::URLPattern pattern{.port = "443"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "443");
}

TEST(URLPattern_parse, port_3000) {
  const sourcemeta::core::URLPattern pattern{.port = "3000"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "3000");
}

TEST(URLPattern_parse, port_name_token) {
  const sourcemeta::core::URLPattern pattern{.port = ":port"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartName, "port");
}

TEST(URLPattern_parse, port_name_optional) {
  const sourcemeta::core::URLPattern pattern{.port = ":port?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartNameOptional,
                                "port");
}

TEST(URLPattern_parse, port_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.port = ":port+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartNameMultiple,
                                "port");
}

TEST(URLPattern_parse, port_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = ":port*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartNameAsterisk,
                                "port");
}

TEST(URLPattern_parse, port_name_regex) {
  const sourcemeta::core::URLPattern pattern{.port = ":port(\\d+)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.port, URLPatternPartNameRegExp, "port", "\\d+");
}

TEST(URLPattern_parse, port_regex) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.port, URLPatternPartRegExp, "\\d+");
}

TEST(URLPattern_parse, port_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.port, URLPatternPartRegExpOptional,
                                "\\d+");
}

TEST(URLPattern_parse, port_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.port, URLPatternPartRegExpMultiple,
                                "\\d+");
}

TEST(URLPattern_parse, port_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.port, URLPatternPartRegExpAsterisk,
                                "\\d+");
}

TEST(URLPattern_parse, port_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, port_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.port = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, port_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.port = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, port_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, port_group_simple) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartGroup);
}

TEST(URLPattern_parse, port_group_optional) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, port_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, port_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, port_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPort, "{8080", 5);
}

TEST(URLPattern_parse, port_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPort, "(\\d+", 4);
}

TEST(URLPattern_parse, port_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPort, ":port%", 5);
}
