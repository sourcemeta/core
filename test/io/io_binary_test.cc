#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <array>   // std::array
#include <bit>     // std::endian
#include <cstddef> // std::byte
#include <cstdint> // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream
#include <ios>        // std::ios::binary
#include <sstream>    // std::istringstream, std::ostringstream
#include <string>     // std::string
#include <tuple>      // std::ignore

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

// -----------------------------------------------------------------------------
// BinaryWriter — std::ostream backend
// -----------------------------------------------------------------------------

TEST(IO_BinaryWriter, put_byte_emits_one_byte) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_byte(0x42);
  const auto bytes{stream.str()};
  ASSERT_EQ(bytes.size(), 1);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[0]), 0x42);
}

TEST(IO_BinaryWriter, put_word_emits_two_bytes_in_host_order) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_word(0xCAFE);
  const auto bytes{stream.str()};
  ASSERT_EQ(bytes.size(), 2);
  if constexpr (std::endian::native == std::endian::little) {
    EXPECT_EQ(static_cast<std::uint8_t>(bytes[0]), 0xFE);
    EXPECT_EQ(static_cast<std::uint8_t>(bytes[1]), 0xCA);
  } else {
    EXPECT_EQ(static_cast<std::uint8_t>(bytes[0]), 0xCA);
    EXPECT_EQ(static_cast<std::uint8_t>(bytes[1]), 0xFE);
  }
}

TEST(IO_BinaryWriter, put_dword_emits_four_bytes) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_dword(0x12345678);
  EXPECT_EQ(stream.str().size(), 4);
}

TEST(IO_BinaryWriter, put_qword_emits_eight_bytes) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_qword(0x0123456789ABCDEFULL);
  EXPECT_EQ(stream.str().size(), 8);
}

TEST(IO_BinaryWriter, put_bytes_emits_raw_buffer) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  constexpr std::array<std::byte, 3> payload{
      {std::byte{0xDE}, std::byte{0xAD}, std::byte{0xBE}}};
  writer.put_bytes(payload.data(), payload.size());
  const auto bytes{stream.str()};
  ASSERT_EQ(bytes.size(), 3);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[0]), 0xDE);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[1]), 0xAD);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[2]), 0xBE);
}

TEST(IO_BinaryWriter, put_bytes_zero_length_is_noop) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_bytes(nullptr, 0);
  EXPECT_EQ(stream.str().size(), 0);
}

TEST(IO_BinaryWriter, put_pod_via_put_bytes) {
  const ExampleHeader expected{0xDEADBEEF, 7, 0x42, 1024};
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  writer.put_bytes(reinterpret_cast<const std::byte *>(&expected),
                   sizeof(expected));
  EXPECT_EQ(stream.str().size(), sizeof(ExampleHeader));
}

TEST(IO_BinaryWriter, position_advances_after_writes) {
  std::ostringstream stream;
  sourcemeta::core::BinaryWriter writer{stream};
  EXPECT_EQ(writer.position(), 0);
  writer.put_byte(0xAB);
  EXPECT_EQ(writer.position(), 1);
  writer.put_word(0xCAFE);
  EXPECT_EQ(writer.position(), 3);
  writer.put_dword(0xDEADBEEF);
  EXPECT_EQ(writer.position(), 7);
}

// -----------------------------------------------------------------------------
// BinaryReader — std::istream backend
// -----------------------------------------------------------------------------

TEST(IO_BinaryReader, get_byte_from_stringstream) {
  std::istringstream input{"X"};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_EQ(reader.get_byte(), 'X');
}

TEST(IO_BinaryReader, get_word_roundtrip_via_stringstream) {
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_word(0xCAFE);

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_EQ(reader.get_word(), 0xCAFE);
}

TEST(IO_BinaryReader, get_dword_roundtrip_via_stringstream) {
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_dword(0x12345678);

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_EQ(reader.get_dword(), 0x12345678);
  EXPECT_EQ(reader.position(), sizeof(std::uint32_t));
}

