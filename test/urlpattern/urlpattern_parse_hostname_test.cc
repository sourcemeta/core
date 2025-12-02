#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, hostname_copy_constructor) {
  const sourcemeta::core::URLPatternHostname original{"example.com"};
  const sourcemeta::core::URLPatternHostname copy{original};
  EXPECT_EQ(copy.value.size(), 2);
  EXPECT_PART_WITH_VALUE(copy, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(copy, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_move_constructor) {
  sourcemeta::core::URLPatternHostname original{"example.com"};
  const sourcemeta::core::URLPatternHostname moved{std::move(original)};
  EXPECT_EQ(moved.value.size(), 2);
  EXPECT_PART_WITH_VALUE(moved, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(moved, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternHostname original{"example.com"};
  pattern.hostname = original;
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternHostname original{"example.com"};
  pattern.hostname = std::move(original);
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_default_constructor) {
  const sourcemeta::core::URLPatternHostname hostname;
  EXPECT_EQ(hostname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(hostname, 0, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_combined_all_token_types) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.:bar.*"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 2, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.hostname = "example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_single_char_token) {
  const sourcemeta::core::URLPattern pattern{.hostname = "localhost"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "localhost");
}

TEST(URLPattern_parse, hostname_multiple_char_tokens) {
  const sourcemeta::core::URLPattern pattern{.hostname = "api.example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_empty_segment_double_dot) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo..bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_empty_segment_trailing_dot) {
  const sourcemeta::core::URLPattern pattern{.hostname = "example.com."};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "");
}

TEST(URLPattern_parse, hostname_empty_segment_leading_dot) {
  const sourcemeta::core::URLPattern pattern{.hostname = ".example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_name_token_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":name.example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "name");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_error_name_starts_with_digit) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":123", 1);
}

TEST(URLPattern_parse, hostname_error_name_starts_with_hyphen) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":foo-bar", 4);
}

TEST(URLPattern_parse, hostname_error_name_with_at_sign) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":foo@bar", 4);
}

TEST(URLPattern_parse, hostname_error_empty_name_token) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":", 1);
}

TEST(URLPattern_parse, hostname_error_empty_name_before_dot) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":.example.com", 1);
}

TEST(URLPattern_parse, hostname_error_empty_name_before_asterisk) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":*", 1);
}

TEST(URLPattern_parse, hostname_name_token_valid_start_lowercase) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "foo");
}

TEST(URLPattern_parse, hostname_name_token_valid_start_uppercase) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":Foo"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "Foo");
}

TEST(URLPattern_parse, hostname_name_token_valid_start_underscore) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":_foo"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "_foo");
}

TEST(URLPattern_parse, hostname_name_token_valid_start_dollar) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":$foo"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "$foo");
}

TEST(URLPattern_parse, hostname_asterisk_alone) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_asterisk_with_segments) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*.example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_multiple_asterisks) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*.*"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_combined_char_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.:bar.*"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 2, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_asterisk_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*?"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, hostname_asterisk_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.*?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, hostname_asterisk_optional_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*?.bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_multiple_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*?.*?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, hostname_asterisk_optional_mixed) {
  const sourcemeta::core::URLPattern pattern{.hostname =
                                                 "api.:version.*?.example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 5);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 2,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 3, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 4, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_asterisk_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*+"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, hostname_asterisk_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.*+"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, hostname_asterisk_multiple_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*+.bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_multiple_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.*+.bar.*+"};
  EXPECT_EQ(pattern.hostname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 3,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, hostname_asterisk_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "**"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, hostname_asterisk_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.**"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, hostname_asterisk_asterisk_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "**.bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_multiple_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.**.bar.**"};
  EXPECT_EQ(pattern.hostname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 1,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 3,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, hostname_escape_colon) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo\\:bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo:bar");
}

TEST(URLPattern_parse, hostname_escape_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo\\*bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo*bar");
}

TEST(URLPattern_parse, hostname_escape_backslash) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo\\\\bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo\\bar");
}

TEST(URLPattern_parse, hostname_escape_dot) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo\\.bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo.bar");
}

TEST(URLPattern_parse, hostname_multiple_escapes) {
  const sourcemeta::core::URLPattern pattern{.hostname = "\\:\\*\\\\"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, ":*\\");
}

TEST(URLPattern_parse, hostname_name_token_with_digits) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo123"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "foo123");
}

TEST(URLPattern_parse, hostname_name_token_with_underscores) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo_bar_baz"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName,
                         "foo_bar_baz");
}

TEST(URLPattern_parse, hostname_name_token_with_dollar_signs) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":$foo$bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "$foo$bar");
}

TEST(URLPattern_parse, hostname_multiple_name_tokens) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo.:bar.:baz"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "foo");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartName, "baz");
}

TEST(URLPattern_parse, hostname_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(.*)"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExp, ".*");
}

TEST(URLPattern_parse, hostname_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.(.*)"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.hostname, 1, URLPatternPartRegExp, ".*");
}

TEST(URLPattern_parse, hostname_regex_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(.*).bar"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExp, ".*");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_multiple_regexes) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(foo).(bar)"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExp, "foo");
  EXPECT_PART_WITH_REGEX(pattern.hostname, 1, URLPatternPartRegExp, "bar");
}

TEST(URLPattern_parse, hostname_regex_unclosed) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, "foo.(.*", 7);
}

