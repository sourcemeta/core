#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, pathname_copy_constructor) {
  const sourcemeta::core::URLPatternPathname original{"/api/v1"};
  const sourcemeta::core::URLPatternPathname copy{original};
  EXPECT_EQ(copy.value.size(), 2);
  EXPECT_PART_WITH_VALUE(copy, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(copy, 1, URLPatternPartChar, "v1");
}

TEST(URLPattern_parse, pathname_move_constructor) {
  sourcemeta::core::URLPatternPathname original{"/api/v1"};
  const sourcemeta::core::URLPatternPathname moved{std::move(original)};
  EXPECT_EQ(moved.value.size(), 2);
  EXPECT_PART_WITH_VALUE(moved, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(moved, 1, URLPatternPartChar, "v1");
}

TEST(URLPattern_parse, pathname_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternPathname original{"/api/v1"};
  pattern.pathname = original;
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
}

TEST(URLPattern_parse, pathname_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternPathname original{"/api/v1"};
  pattern.pathname = std::move(original);
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
}

TEST(URLPattern_parse, pathname_default_constructor) {
  const sourcemeta::core::URLPatternPathname pathname;
  EXPECT_EQ(pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pathname, 0, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_combined_all_token_types) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/:bar/*"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 2, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_error_empty_input) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "", 0);
}

TEST(URLPattern_parse, pathname_error_trailing_backslash) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/foo\\", 4);
}

TEST(URLPattern_parse, pathname_error_name_starts_with_digit) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:123", 2);
}

TEST(URLPattern_parse, pathname_error_name_starts_with_hyphen) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:foo-bar", 5);
}

TEST(URLPattern_parse, pathname_error_name_with_at_sign) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:foo@bar", 5);
}

TEST(URLPattern_parse, pathname_error_empty_name_token) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:", 2);
}

TEST(URLPattern_parse, pathname_error_empty_name_before_slash) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:/foo", 2);
}

TEST(URLPattern_parse, pathname_error_empty_name_before_asterisk) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:*", 2);
}

TEST(URLPattern_parse, pathname_name_token_valid_start_lowercase) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "foo");
}

TEST(URLPattern_parse, pathname_name_token_valid_start_uppercase) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:Foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "Foo");
}

TEST(URLPattern_parse, pathname_name_token_valid_start_underscore) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:_foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "_foo");
}

TEST(URLPattern_parse, pathname_name_token_valid_start_dollar) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:$foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "$foo");
}

TEST(URLPattern_parse, pathname_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
}

TEST(URLPattern_parse, pathname_multiple_char_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/bar/baz"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "baz");
}

TEST(URLPattern_parse, pathname_empty_segment_double_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo//bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_empty_segment_trailing_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "");
}

TEST(URLPattern_parse, pathname_single_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/"};
  EXPECT_EQ(pattern.pathname.value.size(), 0);
}

TEST(URLPattern_parse, pathname_asterisk_alone) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_bare_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "*"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_bare_name) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "path");
}

TEST(URLPattern_parse, pathname_bare_char) {
  const sourcemeta::core::URLPattern pattern{.pathname = "foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
}

TEST(URLPattern_parse, pathname_bare_regex) {
  const sourcemeta::core::URLPattern pattern{.pathname = "(\\\\w+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExp, "\\\\w+");
}

TEST(URLPattern_parse, pathname_bare_name_regex) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path(\\\\d+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 0,
                                   URLPatternPartNameRegExp, "path", "\\\\d+");
}

TEST(URLPattern_parse, pathname_bare_group) {
  const sourcemeta::core::URLPattern pattern{.pathname = "{foo}"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartGroup);
}

TEST(URLPattern_parse, pathname_bare_asterisk_with_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "*/foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "foo");
}

TEST(URLPattern_parse, pathname_asterisk_with_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*/foo"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "foo");
}

TEST(URLPattern_parse, pathname_multiple_asterisks) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*/*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_combined_char_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/:bar/*"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 2, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_asterisk_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, pathname_asterisk_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, pathname_asterisk_optional_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?/*?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, pathname_asterisk_optional_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/*?/users"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 2,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartChar, "users");
}

TEST(URLPattern_parse, pathname_asterisk_optional_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/*?/:bar/*/baz/*?"};
  EXPECT_EQ(pattern.pathname.value.size(), 6);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskOptional);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 4, URLPatternPartChar, "baz");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 5,
                            URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, pathname_asterisk_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*+"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, pathname_asterisk_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, pathname_asterisk_multiple_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*+/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+/bar/*+"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, pathname_asterisk_multiple_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+/bar/:name"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "name");
}

TEST(URLPattern_parse, pathname_asterisk_multiple_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/*+/:bar/*/baz/*+"};
  EXPECT_EQ(pattern.pathname.value.size(), 6);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskMultiple);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 4, URLPatternPartChar, "baz");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 5,
                            URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, pathname_asterisk_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/**"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, pathname_asterisk_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, pathname_asterisk_asterisk_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/**/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**/bar/**"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, pathname_asterisk_asterisk_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**/bar/:name"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "name");
}

