#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <filesystem>
#include <string>

TEST(Text_is_lowercase, lowercase_a) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('a'));
}

TEST(Text_is_lowercase, lowercase_z) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('z'));
}

TEST(Text_is_lowercase, lowercase_middle) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('m'));
}

TEST(Text_is_lowercase, uppercase_a) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase('A'));
}

TEST(Text_is_lowercase, uppercase_z) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase('Z'));
}

TEST(Text_is_lowercase, uppercase_middle) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase('M'));
}

TEST(Text_is_lowercase, digit) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('5'));
}

TEST(Text_is_lowercase, punctuation) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('!'));
}

TEST(Text_is_lowercase, space) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(' '));
}

TEST(Text_is_lowercase, null) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('\0'));
}

TEST(Text_is_lowercase, just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('@'));
}

TEST(Text_is_lowercase, just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('['));
}

TEST(Text_is_lowercase, just_before_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('`'));
}

TEST(Text_is_lowercase, just_after_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('{'));
}

TEST(Text_is_lowercase, non_ascii_byte) {
  const char character{static_cast<char>(0xC3)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, high_bit_byte) {
  const char character{static_cast<char>(0xFF)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, signed_char_uppercase) {
  const signed char character{'A'};
  EXPECT_FALSE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, signed_char_lowercase) {
  const signed char character{'a'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, signed_char_digit) {
  const signed char character{'5'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, signed_char_high_bit) {
  const signed char character{static_cast<signed char>(-1)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, unsigned_char_uppercase) {
  const unsigned char character{'A'};
  EXPECT_FALSE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, unsigned_char_lowercase) {
  const unsigned char character{'a'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, unsigned_char_digit) {
  const unsigned char character{'5'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, unsigned_char_high_bit) {
  const unsigned char character{0xFFu};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(Text_is_lowercase, wide_lowercase_a) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'a'));
}

TEST(Text_is_lowercase, wide_lowercase_z) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'z'));
}

TEST(Text_is_lowercase, wide_lowercase_middle) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'm'));
}

TEST(Text_is_lowercase, wide_uppercase_a) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(L'A'));
}

TEST(Text_is_lowercase, wide_uppercase_z) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(L'Z'));
}

TEST(Text_is_lowercase, wide_uppercase_middle) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(L'M'));
}

TEST(Text_is_lowercase, wide_digit) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'5'));
}

TEST(Text_is_lowercase, wide_punctuation) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'!'));
}

TEST(Text_is_lowercase, wide_null) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\0'));
}

TEST(Text_is_lowercase, wide_just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'@'));
}

TEST(Text_is_lowercase, wide_just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'['));
}

TEST(Text_is_lowercase, wide_just_before_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'`'));
}

TEST(Text_is_lowercase, wide_just_after_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'{'));
}

TEST(Text_is_lowercase, wide_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\u00C0'));
}

TEST(Text_is_lowercase, wide_far_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\u2603'));
}

TEST(Text_is_lowercase_string, empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{""}));
}

TEST(Text_is_lowercase_string, single_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"a"}));
}

TEST(Text_is_lowercase_string, single_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"A"}));
}

TEST(Text_is_lowercase_string, single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"5"}));
}

TEST(Text_is_lowercase_string, all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"hello"}));
}

TEST(Text_is_lowercase_string, all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"HELLO"}));
}

TEST(Text_is_lowercase_string, leading_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Hello"}));
}

TEST(Text_is_lowercase_string, trailing_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"hellO"}));
}

TEST(Text_is_lowercase_string, middle_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"heLlo"}));
}

TEST(Text_is_lowercase_string, mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Hello WORLD"}));
}

TEST(Text_is_lowercase_string, digits_and_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"abc123xyz"}));
}

TEST(Text_is_lowercase_string, digits_and_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"ABC123xyz"}));
}

TEST(Text_is_lowercase_string, punctuation_only) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"!?,.:;"}));
}

TEST(Text_is_lowercase_string, whitespace_only) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"\t\n\r "}));
}

TEST(Text_is_lowercase_string, lowercase_with_punctuation) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"foo, bar!"}));
}

TEST(Text_is_lowercase_string, uppercase_with_punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Foo, BAR!"}));
}

