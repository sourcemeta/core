#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream, std::ifstream
#include <string>     // std::string

TEST(IO_hardlink_directory, mirrors_structure) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-hardlink-"};
  const auto source{workspace.path() / "source"};
  const auto destination{workspace.path() / "destination"};

  std::filesystem::create_directories(source / "subdir");
  std::ofstream{source / "a.txt"} << "alpha";
  std::ofstream{source / "subdir" / "b.txt"} << "beta";

  sourcemeta::core::hardlink_directory(source, destination);

  EXPECT_TRUE(std::filesystem::exists(destination / "a.txt"));
  EXPECT_TRUE(std::filesystem::exists(destination / "subdir" / "b.txt"));
  EXPECT_TRUE(std::filesystem::is_directory(destination / "subdir"));
}

TEST(IO_hardlink_directory, files_share_inodes) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-hardlink-"};
  const auto source{workspace.path() / "source"};
  const auto destination{workspace.path() / "destination"};

  std::filesystem::create_directory(source);
  std::ofstream{source / "file.txt"} << "content";

  sourcemeta::core::hardlink_directory(source, destination);

  EXPECT_EQ(std::filesystem::hard_link_count(source / "file.txt"), 2);
  EXPECT_EQ(std::filesystem::hard_link_count(destination / "file.txt"), 2);
  EXPECT_TRUE(std::filesystem::equivalent(source / "file.txt",
                                          destination / "file.txt"));
}

TEST(IO_hardlink_directory, unlink_then_write_independence) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-hardlink-"};
  const auto source{workspace.path() / "source"};
  const auto destination{workspace.path() / "destination"};

  std::filesystem::create_directory(source);
  std::ofstream{source / "file.txt"} << "original";

  sourcemeta::core::hardlink_directory(source, destination);

  std::filesystem::remove(destination / "file.txt");
  std::ofstream{destination / "file.txt"} << "modified";

  std::ifstream source_stream{source / "file.txt"};
  std::string source_content;
  std::getline(source_stream, source_content);
  EXPECT_EQ(source_content, "original");

  std::ifstream destination_stream{destination / "file.txt"};
  std::string destination_content;
  std::getline(destination_stream, destination_content);
  EXPECT_EQ(destination_content, "modified");
}

TEST(IO_hardlink_directory, empty_source) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-hardlink-"};
  const auto source{workspace.path() / "source"};
  const auto destination{workspace.path() / "destination"};

  std::filesystem::create_directory(source);

  sourcemeta::core::hardlink_directory(source, destination);

  EXPECT_TRUE(std::filesystem::exists(destination));
  EXPECT_TRUE(std::filesystem::is_directory(destination));
  EXPECT_TRUE(std::filesystem::is_empty(destination));
}

TEST(IO_hardlink_directory, deeply_nested) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-hardlink-"};
  const auto source{workspace.path() / "source"};
  const auto destination{workspace.path() / "destination"};

  std::filesystem::create_directories(source / "a" / "b" / "c");
  std::ofstream{source / "a" / "b" / "c" / "deep.txt"} << "deep";

  sourcemeta::core::hardlink_directory(source, destination);

  EXPECT_TRUE(
      std::filesystem::exists(destination / "a" / "b" / "c" / "deep.txt"));

  std::ifstream stream{destination / "a" / "b" / "c" / "deep.txt"};
  std::string content;
  std::getline(stream, content);
  EXPECT_EQ(content, "deep");
}
