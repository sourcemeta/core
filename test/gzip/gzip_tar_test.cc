#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <filesystem>
#include <string_view>

TEST(GZIP_tar, single_file_string_view) {
  sourcemeta::core::GZIPTar archive;
  archive.push("hello.txt", "Hello World");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, single_file_raw_data) {
  sourcemeta::core::GZIPTar archive;
  const char data[] = "Hello World";
  archive.push("hello.txt", data, sizeof(data) - 1);

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, multiple_files) {
  sourcemeta::core::GZIPTar archive;
  archive.push("file1.txt", "First file content");
  archive.push("file2.md", "# Second file");
  archive.push("file3.json", "{\"key\": \"value\"}");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, empty_file) {
  sourcemeta::core::GZIPTar archive;
  archive.push("empty.txt", "");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, filesystem_path) {
  sourcemeta::core::GZIPTar archive;
  std::filesystem::path file_path{"subdir/test.txt"};
  archive.push(file_path, "Content in subdirectory");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, large_content) {
  sourcemeta::core::GZIPTar archive;
  std::string large_content(10000, 'A');
  archive.push("large.txt", large_content);

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, binary_data) {
  sourcemeta::core::GZIPTar archive;
  const unsigned char binary_data[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD};
  archive.push("binary.dat", binary_data, sizeof(binary_data));

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, null_data_zero_size) {
  sourcemeta::core::GZIPTar archive;
  archive.push("null.txt", nullptr, 0);

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, multiple_data_calls) {
  sourcemeta::core::GZIPTar archive;
  archive.push("file1.txt", "First");
  archive.push("file2.txt", "Second");

  const auto first_size = archive.size();
  const auto first_data = archive.data();

  // Calling data() again should return same values
  EXPECT_EQ(archive.size(), first_size);
  EXPECT_EQ(archive.data(), first_data);
}

TEST(GZIP_tar, special_characters) {
  sourcemeta::core::GZIPTar archive;
  archive.push("special.txt", "Héllo Wörld! 你好世界 🌍");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, long_filename) {
  sourcemeta::core::GZIPTar archive;
  std::string long_name(200, 'a');
  long_name += ".txt";
  archive.push(long_name, "Content with very long filename");

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}

TEST(GZIP_tar, cannot_add_after_finalized) {
  sourcemeta::core::GZIPTar archive;
  archive.push("file1.txt", "First file");

  // Finalize by calling data()
  archive.data();

  // Now adding another file should throw
  EXPECT_THROW(archive.push("file2.txt", "Second file"),
               sourcemeta::core::GZIPArchiveError);
}

TEST(GZIP_tar, cannot_add_after_size_call) {
  sourcemeta::core::GZIPTar archive;
  archive.push("file1.txt", "First file");

  // Finalize by calling size()
  archive.size();

  // Now adding another file should throw
  EXPECT_THROW(archive.push("file2.txt", "Second file"),
               sourcemeta::core::GZIPArchiveError);
}

TEST(GZIP_tar, empty_archive) {
  sourcemeta::core::GZIPTar archive;

  EXPECT_GT(archive.size(), 0);
  EXPECT_NE(archive.data(), nullptr);
}
