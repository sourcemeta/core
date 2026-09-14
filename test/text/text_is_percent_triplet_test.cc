#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <cstddef> // std::size_t
#include <limits>  // std::numeric_limits

TEST(at_the_start) {
  EXPECT_TRUE(sourcemeta::core::is_percent_triplet("%20", 0));
}

TEST(within_a_string) {
  EXPECT_TRUE(sourcemeta::core::is_percent_triplet("a%20b", 1));
}

TEST(uppercase_digits) {
  EXPECT_TRUE(sourcemeta::core::is_percent_triplet("%2F", 0));
}

TEST(lowercase_digits) {
  EXPECT_TRUE(sourcemeta::core::is_percent_triplet("%2f", 0));
}

TEST(letter_digits) {
  EXPECT_TRUE(sourcemeta::core::is_percent_triplet("%ab", 0));
}

TEST(not_a_percent_sign) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("a20", 0));
}

TEST(one_digit_short) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("%2", 0));
}

TEST(nothing_after_the_percent_sign) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("%", 0));
}

TEST(first_digit_is_not_hexadecimal) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("%g0", 0));
}

TEST(second_digit_is_not_hexadecimal) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("%0g", 0));
}

TEST(truncated_at_the_end_of_the_input) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("a%2", 1));
}

TEST(position_past_the_end) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet("%20", 3));
}

TEST(empty_input) { EXPECT_FALSE(sourcemeta::core::is_percent_triplet("", 0)); }

TEST(the_largest_position_the_type_can_hold) {
  EXPECT_FALSE(sourcemeta::core::is_percent_triplet(
      "%20", std::numeric_limits<std::size_t>::max()));
}
