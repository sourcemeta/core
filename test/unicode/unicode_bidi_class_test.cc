#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_bidi_class, null) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x0000),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(Unicode_bidi_class, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'A'),
            sourcemeta::core::BidiClass::LeftToRight);
}

TEST(Unicode_bidi_class, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'0'),
            sourcemeta::core::BidiClass::EuropeanNumber);
}

TEST(Unicode_bidi_class, tab) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x0009),
            sourcemeta::core::BidiClass::SegmentSeparator);
}

TEST(Unicode_bidi_class, space) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U' '),
            sourcemeta::core::BidiClass::WhiteSpace);
}

TEST(Unicode_bidi_class, dollar_sign) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'$'),
            sourcemeta::core::BidiClass::EuropeanTerminator);
}

TEST(Unicode_bidi_class, plus_sign) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'+'),
            sourcemeta::core::BidiClass::EuropeanSeparator);
}

TEST(Unicode_bidi_class, comma) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U','),
            sourcemeta::core::BidiClass::CommonSeparator);
}

TEST(Unicode_bidi_class, combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0301'),
            sourcemeta::core::BidiClass::NonspacingMark);
}

TEST(Unicode_bidi_class, hebrew_letter_alef) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u05D0'),
            sourcemeta::core::BidiClass::RightToLeft);
}

TEST(Unicode_bidi_class, arabic_letter_alef) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0627'),
            sourcemeta::core::BidiClass::ArabicLetter);
}

TEST(Unicode_bidi_class, arabic_indic_digit_zero) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0660'),
            sourcemeta::core::BidiClass::ArabicNumber);
}

TEST(Unicode_bidi_class, zero_width_joiner) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u200D'),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(Unicode_bidi_class, zero_width_non_joiner) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u200C'),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(Unicode_bidi_class, paragraph_separator) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u2029'),
            sourcemeta::core::BidiClass::ParagraphSeparator);
}

TEST(Unicode_bidi_class, left_to_right_embedding) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u202A'),
            sourcemeta::core::BidiClass::LeftToRightEmbedding);
}

TEST(Unicode_bidi_class, left_to_right_isolate) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u2066'),
            sourcemeta::core::BidiClass::LeftToRightIsolate);
}

TEST(Unicode_bidi_class, emoji_grinning_face) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\U0001F600'),
            sourcemeta::core::BidiClass::OtherNeutral);
}

TEST(Unicode_bidi_class, unassigned_in_hebrew_block) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x05FF),
            sourcemeta::core::BidiClass::RightToLeft);
}

TEST(Unicode_bidi_class, unassigned_in_arabic_block) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x07BF),
            sourcemeta::core::BidiClass::ArabicLetter);
}

TEST(Unicode_bidi_class, above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x110000),
            sourcemeta::core::BidiClass::LeftToRight);
}
