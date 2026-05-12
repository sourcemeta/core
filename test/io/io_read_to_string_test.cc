#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <algorithm> // std::remove
#include <fstream>   // std::ofstream
#include <sstream>   // std::istringstream
#include <string>    // std::string

TEST(IO_read_to_string, empty_stream) {
  std::istringstream stream{""};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "");
}

TEST(IO_read_to_string, single_line) {
  std::istringstream stream{"hello"};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "hello");
}

TEST(IO_read_to_string, multiple_lines) {
  std::istringstream stream{"line one\nline two\nline three"};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream),
            "line one\nline two\nline three");
}

TEST(IO_read_to_string, large_input) {
  const std::string payload(8192, 'a');
  std::istringstream stream{payload};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), payload);
}

TEST(IO_read_to_string, leaves_stream_at_eof) {
  std::istringstream stream{"hello"};
  sourcemeta::core::read_to_string(stream);
  EXPECT_EQ(stream.peek(), std::char_traits<char>::eof());
}

TEST(IO_read_file_to_string, stub_text_file) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "test.txt"};
  auto contents{sourcemeta::core::read_file_to_string(path)};
  contents.erase(std::remove(contents.begin(), contents.end(), '\r'),
                 contents.end());
  EXPECT_EQ(contents, "Hello World\n");
}

TEST(IO_read_file_to_string, directory) {
  const std::filesystem::path path{STUBS_DIRECTORY};
  try {
    sourcemeta::core::read_file_to_string(path);
    FAIL();
  } catch (const sourcemeta::core::IOIsADirectoryError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

TEST(IO_read_file_to_string, missing_file_throws) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "no-such-file.txt"};
  try {
    sourcemeta::core::read_file_to_string(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

class IOReadFileToStringTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::filesystem::create_directories(this->workspace);
  }

  void TearDown() override { std::filesystem::remove_all(this->workspace); }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace{
      std::filesystem::path{BUILD_DIRECTORY} /
      "sourcemeta_core_io_read_file_to_string_test"};
};

TEST_F(IOReadFileToStringTest, freshly_written_file) {
  const auto path{this->workspace / "payload.txt"};
  {
    std::ofstream stream{path};
    stream << "first line\nsecond line\n";
  }
  auto contents{sourcemeta::core::read_file_to_string(path)};
  contents.erase(std::remove(contents.begin(), contents.end(), '\r'),
                 contents.end());
  EXPECT_EQ(contents, "first line\nsecond line\n");
}

TEST_F(IOReadFileToStringTest, empty_file) {
  const auto path{this->workspace / "empty.txt"};
  std::ofstream{path};
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "");
}
