#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_joining_type, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'A'),
            sourcemeta::core::JoiningType::NonJoining);
}

TEST(Unicode_joining_type, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'0'),
            sourcemeta::core::JoiningType::NonJoining);
}

TEST(Unicode_joining_type, null) {
  EXPECT_EQ(sourcemeta::core::joining_type(0x0000),
            sourcemeta::core::JoiningType::NonJoining);
}

TEST(Unicode_joining_type, arabic_tatweel) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0640'),
            sourcemeta::core::JoiningType::JoinCausing);
}

TEST(Unicode_joining_type, zero_width_joiner) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u200D'),
            sourcemeta::core::JoiningType::JoinCausing);
}

TEST(Unicode_joining_type, zero_width_non_joiner) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u200C'),
            sourcemeta::core::JoiningType::NonJoining);
}

TEST(Unicode_joining_type, arabic_letter_beh) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0628'),
            sourcemeta::core::JoiningType::DualJoining);
}

TEST(Unicode_joining_type, arabic_letter_alef) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0627'),
            sourcemeta::core::JoiningType::RightJoining);
}

TEST(Unicode_joining_type, syriac_letter_alaph) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0710'),
            sourcemeta::core::JoiningType::RightJoining);
}

TEST(Unicode_joining_type, mandaic_letter_az) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0846'),
            sourcemeta::core::JoiningType::RightJoining);
}

TEST(Unicode_joining_type, phags_pa_superfixed_letter_ra) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\uA872'),
            sourcemeta::core::JoiningType::LeftJoining);
}

TEST(Unicode_joining_type, manichaean_letter_aleph) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\U00010AC0'),
            sourcemeta::core::JoiningType::DualJoining);
}

TEST(Unicode_joining_type, combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::joining_type(U'\u0301'),
            sourcemeta::core::JoiningType::Transparent);
}

TEST(Unicode_joining_type, max_codepoint) {
  EXPECT_EQ(sourcemeta::core::joining_type(0x10FFFF),
            sourcemeta::core::JoiningType::NonJoining);
}

TEST(Unicode_joining_type, above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::joining_type(0x110000),
            sourcemeta::core::JoiningType::NonJoining);
}
