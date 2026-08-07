#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

TEST(test_txt) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "test.txt"};
  sourcemeta::core::flush(path);
}

TEST(not_exists) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "foo.txt"};

  try {
    sourcemeta::core::flush(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

TEST(flush_a_directory_throws) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY}};
  try {
    sourcemeta::core::flush(path);
    FAIL();
  } catch (const std::filesystem::filesystem_error &) {
  } catch (...) {
    FAIL();
  }
}

// POSIX permission bits don't map cleanly to Windows ACLs
#if !defined(_WIN32)
TEST(flush_an_unreadable_file_throws_permission_error) {
  const auto path{std::filesystem::temp_directory_path() /
                  "sourcemeta_core_io_flush_locked.txt"};
  std::ofstream output{path};
  output << "content";
  output.close();
  std::filesystem::permissions(path, std::filesystem::perms::none,
                               std::filesystem::perm_options::replace);
  try {
    sourcemeta::core::flush(path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(path);
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(path);
  } catch (...) {
    FAIL();
  }
}
#endif
