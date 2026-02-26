#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream, std::ifstream
#include <string>     // std::string

TEST(IO_atomic_directory_replace, creates_when_original_absent) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "file.txt"} << "hello";

  EXPECT_FALSE(std::filesystem::exists(original));

  sourcemeta::core::atomic_directory_replace(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original));
  EXPECT_TRUE(std::filesystem::is_directory(original));
  EXPECT_FALSE(std::filesystem::exists(replacement));

  std::ifstream stream{original / "file.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "hello");
}

TEST(IO_atomic_directory_replace, replaces_existing_directory) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "old.txt"} << "old";

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "new.txt"} << "new";

  sourcemeta::core::atomic_directory_replace(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original));
  EXPECT_FALSE(std::filesystem::exists(original / "old.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "new.txt"));

  std::ifstream stream{original / "new.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "new");
}

TEST(IO_atomic_directory_replace, replacement_is_consumed) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "file.txt"} << "data";

  sourcemeta::core::atomic_directory_replace(original, replacement);

  EXPECT_FALSE(std::filesystem::exists(replacement));
}

TEST(IO_atomic_directory_replace, preserves_nested_structure) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);

  std::filesystem::create_directories(replacement / "a" / "b");
  std::ofstream{replacement / "root.txt"} << "root";
  std::ofstream{replacement / "a" / "mid.txt"} << "mid";
  std::ofstream{replacement / "a" / "b" / "deep.txt"} << "deep";

  sourcemeta::core::atomic_directory_replace(original, replacement);

  EXPECT_TRUE(std::filesystem::exists(original / "root.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "a" / "mid.txt"));
  EXPECT_TRUE(std::filesystem::exists(original / "a" / "b" / "deep.txt"));

  std::ifstream stream{original / "a" / "b" / "deep.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "deep");
}

TEST(IO_atomic_directory_replace, no_leftover_backup) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-atomic-"};
  const auto original{workspace.path() / "original"};
  const auto replacement{workspace.path() / "replacement"};

  std::filesystem::create_directory(original);
  std::ofstream{original / "old.txt"} << "old";

  std::filesystem::create_directory(replacement);
  std::ofstream{replacement / "new.txt"} << "new";

  sourcemeta::core::atomic_directory_replace(original, replacement);

  for (const auto &entry :
       std::filesystem::directory_iterator{workspace.path()}) {
    const auto filename{entry.path().filename().string()};
    EXPECT_FALSE(filename.starts_with(".backup-"))
        << "leftover backup directory found: " << filename;
  }
}
