#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <filesystem>
#include <memory>
#include <string>

TEST(Text_to_lowercase, uppercase_a) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('A'), 'a');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('A')));
}

TEST(Text_to_lowercase, uppercase_z) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('Z'), 'z');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('Z')));
}

TEST(Text_to_lowercase, uppercase_middle) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('M'), 'm');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('M')));
}

TEST(Text_to_lowercase, lowercase_a_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('a'), 'a');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('a')));
}

TEST(Text_to_lowercase, lowercase_z_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('z'), 'z');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('z')));
}

TEST(Text_to_lowercase, digit_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('5'), '5');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('5')));
}

TEST(Text_to_lowercase, punctuation_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('!'), '!');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('!')));
}

TEST(Text_to_lowercase, space_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(' '), ' ');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(' ')));
}

TEST(Text_to_lowercase, null_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('\0'), '\0');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('\0')));
}

TEST(Text_to_lowercase, just_before_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('@'), '@');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('@')));
}

TEST(Text_to_lowercase, just_after_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('['), '[');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('[')));
}

TEST(Text_to_lowercase, just_before_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('`'), '`');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('`')));
}

TEST(Text_to_lowercase, just_after_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase('{'), '{');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase('{')));
}

TEST(Text_to_lowercase, non_ascii_byte_unchanged) {
  const char character{static_cast<char>(0xC3)};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, high_bit_byte_unchanged) {
  const char character{static_cast<char>(0xFF)};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, signed_char_uppercase) {
  const signed char character{'A'};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character),
            static_cast<signed char>('a'));
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, signed_char_digit_unchanged) {
  const signed char character{'5'};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, signed_char_high_bit_unchanged) {
  const signed char character{static_cast<signed char>(-1)};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, unsigned_char_uppercase) {
  const unsigned char character{'A'};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character),
            static_cast<unsigned char>('a'));
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, unsigned_char_digit_unchanged) {
  const unsigned char character{'5'};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, unsigned_char_high_bit_unchanged) {
  const unsigned char character{0xFFu};
  EXPECT_EQ(sourcemeta::core::to_lowercase(character), character);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(character)));
}

TEST(Text_to_lowercase, wide_uppercase_a) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'A'), L'a');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'A')));
}

TEST(Text_to_lowercase, wide_uppercase_z) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'Z'), L'z');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'Z')));
}

TEST(Text_to_lowercase, wide_uppercase_middle) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'M'), L'm');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'M')));
}

TEST(Text_to_lowercase, wide_lowercase_a_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'a'), L'a');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'a')));
}

TEST(Text_to_lowercase, wide_lowercase_z_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'z'), L'z');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'z')));
}

TEST(Text_to_lowercase, wide_digit_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'5'), L'5');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'5')));
}

TEST(Text_to_lowercase, wide_punctuation_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'!'), L'!');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'!')));
}

TEST(Text_to_lowercase, wide_null_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'\0'), L'\0');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'\0')));
}

TEST(Text_to_lowercase, wide_just_before_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'@'), L'@');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'@')));
}

TEST(Text_to_lowercase, wide_just_after_uppercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'['), L'[');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'[')));
}

TEST(Text_to_lowercase, wide_just_before_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'`'), L'`');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'`')));
}

TEST(Text_to_lowercase, wide_just_after_lowercase_range_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'{'), L'{');
  EXPECT_TRUE(
      sourcemeta::core::is_lowercase(sourcemeta::core::to_lowercase(L'{')));
}

TEST(Text_to_lowercase, wide_above_ascii_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'\u00C0'), L'\u00C0');
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(L'\u00C0')));
}

TEST(Text_to_lowercase, wide_far_above_ascii_unchanged) {
  EXPECT_EQ(sourcemeta::core::to_lowercase(L'\u2603'), L'\u2603');
  EXPECT_TRUE(sourcemeta::core::is_lowercase(
      sourcemeta::core::to_lowercase(L'\u2603')));
}

