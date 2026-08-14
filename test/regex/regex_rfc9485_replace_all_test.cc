#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

static auto compile(const std::string &pattern) -> sourcemeta::core::Regex {
  return sourcemeta::core::to_regex(
             pattern, sourcemeta::core::RegexDialect::IRegexp, false)
      .value();
}

TEST(rfc9485_replace_all_whole_subject) {
  const auto regex{compile("a")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a", "#"), "#");
}

TEST(rfc9485_replace_all_partial_subject_is_left_alone) {
  const auto regex{compile("a")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "bab", "#"), "bab");
}

TEST(rfc9485_replace_all_without_a_match_leaves_the_subject_alone) {
  const auto regex{compile("a")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "b", "#"), "b");
}

TEST(rfc9485_replace_all_empty_pattern_on_an_empty_subject) {
  const auto regex{compile("")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "", "#"), "#");
}

TEST(rfc9485_replace_all_empty_pattern_on_a_non_empty_subject) {
  const auto regex{compile("")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a", "#"), "a");
}

TEST(rfc9485_replace_all_character_class) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "123", "#"), "#");
}

TEST(rfc9485_replace_all_character_class_partial_is_left_alone) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a123", "#"), "a123");
}

TEST(rfc9485_replace_all_dot_asterisk_matches_once_being_anchored) {
  const auto regex{compile(".*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "#");
}

TEST(rfc9485_replace_all_with_an_empty_replacement_deletes) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "123", ""), "");
}

TEST(rfc9485_replace_all_with_a_longer_replacement) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "12", "wwwwwwwwww"),
            "wwwwwwwwww");
}

TEST(rfc9485_replace_all_replacement_group_reference_is_literal) {
  const auto regex{compile("([0-9]+)")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "12", "$1"), "$1");
}

TEST(rfc9485_replace_all_preserves_multibyte_characters) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "12", "é"), "é");
}
