#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem> // std::filesystem
#include <ios>        // std::streampos
#include <sstream>    // std::istringstream

TEST(from_the_beginning) {
  std::istringstream stream{"foobar"};
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 3);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), 'b');
}

TEST(zero_characters_stays_at_the_start) {
  std::istringstream stream{"foobar"};
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 0);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), 'f');
}

TEST(from_a_later_position) {
  std::istringstream stream{"foobar"};
  stream.ignore(3);
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 2);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), 'r');
}

TEST(up_to_the_end) {
  std::istringstream stream{"foobar"};
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 6);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), std::char_traits<char>::eof());
}

TEST(beyond_the_end) {
  std::istringstream stream{"foobar"};
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 100);
  EXPECT_EQ(stream.peek(), std::char_traits<char>::eof());
}

TEST(after_the_stream_was_drained) {
  std::istringstream stream{"foobar"};
  const auto start{stream.tellg()};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "foobar");
  sourcemeta::core::resume_stream(stream, start, 3);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), 'b');
}

TEST(without_a_position_leaves_the_stream_untouched) {
  std::istringstream stream{"foobar"};
  stream.ignore(3);
  sourcemeta::core::resume_stream(stream, static_cast<std::streampos>(-1), 2);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), 'b');
}

TEST(file_stream) {
  auto stream{sourcemeta::core::read_file(
      std::filesystem::path{STUBS_DIRECTORY} / "test.txt")};
  const auto start{stream.tellg()};
  sourcemeta::core::resume_stream(stream, start, 5);
  EXPECT_TRUE(stream.good());
  EXPECT_EQ(stream.peek(), ' ');
}
