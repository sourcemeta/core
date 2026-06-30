#include <sourcemeta/core/test.h>

#include <sourcemeta/core/unicode.h>

TEST(null) {
  EXPECT_EQ(sourcemeta::core::script(0x0000),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(ascii_letter) {
  EXPECT_EQ(sourcemeta::core::script(U'A'),
            sourcemeta::core::UnicodeScript::Latin);
}

TEST(ascii_digit) {
  EXPECT_EQ(sourcemeta::core::script(U'0'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(space) {
  EXPECT_EQ(sourcemeta::core::script(U' '),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(greek_capital_alpha) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0391'),
            sourcemeta::core::UnicodeScript::Greek);
}

TEST(greek_lower_numeral_sign) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0375'),
            sourcemeta::core::UnicodeScript::Greek);
}

TEST(hebrew_letter_alef) {
  EXPECT_EQ(sourcemeta::core::script(U'\u05D0'),
            sourcemeta::core::UnicodeScript::Hebrew);
}

TEST(arabic_letter_alef) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0627'),
            sourcemeta::core::UnicodeScript::Arabic);
}

TEST(hiragana_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u3042'),
            sourcemeta::core::UnicodeScript::Hiragana);
}

TEST(katakana_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u30A2'),
            sourcemeta::core::UnicodeScript::Katakana);
}

TEST(katakana_middle_dot) {
  EXPECT_EQ(sourcemeta::core::script(U'\u30FB'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(han_ideograph) {
  EXPECT_EQ(sourcemeta::core::script(U'\u6F22'),
            sourcemeta::core::UnicodeScript::Han);
}

TEST(devanagari_letter_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0905'),
            sourcemeta::core::UnicodeScript::Devanagari);
}

TEST(cyrillic_letter_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0410'),
            sourcemeta::core::UnicodeScript::Cyrillic);
}

TEST(combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0301'),
            sourcemeta::core::UnicodeScript::Inherited);
}

TEST(emoji_grinning_face) {
  EXPECT_EQ(sourcemeta::core::script(U'\U0001F600'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(unassigned_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x0E80),
            sourcemeta::core::UnicodeScript::Unknown);
}

TEST(max_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x10FFFF),
            sourcemeta::core::UnicodeScript::Unknown);
}

TEST(above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x110000),
            sourcemeta::core::UnicodeScript::Unknown);
}
