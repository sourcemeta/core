#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string_view> // std::string_view

TEST(identical) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("hello", "hello"));
}

TEST(differing_case) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("Hello", "hELLO"));
}

TEST(all_uppercase_against_all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("AT+JWT", "at+jwt"));
}

TEST(different_content) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("foo", "bar"));
}

TEST(different_length) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("hello", "hell"));
}

TEST(prefix_is_not_equal) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("hell", "hello"));
}

TEST(both_empty) { EXPECT_TRUE(sourcemeta::core::equals_ignore_case("", "")); }

TEST(one_empty) { EXPECT_FALSE(sourcemeta::core::equals_ignore_case("", "x")); }

TEST(non_alphabetic_characters_match) {
  EXPECT_TRUE(sourcemeta::core::equals_ignore_case("a-1_2.3", "A-1_2.3"));
}

TEST(digits_are_not_case_folded) {
  EXPECT_FALSE(sourcemeta::core::equals_ignore_case("123", "124"));
}
