#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string>      // std::wstring
#include <string_view> // std::string_view

TEST(empty_string) { EXPECT_EQ(sourcemeta::core::utf8_to_wide(""), L""); }

TEST(single_ascii_character) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("a"), L"a");
}

TEST(ascii_word) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("hello"), L"hello");
}

TEST(ascii_sentence_with_punctuation) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("Hello, World! 123"),
            L"Hello, World! 123");
}

TEST(ascii_url) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("https://example.com/foo?bar=baz"),
            L"https://example.com/foo?bar=baz");
}

TEST(two_byte_latin_small_letter_e_with_acute) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xc3\xa9"), L"\x00e9");
}

TEST(two_byte_word) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("caf\xc3\xa9"), L"caf\x00e9");
}

TEST(two_byte_greek_word) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xce\xb1\xce\xb2\xce\xb3"),
            L"\x03b1\x03b2\x03b3");
}

TEST(two_byte_cyrillic_word) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide(
                "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82"),
            L"\x041f\x0440\x0438\x0432\x0435\x0442");
}

TEST(three_byte_euro_sign) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xe2\x82\xac"), L"\x20ac");
}

TEST(three_byte_cjk_word) {
  EXPECT_EQ(
      sourcemeta::core::utf8_to_wide("\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e"),
      L"\x65e5\x672c\x8a9e");
}

TEST(four_byte_grinning_face_emoji) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xf0\x9f\x98\x80"), L"\U0001f600");
}

TEST(four_byte_musical_symbol_g_clef) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xf0\x9d\x84\x9e"), L"\U0001d11e");
}

TEST(consecutive_four_byte_code_points) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xf0\x9f\x98\x80\xf0\x9f\x98\x81"),
            L"\U0001f600\U0001f601");
}

TEST(mixed_sequence_lengths) {
  EXPECT_EQ(
      sourcemeta::core::utf8_to_wide("a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80"),
      L"a\x00e9\x20ac\U0001f600");
}

TEST(multibyte_at_start) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xe2\x82\xacxyz"), L"\x20acxyz");
}

TEST(multibyte_at_end) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("xyz\xe2\x82\xac"), L"xyz\x20ac");
}

TEST(boundary_last_one_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\x7f"), L"\x007f");
}

TEST(boundary_first_two_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xc2\x80"), L"\x0080");
}

TEST(boundary_last_two_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xdf\xbf"), L"\x07ff");
}

TEST(boundary_first_three_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xe0\xa0\x80"), L"\x0800");
}

TEST(boundary_last_three_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xef\xbf\xbf"), L"\xffff");
}

TEST(boundary_first_four_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xf0\x90\x80\x80"), L"\U00010000");
}

TEST(boundary_last_four_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide("\xf4\x8f\xbf\xbf"), L"\U0010ffff");
}

TEST(embedded_null_byte) {
  const std::string_view input{"a\0b", 3};
  const std::wstring expected{L"a\0b", 3};
  EXPECT_EQ(sourcemeta::core::utf8_to_wide(input), expected);
}
