#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream, std::ifstream
#include <string>     // std::string

TEST(IO_atomic_directory_swap, creates_when_original_absent) {
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

TEST(IO_atomic_directory_swap, swaps_existing_directories) {
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

TEST(IO_atomic_directory_swap, old_directory_preserved_at_replacement_path) {
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

TEST(IO_atomic_directory_swap, preserves_nested_structure) {
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

TEST(IO_atomic_directory_swap, no_leftover_temporary) {
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
    EXPECT_FALSE(filename.starts_with(".swap-"))
        << "leftover temporary directory found: " << filename;
  }
}