TEST(IO_BinaryReader, get_qword_roundtrip_via_stringstream) {
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_qword(0x0123456789ABCDEFULL);

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_EQ(reader.get_qword(), 0x0123456789ABCDEFULL);
  EXPECT_EQ(reader.position(), sizeof(std::uint64_t));
}

TEST(IO_BinaryReader, get_pod_roundtrip_via_stringstream) {
  const ExampleHeader expected{0xDEADBEEF, 7, 0x42, 1024};
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_bytes(reinterpret_cast<const std::byte *>(&expected),
                   sizeof(expected));

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  ExampleHeader actual{};
  reader.get_bytes(reinterpret_cast<std::byte *>(&actual), sizeof(actual));
  EXPECT_EQ(actual.magic, expected.magic);
  EXPECT_EQ(actual.version, expected.version);
  EXPECT_EQ(actual.flags, expected.flags);
  EXPECT_EQ(actual.payload_size, expected.payload_size);
}

TEST(IO_BinaryReader, sequential_gets_from_stringstream) {
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_dword(10);
  writer.put_dword(20);
  writer.put_dword(30);

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_EQ(reader.get_dword(), 10);
  EXPECT_EQ(reader.position(), 4);
  EXPECT_EQ(reader.get_dword(), 20);
  EXPECT_EQ(reader.position(), 8);
  EXPECT_EQ(reader.get_dword(), 30);
  EXPECT_EQ(reader.position(), 12);
}

TEST(IO_BinaryReader, seek_on_stringstream) {
  std::ostringstream output;
  sourcemeta::core::BinaryWriter writer{output};
  writer.put_dword(0xAAAAAAAA);
  writer.put_dword(0xBBBBBBBB);
  writer.put_dword(0xCCCCCCCC);

  std::istringstream input{output.str()};
  sourcemeta::core::BinaryReader reader{input};
  reader.seek(8);
  EXPECT_EQ(reader.position(), 8);
  EXPECT_EQ(reader.get_dword(), 0xCCCCCCCC);
}

TEST(IO_BinaryReader, read_past_end_of_stringstream_throws) {
  std::istringstream input{"AB"};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_THROW(std::ignore = reader.get_dword(),
               sourcemeta::core::IOReadOutOfBoundsError);
}

TEST(IO_BinaryReader, has_more_data_on_stringstream) {
  std::istringstream input{"ABC"};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_TRUE(reader.has_more_data());
  EXPECT_EQ(reader.get_byte(), 'A');
  EXPECT_TRUE(reader.has_more_data());
  EXPECT_EQ(reader.get_byte(), 'B');
  EXPECT_TRUE(reader.has_more_data());
  EXPECT_EQ(reader.get_byte(), 'C');
  EXPECT_FALSE(reader.has_more_data());
}

TEST(IO_BinaryReader, has_more_data_on_empty_stringstream) {
  std::istringstream input{""};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_FALSE(reader.has_more_data());
}

TEST(IO_BinaryReader, has_more_data_treats_null_byte_as_data) {
  // A null byte at the cursor is a legitimate value, not end-of-stream.
  std::istringstream input{std::string(1, '\0')};
  sourcemeta::core::BinaryReader reader{input};
  EXPECT_TRUE(reader.has_more_data());
}

// -----------------------------------------------------------------------------
// BinaryWriter — file backend (via std::ofstream)
// -----------------------------------------------------------------------------

TEST_F(IOBinaryTest, put_dword_to_file) {
  const auto path{this->workspace / "value.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(0x12345678);
  }
  EXPECT_EQ(std::filesystem::file_size(path), sizeof(std::uint32_t));
}

TEST_F(IOBinaryTest, put_bytes_to_file) {
  const auto path{this->workspace / "values.bin"};
  constexpr std::array<std::byte, 4> payload{
      {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}}};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_bytes(payload.data(), payload.size());
  }
  EXPECT_EQ(std::filesystem::file_size(path), payload.size());
}

