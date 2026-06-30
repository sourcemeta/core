#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <cstdint>     // std::uint32_t
#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

TEST(IO_FileView, size) {
  const sourcemeta::core::FileView view{std::filesystem::path{STUBS_DIRECTORY} /
                                        "fileview.bin"};
  EXPECT_EQ(view.size(), 20);
}

TEST(IO_FileView, as_header) {
  struct Header {
    std::uint32_t magic;
    std::uint32_t version;
    std::uint32_t count;
  };

  const sourcemeta::core::FileView view{std::filesystem::path{STUBS_DIRECTORY} /
                                        "fileview.bin"};
  const auto *header = view.as<Header>();

  EXPECT_EQ(header->magic, 0x56574946);
  EXPECT_EQ(header->version, 1);
  EXPECT_EQ(header->count, 42);
}

TEST(IO_FileView, as_with_offset) {
  struct Data {
    std::uint32_t value1;
    std::uint32_t value2;
  };

  const sourcemeta::core::FileView view{std::filesystem::path{STUBS_DIRECTORY} /
                                        "fileview.bin"};
  const auto *data = view.as<Data>(12);

  EXPECT_EQ(data->value1, 0xDEADBEEF);
  EXPECT_EQ(data->value2, 0xCAFEBABE);
}

TEST(IO_FileView, file_not_found) {
  try {
    sourcemeta::core::FileView(std::filesystem::path{STUBS_DIRECTORY} /
                               "nonexistent.bin");
    FAIL();
  } catch (const sourcemeta::core::FileViewError &error) {
    EXPECT_EQ(error.path(),
              std::filesystem::path{STUBS_DIRECTORY} / "nonexistent.bin");
  }
}

TEST(IO_FileView, empty_file_does_not_throw) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".fileview-"};
  const auto path{directory.path() / "empty.bin"};
  sourcemeta::core::write_file(path, std::string_view{""});

  const sourcemeta::core::FileView view{path};
  EXPECT_EQ(view.size(), 0);
}
