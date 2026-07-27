#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <string>      // std::string
#include <string_view> // std::string_view

TEST(equal_empty_inputs) {
  EXPECT_TRUE(sourcemeta::core::secure_equals("", ""));
}

TEST(equal_single_byte) {
  EXPECT_TRUE(sourcemeta::core::secure_equals("a", "a"));
}

TEST(equal_multi_byte) {
  EXPECT_TRUE(
      sourcemeta::core::secure_equals("session-token", "session-token"));
}

TEST(different_lengths_left_shorter) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("token", "token-longer"));
}

TEST(different_lengths_right_shorter) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("token-longer", "token"));
}

TEST(empty_against_non_empty) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("", "a"));
}

TEST(non_empty_against_empty) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("a", ""));
}

TEST(same_length_differ_in_first_byte) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("Xoken", "token"));
}

TEST(same_length_differ_in_last_byte) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("tokeX", "token"));
}

TEST(same_length_differ_in_middle_byte) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("toXen", "token"));
}

TEST(same_length_completely_different) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("aaaaa", "bbbbb"));
}

TEST(case_sensitive) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("Token", "token"));
}

TEST(equal_with_embedded_null) {
  const std::string left{"ab\0cd", 5};
  const std::string right{"ab\0cd", 5};
  EXPECT_TRUE(sourcemeta::core::secure_equals(left, right));
}

TEST(differ_only_after_embedded_null) {
  const std::string left{"ab\0cd", 5};
  const std::string right{"ab\0ce", 5};
  EXPECT_FALSE(sourcemeta::core::secure_equals(left, right));
}

TEST(prefix_is_not_equal) {
  EXPECT_FALSE(sourcemeta::core::secure_equals("token", "tok"));
}

TEST(equal_high_bytes) {
  const std::string left(32, '\xff');
  const std::string right(32, '\xff');
  EXPECT_TRUE(sourcemeta::core::secure_equals(left, right));
}

TEST(differ_by_single_bit_high_byte) {
  const std::string left(32, '\xff');
  std::string right(32, '\xff');
  right[16] = '\xfe';
  EXPECT_FALSE(sourcemeta::core::secure_equals(left, right));
}

TEST(equal_full_digest) {
  const std::array<std::uint8_t, 32> digest{
      sourcemeta::core::hmac_sha256_digest("key", "the quick brown fox")};
  const std::string_view left{reinterpret_cast<const char *>(digest.data()),
                              digest.size()};
  EXPECT_TRUE(sourcemeta::core::secure_equals(left, left));
}

TEST(default_constructed_views_are_equal) {
  EXPECT_TRUE(
      sourcemeta::core::secure_equals(std::string_view{}, std::string_view{}));
}

TEST(default_constructed_view_against_empty_literal) {
  EXPECT_TRUE(sourcemeta::core::secure_equals(std::string_view{}, ""));
}
