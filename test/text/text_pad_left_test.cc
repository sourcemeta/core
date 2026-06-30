#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(pads_to_width) {
  EXPECT_EQ(sourcemeta::core::pad_left("42", 5, '0'), "00042");
}

TEST(already_at_width) {
  EXPECT_EQ(sourcemeta::core::pad_left("hello", 5, '0'), "hello");
}

TEST(longer_than_width) {
  EXPECT_EQ(sourcemeta::core::pad_left("hello", 3, '0'), "hello");
}

TEST(single_character_pad) {
  EXPECT_EQ(sourcemeta::core::pad_left("x", 4, ' '), "   x");
}

TEST(empty_input) { EXPECT_EQ(sourcemeta::core::pad_left("", 3, '0'), "000"); }

TEST(zero_width) {
  EXPECT_EQ(sourcemeta::core::pad_left("abc", 0, '0'), "abc");
}

TEST(pads_with_nul_bytes) {
  const std::string expected("\x00\x00\x2a", 3);
  EXPECT_EQ(sourcemeta::core::pad_left("\x2a", 3, '\x00'), expected);
}