TEST(URLPattern_parse, pathname_asterisk_asterisk_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/**/:bar/*/baz/**"};
  EXPECT_EQ(pattern.pathname.value.size(), 6);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1,
                            URLPatternPartAsteriskAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "bar");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 4, URLPatternPartChar, "baz");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 5,
                            URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, pathname_escape_colon) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\:bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo:bar");
}

TEST(URLPattern_parse, pathname_escape_colon_at_end) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\:"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo:");
}

TEST(URLPattern_parse, pathname_escape_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\*bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo*bar");
}

TEST(URLPattern_parse, pathname_escape_backslash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\\\bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo\\bar");
}

TEST(URLPattern_parse, pathname_escape_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo/bar");
}

TEST(URLPattern_parse, pathname_multiple_escapes) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/\\:\\*\\\\"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, ":*\\");
}

TEST(URLPattern_parse, pathname_name_token_with_digits) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo123"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "foo123");
}

TEST(URLPattern_parse, pathname_name_token_with_underscores) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo_bar_baz"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName,
                         "foo_bar_baz");
}

TEST(URLPattern_parse, pathname_name_token_with_dollar_signs) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:$foo$bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "$foo$bar");
}

TEST(URLPattern_parse, pathname_multiple_name_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo/:bar/:baz"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "baz");
}

TEST(URLPattern_parse, pathname_complex_realistic_pattern) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/users/:id"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "users");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "id");
}

TEST(URLPattern_parse, pathname_wildcard_prefix_pattern) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "static");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_escaped_characters_in_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\:\\*\\\\/:bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo:*\\");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");
}

TEST(URLPattern_parse, pathname_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExp, ".*");
}

TEST(URLPattern_parse, pathname_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExp, ".*");
}

TEST(URLPattern_parse, pathname_regex_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExp, ".*");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_regexes) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(foo)/(bar)"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExp, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExp, "bar");
}

TEST(URLPattern_parse, pathname_regex_complex_pattern) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/:bar/(.*)/*/([0-9]+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 5);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 2, URLPatternPartRegExp, ".*");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 3, URLPatternPartAsterisk);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 4, URLPatternPartRegExp, "[0-9]+");
}

TEST(URLPattern_parse, pathname_regex_unclosed) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/foo/(.*", 8);
}

TEST(URLPattern_parse, pathname_regex_empty) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/()", 2);
}

TEST(URLPattern_parse, pathname_name_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo(.*)"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 0,
                                   URLPatternPartNameRegExp, "foo", ".*");
}

TEST(URLPattern_parse, pathname_name_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version(\\d+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 1,
                                   URLPatternPartNameRegExp, "version", "\\d+");
}

TEST(URLPattern_parse, pathname_name_regex_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id(\\d+)/profile"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 0,
                                   URLPatternPartNameRegExp, "id", "\\d+");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "profile");
}

TEST(URLPattern_parse, pathname_multiple_name_regex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/:foo(\\w+)/:bar(\\d+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 0,
                                   URLPatternPartNameRegExp, "foo", "\\w+");
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 1,
                                   URLPatternPartNameRegExp, "bar", "\\d+");
}

TEST(URLPattern_parse, pathname_name_regex_complex) {
  const sourcemeta::core::URLPattern pattern{
      .pathname = "/api/:version(v\\d+)/users/:id(\\d+)/*/posts/(\\w+)"};
  EXPECT_EQ(pattern.pathname.value.size(), 7);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE_AND_REGEX(
      pattern.pathname, 1, URLPatternPartNameRegExp, "version", "v\\d+");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "users");
  EXPECT_PART_WITH_VALUE_AND_REGEX(pattern.pathname, 3,
                                   URLPatternPartNameRegExp, "id", "\\d+");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 4, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 5, URLPatternPartChar, "posts");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 6, URLPatternPartRegExp, "\\w+");
}

TEST(URLPattern_parse, pathname_name_regex_unclosed) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:foo(.*", 8);
}

TEST(URLPattern_parse, pathname_name_regex_empty) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternPathname, "/:foo()", 6);
}

TEST(URLPattern_parse, pathname_name_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameOptional,
                         "foo");
}

