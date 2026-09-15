#include <sourcemeta/core/html.h>
#include <sourcemeta/core/test.h>

#include <sstream> // std::ostringstream
#include <string>  // std::string

TEST(empty) {
  sourcemeta::core::HTMLBuffer buffer;
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.str(), "");
}

TEST(append_character) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append('a');
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_EQ(buffer.back(), 'a');
  EXPECT_EQ(buffer.str(), "a");
}

TEST(append_string) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer.back(), 'c');
  EXPECT_EQ(buffer.str(), "abc");
}

TEST(append_empty_string) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("");
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.str(), "");
}

TEST(grows_past_initial_capacity) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append(std::string(3000, 'x'));
  buffer.append('y');
  EXPECT_EQ(buffer.size(), 3001u);
  EXPECT_EQ(buffer.str(), std::string(3000, 'x') + "y");
}

TEST(reserve_keeps_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.reserve(4096);
  buffer.append("def");
  EXPECT_EQ(buffer.str(), "abcdef");
}

TEST(reserve_smaller_than_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abcdef");
  buffer.reserve(2);
  EXPECT_EQ(buffer.str(), "abcdef");
}

TEST(append_after_str) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  EXPECT_EQ(buffer.str(), "abc");
  buffer.append("def");
  EXPECT_EQ(buffer.size(), 6u);
  EXPECT_EQ(buffer.str(), "abcdef");
}

TEST(take_moves_contents_out) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  const auto result{buffer.take()};
  EXPECT_EQ(result, "abc");
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.str(), "");
}

TEST(append_after_take) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  const auto result{buffer.take()};
  buffer.append("def");
  EXPECT_EQ(result, "abc");
  EXPECT_EQ(buffer.str(), "def");
}

TEST(write_to_stream) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  std::ostringstream output;
  buffer.write(output);
  EXPECT_EQ(output.str(), "abc");
}

TEST(write_to_stream_after_str) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  EXPECT_EQ(buffer.str(), "abc");
  std::ostringstream output;
  buffer.write(output);
  EXPECT_EQ(output.str(), "abc");
}

TEST(remove_suffix) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abcdef");
  buffer.remove_suffix(2);
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(buffer.str(), "abcd");
}

TEST(append_after_remove_suffix) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc>");
  buffer.remove_suffix(1);
  buffer.append(" x>");
  EXPECT_EQ(buffer.str(), "abc x>");
}

TEST(remove_whole_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.remove_suffix(3);
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.str(), "");
}

TEST(append_unchecked_after_reserve_additional) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.reserve_additional(3000);
  buffer.append_unchecked(std::string(3000, 'x'));
  EXPECT_EQ(buffer.size(), 3003u);
  EXPECT_EQ(buffer.str(), "abc" + std::string(3000, 'x'));
}

TEST(append_unchecked_empty_string) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.reserve_additional(0);
  buffer.append_unchecked("");
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.str(), "");
}

TEST(reserve_additional_keeps_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.reserve_additional(4096);
  buffer.append_unchecked("def");
  EXPECT_EQ(buffer.str(), "abcdef");
}

TEST(view_of_empty_buffer) {
  sourcemeta::core::HTMLBuffer buffer;
  EXPECT_EQ(buffer.view(), "");
}

TEST(view_of_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  EXPECT_EQ(buffer.view(), "abc");
  EXPECT_EQ(buffer.size(), 3u);
}

TEST(clear_discards_contents) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.clear();
  EXPECT_EQ(buffer.size(), 0u);
  EXPECT_EQ(buffer.view(), "");
}

TEST(append_after_clear) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.append("abc");
  buffer.clear();
  buffer.append("de");
  EXPECT_EQ(buffer.view(), "de");
  EXPECT_EQ(buffer.str(), "de");
}
