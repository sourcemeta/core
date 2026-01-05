#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <cstdint> // std::uint32_t

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
  EXPECT_THROW(sourcemeta::core::FileView(
                   std::filesystem::path{STUBS_DIRECTORY} / "nonexistent.bin"),
               sourcemeta::core::FileViewError);
}
