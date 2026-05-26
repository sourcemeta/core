#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

TEST(Unicode_script, null) {
  EXPECT_EQ(sourcemeta::core::script(0x0000),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(Unicode_script, ascii_letter) {
  EXPECT_EQ(sourcemeta::core::script(U'A'),
            sourcemeta::core::UnicodeScript::Latin);
}

TEST(Unicode_script, ascii_digit) {
  EXPECT_EQ(sourcemeta::core::script(U'0'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(Unicode_script, space) {
  EXPECT_EQ(sourcemeta::core::script(U' '),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(Unicode_script, greek_capital_alpha) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0391'),
            sourcemeta::core::UnicodeScript::Greek);
}

TEST(Unicode_script, greek_lower_numeral_sign) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0375'),
            sourcemeta::core::UnicodeScript::Greek);
}

TEST(Unicode_script, hebrew_letter_alef) {
  EXPECT_EQ(sourcemeta::core::script(U'\u05D0'),
            sourcemeta::core::UnicodeScript::Hebrew);
}

TEST(Unicode_script, arabic_letter_alef) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0627'),
            sourcemeta::core::UnicodeScript::Arabic);
}

TEST(Unicode_script, hiragana_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u3042'),
            sourcemeta::core::UnicodeScript::Hiragana);
}

TEST(Unicode_script, katakana_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u30A2'),
            sourcemeta::core::UnicodeScript::Katakana);
}

TEST(Unicode_script, katakana_middle_dot) {
  EXPECT_EQ(sourcemeta::core::script(U'\u30FB'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(Unicode_script, han_ideograph) {
  EXPECT_EQ(sourcemeta::core::script(U'\u6F22'),
            sourcemeta::core::UnicodeScript::Han);
}

TEST(Unicode_script, devanagari_letter_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0905'),
            sourcemeta::core::UnicodeScript::Devanagari);
}

TEST(Unicode_script, cyrillic_letter_a) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0410'),
            sourcemeta::core::UnicodeScript::Cyrillic);
}

TEST(Unicode_script, combining_acute_accent) {
  EXPECT_EQ(sourcemeta::core::script(U'\u0301'),
            sourcemeta::core::UnicodeScript::Inherited);
}

TEST(Unicode_script, emoji_grinning_face) {
  EXPECT_EQ(sourcemeta::core::script(U'\U0001F600'),
            sourcemeta::core::UnicodeScript::Common);
}

TEST(Unicode_script, unassigned_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x0E80),
            sourcemeta::core::UnicodeScript::Unknown);
}

TEST(Unicode_script, max_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x10FFFF),
            sourcemeta::core::UnicodeScript::Unknown);
}

TEST(Unicode_script, above_max_codepoint) {
  EXPECT_EQ(sourcemeta::core::script(0x110000),
            sourcemeta::core::UnicodeScript::Unknown);
}
