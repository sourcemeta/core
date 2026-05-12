#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <array>      // std::array
#include <cstddef>    // std::byte
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream, std::istreambuf_iterator
#include <ios>        // std::ios::binary
#include <ostream>    // std::ostream
#include <span>       // std::span
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string

class IOAtomicWriteFileTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::filesystem::create_directories(this->workspace);
  }

  void TearDown() override { std::filesystem::remove_all(this->workspace); }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace{std::filesystem::path{BUILD_DIRECTORY} /
                                  "sourcemeta_core_io_atomic_write_test"};
};

TEST_F(IOAtomicWriteFileTest, string_view_creates_file) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "hello");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "hello");
}

TEST_F(IOAtomicWriteFileTest, empty_contents) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST_F(IOAtomicWriteFileTest, byte_span_creates_file) {
  const auto path{this->workspace / "out.bin"};
  constexpr std::array<std::byte, 5> bytes{{std::byte{0x48}, std::byte{0x65},
                                            std::byte{0x6c}, std::byte{0x6c},
                                            std::byte{0x6f}}};
  sourcemeta::core::atomic_write_file(path, std::span<const std::byte>{bytes});
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "Hello");
}

TEST_F(IOAtomicWriteFileTest, callback_variant) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, [](std::ostream &stream) -> void {
    stream << "first";
    stream << " ";
    stream << "second";
  });
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "first second");
}

TEST_F(IOAtomicWriteFileTest, creates_parent_directories) {
  const auto path{this->workspace / "a" / "b" / "c" / "deep.txt"};
  sourcemeta::core::atomic_write_file(path, "nested");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "nested");
}

TEST_F(IOAtomicWriteFileTest, overwrites_existing_file) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "original");
  sourcemeta::core::atomic_write_file(path, "replacement");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "replacement");
}

TEST_F(IOAtomicWriteFileTest,
       exception_in_callback_leaves_destination_untouched) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "original");

  try {
    sourcemeta::core::atomic_write_file(path, [&path](std::ostream &) -> void {
      if (!path.empty()) {
        throw std::runtime_error{"boom"};
      }
    });
    FAIL();
  } catch (const std::runtime_error &) {
    EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "original");
  } catch (...) {
    FAIL();
  }
}

TEST_F(IOAtomicWriteFileTest, exception_in_callback_removes_staging_file) {
  const auto path{this->workspace / "out.txt"};
  auto staging{path};
  staging += ".tmp";

  try {
    sourcemeta::core::atomic_write_file(path, [&path](std::ostream &) -> void {
      if (!path.empty()) {
        throw std::runtime_error{"boom"};
      }
    });
    FAIL();
  } catch (const std::runtime_error &) {
    EXPECT_FALSE(std::filesystem::exists(staging));
    EXPECT_FALSE(std::filesystem::exists(path));
  } catch (...) {
    FAIL();
  }
}

TEST_F(IOAtomicWriteFileTest, no_leftover_staging_on_success) {
  const auto path{this->workspace / "out.txt"};
  auto staging{path};
  staging += ".tmp";

  sourcemeta::core::atomic_write_file(path, "done");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_FALSE(std::filesystem::exists(staging));
}

TEST_F(IOAtomicWriteFileTest, binary_writes_preserve_exact_bytes) {
  const auto path{this->workspace / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "line one\nline two\n");

  std::ifstream stream{path, std::ios::binary};
  std::string actual;
  actual.assign(std::istreambuf_iterator<char>{stream},
                std::istreambuf_iterator<char>{});
  EXPECT_EQ(actual, "line one\nline two\n");
}

#if !defined(_WIN32)
TEST_F(IOAtomicWriteFileTest, read_only_parent_throws_permission_error) {
  const auto read_only_dir{this->workspace / "locked"};
  std::filesystem::create_directory(read_only_dir);
  std::filesystem::permissions(read_only_dir,
                               std::filesystem::perms::owner_read |
                                   std::filesystem::perms::owner_exec,
                               std::filesystem::perm_options::replace);
  const auto path{read_only_dir / "out.txt"};

  try {
    sourcemeta::core::atomic_write_file(path, "should fail");
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}
#endif
