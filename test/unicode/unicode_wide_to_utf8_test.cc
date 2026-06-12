#include <gtest/gtest.h>

#include <sourcemeta/core/unicode.h>

#include <string>      // std::string
#include <string_view> // std::wstring_view

TEST(Unicode_wide_to_utf8, empty_string) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L""), "");
}

TEST(Unicode_wide_to_utf8, single_ascii_character) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"a"), "a");
}

TEST(Unicode_wide_to_utf8, ascii_word) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"hello"), "hello");
}

TEST(Unicode_wide_to_utf8, ascii_sentence_with_punctuation) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"Hello, World! 123"),
            "Hello, World! 123");
}

TEST(Unicode_wide_to_utf8, ascii_url) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"https://example.com/foo?bar=baz"),
            "https://example.com/foo?bar=baz");
}

TEST(Unicode_wide_to_utf8, two_byte_latin_small_letter_e_with_acute) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x00e9"), "\xc3\xa9");
}

TEST(Unicode_wide_to_utf8, two_byte_word) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"caf\x00e9"), "caf\xc3\xa9");
}

TEST(Unicode_wide_to_utf8, two_byte_greek_word) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x03b1\x03b2\x03b3"),
            "\xce\xb1\xce\xb2\xce\xb3");
}

TEST(Unicode_wide_to_utf8, two_byte_cyrillic_word) {
  EXPECT_EQ(
      sourcemeta::core::wide_to_utf8(L"\x041f\x0440\x0438\x0432\x0435\x0442"),
      "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
}

TEST(Unicode_wide_to_utf8, three_byte_euro_sign) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x20ac"), "\xe2\x82\xac");
}

TEST(Unicode_wide_to_utf8, three_byte_cjk_word) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x65e5\x672c\x8a9e"),
            "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e");
}

TEST(Unicode_wide_to_utf8, four_byte_grinning_face_emoji) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\U0001f600"), "\xf0\x9f\x98\x80");
}

TEST(Unicode_wide_to_utf8, four_byte_musical_symbol_g_clef) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\U0001d11e"), "\xf0\x9d\x84\x9e");
}

TEST(Unicode_wide_to_utf8, consecutive_four_byte_code_points) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\U0001f600\U0001f601"),
            "\xf0\x9f\x98\x80\xf0\x9f\x98\x81");
}

TEST(Unicode_wide_to_utf8, mixed_sequence_lengths) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"a\x00e9\x20ac\U0001f600"),
            "a\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80");
}

TEST(Unicode_wide_to_utf8, multibyte_at_start) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x20acxyz"), "\xe2\x82\xacxyz");
}

TEST(Unicode_wide_to_utf8, multibyte_at_end) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"xyz\x20ac"), "xyz\xe2\x82\xac");
}

TEST(Unicode_wide_to_utf8, boundary_last_one_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x007f"), "\x7f");
}

TEST(Unicode_wide_to_utf8, boundary_first_two_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x0080"), "\xc2\x80");
}

TEST(Unicode_wide_to_utf8, boundary_last_two_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x07ff"), "\xdf\xbf");
}

TEST(Unicode_wide_to_utf8, boundary_first_three_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\x0800"), "\xe0\xa0\x80");
}

TEST(Unicode_wide_to_utf8, boundary_last_three_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\xffff"), "\xef\xbf\xbf");
}

TEST(Unicode_wide_to_utf8, boundary_first_four_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\U00010000"), "\xf0\x90\x80\x80");
}

TEST(Unicode_wide_to_utf8, boundary_last_four_byte_code_point) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(L"\U0010ffff"), "\xf4\x8f\xbf\xbf");
}

TEST(Unicode_wide_to_utf8, embedded_null_character) {
  const std::wstring_view input{L"a\0b", 3};
  const std::string expected{"a\0b", 3};
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(input), expected);
}

TEST(Unicode_wide_to_utf8, round_trip_from_utf8) {
  EXPECT_EQ(sourcemeta::core::wide_to_utf8(sourcemeta::core::utf8_to_wide(
                "caf\xc3\xa9 \xe2\x82\xac \xf0\x9f\x98\x80")),
            "caf\xc3\xa9 \xe2\x82\xac \xf0\x9f\x98\x80");
}

TEST(Unicode_wide_to_utf8, round_trip_from_wide) {
  EXPECT_EQ(sourcemeta::core::utf8_to_wide(
                sourcemeta::core::wide_to_utf8(L"caf\x00e9 \x20ac \U0001f600")),
            L"caf\x00e9 \x20ac \U0001f600");
}
