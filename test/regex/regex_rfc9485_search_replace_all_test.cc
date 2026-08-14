#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

static auto compile(const std::string &pattern) -> sourcemeta::core::Regex {
  return sourcemeta::core::to_regex(
             pattern, sourcemeta::core::RegexDialect::IRegexpSearch, false)
      .value();
}

TEST(rfc9485_search_replace_all_substring_at_start) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abyy", "#"), "#yy");
}

TEST(rfc9485_search_replace_all_substring_in_middle) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "xxabyy", "#"), "xx#yy");
}

TEST(rfc9485_search_replace_all_substring_at_end) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "xxab", "#"), "xx#");
}

TEST(rfc9485_search_replace_all_every_occurrence) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abxxabyyab", "#"), "#xx#yy#");
}

TEST(rfc9485_search_replace_all_without_a_match_leaves_the_subject_alone) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "xyz", "#"), "xyz");
}

TEST(rfc9485_search_replace_all_on_an_empty_subject) {
  const auto regex{compile("ab")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "", "#"), "");
}

TEST(rfc9485_search_replace_all_character_class) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b22c", "#"), "a#b#c");
}

TEST(rfc9485_search_replace_all_with_an_empty_replacement_deletes) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b22c", ""), "abc");
}

TEST(rfc9485_search_replace_all_with_a_longer_replacement) {
  const auto regex{compile("[0-9]")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "12", "wwwwwwwwww"),
            "wwwwwwwwwwwwwwwwwwww");
}

TEST(rfc9485_search_replace_all_replacement_group_reference_is_literal) {
  const auto regex{compile("([0-9]+)")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "$1"), "a$1b");
}

TEST(rfc9485_search_replace_all_preserves_multibyte_characters) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "é1é", "#"), "é#é");
}