TEST(Text_is_lowercase_string, embedded_null_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"ab\0cd", 5}));
}

TEST(Text_is_lowercase_string, embedded_null_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"ab\0CD", 5}));
}

TEST(Text_is_lowercase_string, non_ascii_bytes) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"\xC3"
                                                         "\x84"
                                                         "\xC3"
                                                         "\x96"}));
}

TEST(Text_is_lowercase_string, mixed_ascii_lowercase_and_non_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"foo"
                                                         "\xC3"
                                                         "\x84"
                                                         "bar"}));
}

TEST(Text_is_lowercase_string, mixed_ascii_uppercase_and_non_ascii) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"FOO"
                                                          "\xC3"
                                                          "\x84"
                                                          "bar"}));
}

TEST(Text_is_lowercase_string, just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"@@@"}));
}

TEST(Text_is_lowercase_string, just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"[[["}));
}

TEST(Text_is_lowercase_string, short_circuits_on_first_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Aaaaaaaaaa"}));
}

TEST(Text_is_lowercase_string_view, empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string_view{""}));
}

TEST(Text_is_lowercase_string_view, all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string_view{"hello"}));
}

TEST(Text_is_lowercase_string_view, all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string_view{"HELLO"}));
}

TEST(Text_is_lowercase_string_view, mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string_view{"Hello"}));
}

TEST(Text_is_lowercase_wstring, empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L""}));
}

TEST(Text_is_lowercase_wstring, single_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"a"}));
}

TEST(Text_is_lowercase_wstring, single_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"A"}));
}

TEST(Text_is_lowercase_wstring, all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"hello"}));
}

TEST(Text_is_lowercase_wstring, all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"HELLO"}));
}

TEST(Text_is_lowercase_wstring, mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"Hello"}));
}

TEST(Text_is_lowercase_wstring, digits_and_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"abc123xyz"}));
}

TEST(Text_is_lowercase_wstring, digits_and_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"ABC123xyz"}));
}

TEST(Text_is_lowercase_wstring, above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"\u00C0\u00C9"}));
}

TEST(Text_is_lowercase_wstring, mixed_lowercase_and_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"foo\u00C0bar"}));
}

TEST(Text_is_lowercase_wstring, mixed_uppercase_and_above_ascii) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"FOO\u00C0bar"}));
}

TEST(Text_is_lowercase_path, empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{""}));
}

TEST(Text_is_lowercase_path, single_lowercase_filename) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{"foo"}));
}

TEST(Text_is_lowercase_path, single_uppercase_filename) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::filesystem::path{"FOO"}));
}

TEST(Text_is_lowercase_path, mixed_case_filename) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::filesystem::path{"FooBar"}));
}

TEST(Text_is_lowercase_path, lowercase_extension) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"schema.json"}));
}

TEST(Text_is_lowercase_path, uppercase_extension) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"schema.JSON"}));
}

TEST(Text_is_lowercase_path, mixed_case_extension) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Schema.Json"}));
}

TEST(Text_is_lowercase_path, lowercase_absolute_path) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      std::filesystem::path{"/foo/bar/baz.json"}));
}

TEST(Text_is_lowercase_path, uppercase_in_absolute_path) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(
      std::filesystem::path{"/Foo/BAR/Baz.JSON"}));
}

TEST(Text_is_lowercase_path, lowercase_relative_path) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"foo/bar/baz"}));
}

TEST(Text_is_lowercase_path, uppercase_in_relative_path) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Foo/BAR/baz"}));
}

TEST(Text_is_lowercase_path, digits_only) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"file123.txt"}));
}

TEST(Text_is_lowercase_path, digits_with_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"FILE123.TXT"}));
}

TEST(Text_is_lowercase_path, dot_segments_lowercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"./foo/../bar"}));
}

TEST(Text_is_lowercase_path, dot_segments_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"./FOO/../BAR"}));
}

TEST(Text_is_lowercase_path, hidden_file_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{".config"}));
}

TEST(Text_is_lowercase_path, hidden_file_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{".CONFIG"}));
}

TEST(Text_is_lowercase_path, multiple_extensions_lowercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"archive.tar.gz"}));
}

TEST(Text_is_lowercase_path, multiple_extensions_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Archive.TAR.GZ"}));
}
