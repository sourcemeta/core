#include <gtest/gtest.h>

#include <sourcemeta/core/md5.h>

#include <sstream>
#include <string>

// Try $ echo -n "foo bar" | md5sum
TEST(MD5, example_string) {
  std::ostringstream result;
  sourcemeta::core::md5("foo bar", result);
  EXPECT_EQ(result.str(), "327b6f07435811239bc47e1544353273");
}

// Try $ echo -n "" | md5sum
TEST(MD5, empty_string) {
  std::ostringstream result;
  sourcemeta::core::md5("", result);
  EXPECT_EQ(result.str(), "d41d8cd98f00b204e9800998ecf8427e");
}

TEST(MD5, abc_string) {
  std::ostringstream result;
  sourcemeta::core::md5("abc", result);
  EXPECT_EQ(result.str(), "900150983cd24fb0d6963f7d28e17f72");
}

TEST(MD5, quick_brown_fox) {
  std::ostringstream result;
  sourcemeta::core::md5("The quick brown fox jumps over the lazy dog", result);
  EXPECT_EQ(result.str(), "9e107d9d372bb6826bd81d3542a419d6");
}

TEST(MD5, quick_brown_fox_with_period) {
  std::ostringstream result;
  sourcemeta::core::md5("The quick brown fox jumps over the lazy dog.", result);
  EXPECT_EQ(result.str(), "e4d909c290d0fb1ca068ffaddf22cbd0");
}

TEST(MD5, one_million_a_chars) {
  const std::string input(1000000, 'a');
  std::ostringstream result;
  sourcemeta::core::md5(input, result);
  EXPECT_EQ(result.str(), "7707d6ae4e027c70eea2a935c2296f21");
}

TEST(MD5, embedded_nuls_and_binary_bytes) {
  // Contains NUL and other small bytes
  const std::string input("\x00\x01\x02", 3);
  std::ostringstream result;
  sourcemeta::core::md5(input, result);
  EXPECT_EQ(result.str(), "b95f67f61ebb03619622d798f45fc2d3");
}
