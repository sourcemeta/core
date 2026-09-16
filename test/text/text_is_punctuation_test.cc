#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <string> // std::string

TEST(range_boundaries) {
  EXPECT_TRUE(sourcemeta::core::is_punctuation('!'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('/'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation(':'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('@'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('['));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('`'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('{'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('~'));
}

TEST(inside_the_ranges) {
  EXPECT_TRUE(sourcemeta::core::is_punctuation('"'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('-'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation(';'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('\\'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('_'));
  EXPECT_TRUE(sourcemeta::core::is_punctuation('|'));
}

TEST(letters) {
  EXPECT_FALSE(sourcemeta::core::is_punctuation('a'));
  EXPECT_FALSE(sourcemeta::core::is_punctuation('Z'));
}

TEST(digits) {
  EXPECT_FALSE(sourcemeta::core::is_punctuation('0'));
  EXPECT_FALSE(sourcemeta::core::is_punctuation('9'));
}

TEST(space_and_control_characters) {
  EXPECT_FALSE(sourcemeta::core::is_punctuation(' '));
  EXPECT_FALSE(sourcemeta::core::is_punctuation('\t'));
  EXPECT_FALSE(sourcemeta::core::is_punctuation('\x7F'));
}

TEST(non_ascii_bytes) {
  EXPECT_FALSE(sourcemeta::core::is_punctuation(static_cast<char>(0xA1)));
  EXPECT_FALSE(
      sourcemeta::core::is_punctuation(static_cast<unsigned char>(0xA1)));
}

TEST(characters_of_a_string) {
  const std::string text{"a!"};
  EXPECT_FALSE(sourcemeta::core::is_punctuation(text.front()));
  EXPECT_TRUE(sourcemeta::core::is_punctuation(text.back()));
}