TEST(URLPattern_parse, pathname_name_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameOptional,
                         "version");
}

TEST(URLPattern_parse, pathname_name_optional_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id?/profile"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameOptional, "id");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "profile");
}

TEST(URLPattern_parse, pathname_multiple_name_optional) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?/:bar?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameOptional,
                         "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameOptional,
                         "bar");
}

TEST(URLPattern_parse, pathname_name_optional_mixed_with_required) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/:required/:optional?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "required");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameOptional,
                         "optional");
}

TEST(URLPattern_parse, pathname_name_optional_complex) {
  const sourcemeta::core::URLPattern pattern{
      .pathname = "/api/:version?/users/:id/:action?"};
  EXPECT_EQ(pattern.pathname.value.size(), 5);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameOptional,
                         "version");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "users");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "id");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 4, URLPatternPartNameOptional,
                         "action");
}

TEST(URLPattern_parse, pathname_name_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameMultiple,
                         "foo");
}

TEST(URLPattern_parse, pathname_name_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameMultiple,
                         "version");
}

TEST(URLPattern_parse, pathname_name_multiple_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id+/profile"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameMultiple, "id");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "profile");
}

TEST(URLPattern_parse, pathname_multiple_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+/:bar+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameMultiple,
                         "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameMultiple,
                         "bar");
}

TEST(URLPattern_parse, pathname_name_multiple_mixed_with_required) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/:required/:multiple+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "required");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameMultiple,
                         "multiple");
}

TEST(URLPattern_parse, pathname_name_multiple_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/users/:path+"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "users");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartNameMultiple,
                         "path");
}

TEST(URLPattern_parse, pathname_name_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameAsterisk,
                         "foo");
}

TEST(URLPattern_parse, pathname_name_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameAsterisk,
                         "version");
}

TEST(URLPattern_parse, pathname_name_asterisk_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id*/profile"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameAsterisk, "id");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "profile");
}

TEST(URLPattern_parse, pathname_multiple_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*/:bar*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartNameAsterisk,
                         "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameAsterisk,
                         "bar");
}

TEST(URLPattern_parse, pathname_name_asterisk_mixed_with_required) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/:required/:asterisk*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "required");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartNameAsterisk,
                         "asterisk");
}

TEST(URLPattern_parse, pathname_name_asterisk_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/files/:path*"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "files");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartNameAsterisk,
                         "path");
}

TEST(URLPattern_parse, pathname_regex_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)?"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpOptional,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_optional_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpOptional,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_optional_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)?/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpOptional,
                         ".*");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(foo)?/(bar)?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpOptional,
                         "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpOptional,
                         "bar");
}

TEST(URLPattern_parse, pathname_regex_optional_mixed_with_required) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)/(.*)?/:name"};
  EXPECT_EQ(pattern.pathname.value.size(), 3);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExp, "\\d+");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpOptional,
                         ".*");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "name");
}

TEST(URLPattern_parse, pathname_regex_optional_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/(\\d+)?/users"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 2, URLPatternPartRegExpOptional,
                         "\\d+");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartChar, "users");
}

TEST(URLPattern_parse, pathname_regex_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)+"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpMultiple,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpMultiple,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_multiple_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.* )+/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpMultiple,
                         ".* ");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(foo)+/(bar)+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpMultiple,
                         "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpMultiple,
                         "bar");
}

TEST(URLPattern_parse, pathname_regex_multiple_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/(.* )+/bar/:name"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpMultiple,
                         ".* ");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "name");
}

TEST(URLPattern_parse, pathname_regex_multiple_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/(\\d+)+/users"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 2, URLPatternPartRegExpMultiple,
                         "\\d+");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartChar, "users");
}

TEST(URLPattern_parse, pathname_regex_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)*"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpAsterisk,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpAsterisk,
                         ".*");
}

TEST(URLPattern_parse, pathname_regex_asterisk_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.* )*/bar"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpAsterisk,
                         ".* ");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_multiple_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(foo)*/(bar)*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_REGEX(pattern.pathname, 0, URLPatternPartRegExpAsterisk,
                         "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpAsterisk,
                         "bar");
}

TEST(URLPattern_parse, pathname_regex_asterisk_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/foo/(.* )*/bar/:name"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 1, URLPatternPartRegExpAsterisk,
                         ".* ");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartChar, "bar");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "name");
}

TEST(URLPattern_parse, pathname_regex_asterisk_complex) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/(\\d+)*/users"};
  EXPECT_EQ(pattern.pathname.value.size(), 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_REGEX(pattern.pathname, 2, URLPatternPartRegExpAsterisk,
                         "\\d+");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartChar, "users");
}

