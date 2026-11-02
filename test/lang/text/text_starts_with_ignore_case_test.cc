#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(exact_match) {
  EXPECT_TRUE(
      sourcemeta::core::starts_with_ignore_case("__Host-id", "__Host-"));
}

TEST(differing_case_prefix) {
  EXPECT_TRUE(
      sourcemeta::core::starts_with_ignore_case("__Host-id", "__host-"));
}

TEST(mixed_case_prefix) {
  EXPECT_TRUE(sourcemeta::core::starts_with_ignore_case("ABCdef", "aBcD"));
}

TEST(prefix_longer_than_value) {
  EXPECT_FALSE(sourcemeta::core::starts_with_ignore_case("ab", "abc"));
}

TEST(non_matching_prefix) {
  EXPECT_FALSE(sourcemeta::core::starts_with_ignore_case("hello", "world"));
}

TEST(empty_prefix_matches_any_value) {
  EXPECT_TRUE(sourcemeta::core::starts_with_ignore_case("hello", ""));
}

TEST(empty_prefix_matches_empty_value) {
  EXPECT_TRUE(sourcemeta::core::starts_with_ignore_case("", ""));
}

TEST(empty_value_with_non_empty_prefix) {
  EXPECT_FALSE(sourcemeta::core::starts_with_ignore_case("", "a"));
}

TEST(value_equal_to_prefix) {
  EXPECT_TRUE(sourcemeta::core::starts_with_ignore_case("Token", "token"));
}

TEST(non_alphabetic_characters_match) {
  EXPECT_TRUE(sourcemeta::core::starts_with_ignore_case("__x-1.2", "__X-1.2"));
}

TEST(digits_are_not_case_folded) {
  EXPECT_FALSE(sourcemeta::core::starts_with_ignore_case("1abc", "2"));
}

TEST(non_ascii_bytes_are_not_folded) {
  EXPECT_FALSE(sourcemeta::core::starts_with_ignore_case("\xc0z", "\xe0"));
}
