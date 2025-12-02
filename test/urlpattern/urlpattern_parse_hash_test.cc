#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, hash_copy_constructor) {
  const sourcemeta::core::URLPatternHash original{"section"};
  const sourcemeta::core::URLPatternHash copy{original};
  EXPECT_SINGLE_PART_WITH_VALUE(copy, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hash_move_constructor) {
  sourcemeta::core::URLPatternHash original{"section"};
  const sourcemeta::core::URLPatternHash moved{std::move(original)};
  EXPECT_SINGLE_PART_WITH_VALUE(moved, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hash_copy_assignment) {
  sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternHash original{"section"};
  pattern.hash = original;
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hash_move_assignment) {
  sourcemeta::core::URLPattern pattern;
  sourcemeta::core::URLPatternHash original{"section"};
  pattern.hash = std::move(original);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hash_default_constructor) {
  const sourcemeta::core::URLPatternHash hash;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hash_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.hash = "section"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hash_intro) {
  const sourcemeta::core::URLPattern pattern{.hash = "intro"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "intro");
}

TEST(URLPattern_parse, hash_heading) {
  const sourcemeta::core::URLPattern pattern{.hash = "heading-1"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "heading-1");
}

TEST(URLPattern_parse, hash_name_token) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartName, "fragment");
}

TEST(URLPattern_parse, hash_name_optional) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment?"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartNameOptional,
                                "fragment");
}

TEST(URLPattern_parse, hash_name_multiple) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment+"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartNameMultiple,
                                "fragment");
}

TEST(URLPattern_parse, hash_name_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment*"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartNameAsterisk,
                                "fragment");
}

TEST(URLPattern_parse, hash_name_regex) {
  const sourcemeta::core::URLPattern pattern{.hash = ":frag(\\w+)"};
  EXPECT_SINGLE_PART_WITH_VALUE_AND_REGEX(
      pattern.hash, URLPatternPartNameRegExp, "frag", "\\w+");
}

TEST(URLPattern_parse, hash_regex) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\w+)"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.hash, URLPatternPartRegExp, "\\w+");
}

TEST(URLPattern_parse, hash_regex_optional) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\w+)?"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.hash, URLPatternPartRegExpOptional,
                                "\\w+");
}

TEST(URLPattern_parse, hash_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\w+)+"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.hash, URLPatternPartRegExpMultiple,
                                "\\w+");
}

TEST(URLPattern_parse, hash_regex_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\w+)*"};
  EXPECT_SINGLE_PART_WITH_REGEX(pattern.hash, URLPatternPartRegExpAsterisk,
                                "\\w+");
}

TEST(URLPattern_parse, hash_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = "*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, hash_asterisk_optional) {
  const sourcemeta::core::URLPattern pattern{.hash = "*?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash,
                                   URLPatternPartAsteriskOptional);
}

TEST(URLPattern_parse, hash_asterisk_multiple) {
  const sourcemeta::core::URLPattern pattern{.hash = "*+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash,
                                   URLPatternPartAsteriskMultiple);
}

TEST(URLPattern_parse, hash_asterisk_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = "**"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash,
                                   URLPatternPartAsteriskAsterisk);
}

TEST(URLPattern_parse, hash_group_simple) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartGroup);
}

TEST(URLPattern_parse, hash_group_optional) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}?"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartGroupOptional);
}

TEST(URLPattern_parse, hash_group_multiple) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}+"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartGroupMultiple);
}

TEST(URLPattern_parse, hash_group_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartGroupAsterisk);
}

TEST(URLPattern_parse, hash_error_unclosed_group) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHash, "{section", 8);
}

TEST(URLPattern_parse, hash_error_unclosed_regex) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHash, "(\\w+", 4);
}

TEST(URLPattern_parse, hash_error_invalid_modifier) {
  EXPECT_COMPONENT_PARSE_ERROR(URLPatternHash, ":frag%", 5);
}
