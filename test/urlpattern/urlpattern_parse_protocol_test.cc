#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, protocol_copy_constructor) {
  const sourcemeta::core::URLPatternProtocol original{"https"};
  const sourcemeta::core::URLPatternProtocol copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "https");
}

TEST(URLPattern_parse, protocol_move_constructor) {
  sourcemeta::core::URLPatternProtocol original{"https"};
  const sourcemeta::core::URLPatternProtocol moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "https");
}

TEST(URLPattern_parse, protocol_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternProtocol original{"https"};
  pattern.protocol = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
}

TEST(URLPattern_parse, protocol_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternProtocol original{"https"};
  pattern.protocol = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
}

TEST(URLPattern_parse, protocol_default_constructor) {
  const sourcemeta::core::URLPatternProtocol protocol;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(protocol, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, protocol_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
}

TEST(URLPattern_parse, protocol_http) {
  const sourcemeta::core::URLPattern pattern{.protocol = "http"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "http");
}

TEST(URLPattern_parse, protocol_ftp) {
  const sourcemeta::core::URLPattern pattern{.protocol = "ftp"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "ftp");
}

TEST(URLPattern_parse, protocol_name_token) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":protocol"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartName,
                                "protocol");
}

TEST(URLPattern_parse, protocol_name_optional) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":protocol?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartNameOptional,
                                "protocol");
}

TEST(URLPattern_parse, protocol_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":protocol+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartNameMultiple,
                                "protocol");
}

TEST(URLPattern_parse, protocol_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":protocol*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartNameAsterisk,
                                "protocol");
}

TEST(URLPattern_parse, protocol_name_regex) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":proto(https?)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.protocol, URLPatternPartNameRegExp, "proto", "https?");
}

TEST(URLPattern_parse, protocol_regex) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.protocol, URLPatternPartRegExp,
                                "https?");
}

TEST(URLPattern_parse, protocol_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.protocol, URLPatternPartRegExpOptional,
                                "https?");
}

TEST(URLPattern_parse, protocol_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.protocol, URLPatternPartRegExpMultiple,
                                "https?");
}

TEST(URLPattern_parse, protocol_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = "(https?)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.protocol, URLPatternPartRegExpAsterisk,
                                "https?");
}

TEST(URLPattern_parse, protocol_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, protocol_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.protocol = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, protocol_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.protocol = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, protocol_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, protocol_group_simple) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartGroup);
}

TEST(URLPattern_parse, protocol_group_optional) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, protocol_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, protocol_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.protocol = "{https}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol,
                                   URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, protocol_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternProtocol, "{http", 5);
}

TEST(URLPattern_parse, protocol_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternProtocol, "(https?", 7);
}

TEST(URLPattern_parse, protocol_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternProtocol, ":proto%", 6);
}
