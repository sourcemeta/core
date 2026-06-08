#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>

#include <array>    // std::array
#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t
#include <cstring>  // std::memcmp
#include <istream>  // std::istream
#include <iterator> // std::istreambuf_iterator
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <sstream>  // std::istringstream
#include <string>   // std::string

namespace {

auto decompress_via_stream(std::istream &compressed) -> std::string {
  sourcemeta::core::GZIPStreamBuffer buffer{compressed};
  std::istream decompressed{&buffer};
  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  return result;
}

auto decompress_via_stream(const std::string &compressed) -> std::string {
  std::istringstream stream{compressed};
  return decompress_via_stream(stream);
}

} // namespace

TEST(GZIP_stream_buffer, empty) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_TRUE(result.empty());
}

TEST(GZIP_stream_buffer, hello_world) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, large_input) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, binary_data) {
  std::string input;
  input.resize(256);
  for (unsigned int index = 0; index < 256; ++index) {
    input[index] = static_cast<char>(index);
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, read_char_by_char) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  char character{};
  while (decompressed.get(character)) {
    result += character;
  }

  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, multiple_small_reads) {
  const std::string pattern{"abcdefghij"};
  std::string input;
  for (int index = 0; index < 500; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 7> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }

  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, invalid_input_throws) {
  const std::string garbage{"this is not gzip data"};
  EXPECT_THROW(decompress_via_stream(garbage), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, truncated_input_throws) {
  const std::string input{"hello world, this needs to be long enough"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed.resize(compressed.size() / 2);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, output_size_one_byte) {
  const std::string input{"X"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, output_size_two_bytes) {
  const std::string input{"AB"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, output_size_16383) {
  std::string input;
  input.resize(16383);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, output_size_16384) {
  std::string input;
  input.resize(16384);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, output_size_16385) {
  std::string input;
  input.resize(16385);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, output_size_32768) {
  std::string input;
  input.resize(32768);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, output_size_one_megabyte) {
  std::string input;
  input.resize(1024 * 1024);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, highly_compressible_zeros) {
  const std::string input(65536, '\0');
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_LT(compressed.size(), input.size() / 10);
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, highly_compressible_repeated_byte) {
  const std::string input(65536, static_cast<char>(0xff));
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_LT(compressed.size(), input.size() / 10);
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, incompressible_random_bytes) {
  std::mt19937 generator{42};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  input.resize(65536);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(distribution(generator));
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto result{decompress_via_stream(compressed)};
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, read_after_end_returns_eof) {
  const std::string input{"hello"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_EQ(result, input);
  EXPECT_EQ(decompressed.get(), std::char_traits<char>::eof());
  EXPECT_EQ(decompressed.get(), std::char_traits<char>::eof());
}

TEST(GZIP_stream_buffer, peek_returns_first_byte) {
  const std::string input{"hello"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  EXPECT_EQ(decompressed.peek(), 'h');
  EXPECT_EQ(decompressed.peek(), 'h');
  EXPECT_EQ(decompressed.get(), 'h');
  EXPECT_EQ(decompressed.peek(), 'e');
}

TEST(GZIP_stream_buffer, mixed_peek_and_get) {
  const std::string input{"abcdef"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  EXPECT_EQ(decompressed.peek(), 'a');
  EXPECT_EQ(decompressed.get(), 'a');
  EXPECT_EQ(decompressed.peek(), 'b');
  EXPECT_EQ(decompressed.get(), 'b');
  EXPECT_EQ(decompressed.get(), 'c');
  EXPECT_EQ(decompressed.peek(), 'd');
  EXPECT_EQ(decompressed.get(), 'd');
  EXPECT_EQ(decompressed.get(), 'e');
  EXPECT_EQ(decompressed.get(), 'f');
  EXPECT_EQ(decompressed.peek(), std::char_traits<char>::eof());
}

TEST(GZIP_stream_buffer, read_chunk_equal_to_internal_buffer) {
  std::string input;
  input.resize(50000);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 16384> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }
  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, read_chunk_larger_than_internal_buffer) {
  std::string input;
  input.resize(50000);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 40000> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }
  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, header_with_ftext_flag) {
  // RFC 1952 section 2.3 gzip stream with FLG.FTEXT (0x01) set.
  // FTEXT is informational, so the streambuf must accept and ignore it.
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,             // ID1, ID2 (gzip magic)
      0x08,                   // CM = 8 (deflate)
      0x01,                   // FLG = FTEXT
      0x00, 0x00, 0x00, 0x00, // MTIME
      0x00,                   // XFL
      0xff,                   // OS = unknown
      // Deflate stored block for "hello world"
      0x01,       // BFINAL=1, BTYPE=00
      0x0b, 0x00, // LEN = 11
      0xf4, 0xff, // NLEN = ~LEN
      'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
      // Trailer
      0x85, 0x11, 0x4a, 0x0d,  // CRC32 of "hello world" little-endian
      0x0b, 0x00, 0x00, 0x00}; // ISIZE = 11 little-endian
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fname) {
  // FLG.FNAME (0x08) introduces a null-terminated filename after the fixed
  // 10-byte header
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,                                         // ID1, ID2
      0x08,                                               // CM
      0x08,                                               // FLG = FNAME
      0x00, 0x00, 0x00, 0x00,                             // MTIME
      0x00,                                               // XFL
      0xff,                                               // OS
      'd',  'a',  't',  'a',  '.',  't',  'x', 't', 0x00, // FNAME
      0x01, 0x0b, 0x00, 0xf4, 0xff,                       // stored block header
      'h',  'e',  'l',  'l',  'o',  ' ',  'w', 'o', 'r',
      'l',  'd',  0x85, 0x11, 0x4a, 0x0d, // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00};            // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fcomment) {
  // FLG.FCOMMENT (0x10) introduces a null-terminated comment after the fixed
  // 10-byte header
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,             // ID1, ID2
      0x08,                   // CM
      0x10,                   // FLG = FCOMMENT
      0x00, 0x00, 0x00, 0x00, // MTIME
      0x00,                   // XFL
      0xff,                   // OS
      't',  'h',  'i',  's',  ' ',  'i',  's', ' ', 'a',
      ' ',  'c',  'o',  'm',  'm',  'e',  'n', 't', 0x00, // FCOMMENT
      0x01, 0x0b, 0x00, 0xf4, 0xff,                       // stored block header
      'h',  'e',  'l',  'l',  'o',  ' ',  'w', 'o', 'r',
      'l',  'd',  0x85, 0x11, 0x4a, 0x0d, // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00};            // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fextra) {
  // FLG.FEXTRA (0x04) introduces a length-prefixed extra field after the
  // fixed 10-byte header. XLEN is a little-endian 16-bit count of the
  // following bytes
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,                   // ID1, ID2
      0x08,                         // CM
      0x04,                         // FLG = FEXTRA
      0x00, 0x00, 0x00, 0x00,       // MTIME
      0x00,                         // XFL
      0xff,                         // OS
      0x05, 0x00,                   // XLEN = 5 little-endian
      'E',  'X',  'T',  'R',  'A',  // FEXTRA data
      0x01, 0x0b, 0x00, 0xf4, 0xff, // stored block header
      'h',  'e',  'l',  'l',  'o',  ' ',  'w',  'o',
      'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00};                  // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fhcrc) {
  // FLG.FHCRC (0x02) appends a 16-bit CRC of the preceding header bytes.
  // The 16-bit CRC is the low 16 bits of the CRC-32 of the 10 header bytes
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,                   // ID1, ID2
      0x08,                         // CM
      0x02,                         // FLG = FHCRC
      0x00, 0x00, 0x00, 0x00,       // MTIME
      0x00,                         // XFL
      0xff,                         // OS
      0x90, 0xc9,                   // FHCRC over preceding 10 bytes
      0x01, 0x0b, 0x00, 0xf4, 0xff, // stored block header
      'h',  'e',  'l',  'l',  'o',  ' ',  'w',  'o',
      'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00};                  // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_all_optional_flags) {
  // Every optional FLG bit set at once. RFC 1952 fixes the order of optional
  // fields: FEXTRA, FNAME, FCOMMENT, FHCRC
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,             // ID1, ID2
      0x08,                   // CM
      0x1f,                   // FLG = FTEXT|FHCRC|FEXTRA|FNAME|FCOMMENT
      0x00, 0x00, 0x00, 0x00, // MTIME
      0x00,                   // XFL
      0xff,                   // OS
      0x04, 0x00,             // FEXTRA XLEN = 4
      0xaa, 0xbb, 0xcc, 0xdd, // FEXTRA data
      'n',  'a',  'm',  'e',  '.',  't',  'x', 't',  0x00, // FNAME
      'c',  'o',  'm',  'm',  'e',  'n',  't', 0x00,       // FCOMMENT
      0x90, 0x3e,                   // FHCRC over all preceding header bytes
      0x01, 0x0b, 0x00, 0xf4, 0xff, // stored block header
      'h',  'e',  'l',  'l',  'o',  ' ',  'w', 'o',  'r',
      'l',  'd',  0x85, 0x11, 0x4a, 0x0d, // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00};            // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, wrong_first_magic_byte_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[0] = static_cast<char>(0x00);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, wrong_second_magic_byte_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[1] = static_cast<char>(0x00);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, unsupported_compression_method_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[2] = static_cast<char>(0x09);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, reserved_flag_bit_5_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x20);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, reserved_flag_bit_6_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x40);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, reserved_flag_bit_7_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x80);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, corrupted_trailing_crc32_throws) {
  const std::string input{
      "hello world, this is long enough to produce a real deflate body"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[compressed.size() - 8] =
      static_cast<char>(compressed[compressed.size() - 8] ^ 0xff);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, corrupted_trailing_isize_throws) {
  const std::string input{
      "hello world, this is long enough to produce a real deflate body"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[compressed.size() - 1] =
      static_cast<char>(compressed[compressed.size() - 1] ^ 0xff);
  EXPECT_THROW(decompress_via_stream(compressed), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, empty_source_stream_throws) {
  EXPECT_THROW(decompress_via_stream(""), sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, trailing_data_after_member_is_ignored) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const std::string padded{compressed + "garbage data after gzip end"};
  const auto result{decompress_via_stream(padded)};
  EXPECT_EQ(result, input);
}

// TODO: Enable once the streaming decompressor supports multi-member gzip
// streams as required by RFC 1952 section 2.2
TEST(GZIP_stream_buffer, DISABLED_two_concatenated_members) {
  const std::string first{"hello"};
  const std::string second{" world"};
  const auto first_gzip{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(first.data()), first.size())};
  const auto second_gzip{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(second.data()), second.size())};
  const std::string combined{first_gzip + second_gzip};
  const auto result{decompress_via_stream(combined)};
  EXPECT_EQ(result, first + second);
}

// TODO: Enable once the streaming decompressor supports multi-member gzip
// streams as required by RFC 1952 section 2.2
TEST(GZIP_stream_buffer, DISABLED_three_concatenated_members_with_empty) {
  const std::string first{"foo"};
  const std::string second;
  const std::string third{"baz"};
  const auto first_gzip{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(first.data()), first.size())};
  const auto second_gzip{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(second.data()), second.size())};
  const auto third_gzip{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(third.data()), third.size())};
  const std::string combined{first_gzip + second_gzip + third_gzip};
  const auto result{decompress_via_stream(combined)};
  EXPECT_EQ(result, first + second + third);
}

// TODO: Enable once the streaming decompressor rejects a member truncated
// before its deflate body instead of silently returning empty output
TEST(GZIP_stream_buffer, DISABLED_header_only_source_throws) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const std::string header_only{compressed.substr(0, 10)};
  EXPECT_THROW(decompress_via_stream(header_only), sourcemeta::core::GZIPError);
}
