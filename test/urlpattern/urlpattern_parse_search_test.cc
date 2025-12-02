#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, search_copy_constructor) {
  const sourcemeta::core::URLPatternSearch original{"q=test"};
  const sourcemeta::core::URLPatternSearch copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "q=test");
}

TEST(URLPattern_parse, search_move_constructor) {
  sourcemeta::core::URLPatternSearch original{"q=test"};
  const sourcemeta::core::URLPatternSearch moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "q=test");
}

TEST(URLPattern_parse, search_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternSearch original{"q=test"};
  pattern.search = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "q=test");
}

TEST(URLPattern_parse, search_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternSearch original{"q=test"};
  pattern.search = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "q=test");
}

TEST(URLPattern_parse, search_default_constructor) {
  const sourcemeta::core::URLPatternSearch search;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(search, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, search_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.search = "q=test"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "q=test");
}

TEST(URLPattern_parse, search_query_params) {
  const sourcemeta::core::URLPattern pattern{.search = "q=search&lang=en"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar,
                                "q=search&lang=en");
}

TEST(URLPattern_parse, search_single_param) {
  const sourcemeta::core::URLPattern pattern{.search = "page=1"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "page=1");
}

TEST(URLPattern_parse, search_name_token) {
  const sourcemeta::core::URLPattern pattern{.search = ":query"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartName, "query");
}

TEST(URLPattern_parse, search_name_optional) {
  const sourcemeta::core::URLPattern pattern{.search = ":query?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartNameOptional,
                                "query");
}

TEST(URLPattern_parse, search_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.search = ":query+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartNameMultiple,
                                "query");
}

TEST(URLPattern_parse, search_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = ":query*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartNameAsterisk,
                                "query");
}

TEST(URLPattern_parse, search_name_regex) {
  const sourcemeta::core::URLPattern pattern{.search = ":q([^&]+)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.search, URLPatternPartNameRegExp, "q", "[^&]+");
}

TEST(URLPattern_parse, search_regex) {
  const sourcemeta::core::URLPattern pattern{.search = "([^&]+)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.search, URLPatternPartRegExp, "[^&]+");
}

TEST(URLPattern_parse, search_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.search = "([^&]+)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.search, URLPatternPartRegExpOptional,
                                "[^&]+");
}

TEST(URLPattern_parse, search_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.search = "([^&]+)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.search, URLPatternPartRegExpMultiple,
                                "[^&]+");
}

TEST(URLPattern_parse, search_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = "([^&]+)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.search, URLPatternPartRegExpAsterisk,
                                "[^&]+");
}

TEST(URLPattern_parse, search_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, search_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.search = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, search_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.search = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, search_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, search_group_simple) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartGroup);
}

TEST(URLPattern_parse, search_group_optional) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, search_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, search_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, search_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternSearch, "{q=test", 7);
}

TEST(URLPattern_parse, search_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternSearch, "([^&]+", 6);
}

TEST(URLPattern_parse, search_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternSearch, ":query%", 6);
}
