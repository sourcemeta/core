#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem>   // std::filesystem
#include <fstream>      // std::ofstream, std::ifstream
#include <string>       // std::string
#include <system_error> // std::error_code

TEST(creates_when_original_absent) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "file.txt"} << "hello";

  EXPECT_FALSE(std::filesystem::exists(original));

  sourcemeta::core::atomic_directory_swap(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original));
  EXPECT_TRUE(std::filesystem::is_directory(original));
  EXPECT_FALSE(std::filesystem::exists(replacement));

  std::ifstream stream{original / "file.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "hello");
}

TEST(swaps_existing_directories) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "old.txt"} << "old";

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "new.txt"} << "new";

  sourcemeta::core::atomic_directory_swap(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original));
  EXPECT_FALSE(std::filesystem::exists(original / "old.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "new.txt"));

  std::ifstream new_stream{original / "new.txt"};
  std::string new_content;
  std::getline(new_stream, new_content);
  EXPECT_EQ(new_content, "new");

  EXPECT_TRUE(std::filesystem::exists(replacement));
  EXPECT_FALSE(std::filesystem::exists(replacement / "new.txt"));
  EXPECT_TRUE(std::filesystem::exists(replacement / "old.txt"));

  std::ifstream old_stream{replacement / "old.txt"};
  std::string old_content;
  std::getline(old_stream, old_content);
  EXPECT_EQ(old_content, "old");
}

TEST(old_directory_preserved_at_replacement_path) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "a.txt"} << "aaa";
  std::ofstream{original / "b.txt"} << "bbb";

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "file.txt"} << "data";

  sourcemeta::core::atomic_directory_swap(original, replacement);

  EXPECT_TRUE(std::filesystem::is_directory(replacement));
  EXPECT_TRUE(std::filesystem::exists(replacement / "a.txt"));
  EXPECT_TRUE(std::filesystem::exists(replacement / "b.txt"));
  EXPECT_FALSE(std::filesystem::exists(replacement / "file.txt"));
}

TEST(preserves_nested_structure) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "orig.txt"} << "orig";

  std::filesystem::create_directories(replacement / "a" / "b");
  std::ofstream{replacement / "root.txt"} << "root";
  std::ofstream{replacement / "a" / "mid.txt"} << "mid";
  std::ofstream{replacement / "a" / "b" / "deep.txt"} << "deep";

  sourcemeta::core::atomic_directory_swap(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original / "root.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "a" / "mid.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "a" / "b" / "deep.txt"));

  std::ifstream stream{original / "a" / "b" / "deep.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "deep");

  EXPECT_TRUE(std::filesystem::exists(replacement / "orig.txt"));
  EXPECT_FALSE(std::filesystem::exists(replacement / "root.txt"));
}

TEST(no_leftover_temporary) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "old.txt"} << "old";

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "new.txt"} << "new";

  sourcemeta::core::atomic_directory_swap(original, replacement);

  for (const auto &entry :
       std::filesystem::directory_iterator{workspace.path()}) {
    const auto filename{entry.path().filename().string()};
    EXPECT_FALSE(filename.starts_with(".swap-"));
  }
}

TEST(missing_destination_parent_preserves_replacement) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "missing" / "original"};
  const auto replacement{workspace.path() / "replacement"};
  std::filesystem::create_directory(replacement);
  sourcemeta::core::write_file(replacement / "keep", "replacement");
  try {
    sourcemeta::core::atomic_directory_swap(original, replacement);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), replacement);
  }
  EXPECT_FALSE(std::filesystem::exists(original));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(replacement / "keep"),
            "replacement");
}

#if !defined(_WIN32)
TEST(destination_parent_is_a_file_preserves_replacement) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto blocker{workspace.path() / "blocker"};
  sourcemeta::core::write_file(blocker, "original");
  const auto original{blocker / "original"};
  const auto replacement{workspace.path() / "replacement"};
  std::filesystem::create_directory(replacement);
  sourcemeta::core::write_file(replacement / "keep", "replacement");
  try {
    sourcemeta::core::atomic_directory_swap(original, replacement);
    FAIL();
  } catch (const sourcemeta::core::IOFilesystemError &error) {
    EXPECT_TRUE(static_cast<bool>(error.code()));
    EXPECT_EQ(error.path(), replacement);
    EXPECT_EQ(error.other(), original);
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(blocker), "original");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(replacement / "keep"),
            "replacement");
}

