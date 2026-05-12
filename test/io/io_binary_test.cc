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

auto write_via_binary_writer(const std::filesystem::path &path,
                             auto &&writer_callback) -> void {
  std::ofstream raw{path, std::ios::binary};
  sourcemeta::core::BinaryWriter writer{raw};
  writer_callback(writer);
}

} // namespace

TEST(IO_BinaryWriter, write_single_integer) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "value.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{0x12345678});
  });

  EXPECT_EQ(std::filesystem::file_size(path), sizeof(std::uint32_t));
}

TEST(IO_BinaryWriter, write_span_of_integers) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "values.bin"};

  constexpr std::array<std::uint32_t, 4> values{{1, 2, 3, 4}};

  write_via_binary_writer(path, [&](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::span<const std::uint32_t>{values});
  });

  EXPECT_EQ(std::filesystem::file_size(path),
            values.size() * sizeof(std::uint32_t));
}

TEST(IO_BinaryWriter, write_empty_span) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "empty.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::span<const std::uint32_t>{});
  });

  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST(IO_BinaryReader, read_after_write_integer_roundtrip) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "value.bin"};

  constexpr std::uint32_t expected{0x12345678};
  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{0x12345678});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.read<std::uint32_t>(), expected);
  EXPECT_EQ(reader.offset(), sizeof(std::uint32_t));
}

TEST(IO_BinaryReader, read_after_write_struct_roundtrip) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "header.bin"};

  const ExampleHeader expected{0xDEADBEEF, 7, 0x42, 1024};
  write_via_binary_writer(path, [&](sourcemeta::core::BinaryWriter &writer) {
    writer.write(expected);
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  const auto actual{reader.read<ExampleHeader>()};
  EXPECT_EQ(actual.magic, expected.magic);
  EXPECT_EQ(actual.version, expected.version);
  EXPECT_EQ(actual.flags, expected.flags);
  EXPECT_EQ(actual.payload_size, expected.payload_size);
}

TEST(IO_BinaryReader, sequential_reads_advance_cursor) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "sequence.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{10});
    writer.write(std::uint32_t{20});
    writer.write(std::uint32_t{30});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.read<std::uint32_t>(), 10);
  EXPECT_EQ(reader.offset(), 4);
  EXPECT_EQ(reader.read<std::uint32_t>(), 20);
  EXPECT_EQ(reader.offset(), 8);
  EXPECT_EQ(reader.read<std::uint32_t>(), 30);
  EXPECT_EQ(reader.offset(), 12);
}

TEST(IO_BinaryReader, seek_moves_cursor) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "seek.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{0xAAAAAAAA});
    writer.write(std::uint32_t{0xBBBBBBBB});
    writer.write(std::uint32_t{0xCCCCCCCC});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(8);
  EXPECT_EQ(reader.offset(), 8);
  EXPECT_EQ(reader.read<std::uint32_t>(), 0xCCCCCCCC);
}

TEST(IO_BinaryReader, seek_to_end_is_allowed) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "end.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{42});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(view.size());
  EXPECT_EQ(reader.offset(), view.size());
}

TEST(IO_BinaryReader, seek_past_end_throws) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "tiny.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint32_t{1});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW(reader.seek(view.size() + 1), std::filesystem::filesystem_error);
}

TEST(IO_BinaryReader, read_past_end_throws) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "tiny.bin"};

  write_via_binary_writer(path, [](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::uint8_t{1});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW((void)reader.read<std::uint32_t>(),
               std::filesystem::filesystem_error);
}

TEST(IO_BinaryReader, read_byte_span_roundtrip) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-binary-"};
  const auto path{workspace.path() / "bytes.bin"};

  constexpr std::array<std::byte, 4> payload{
      {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}}};

  write_via_binary_writer(path, [&](sourcemeta::core::BinaryWriter &writer) {
    writer.write(std::span<const std::byte>{payload});
  });

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  std::array<std::byte, 4> destination{};
  reader.read_bytes(destination.data(), destination.size());
  EXPECT_EQ(destination, payload);
  EXPECT_EQ(reader.offset(), payload.size());
}
