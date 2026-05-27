#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

TEST(Text_to_lowercase, uppercase_a) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('A'), 'a');
}

TEST(Text_to_lowercase, uppercase_z) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('Z'), 'z');
}

TEST(Text_to_lowercase, uppercase_middle) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('M'), 'm');
}

TEST(Text_to_lowercase, lowercase_a_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('a'), 'a');
}

TEST(Text_to_lowercase, lowercase_z_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('z'), 'z');
}

TEST(Text_to_lowercase, digit_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('5'), '5');
}

TEST(Text_to_lowercase, punctuation_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('!'), '!');
}

TEST(Text_to_lowercase, space_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(' '), ' ');
}

TEST(Text_to_lowercase, null_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('\0'), '\0');
}

TEST(Text_to_lowercase, just_before_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('@'), '@');
}

TEST(Text_to_lowercase, just_after_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('['), '[');
}

TEST(Text_to_lowercase, just_before_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('`'), '`');
}

TEST(Text_to_lowercase, just_after_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('{'), '{');
}

TEST(Text_to_lowercase, non_ascii_byte_unchanged) {
  const char character{static_cast<char>(0xC3)};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
}

TEST(Text_to_lowercase, high_bit_byte_unchanged) {
  const char character{static_cast<char>(0xFF)};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
}
