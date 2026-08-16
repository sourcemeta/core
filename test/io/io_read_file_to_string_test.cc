#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem> // std::filesystem::path
#include <fstream>    // std::ifstream
#include <string>     // std::string

// A whole-file read hands back the bytes the file holds. Text mode would
// collapse a carriage return before a line feed on some platforms, which makes
// the contents, its size and every offset into it disagree with the file itself
TEST(carriage_returns_survive) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "crlf.txt"};
  sourcemeta::core::write_file(path, std::string{"alpha\r\nbeta\r\n"});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "alpha\r\nbeta\r\n");
}

TEST(the_size_matches_the_file) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "crlf.txt"};
  sourcemeta::core::write_file(path, std::string{"a\r\nb\r\n"});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path).size(),
            std::filesystem::file_size(path));
}

TEST(a_lone_carriage_return_survives) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "cr.txt"};
  sourcemeta::core::write_file(path, std::string{"alpha\rbeta"});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "alpha\rbeta");
}

TEST(an_arbitrary_byte_survives) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "binary.bin"};
  sourcemeta::core::write_file(path, std::string{"\x01\x1A\x7F", 3});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path),
            (std::string{"\x01\x1A\x7F", 3}));
}

TEST(an_empty_file) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "empty.txt"};
  sourcemeta::core::write_file(path, std::string{});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "");
}

TEST(a_missing_file) {
  const std::filesystem::path path{"/nonexistent-core-io-fixture.txt"};
  try {
    sourcemeta::core::read_file_to_string(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_STREQ(error.what(), "File not found");
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

TEST(a_directory) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  try {
    sourcemeta::core::read_file_to_string(directory.path());
    FAIL();
  } catch (const sourcemeta::core::IOIsADirectoryError &error) {
    EXPECT_STREQ(error.what(), "Expected a file but got a directory");
    EXPECT_EQ(error.path(), directory.path());
  } catch (...) {
    FAIL();
  }
}

// POSIX permission bits don't map cleanly to Windows ACLs
#if !defined(_WIN32)
TEST(an_unreadable_file) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), "core-io-"};
  const auto path{directory.path() / "locked.txt"};
  sourcemeta::core::write_file(path, std::string{"secret"});
  std::filesystem::permissions(path, std::filesystem::perms::none,
                               std::filesystem::perm_options::replace);

  // A process that bypasses the mode bits, such as one running as root, and a
  // filesystem that ignores them at all, both still hand the file over. What
  // the mode amounts to here is settled by trying it rather than by assuming
  std::ifstream probe{path};
  if (probe.is_open()) {
    EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "secret");
    return;
  }

  try {
    sourcemeta::core::read_file_to_string(path);
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_STREQ(error.what(), "Permission denied");
    // The path is resolved before the file is opened, so the error carries the
    // canonical form rather than the one that was asked for
    EXPECT_EQ(error.path(), sourcemeta::core::canonical(path));
  } catch (...) {
    FAIL();
  }
}
#endif