TEST(Text_to_lowercase_string, empty) {
  std::string value{""};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, single_uppercase) {
  std::string value{"A"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "a");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, single_lowercase_unchanged) {
  std::string value{"a"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "a");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, single_digit_unchanged) {
  std::string value{"5"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "5");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, all_uppercase) {
  std::string value{"HELLO"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "hello");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, all_lowercase_unchanged) {
  std::string value{"hello"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "hello");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, mixed_case) {
  std::string value{"Hello WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "hello world");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, digits_and_letters) {
  std::string value{"ABC123xyz"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "abc123xyz");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, punctuation_preserved) {
  std::string value{"Foo, BAR!"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "foo, bar!");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, whitespace_preserved) {
  std::string value{"\tFOO\nBAR\r"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "\tfoo\nbar\r");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, embedded_null_preserved) {
  std::string value{"AB\0CD", 5};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::string("ab\0cd", 5));
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, non_ascii_bytes_unchanged) {
  std::string value{"\xC3"
                    "\x84"
                    "\xC3"
                    "\x96"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "\xC3"
                   "\x84"
                   "\xC3"
                   "\x96");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, mixed_ascii_and_non_ascii) {
  std::string value{"FOO"
                    "\xC3"
                    "\x84"
                    "BAR"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "foo"
                   "\xC3"
                   "\x84"
                   "bar");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_string, size_unchanged) {
  std::string value{"HELLO_WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value.size(), 11);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, empty) {
  std::wstring value{L""};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, single_uppercase) {
  std::wstring value{L"A"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"a");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, single_lowercase_unchanged) {
  std::wstring value{L"a"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"a");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, single_digit_unchanged) {
  std::wstring value{L"5"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"5");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, all_uppercase) {
  std::wstring value{L"HELLO"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"hello");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, all_lowercase_unchanged) {
  std::wstring value{L"hello"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"hello");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, mixed_case) {
  std::wstring value{L"Hello WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"hello world");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, digits_and_letters) {
  std::wstring value{L"ABC123xyz"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"abc123xyz");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, punctuation_preserved) {
  std::wstring value{L"Foo, BAR!"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"foo, bar!");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, whitespace_preserved) {
  std::wstring value{L"\tFOO\nBAR\r"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"\tfoo\nbar\r");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, embedded_null_preserved) {
  std::wstring value{L"AB\0CD", 5};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::wstring(L"ab\0cd", 5));
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, above_ascii_unchanged) {
  std::wstring value{L"\u00C0\u00C9\u00CE"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"\u00C0\u00C9\u00CE");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, mixed_ascii_and_above_ascii) {
  std::wstring value{L"FOO\u00C0BAR"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"foo\u00C0bar");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_wstring, size_unchanged) {
  std::wstring value{L"HELLO_WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value.size(), 11);
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_basic_string, custom_allocator_char) {
  std::basic_string<char, std::char_traits<char>, std::allocator<char>> value{
      "Hello WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, "hello world");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_basic_string, custom_allocator_wchar_t) {
  std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>
      value{L"Hello WORLD"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, L"hello world");
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

// This exercises the conversion over a string of a non-standard character
// type, whose traits some standard libraries now mark deprecated
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
TEST(Text_to_lowercase_basic_string, unsigned_char_string) {
  std::basic_string<unsigned char> value{
      reinterpret_cast<const unsigned char *>("Hello WORLD")};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, reinterpret_cast<const unsigned char *>("hello world"));
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}
#pragma GCC diagnostic pop

TEST(Text_to_lowercase_path, empty) {
  std::filesystem::path value{""};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{""});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, single_uppercase_filename) {
  std::filesystem::path value{"FOO"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"foo"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, single_lowercase_filename_unchanged) {
  std::filesystem::path value{"foo"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"foo"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, mixed_case_filename) {
  std::filesystem::path value{"FooBar"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"foobar"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, uppercase_extension) {
  std::filesystem::path value{"schema.JSON"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"schema.json"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, mixed_case_extension) {
  std::filesystem::path value{"Schema.Json"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"schema.json"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, absolute_path_with_directories) {
  std::filesystem::path value{"/Foo/BAR/Baz.JSON"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"/foo/bar/baz.json"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, relative_path_with_directories) {
  std::filesystem::path value{"Foo/BAR/baz"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"foo/bar/baz"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, separators_preserved) {
  std::filesystem::path value{"/A/B/C"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"/a/b/c"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, digits_preserved) {
  std::filesystem::path value{"FILE123.TXT"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"file123.txt"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, dot_segments_preserved) {
  std::filesystem::path value{"./FOO/../BAR"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"./foo/../bar"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, hidden_file_uppercase) {
  std::filesystem::path value{".CONFIG"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{".config"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, multiple_extensions) {
  std::filesystem::path value{"Archive.TAR.GZ"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"archive.tar.gz"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}

TEST(Text_to_lowercase_path, already_lowercase_unchanged) {
  std::filesystem::path value{"/foo/bar/baz.json"};
  sourcemeta::core::to_lowercase(value);
  EXPECT_EQ(value, std::filesystem::path{"/foo/bar/baz.json"});
  EXPECT_TRUE(sourcemeta::core::is_lowercase(value));
}
