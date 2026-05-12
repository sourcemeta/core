#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <array>   // std::array
#include <cstddef> // std::byte
#include <cstdint> // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream
#include <ios>        // std::ios::binary
#include <span>       // std::span

namespace {

#pragma pack(push, 1)
struct ExampleHeader {
  std::uint32_t magic;
  std::uint16_t version;
  std::uint8_t flags;
  std::uint64_t payload_size;
};
#pragma pack(pop)

} // namespace

class IOBinaryTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::filesystem::create_directories(this->workspace);
  }

  void TearDown() override { std::filesystem::remove_all(this->workspace); }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace{std::filesystem::path{BUILD_DIRECTORY} /
                                  "sourcemeta_core_io_binary_test"};
};

TEST_F(IOBinaryTest, write_single_integer) {
  const auto path{this->workspace / "value.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{0x12345678});
  }
  EXPECT_EQ(std::filesystem::file_size(path), sizeof(std::uint32_t));
}

TEST_F(IOBinaryTest, write_span_of_integers) {
  const auto path{this->workspace / "values.bin"};
  constexpr std::array<std::uint32_t, 4> values{{1, 2, 3, 4}};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::span<const std::uint32_t>{values});
  }
  EXPECT_EQ(std::filesystem::file_size(path),
            values.size() * sizeof(std::uint32_t));
}

TEST_F(IOBinaryTest, write_empty_span) {
  const auto path{this->workspace / "empty.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::span<const std::uint32_t>{});
  }
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST_F(IOBinaryTest, read_after_write_integer_roundtrip) {
  const auto path{this->workspace / "value.bin"};
  constexpr std::uint32_t expected{0x12345678};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{0x12345678});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.read<std::uint32_t>(), expected);
  EXPECT_EQ(reader.offset(), sizeof(std::uint32_t));
}

TEST_F(IOBinaryTest, read_after_write_struct_roundtrip) {
  const auto path{this->workspace / "header.bin"};
  const ExampleHeader expected{0xDEADBEEF, 7, 0x42, 1024};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(expected);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  const auto actual{reader.read<ExampleHeader>()};
  EXPECT_EQ(actual.magic, expected.magic);
  EXPECT_EQ(actual.version, expected.version);
  EXPECT_EQ(actual.flags, expected.flags);
  EXPECT_EQ(actual.payload_size, expected.payload_size);
}

TEST_F(IOBinaryTest, sequential_reads_advance_cursor) {
  const auto path{this->workspace / "sequence.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{10});
    writer.write(std::uint32_t{20});
    writer.write(std::uint32_t{30});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.read<std::uint32_t>(), 10);
  EXPECT_EQ(reader.offset(), 4);
  EXPECT_EQ(reader.read<std::uint32_t>(), 20);
  EXPECT_EQ(reader.offset(), 8);
  EXPECT_EQ(reader.read<std::uint32_t>(), 30);
  EXPECT_EQ(reader.offset(), 12);
}

TEST_F(IOBinaryTest, seek_moves_cursor) {
  const auto path{this->workspace / "seek.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{0xAAAAAAAA});
    writer.write(std::uint32_t{0xBBBBBBBB});
    writer.write(std::uint32_t{0xCCCCCCCC});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(8);
  EXPECT_EQ(reader.offset(), 8);
  EXPECT_EQ(reader.read<std::uint32_t>(), 0xCCCCCCCC);
}

TEST_F(IOBinaryTest, seek_to_end_is_allowed) {
  const auto path{this->workspace / "end.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{42});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(view.size());
  EXPECT_EQ(reader.offset(), view.size());
}

TEST_F(IOBinaryTest, seek_past_end_throws) {
  const auto path{this->workspace / "tiny.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint32_t{1});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW(reader.seek(view.size() + 1),
               sourcemeta::core::IOReadOutOfBoundsError);
}

TEST_F(IOBinaryTest, read_past_end_throws) {
  const auto path{this->workspace / "tiny.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::uint8_t{1});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW((void)reader.read<std::uint32_t>(),
               sourcemeta::core::IOReadOutOfBoundsError);
}

TEST_F(IOBinaryTest, read_byte_span_roundtrip) {
  const auto path{this->workspace / "bytes.bin"};
  constexpr std::array<std::byte, 4> payload{
      {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}}};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.write(std::span<const std::byte>{payload});
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  std::array<std::byte, 4> destination{};
  reader.read_bytes(destination.data(), destination.size());
  EXPECT_EQ(destination, payload);
  EXPECT_EQ(reader.offset(), payload.size());
}
