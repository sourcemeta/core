#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <string>

TEST(Regex_matches_if_valid, match_pattern_true) {
  EXPECT_TRUE(
      sourcemeta::core::matches_if_valid<std::string>("^foo", "foobar"));
}

TEST(Regex_matches_if_valid, match_pattern_false) {
  EXPECT_FALSE(
      sourcemeta::core::matches_if_valid<std::string>("^bar", "foobar"));
}

TEST(Regex_matches, match_true_1) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_2) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_3) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^(.*)$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_4) {
  const auto regex{sourcemeta::core::to_regex<std::string>(".*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_5) {
  const auto regex{sourcemeta::core::to_regex<std::string>(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foobar"));
}

TEST(Regex_matches, match_true_6) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "/foo/bar"));
}

TEST(Regex_matches, match_true_7) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "/"));
}

TEST(Regex_matches, match_true_8) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foo"));
}

TEST(Regex_matches, match_true_9) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "f"));
}

TEST(Regex_matches, match_true_10) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(),
                                                     std::string(256, 'x')));
}

TEST(Regex_matches, match_true_11) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^@")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "@foo"));
}

TEST(Regex_matches, match_true_12) {
  const auto regex{sourcemeta::core::to_regex<std::string>("o$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "foo"));
}

TEST(Regex_matches, match_true_13) {
  const auto regex{
      sourcemeta::core::to_regex<std::string>("^[a-z][a-z0-9-_]{1,63}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "abcd"));
}

TEST(Regex_matches, match_true_14) {
  const auto regex{sourcemeta::core::to_regex<std::string>(
      "^(?:@[0-9a-z-_.]+\\/)?[a-z][0-9a-z-_.]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(),
                                                     "@namespace/mypackage"));
}

TEST(Regex_matches, match_true_15) {
  const auto regex{sourcemeta::core::to_regex<std::string>(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "\n"));
}

TEST(Regex_matches, match_true_16) {
  const auto regex{sourcemeta::core::to_regex<std::string>(".")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "\r"));
}

TEST(Regex_matches, match_true_17) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.+$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "foo\nbar\r"));
}

TEST(Regex_matches, match_false_1) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^foo")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "bar"));
}

TEST(Regex_matches, match_false_2) {
  const auto regex{sourcemeta::core::to_regex<std::string>(".+")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), ""));
}

TEST(Regex_matches, match_false_3) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^/.*")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "foo/bar"));
}

TEST(Regex_matches, match_false_4) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.{1,256}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(),
                                                      std::string(257, 'x')));
}

TEST(Regex_matches, match_false_5) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^@")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "foo"));
}

TEST(Regex_matches, match_false_6) {
  const auto regex{sourcemeta::core::to_regex<std::string>("o$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "bar"));
}

TEST(Regex_matches, unicode_range_arabic_indic_digit) {
  const auto regex{
      sourcemeta::core::to_regex<std::string>("[\\u0660-\\u0669]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "\u0660"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "\u0669"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "0"));
}

TEST(Regex_matches, unicode_range_4byte_deseret) {
  const auto regex{
      sourcemeta::core::to_regex<std::string>("[\\u{10400}-\\u{1044F}]")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U00010400"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U0001044F"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "A"));
}

TEST(Regex_matches, unicode_property_letter) {
  const auto regex{sourcemeta::core::to_regex<std::string>(
      "^\\p{Letter}[\\p{Letter}\\p{Number}]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "hello"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "hello123"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "Àlement"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "中文"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U00010400test"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "123hello"));
}

TEST(Regex_matches, unicode_property_exclude_digit) {
  const auto regex{sourcemeta::core::to_regex<std::string>(
      "^(?!\\p{Number})\\p{Letter}[\\p{Letter}\\p{Number}-_.]*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "element123"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "element٠"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "٠element"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "0element"));
}

TEST(Regex_matches, unicode_dot_matches_codepoint) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "A"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "À"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "中"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U00010400"));
}

TEST(Regex_matches, unicode_quantifier_on_codepoints) {
  const auto regex{sourcemeta::core::to_regex<std::string>("^.{3}$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "ABC"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "ÀÁÂ"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "中文字"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(
      regex.value(), "\U00010400\U00010401\U00010402"));
}

TEST(Regex_matches, digit_ascii_only) {
  // \d should only match ASCII digits 0-9, not Unicode digits
  const auto regex{sourcemeta::core::to_regex<std::string>("^\\d$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "0"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "\u07C0"));
}

TEST(Regex_matches, word_ascii_only) {
  // \w should only match ASCII [a-zA-Z0-9_], not Unicode letters
  const auto regex{sourcemeta::core::to_regex<std::string>("^\\w$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "a"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "é"));
}

TEST(Regex_matches, nonbmp_emoji_quantifier) {
  // Dragon emoji (U+1F432) is 4-byte UTF-8
  // Pattern ^🐲*$ should match zero or more dragon emojis
  // This tests that quantifiers work on codepoints, not bytes
  const auto regex{sourcemeta::core::to_regex<std::string>("^\U0001F432*$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), ""));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U0001F432"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(),
                                                     "\U0001F432\U0001F432"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U0001F409"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(), "D"));
}

TEST(Regex_matches, nonbmp_literal_match) {
  // Test literal matching of 4-byte UTF-8 characters
  const auto regex{sourcemeta::core::to_regex<std::string>("^\U0001F432$")};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U0001F432"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "\U0001F409"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(),
                                                      "\U0001F432\U0001F432"));
}

TEST(Regex_matches, xml_ncname_simplified) {
  const auto regex{sourcemeta::core::to_regex<std::string>(
      "^(?![:\\p{Nd}])[\\p{L}_][\\p{L}\\p{Nd}\\-._·]*$")};
  EXPECT_TRUE(regex.has_value());

  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "element"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "_element"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "element123"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(), "élément"));
  EXPECT_TRUE(
      sourcemeta::core::matches<std::string>(regex.value(), "element٠"));
  EXPECT_TRUE(sourcemeta::core::matches<std::string>(regex.value(),
                                                     "\U00010400element"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), ":element"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "0element"));
  EXPECT_FALSE(
      sourcemeta::core::matches<std::string>(regex.value(), "٠element"));
  EXPECT_FALSE(sourcemeta::core::matches<std::string>(regex.value(),
                                                      "\U000104A0element"));
}