TEST(URLPattern_parse, hostname_regex_empty) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, "()", 1);
}

TEST(URLPattern_parse, hostname_name_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo(.*)"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.hostname, 0,
                                   URLPatternPartNameRegExp, "foo", ".*");
}

TEST(URLPattern_parse, hostname_name_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "api.:version(\\d+)"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.hostname, 1,
                                   URLPatternPartNameRegExp, "version", "\\d+");
}

TEST(URLPattern_parse, hostname_name_regex_unclosed) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":foo(.*", 7);
}

TEST(URLPattern_parse, hostname_name_regex_empty) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, ":foo()", 5);
}

TEST(URLPattern_parse, hostname_name_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo?"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartNameOptional,
                         "foo");
}

TEST(URLPattern_parse, hostname_name_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "api.:version?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartNameOptional,
                         "version");
}

TEST(URLPattern_parse, hostname_name_optional_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":id?.example.com"};
  EXPECT_EQ(pattern.hostname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartNameOptional, "id");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
}

TEST(URLPattern_parse, hostname_multiple_name_optional) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo?.:bar?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartNameOptional,
                         "foo");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartNameOptional,
                         "bar");
}

TEST(URLPattern_parse, hostname_name_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo+"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartNameMultiple,
                         "foo");
}

TEST(URLPattern_parse, hostname_name_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "api.:version+"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartNameMultiple,
                         "version");
}

TEST(URLPattern_parse, hostname_name_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = ":foo*"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartNameAsterisk,
                         "foo");
}

TEST(URLPattern_parse, hostname_name_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "api.:version*"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartNameAsterisk,
                         "version");
}

TEST(URLPattern_parse, hostname_regex_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(.*)?"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExpOptional,
                         ".*");
}

TEST(URLPattern_parse, hostname_regex_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.(.*)?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.hostname, 1, URLPatternPartRegExpOptional,
                         ".*");
}

TEST(URLPattern_parse, hostname_regex_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(.*)+"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExpMultiple,
                         ".*");
}

TEST(URLPattern_parse, hostname_regex_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.(.*)+"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.hostname, 1, URLPatternPartRegExpMultiple,
                         ".*");
}

TEST(URLPattern_parse, hostname_regex_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "(.*)*"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.hostname, 0, URLPatternPartRegExpAsterisk,
                         ".*");
}

TEST(URLPattern_parse, hostname_regex_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo.(.*)*"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.hostname, 1, URLPatternPartRegExpAsterisk,
                         ".*");
}

TEST(URLPattern_parse, hostname_group_simple_char) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.bar}"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroup, 1, true, false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1, URLPatternPartGroup, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_group_multiple_tokens) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.bar.:baz}"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroup, 2, true, false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1, URLPatternPartGroup, 0,
                                URLPatternPartChar, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1, URLPatternPartGroup, 1,
                                URLPatternPartName, "baz");
}

TEST(URLPattern_parse, hostname_group_with_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.*}"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroup, 1, true, false);
  EXPECT_PART_NESTED_WITHOUT_VALUE(pattern.hostname, 1, URLPatternPartGroup, 0,
                                   URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hostname_group_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.bar}?"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroupOptional, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1,
                                URLPatternPartGroupOptional, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_group_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.bar}+"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroupMultiple, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1,
                                URLPatternPartGroupMultiple, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_group_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo{.bar}*"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.hostname, 1, URLPatternPartGroupAsterisk, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.hostname, 1,
                                URLPatternPartGroupAsterisk, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, hostname_complex_segment_char_name_char) {
  const sourcemeta::core::URLPattern pattern{.hostname = "file-:name.json"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_COMPLEX_SIZE(pattern.hostname, 0, 2);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 0, URLPatternPartChar,
                                 "file-");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 1, URLPatternPartName,
                                 "name");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "json");
}

TEST(URLPattern_parse, hostname_complex_segment_multiple_names) {
  const sourcemeta::core::URLPattern pattern{.hostname =
                                                 "v:major:minor.:resource"};
  EXPECT_EQ(pattern.hostname.value.size(), 2);
  EXPECT_PART_COMPLEX_SIZE(pattern.hostname, 0, 3);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 0, URLPatternPartChar,
                                 "v");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 1, URLPatternPartName,
                                 "major");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 2, URLPatternPartName,
                                 "minor");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartName, "resource");
}

TEST(URLPattern_parse, hostname_complex_segment_with_group) {
  const sourcemeta::core::URLPattern pattern{.hostname = "foo-:bar{baz}+"};
  EXPECT_EQ(pattern.hostname.value.size(), 1);
  EXPECT_PART_COMPLEX_SIZE(pattern.hostname, 0, 3);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 0, URLPatternPartChar,
                                 "foo-");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.hostname, 0, 1, URLPatternPartName,
                                 "bar");
  EXPECT_PART_COMPLEX_NESTED_SIZE(pattern.hostname, 0, 2,
                                  URLPatternPartGroupMultiple, 1);
  EXPECT_PART_COMPLEX_NESTED_WITH_VALUE(pattern.hostname, 0, 2,
                                        URLPatternPartGroupMultiple, 0,
                                        URLPatternPartChar, "baz");
}

TEST(URLPattern_parse, hostname_error_trailing_backslash) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHostname, "foo\\", 3);
}
