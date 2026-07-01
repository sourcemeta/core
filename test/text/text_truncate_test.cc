#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(no_op_when_within_limit) {
  std::string value{"hello"};
  sourcemeta::core::truncate(value, 10, "...");
  EXPECT_EQ(value, "hello");
}

TEST(no_op_when_equal_to_limit) {
  std::string value{"hello"};
  sourcemeta::core::truncate(value, 5, "...");
  EXPECT_EQ(value, "hello");
}

TEST(truncates_and_appends_marker) {
  std::string value{"abcdef"};
  sourcemeta::core::truncate(value, 5, "...");
  EXPECT_EQ(value, "abcde...");
}

TEST(truncates_to_single_byte) {
  std::string value{"hello"};
  sourcemeta::core::truncate(value, 1, "...");
  EXPECT_EQ(value, "h...");
}

TEST(truncates_with_zero_limit) {
  std::string value{"hello"};
  sourcemeta::core::truncate(value, 0, "...");
  EXPECT_EQ(value, "...");
}

TEST(empty_input_within_limit) {
  std::string value;
  sourcemeta::core::truncate(value, 10, "...");
  EXPECT_EQ(value, "");
}

TEST(empty_input_zero_limit) {
  std::string value;
  sourcemeta::core::truncate(value, 0, "...");
  EXPECT_EQ(value, "");
}

TEST(empty_marker_just_truncates) {
  std::string value{"hello"};
  sourcemeta::core::truncate(value, 2, "");
  EXPECT_EQ(value, "he");
}

TEST(custom_marker) {
  std::string value{"hello world"};
  sourcemeta::core::truncate(value, 5, " [more]");
  EXPECT_EQ(value, "hello [more]");
}

TEST(unicode_marker) {
  std::string value{"abcdef"};
  sourcemeta::core::truncate(value, 3, "\xe2\x80\xa6");
  EXPECT_EQ(value, "abc\xe2\x80\xa6");
}

TEST(utf8_two_byte_boundary_aware) {
  std::string value{"caf\xc3\xa9"};
  sourcemeta::core::truncate(value, 4, "...");
  EXPECT_EQ(value, "caf...");
}

TEST(utf8_two_byte_no_split_at_exact_boundary) {
  std::string value{"caf\xc3\xa9"};
  sourcemeta::core::truncate(value, 5, "...");
  EXPECT_EQ(value, "caf\xc3\xa9");
}

TEST(utf8_four_byte_emoji_split_inside_rewinds) {
  std::string value{"ab\xf0\x9f\x98\x80"
                    "c"};
  sourcemeta::core::truncate(value, 4, "...");
  EXPECT_EQ(value, "ab...");
}

TEST(utf8_four_byte_emoji_kept_when_boundary_lands_after) {
  std::string value{"ab\xf0\x9f\x98\x80"
                    "c"};
  sourcemeta::core::truncate(value, 6, "...");
  EXPECT_EQ(value, "ab\xf0\x9f\x98\x80...");
}

TEST(utf8_no_split_when_no_truncation) {
  std::string value{"\xc3\xa9\xc3\xa9"};
  sourcemeta::core::truncate(value, 4, "...");
  EXPECT_EQ(value, "\xc3\xa9\xc3\xa9");
}

TEST(ascii_truncation_at_exact_boundary) {
  std::string value{"abcdefghij"};
  sourcemeta::core::truncate(value, 6, "...");
  EXPECT_EQ(value, "abcdef...");
}