// -----------------------------------------------------------------------------
// BinaryReader — FileView backend
// -----------------------------------------------------------------------------

TEST_F(IOBinaryTest, get_after_put_integer_roundtrip_via_file) {
  const auto path{this->workspace / "value.bin"};
  constexpr std::uint32_t expected{0x12345678};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(0x12345678);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.get_dword(), expected);
  EXPECT_EQ(reader.position(), sizeof(std::uint32_t));
}

TEST_F(IOBinaryTest, get_after_put_struct_roundtrip_via_file) {
  const auto path{this->workspace / "header.bin"};
  const ExampleHeader expected{0xDEADBEEF, 7, 0x42, 1024};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_bytes(reinterpret_cast<const std::byte *>(&expected),
                     sizeof(expected));
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  ExampleHeader actual{};
  reader.get_bytes(reinterpret_cast<std::byte *>(&actual), sizeof(actual));
  EXPECT_EQ(actual.magic, expected.magic);
  EXPECT_EQ(actual.version, expected.version);
  EXPECT_EQ(actual.flags, expected.flags);
  EXPECT_EQ(actual.payload_size, expected.payload_size);
}

TEST_F(IOBinaryTest, sequential_gets_advance_cursor_via_file) {
  const auto path{this->workspace / "sequence.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(10);
    writer.put_dword(20);
    writer.put_dword(30);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_EQ(reader.get_dword(), 10);
  EXPECT_EQ(reader.position(), 4);
  EXPECT_EQ(reader.get_dword(), 20);
  EXPECT_EQ(reader.position(), 8);
  EXPECT_EQ(reader.get_dword(), 30);
  EXPECT_EQ(reader.position(), 12);
}

TEST_F(IOBinaryTest, seek_moves_cursor_via_file) {
  const auto path{this->workspace / "seek.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(0xAAAAAAAA);
    writer.put_dword(0xBBBBBBBB);
    writer.put_dword(0xCCCCCCCC);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(8);
  EXPECT_EQ(reader.position(), 8);
  EXPECT_EQ(reader.get_dword(), 0xCCCCCCCC);
}

TEST_F(IOBinaryTest, seek_to_end_is_allowed_via_file) {
  const auto path{this->workspace / "end.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(42);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  reader.seek(view.size());
  EXPECT_EQ(reader.position(), view.size());
}

TEST_F(IOBinaryTest, seek_past_end_throws_via_file) {
  const auto path{this->workspace / "tiny.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_dword(1);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW(reader.seek(view.size() + 1),
               sourcemeta::core::IOReadOutOfBoundsError);
}

TEST_F(IOBinaryTest, get_past_end_throws_via_file) {
  const auto path{this->workspace / "tiny.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_byte(1);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_THROW(std::ignore = reader.get_dword(),
               sourcemeta::core::IOReadOutOfBoundsError);
}

TEST_F(IOBinaryTest, get_byte_span_roundtrip_via_file) {
  const auto path{this->workspace / "bytes.bin"};
  constexpr std::array<std::byte, 4> payload{
      {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}}};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_bytes(payload.data(), payload.size());
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  std::array<std::byte, 4> destination{};
  reader.get_bytes(destination.data(), destination.size());
  EXPECT_EQ(destination, payload);
  EXPECT_EQ(reader.position(), payload.size());
}

TEST_F(IOBinaryTest, has_more_data_on_file_view) {
  const auto path{this->workspace / "value.bin"};
  {
    std::ofstream raw{path, std::ios::binary};
    sourcemeta::core::BinaryWriter writer{raw};
    writer.put_byte(0xAB);
    writer.put_byte(0xCD);
  }

  const sourcemeta::core::FileView view{path};
  sourcemeta::core::BinaryReader reader{view};
  EXPECT_TRUE(reader.has_more_data());
  std::ignore = reader.get_byte();
  EXPECT_TRUE(reader.has_more_data());
  std::ignore = reader.get_byte();
  EXPECT_FALSE(reader.has_more_data());
}
