#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <filesystem>
#include <string>

TEST(lowercase_a) { EXPECT_TRUE(sourcemeta::core::is_lowercase('a')); }

TEST(lowercase_z) { EXPECT_TRUE(sourcemeta::core::is_lowercase('z')); }

TEST(lowercase_middle) { EXPECT_TRUE(sourcemeta::core::is_lowercase('m')); }

TEST(uppercase_a) { EXPECT_FALSE(sourcemeta::core::is_lowercase('A')); }

TEST(uppercase_z) { EXPECT_FALSE(sourcemeta::core::is_lowercase('Z')); }

TEST(uppercase_middle) { EXPECT_FALSE(sourcemeta::core::is_lowercase('M')); }

TEST(digit) { EXPECT_TRUE(sourcemeta::core::is_lowercase('5')); }

TEST(punctuation) { EXPECT_TRUE(sourcemeta::core::is_lowercase('!')); }

TEST(space) { EXPECT_TRUE(sourcemeta::core::is_lowercase(' ')); }

TEST(null) { EXPECT_TRUE(sourcemeta::core::is_lowercase('\0')); }

TEST(just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('@'));
}

TEST(just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('['));
}

TEST(just_before_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('`'));
}

TEST(just_after_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase('{'));
}

TEST(non_ascii_byte) {
  const char character{static_cast<char>(0xC3)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(high_bit_byte) {
  const char character{static_cast<char>(0xFF)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(signed_char_uppercase) {
  const signed char character{'A'};
  EXPECT_FALSE(sourcemeta::core::is_lowercase(character));
}

TEST(signed_char_lowercase) {
  const signed char character{'a'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(signed_char_digit) {
  const signed char character{'5'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(signed_char_high_bit) {
  const signed char character{static_cast<signed char>(-1)};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(unsigned_char_uppercase) {
  const unsigned char character{'A'};
  EXPECT_FALSE(sourcemeta::core::is_lowercase(character));
}

TEST(unsigned_char_lowercase) {
  const unsigned char character{'a'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(unsigned_char_digit) {
  const unsigned char character{'5'};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(unsigned_char_high_bit) {
  const unsigned char character{0xFFu};
  EXPECT_TRUE(sourcemeta::core::is_lowercase(character));
}

TEST(wide_lowercase_a) { EXPECT_TRUE(sourcemeta::core::is_lowercase(L'a')); }

TEST(wide_lowercase_z) { EXPECT_TRUE(sourcemeta::core::is_lowercase(L'z')); }

TEST(wide_lowercase_middle) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'm'));
}

TEST(wide_uppercase_a) { EXPECT_FALSE(sourcemeta::core::is_lowercase(L'A')); }

TEST(wide_uppercase_z) { EXPECT_FALSE(sourcemeta::core::is_lowercase(L'Z')); }

TEST(wide_uppercase_middle) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(L'M'));
}

TEST(wide_digit) { EXPECT_TRUE(sourcemeta::core::is_lowercase(L'5')); }

TEST(wide_punctuation) { EXPECT_TRUE(sourcemeta::core::is_lowercase(L'!')); }

TEST(wide_null) { EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\0')); }

TEST(wide_just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'@'));
}

TEST(wide_just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'['));
}

TEST(wide_just_before_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'`'));
}

TEST(wide_just_after_lowercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'{'));
}

TEST(wide_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\u00C0'));
}

TEST(wide_far_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(L'\u2603'));
}

TEST(empty) { EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{""})); }

TEST(single_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"a"}));
}

TEST(single_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"A"}));
}

TEST(single_digit) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"5"}));
}

TEST(all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"hello"}));
}

TEST(all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"HELLO"}));
}

TEST(leading_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Hello"}));
}

TEST(trailing_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"hellO"}));
}

TEST(middle_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"heLlo"}));
}

TEST(mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Hello WORLD"}));
}

TEST(digits_and_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"abc123xyz"}));
}

TEST(digits_and_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"ABC123xyz"}));
}

TEST(punctuation_only) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"!?,.:;"}));
}

TEST(whitespace_only) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"\t\n\r "}));
}

TEST(lowercase_with_punctuation) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"foo, bar!"}));
}

TEST(uppercase_with_punctuation) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Foo, BAR!"}));
}

TEST(embedded_null_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"ab\0cd", 5}));
}

TEST(embedded_null_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"ab\0CD", 5}));
}

TEST(non_ascii_bytes) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"\xC3"
                                                         "\x84"
                                                         "\xC3"
                                                         "\x96"}));
}

TEST(mixed_ascii_lowercase_and_non_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"foo"
                                                         "\xC3"
                                                         "\x84"
                                                         "bar"}));
}

TEST(mixed_ascii_uppercase_and_non_ascii) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"FOO"
                                                          "\xC3"
                                                          "\x84"
                                                          "bar"}));
}

TEST(string_just_before_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"@@@"}));
}

TEST(string_just_after_uppercase_range) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string{"[[["}));
}

TEST(short_circuits_on_first_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string{"Aaaaaaaaaa"}));
}

TEST(string_view_empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string_view{""}));
}

TEST(string_view_all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::string_view{"hello"}));
}

TEST(string_view_all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string_view{"HELLO"}));
}

TEST(string_view_mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::string_view{"Hello"}));
}

TEST(wstring_empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L""}));
}

TEST(wstring_single_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"a"}));
}

TEST(wstring_single_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"A"}));
}

TEST(wstring_all_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"hello"}));
}

TEST(wstring_all_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"HELLO"}));
}

TEST(wstring_mixed_case) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"Hello"}));
}

TEST(wstring_digits_and_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"abc123xyz"}));
}

TEST(wstring_digits_and_uppercase) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"ABC123xyz"}));
}

TEST(above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"\u00C0\u00C9"}));
}

TEST(mixed_lowercase_and_above_ascii) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::wstring{L"foo\u00C0bar"}));
}

TEST(mixed_uppercase_and_above_ascii) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::wstring{L"FOO\u00C0bar"}));
}

TEST(path_empty) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{""}));
}

TEST(single_lowercase_filename) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{"foo"}));
}

TEST(single_uppercase_filename) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::filesystem::path{"FOO"}));
}

TEST(mixed_case_filename) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(std::filesystem::path{"FooBar"}));
}

TEST(lowercase_extension) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"schema.json"}));
}

TEST(uppercase_extension) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"schema.JSON"}));
}

TEST(mixed_case_extension) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Schema.Json"}));
}

TEST(lowercase_absolute_path) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      std::filesystem::path{"/foo/bar/baz.json"}));
}

TEST(uppercase_in_absolute_path) {
  EXPECT_FALSE(sourcemeta::core::is_lowercase(
      std::filesystem::path{"/Foo/BAR/Baz.JSON"}));
}

TEST(lowercase_relative_path) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"foo/bar/baz"}));
}

TEST(uppercase_in_relative_path) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Foo/BAR/baz"}));
}

TEST(digits_only) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"file123.txt"}));
}

TEST(digits_with_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"FILE123.TXT"}));
}

TEST(dot_segments_lowercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"./foo/../bar"}));
}

TEST(dot_segments_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"./FOO/../BAR"}));
}

TEST(hidden_file_lowercase) {
  EXPECT_TRUE(sourcemeta::core::is_lowercase(std::filesystem::path{".config"}));
}

TEST(hidden_file_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{".CONFIG"}));
}

TEST(multiple_extensions_lowercase) {
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"archive.tar.gz"}));
}

TEST(multiple_extensions_uppercase) {
  EXPECT_FALSE(
      sourcemeta::core::is_lowercase(std::filesystem::path{"Archive.TAR.GZ"}));
}
