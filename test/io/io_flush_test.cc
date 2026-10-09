#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#if defined(__linux__)
#include <sys/stat.h> // mkfifo
#endif

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

// On Windows, opening a directory without backup semantics is reported as
// access denied rather than a generic filesystem error
#if !defined(_WIN32)
TEST(flush_a_directory_throws) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY}};
  try {
    sourcemeta::core::flush(path);
    FAIL();
  } catch (const std::filesystem::filesystem_error &) {
    // Refusing the unflushable path is expected
  } catch (...) {
    FAIL();
  }
}
#endif

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

  // A process that bypasses the mode bits, such as one running as root, and a
  // filesystem that ignores them at all, both still open the file. What the
  // mode amounts to is settled by trying it rather than by assuming
  std::ifstream probe{path};
  const auto readable{probe.is_open()};
  probe.close();

  if (readable) {
    sourcemeta::core::flush(path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(path);
    return;
  }

  try {
    sourcemeta::core::flush(path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(path);
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_STREQ(error.what(), "Permission denied");
    EXPECT_EQ(error.path(), path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace);
    std::filesystem::remove(path);
  } catch (...) {
    FAIL();
  }
}
#endif

// A pipe is a file the filesystem names and holds no blocks for, and Linux
// reports a synchronisation request against one as invalid, which is the one
// way to reach the failure arm without a device that misbehaves. Opening it for
// both reading and writing is what keeps the call from waiting for a peer that
// never arrives. Darwin answers the same request successfully, so this is held
// to the platform that documents the refusal
#if defined(__linux__)
TEST(flush_a_pipe_throws) {
  const auto path{std::filesystem::temp_directory_path() /
                  "sourcemeta_core_io_flush_pipe"};
  std::filesystem::remove(path);
  EXPECT_EQ(::mkfifo(path.c_str(), 0600), 0);

  try {
    sourcemeta::core::flush(path);
    std::filesystem::remove(path);
    FAIL();
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.path1(), path);
    std::filesystem::remove(path);
  } catch (...) {
    std::filesystem::remove(path);
    FAIL();
  }
}
#endif
