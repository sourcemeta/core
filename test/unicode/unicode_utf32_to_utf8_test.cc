#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string> // std::string, std::u32string

TEST(empty_input) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U""), "");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U""), "");
}

TEST(ascii_string) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U"hello"), "hello");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U"hello"), "hello");
}

TEST(two_byte_latin) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U"A\u00E9"), "A\xC3\xA9");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U"A\u00E9"), "A\xC3\xA9");
}

TEST(three_byte_cjk) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U"\u4E16\u754C"),
            "\xE4\xB8\x96\xE7\x95\x8C");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U"\u4E16\u754C"),
            "\xE4\xB8\x96\xE7\x95\x8C");
}

TEST(four_byte_supplementary) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U"\U0001F600"), "\xF0\x9F\x98\x80");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U"\U0001F600"),
            "\xF0\x9F\x98\x80");
}

TEST(mixed_widths) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(U"a\u00E9\u4E16\U0001F600"),
            "a\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(U"a\u00E9\u4E16\U0001F600"),
            "a\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80");
}

TEST(max_codepoint) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(std::u32string{0x10FFFF}),
            "\xF4\x8F\xBF\xBF");
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(std::u32string{0x10FFFF}),
            "\xF4\x8F\xBF\xBF");
}

TEST(round_trips_with_utf8_to_utf32) {
  const std::string original{"a\xC3\xA9\xE4\xB8\x96\xF0\x9F\x98\x80"};
  const auto codepoints{sourcemeta::core::utf8_to_utf32(original)};
  EXPECT_TRUE(codepoints.has_value());
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8(codepoints.value()), original);
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(codepoints.value()),
            original);
}

TEST(lenient_high_surrogate) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(std::u32string{0xD800}),
            "\xED\xA0\x80");
}

TEST(lenient_low_surrogate) {
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(std::u32string{0xDFFF}),
            "\xED\xBF\xBF");
}

TEST(lenient_surrogate_between_ascii) {
  const std::u32string input{U'a', 0xD900, U'b'};
  EXPECT_EQ(sourcemeta::core::utf32_to_utf8_lenient(input),
            "a\xED\xA4\x80\x62");
}
