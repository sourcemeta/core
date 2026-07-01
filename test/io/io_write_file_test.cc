#include <sourcemeta/core/test.h>

#include <sourcemeta/core/io.h>

#include <array>        // std::array
#include <cstddef>      // std::byte
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream, std::istreambuf_iterator
#include <ios>          // std::ios::binary
#include <ostream>      // std::ostream
#include <span>         // std::span
#include <stdexcept>    // std::runtime_error
#include <string>       // std::string
#include <system_error> // std::error_code

class IOWriteFileTest {
protected:
  IOWriteFileTest() { std::filesystem::create_directories(this->workspace); }

  ~IOWriteFileTest() {
    std::error_code error;
    std::filesystem::remove_all(this->workspace, error);
  }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace{std::filesystem::path{BUILD_DIRECTORY} /
                                  "sourcemeta_core_io_write_test"};
};

TEST_F(IOWriteFileTest, string_view_creates_file) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, "hello");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "hello");
}

TEST_F(IOWriteFileTest, empty_contents) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, "");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST_F(IOWriteFileTest, byte_span_creates_file) {
  const auto path{this->workspace / "out.bin"};
  constexpr std::array<std::byte, 5> bytes{{std::byte{0x48}, std::byte{0x65},
                                            std::byte{0x6c}, std::byte{0x6c},
                                            std::byte{0x6f}}};
  sourcemeta::core::write_file(path, std::span<const std::byte>{bytes});
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "Hello");
}

TEST_F(IOWriteFileTest, callback_variant) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, [](std::ostream &stream) -> void {
    stream << "first";
    stream << " ";
    stream << "second";
  });
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "first second");
}

TEST_F(IOWriteFileTest, creates_parent_directories) {
  const auto path{this->workspace / "a" / "b" / "c" / "deep.txt"};
  sourcemeta::core::write_file(path, "nested");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "nested");
}

TEST_F(IOWriteFileTest, overwrites_existing_file) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, "original");
  sourcemeta::core::write_file(path, "replacement");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "replacement");
}

TEST_F(IOWriteFileTest, binary_writes_preserve_exact_bytes) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, "line one\nline two\n");

  std::ifstream stream{path, std::ios::binary};
  std::string actual;
  actual.assign(std::istreambuf_iterator<char>{stream},
                std::istreambuf_iterator<char>{});
  EXPECT_EQ(actual, "line one\nline two\n");
}

TEST_F(IOWriteFileTest, no_staging_file_created) {
  const auto path{this->workspace / "out.txt"};
  auto staging{path};
  staging += ".tmp";

  sourcemeta::core::write_file(path, "done");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_FALSE(std::filesystem::exists(staging));
}

TEST_F(IOWriteFileTest, exception_in_callback_leaves_partial_file) {
  const auto path{this->workspace / "out.txt"};

  try {
    sourcemeta::core::write_file(path, [&path](std::ostream &stream) -> void {
      stream << "partial";
      if (!path.empty()) {
        throw std::runtime_error{"boom"};
      }
    });
    FAIL();
  } catch (const std::runtime_error &) {
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "partial");
  } catch (...) {
    FAIL();
  }
}

TEST_F(IOWriteFileTest, exception_in_callback_truncates_existing_file) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::write_file(path, "original");

  try {
    sourcemeta::core::write_file(path, [&path](std::ostream &) -> void {
      if (!path.empty()) {
        throw std::runtime_error{"boom"};
      }
    });
    FAIL();
  } catch (const std::runtime_error &) {
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_EQ(std::filesystem::file_size(path), 0);
  } catch (...) {
    FAIL();
  }
}

// POSIX permission bits don't map cleanly to Windows ACLs
#if !defined(_WIN32)
TEST_F(IOWriteFileTest, read_only_parent_throws_permission_error) {
  const auto read_only_dir{this->workspace / "locked"};
  std::filesystem::create_directory(read_only_dir);
  std::filesystem::permissions(read_only_dir,
                               std::filesystem::perms::owner_read |
                                   std::filesystem::perms::owner_exec,
                               std::filesystem::perm_options::replace);
  const auto path{read_only_dir / "out.txt"};

  try {
    sourcemeta::core::write_file(path, "should fail");
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}
#endif
