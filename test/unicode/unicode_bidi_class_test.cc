#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(null) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x0000),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(ascii_letter) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'A'),
            sourcemeta::core::BidiClass::LeftToRight);
}

TEST(ascii_digit) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'0'),
            sourcemeta::core::BidiClass::EuropeanNumber);
}

TEST(tab) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x0009),
            sourcemeta::core::BidiClass::SegmentSeparator);
}

TEST(space) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U' '),
            sourcemeta::core::BidiClass::WhiteSpace);
}

TEST(dollar_sign) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'$'),
            sourcemeta::core::BidiClass::EuropeanTerminator);
}

TEST(plus_sign) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'+'),
            sourcemeta::core::BidiClass::EuropeanSeparator);
}

TEST(comma) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U','),
            sourcemeta::core::BidiClass::CommonSeparator);
}

TEST(combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0301'),
            sourcemeta::core::BidiClass::NonspacingMark);
}

TEST(hebrew_letter_alef) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u05D0'),
            sourcemeta::core::BidiClass::RightToLeft);
}

TEST(arabic_letter_alef) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0627'),
            sourcemeta::core::BidiClass::ArabicLetter);
}

TEST(arabic_indic_digit_zero) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u0660'),
            sourcemeta::core::BidiClass::ArabicNumber);
}

TEST(zero_width_joiner) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u200D'),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(zero_width_non_joiner) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u200C'),
            sourcemeta::core::BidiClass::BoundaryNeutral);
}

TEST(paragraph_separator) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u2029'),
            sourcemeta::core::BidiClass::ParagraphSeparator);
}

TEST(left_to_right_embedding) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u202A'),
            sourcemeta::core::BidiClass::LeftToRightEmbedding);
}

TEST(left_to_right_isolate) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\u2066'),
            sourcemeta::core::BidiClass::LeftToRightIsolate);
}

TEST(emoji_grinning_face) {
  EXPECT_EQ(sourcemeta::core::bidi_class(U'\U0001F600'),
            sourcemeta::core::BidiClass::OtherNeutral);
}

TEST(unassigned_in_hebrew_block) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x05FF),
            sourcemeta::core::BidiClass::RightToLeft);
}

TEST(unassigned_in_arabic_block) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x07BF),
            sourcemeta::core::BidiClass::ArabicLetter);
}

TEST(above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::bidi_class(0x110000),
            sourcemeta::core::BidiClass::LeftToRight);
}
