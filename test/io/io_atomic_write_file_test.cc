#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <array>      // std::array
#include <cstddef>    // std::byte
#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream
#include <ostream>    // std::ostream
#include <span>       // std::span
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string

TEST(IO_atomic_write_file, string_view_creates_file) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "hello");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "hello");
}

TEST(IO_atomic_write_file, empty_contents) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST(IO_atomic_write_file, byte_span_creates_file) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.bin"};

  constexpr std::array<std::byte, 5> bytes{std::byte{0x48}, std::byte{0x65},
                                           std::byte{0x6c}, std::byte{0x6c},
                                           std::byte{0x6f}};
  sourcemeta::core::atomic_write_file(path, std::span<const std::byte>{bytes});

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "Hello");
}

TEST(IO_atomic_write_file, callback_variant) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, [](std::ostream &stream) -> void {
    stream << "first";
    stream << " ";
    stream << "second";
  });

  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "first second");
}

TEST(IO_atomic_write_file, creates_parent_directories) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "a" / "b" / "c" / "deep.txt"};

  sourcemeta::core::atomic_write_file(path, "nested");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "nested");
}

TEST(IO_atomic_write_file, overwrites_existing_file) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "original");
  sourcemeta::core::atomic_write_file(path, "replacement");

  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "replacement");
}

TEST(IO_atomic_write_file, exception_in_callback_leaves_destination_untouched) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "original");

  try {
    sourcemeta::core::atomic_write_file(
        path, [](std::ostream &) -> void { throw std::runtime_error{"boom"}; });
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error &) {
    // expected
  }

  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "original");
}

TEST(IO_atomic_write_file, exception_in_callback_removes_staging_file) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};
  auto staging{path};
  staging += ".tmp";

  try {
    sourcemeta::core::atomic_write_file(
        path, [](std::ostream &) -> void { throw std::runtime_error{"boom"}; });
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error &) {
    // expected
  }

  EXPECT_FALSE(std::filesystem::exists(staging));
  EXPECT_FALSE(std::filesystem::exists(path));
}

TEST(IO_atomic_write_file, no_leftover_staging_on_success) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};
  auto staging{path};
  staging += ".tmp";

  sourcemeta::core::atomic_write_file(path, "done");

  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_FALSE(std::filesystem::exists(staging));
}

TEST(IO_atomic_write_file, binary_writes_preserve_exact_bytes) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto path{workspace.path() / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "line one\nline two\n");

  std::ifstream stream{path, std::ios::binary};
  std::string actual;
  actual.assign(std::istreambuf_iterator<char>{stream},
                std::istreambuf_iterator<char>{});
  EXPECT_EQ(actual, "line one\nline two\n");
}

#if !defined(_WIN32)
TEST(IO_atomic_write_file, read_only_parent_throws_permission_error) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-write-"};
  const auto read_only_dir{workspace.path() / "locked"};
  std::filesystem::create_directory(read_only_dir);
  std::filesystem::permissions(read_only_dir,
                               std::filesystem::perms::owner_read |
                                   std::filesystem::perms::owner_exec,
                               std::filesystem::perm_options::replace);
  const auto path{read_only_dir / "out.txt"};

  try {
    sourcemeta::core::atomic_write_file(path, "should fail");
    std::filesystem::permissions(read_only_dir,
                                 std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    FAIL() << "Expected IOFilePermissionError";
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    std::filesystem::permissions(read_only_dir,
                                 std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    FAIL() << "Wrong exception type";
  }

  // Restore permissions so TemporaryDirectory can clean up
  std::filesystem::permissions(read_only_dir, std::filesystem::perms::owner_all,
                               std::filesystem::perm_options::replace);
}
#endif