TEST(URLPattern_parse, pathname_group_simple_char) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroup, 1, true, false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1, URLPatternPartGroup, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_group_multiple_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar/:baz}"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroup, 2, true, false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1, URLPatternPartGroup, 0,
                                URLPatternPartChar, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1, URLPatternPartGroup, 1,
                                URLPatternPartName, "baz");
}

TEST(URLPattern_parse, pathname_group_with_asterisk) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/*}"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroup, 1, true, false);
  EXPECT_PART_NESTED_WITHOUT_VALUE(pattern.pathname, 1, URLPatternPartGroup, 0,
                                   URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_group_optional_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupOptional, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_group_optional_multiple_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar/:baz}?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupOptional, 2, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 0,
                                URLPatternPartChar, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 1,
                                URLPatternPartName, "baz");
}

TEST(URLPattern_parse, pathname_group_multiple_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupMultiple, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupMultiple, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_group_multiple_multiple_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar/:baz}+"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupMultiple, 2, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupMultiple, 0,
                                URLPatternPartChar, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupMultiple, 1,
                                URLPatternPartName, "baz");
}

TEST(URLPattern_parse, pathname_group_asterisk_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupAsterisk, 1, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupAsterisk, 0,
                                URLPatternPartChar, "bar");
}

TEST(URLPattern_parse, pathname_group_asterisk_multiple_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar/:baz}*"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupAsterisk, 2, true,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupAsterisk, 0,
                                URLPatternPartChar, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupAsterisk, 1,
                                URLPatternPartName, "baz");
}

TEST(URLPattern_parse, pathname_group_optional_no_inner_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/{:bar}?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupOptional, 1, false,
                    false);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 0,
                                URLPatternPartName, "bar");
}

TEST(URLPattern_parse, pathname_group_optional_with_inner_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "foo{/:bar/}?"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_GROUP(pattern.pathname, 1, URLPatternPartGroupOptional, 2, true,
                    true);
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 0,
                                URLPatternPartName, "bar");
  EXPECT_PART_NESTED_WITH_VALUE(pattern.pathname, 1,
                                URLPatternPartGroupOptional, 1,
                                URLPatternPartChar, "");
}

TEST(URLPattern_parse, pathname_complex_segment_char_name_char) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/file-:name.json"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_COMPLEX_SIZE(pattern.pathname, 0, 3);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 0, URLPatternPartChar,
                                 "file-");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 1, URLPatternPartName,
                                 "name");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 2, URLPatternPartChar,
                                 ".json");
}

TEST(URLPattern_parse, pathname_complex_segment_multiple_names) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/v:major.:minor/:resource"};
  EXPECT_EQ(pattern.pathname.value.size(), 2);
  EXPECT_PART_COMPLEX_SIZE(pattern.pathname, 0, 4);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 0, URLPatternPartChar,
                                 "v");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 1, URLPatternPartName,
                                 "major");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 2, URLPatternPartChar,
                                 ".");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 3, URLPatternPartName,
                                 "minor");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "resource");
}

TEST(URLPattern_parse, pathname_complex_segment_with_group) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo-:bar{baz}+"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_COMPLEX_SIZE(pattern.pathname, 0, 3);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 0, URLPatternPartChar,
                                 "foo-");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 1, URLPatternPartName,
                                 "bar");
  EXPECT_PART_COMPLEX_NESTED_SIZE(pattern.pathname, 0, 2,
                                  URLPatternPartGroupMultiple, 1);
  EXPECT_PART_COMPLEX_NESTED_WITH_VALUE(pattern.pathname, 0, 2,
                                        URLPatternPartGroupMultiple, 0,
                                        URLPatternPartChar, "baz");
}

TEST(URLPattern_parse, pathname_complex_segment_group_first) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{:bar}?baz"};
  EXPECT_EQ(pattern.pathname.value.size(), 1);
  EXPECT_PART_COMPLEX_SIZE(pattern.pathname, 0, 3);
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 0, URLPatternPartChar,
                                 "foo");
  EXPECT_PART_COMPLEX_NESTED_SIZE(pattern.pathname, 0, 1,
                                  URLPatternPartGroupOptional, 1);
  EXPECT_PART_COMPLEX_NESTED_WITH_VALUE(pattern.pathname, 0, 1,
                                        URLPatternPartGroupOptional, 0,
                                        URLPatternPartName, "bar");
  EXPECT_PART_COMPLEX_WITH_VALUE(pattern.pathname, 0, 2, URLPatternPartChar,
                                 "baz");
}