namespace {
class ReadOnlyAtomicParent {
public:
  explicit ReadOnlyAtomicParent(const std::filesystem::path &path)
      : path_{path}, permissions_{std::filesystem::status(path).permissions()} {
    std::ofstream{path_ / "permission-probe"} << "probe";
    std::filesystem::permissions(path_,
                                 std::filesystem::perms::owner_read |
                                     std::filesystem::perms::owner_exec,
                                 std::filesystem::perm_options::replace);
  }
  ~ReadOnlyAtomicParent() {
    std::error_code error;
    std::filesystem::permissions(path_, permissions_, error);
    std::filesystem::remove(path_ / "permission-probe", error);
    std::filesystem::remove(path_ / "permission-renamed", error);
    std::filesystem::remove(path_ / "permission-created", error);
  }

  // Privileged processes and some filesystems can bypass the requested bits.
  [[nodiscard]] auto allows_modification() const -> bool {
    std::error_code error;
    std::filesystem::rename(path_ / "permission-probe",
                            path_ / "permission-renamed", error);
    if (!error) {
      return true;
    }
    std::ofstream probe{path_ / "permission-created"};
    return probe.is_open();
  }

private:
  std::filesystem::path path_;
  std::filesystem::perms permissions_;
};
} // namespace

TEST(permission_denied_when_original_absent_preserves_replacement) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto parent{workspace.path() / "locked"};
  std::filesystem::create_directory(parent);
  const auto original{parent / "original"};
  const auto replacement{workspace.path() / "replacement"};
  std::filesystem::create_directory(replacement);
  sourcemeta::core::write_file(replacement / "keep", "replacement");
  {
    const ReadOnlyAtomicParent locked{parent};
    if (locked.allows_modification()) {
      return;
    }
    try {
      sourcemeta::core::atomic_directory_swap(original, replacement);
      FAIL();
    } catch (const sourcemeta::core::IOFilePermissionError &error) {
      EXPECT_EQ(error.path(), replacement);
    }
  }
  EXPECT_FALSE(std::filesystem::exists(original));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(replacement / "keep"),
            "replacement");
}

TEST(permission_denied_exchange_preserves_both_directories) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};
  std::filesystem::create_directory(original);
  std::filesystem::create_directory(replacement);
  sourcemeta::core::write_file(original / "keep", "original");
  sourcemeta::core::write_file(replacement / "keep", "replacement");
  {
    const ReadOnlyAtomicParent locked{workspace.path()};
    if (locked.allows_modification()) {
      return;
    }
    try {
      sourcemeta::core::atomic_directory_swap(original, replacement);
      FAIL();
#if defined(__linux__) || defined(__APPLE__)
    } catch (const sourcemeta::core::IOFilePermissionError &error) {
      EXPECT_EQ(error.path(), original);
#else
      // The fallback first creates its swap staging directory in this parent.
    } catch (const sourcemeta::core::IOFilesystemError &error) {
      EXPECT_EQ(error.path(), workspace.path());
      EXPECT_EQ(error.code(), std::errc::permission_denied);
#endif
    }
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(original / "keep"),
            "original");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(replacement / "keep"),
            "replacement");
}
#endif

#if defined(__linux__) || defined(__APPLE__)
// Both paths satisfy the directory preconditions, but a directory cannot be
// exchanged with its own descendant. Neither tree may be partially moved.
TEST(exchange_with_descendant_preserves_both_directories) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{original / "replacement"};
  std::filesystem::create_directories(replacement);
  sourcemeta::core::write_file(original / "keep", "original");
  sourcemeta::core::write_file(replacement / "keep", "replacement");
  try {
    sourcemeta::core::atomic_directory_swap(original, replacement);
    FAIL();
  } catch (const sourcemeta::core::IOFilesystemError &error) {
    EXPECT_TRUE(static_cast<bool>(error.code()));
    EXPECT_EQ(error.path(), replacement);
    EXPECT_EQ(error.other(), original);
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(original / "keep"),
            "original");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(replacement / "keep"),
            "replacement");
}
#endif
