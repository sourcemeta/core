#include <sourcemeta/core/test.h>

#include <sourcemeta/core/io.h>

#include <algorithm>    // std::remove
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ofstream
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <system_error> // std::error_code

#if !defined(_WIN32)
#include <sys/stat.h> // mkfifo, S_IRUSR, S_IWUSR
#include <thread>     // std::thread
#endif

TEST(empty_stream) {
  std::istringstream stream{""};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "");
}

TEST(single_line) {
  std::istringstream stream{"hello"};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), "hello");
}

TEST(multiple_lines) {
  std::istringstream stream{"line one\nline two\nline three"};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream),
            "line one\nline two\nline three");
}

TEST(large_input) {
  const std::string payload(8192, 'a');
  std::istringstream stream{payload};
  EXPECT_EQ(sourcemeta::core::read_to_string(stream), payload);
}

TEST(leaves_stream_at_eof) {
  std::istringstream stream{"hello"};
  sourcemeta::core::read_to_string(stream);
  EXPECT_EQ(stream.peek(), std::char_traits<char>::eof());
}

TEST(stub_text_file) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "test.txt"};
  auto contents{sourcemeta::core::read_file_to_string(path)};
  contents.erase(std::remove(contents.begin(), contents.end(), '\r'),
                 contents.end());
  EXPECT_EQ(contents, "Hello World\n");
}

TEST(directory) {
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

TEST(missing_file_throws) {
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

class IOReadFileToStringTest {
protected:
  IOReadFileToStringTest() {
    std::filesystem::create_directories(this->workspace);
  }

  ~IOReadFileToStringTest() {
    std::error_code error;
    std::filesystem::remove_all(this->workspace, error);
  }

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

#if !defined(_WIN32)
TEST_F(IOReadFileToStringTest, fifo) {
  struct ThreadJoiner {
    std::thread &thread;
    ~ThreadJoiner() {
      if (thread.joinable()) {
        thread.join();
      }
    }
  };

  const auto path{this->workspace / "fifo"};
  std::filesystem::remove(path);
  EXPECT_EQ(::mkfifo(path.c_str(), S_IRUSR | S_IWUSR), 0);
  std::thread writer{[&path]() {
    std::ofstream stream{path};
    stream << "piped payload\nsecond line\n";
  }};
  ThreadJoiner joiner{writer};
  auto contents{sourcemeta::core::read_file_to_string(path)};
  contents.erase(std::remove(contents.begin(), contents.end(), '\r'),
                 contents.end());
  EXPECT_EQ(contents, "piped payload\nsecond line\n");
}
#endif
