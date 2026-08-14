#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

static auto compile(const std::string &pattern) -> sourcemeta::core::Regex {
  return sourcemeta::core::to_regex(
             pattern, sourcemeta::core::RegexDialect::Permissive, false)
      .value();
}

TEST(permissive_replace_all_single_occurrence) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "#"), "a#b");
}

TEST(permissive_replace_all_every_occurrence) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b22c333d", "#"), "a#b#c#d");
}

TEST(permissive_replace_all_without_a_match_leaves_the_subject_alone) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "abc");
}

TEST(permissive_replace_all_on_an_empty_subject) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "", "#"), "");
}

TEST(permissive_replace_all_with_an_empty_replacement_deletes) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b22c", ""), "abc");
}

TEST(permissive_replace_all_with_a_longer_replacement) {
  const auto regex{compile("[0-9]")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "1234", "wwwwwwwwww"),
            "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
}

TEST(permissive_replace_all_the_whole_subject) {
  const auto regex{compile("^abc$")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "x"), "x");
}

TEST(permissive_replace_all_replacement_group_reference_is_literal) {
  const auto regex{compile("([0-9]+)")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "$1"), "a$1b");
}

TEST(permissive_replace_all_replacement_backslash_is_literal) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "\\n"), "a\\nb");
}

TEST(permissive_replace_all_replacement_dollar_brace_is_literal) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "${0}"), "a${0}b");
}

TEST(permissive_replace_all_zero_width_match) {
  const auto regex{compile("x*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "-"), "-a-b-c-");
}

TEST(permissive_replace_all_zero_width_match_around_content) {
  const auto regex{compile("x*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "axxb", "-"), "-a--b-");
}

TEST(permissive_replace_all_across_line_breaks) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1\nb2\n", "#"), "a#\nb#\n");
}

TEST(permissive_replace_all_dollar_anchors_the_subject_end) {
  const auto regex{compile("b$")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "b\nb", "#"), "b\n#");
}

TEST(permissive_replace_all_dollar_does_not_anchor_before_a_trailing_newline) {
  const auto regex{compile("b$")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "ab\n", "#"), "ab\n");
}

TEST(permissive_replace_all_dot_covers_a_line_break) {
  const auto regex{compile("a.b")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a\nb", "#"), "#");
}

TEST(permissive_replace_all_preserves_multibyte_characters) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "é1é", "#"), "é#é");
}

TEST(permissive_replace_all_with_a_multibyte_replacement) {
  const auto regex{compile("[0-9]+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "a1b", "é"), "aéb");
}

TEST(permissive_replace_all_unoptimised_dot_asterisk) {
  const auto regex{compile(".*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "##");
}

TEST(permissive_replace_all_unoptimised_dot) {
  const auto regex{compile(".")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "###");
}

TEST(permissive_replace_all_unoptimised_dot_plus) {
  const auto regex{compile(".+")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "#");
}

TEST(permissive_replace_all_unoptimised_prefix) {
  const auto regex{compile("^foo")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "foobar", "#"), "#bar");
}

TEST(permissive_replace_all_unoptimised_prefix_with_suffix) {
  const auto regex{compile("^foo.*")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "foobar", "#"), "#");
}

TEST(permissive_replace_all_unoptimised_range) {
  const auto regex{compile("^.{2,4}$")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abc", "#"), "#");
}

TEST(permissive_replace_all_unoptimised_range_without_a_match) {
  const auto regex{compile("^.{2,4}$")};
  EXPECT_EQ(sourcemeta::core::replace_all(regex, "abcdefgh", "#"), "abcdefgh");
}
