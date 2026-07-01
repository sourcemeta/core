#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <sstream>     // std::istringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(multiple_lines_with_trailing_newline) {
  std::istringstream stream{"alpha\nbeta\ngamma\n"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 3);
  EXPECT_EQ(lines.at(0), "alpha");
  EXPECT_EQ(lines.at(1), "beta");
  EXPECT_EQ(lines.at(2), "gamma");
}

TEST(multiple_lines_no_trailing_newline) {
  std::istringstream stream{"alpha\nbeta\ngamma"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 3);
  EXPECT_EQ(lines.at(2), "gamma");
}

TEST(single_line) {
  std::istringstream stream{"only one line"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 1);
  EXPECT_EQ(lines.at(0), "only one line");
}

TEST(empty_stream_invokes_callback_zero_times) {
  std::istringstream stream{""};
  std::size_t count{0};
  sourcemeta::core::for_each_line(stream,
                                  [&](const std::string_view) { count += 1; });
  EXPECT_EQ(count, 0);
}

TEST(blank_lines_preserved) {
  std::istringstream stream{"\nfoo\n\nbar\n"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 4);
  EXPECT_EQ(lines.at(0), "");
  EXPECT_EQ(lines.at(1), "foo");
  EXPECT_EQ(lines.at(2), "");
  EXPECT_EQ(lines.at(3), "bar");
}

TEST(view_references_per_iteration_buffer) {
  std::istringstream stream{"alpha\nbeta\n"};
  std::vector<std::size_t> sizes;
  sourcemeta::core::for_each_line(stream, [&](const std::string_view line) {
    sizes.push_back(line.size());
  });
  EXPECT_EQ(sizes.size(), 2);
  EXPECT_EQ(sizes.at(0), 5);
  EXPECT_EQ(sizes.at(1), 4);
}

TEST(strips_trailing_carriage_return) {
  std::istringstream stream{"alpha\r\nbeta\r\n"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 2);
  EXPECT_EQ(lines.at(0), "alpha");
  EXPECT_EQ(lines.at(1), "beta");
}

TEST(carriage_return_only_line_becomes_empty) {
  std::istringstream stream{"\r\nfoo\r\n"};
  std::vector<std::string> lines;
  sourcemeta::core::for_each_line(
      stream, [&](const std::string_view line) { lines.emplace_back(line); });
  EXPECT_EQ(lines.size(), 2);
  EXPECT_EQ(lines.at(0), "");
  EXPECT_EQ(lines.at(1), "foo");
}

TEST(only_newlines) {
  std::istringstream stream{"\n\n\n"};
  std::size_t count{0};
  sourcemeta::core::for_each_line(stream, [&](const std::string_view line) {
    EXPECT_TRUE(line.empty());
    count += 1;
  });
  EXPECT_EQ(count, 3);
}
