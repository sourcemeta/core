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
  try {
    decompress_via_stream(garbage);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Invalid gzip magic bytes");
  }
}

TEST(GZIP_stream_buffer, truncated_input_throws) {
  const std::string input{"hello world, this needs to be long enough"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed.resize(compressed.size() / 2);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unexpected end of source stream");
  }
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
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Invalid gzip magic bytes");
  }
}

TEST(GZIP_stream_buffer, wrong_second_magic_byte_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[1] = static_cast<char>(0x00);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Invalid gzip magic bytes");
  }
}

TEST(GZIP_stream_buffer, unsupported_compression_method_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[2] = static_cast<char>(0x09);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unsupported gzip compression method");
  }
}

TEST(GZIP_stream_buffer, reserved_flag_bit_5_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x20);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Reserved gzip FLG bits must be zero");
  }
}

TEST(GZIP_stream_buffer, reserved_flag_bit_6_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x40);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Reserved gzip FLG bits must be zero");
  }
}

TEST(GZIP_stream_buffer, reserved_flag_bit_7_set_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[3] = static_cast<char>(0x80);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Reserved gzip FLG bits must be zero");
  }
}

TEST(GZIP_stream_buffer, corrupted_trailing_crc32_throws) {
  const std::string input{
      "hello world, this is long enough to produce a real deflate body"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[compressed.size() - 8] =
      static_cast<char>(compressed[compressed.size() - 8] ^ 0xff);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Gzip member CRC32 mismatch");
  }
}

TEST(GZIP_stream_buffer, corrupted_trailing_isize_throws) {
  const std::string input{
      "hello world, this is long enough to produce a real deflate body"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed[compressed.size() - 1] =
      static_cast<char>(compressed[compressed.size() - 1] ^ 0xff);
  try {
    decompress_via_stream(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Gzip member ISIZE mismatch");
  }
}

TEST(GZIP_stream_buffer, empty_source_stream_throws) {
  try {
    decompress_via_stream("");
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Empty source stream");
  }
}

TEST(GZIP_stream_buffer, trailing_data_after_member_is_ignored) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const std::string padded{compressed + "garbage data after gzip end"};
  const auto result{decompress_via_stream(padded)};
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, fhcrc_mismatch_throws) {
  // RFC 1952 section 2.3.1.2: a compliant decoder must reject an FHCRC
  // mismatch. The correct CRC16 over the preceding 10 header bytes is
  // 0x90 0xc9, so flipping a bit must cause a hard failure
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b,             // ID1, ID2
      0x08,                   // CM
      0x02,                   // FLG = FHCRC
      0x00, 0x00, 0x00, 0x00, // MTIME
      0x00,                   // XFL
      0xff,                   // OS
      0x91, 0xc9,             // FHCRC corrupted (correct is 0x90 0xc9)
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',  'l',  'o',  ' ',  'w',
      'o',  'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "FHCRC mismatch");
  }
}

TEST(GZIP_stream_buffer, multiple_deflate_blocks) {
  // RFC 1951 allows a deflate stream to contain multiple blocks. Only the
  // last block has BFINAL set. This fixture splits "hello world" into two
  // stored blocks
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
      // Block 1 (non-final stored)
      0x00,       // BFINAL=0, BTYPE=00 (stored)
      0x05, 0x00, // LEN = 5
      0xfa, 0xff, // NLEN = ~5
      'h', 'e', 'l', 'l', 'o',
      // Block 2 (final stored)
      0x01,       // BFINAL=1, BTYPE=00
      0x06, 0x00, // LEN = 6
      0xf9, 0xff, // NLEN = ~6
      ' ', 'w', 'o', 'r', 'l', 'd', 0x85, 0x11, 0x4a,
      0x0d,                    // CRC32 little-endian
      0x0b, 0x00, 0x00, 0x00}; // ISIZE
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, fextra_with_zero_xlen) {
  // RFC 1952: XLEN=0 is a valid FEXTRA encoding (no extra-field bytes)
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xff, 0x00, 0x00, // XLEN = 0
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',
      'l',  'o',  ' ',  'w',  'o',  'r',  'l',  'd',
      0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, mtime_nonzero_is_accepted) {
  // RFC 1952 section 2.3.1.2: MTIME is informational and any value, including
  // non-zero, must be accepted
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x78, 0x56, 0x34, 0x12, // MTIME = 0x12345678
                                                      // little-endian
      0x00, 0xff, 0x01, 0x0b, 0x00, 0xf4, 0xff, 'h', 'e', 'l', 'l', 'o', ' ',
      'w', 'o', 'r', 'l', 'd', 0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, xfl_max_compression_is_accepted) {
  // RFC 1952 section 2.3.1.2: XFL=2 indicates maximum compression
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, // XFL = 2 (max compression)
      0xff, 0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',
      'l',  'o',  ' ',  'w',  'o',  'r',  'l',  'd',  0x85,
      0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, xfl_fastest_compression_is_accepted) {
  // RFC 1952 section 2.3.1.2: XFL=4 indicates fastest compression
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x04, // XFL = 4 (fastest)
      0xff, 0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',
      'l',  'o',  ' ',  'w',  'o',  'r',  'l',  'd',  0x85,
      0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, os_field_unix_is_accepted) {
  // RFC 1952 section 2.3.1.2: OS=3 indicates Unix; decoders must accept any
  // value
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x03, // OS = Unix
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',  'l',  'o',  ' ',  'w',
      'o',  'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, fname_with_latin1_high_bytes) {
  // RFC 1952 section 2.3.1.2: FNAME is ISO 8859-1 (Latin-1), so bytes in the
  // 0x80 to 0xff range must be accepted
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xa9, 0xc6,
      0xff, 0x00, // FNAME = three Latin-1 bytes + null terminator
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',  'l',  'o',  ' ',  'w',
      'o',  'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, fcomment_with_latin1_high_bytes) {
  // RFC 1952 section 2.3.1.2: FCOMMENT is ISO 8859-1
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0xc1,
      0xfe, 0x00, // FCOMMENT with Latin-1 high bytes
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',  'l',  'o',  ' ',  'w',
      'o',  'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, read_via_getline) {
  const std::string input{"line one\nline two\nline three"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string first_line;
  std::getline(decompressed, first_line);
  EXPECT_EQ(first_line, "line one");
  std::string second_line;
  std::getline(decompressed, second_line);
  EXPECT_EQ(second_line, "line two");
  std::string third_line;
  std::getline(decompressed, third_line);
  EXPECT_EQ(third_line, "line three");
}

TEST(GZIP_stream_buffer, ignore_skips_bytes) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  decompressed.ignore(6);
  std::string remaining;
  remaining.assign(std::istreambuf_iterator<char>(decompressed),
                   std::istreambuf_iterator<char>());
  EXPECT_EQ(remaining, "world");
}

TEST(GZIP_stream_buffer, operator_in_extracts_tokens) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string first;
  std::string second;
  decompressed >> first >> second;
  EXPECT_EQ(first, "hello");
  EXPECT_EQ(second, "world");
}

TEST(GZIP_stream_buffer, stored_block_with_empty_non_final_then_data) {
  // RFC 1951 section 3.2.4: a stored block with LEN=0 is well-formed
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
      // Non-final empty stored block
      0x00,       // BFINAL=0, BTYPE=00
      0x00, 0x00, // LEN = 0
      0xff, 0xff, // NLEN = ~0
      // Final stored block with the actual payload
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r',
      'l', 'd', 0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, empty_payload_via_empty_final_stored_block) {
  // Hand-crafted minimum valid gzip member encoding an empty payload
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
      0x01,                    // BFINAL=1, BTYPE=00
      0x00, 0x00,              // LEN = 0
      0xff, 0xff,              // NLEN = ~0
      0x00, 0x00, 0x00, 0x00,  // CRC32 of empty input
      0x00, 0x00, 0x00, 0x00}; // ISIZE = 0
  EXPECT_TRUE(decompress_via_stream(stream).empty());
}

TEST(GZIP_stream_buffer, header_with_fextra_and_fname) {
  // RFC 1952 fixes the order: FEXTRA precedes FNAME
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x0c, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xff, 0x03, 0x00,       // FEXTRA XLEN = 3
      'x',  'y',  'z',              // FEXTRA data
      'n',  'a',  'm',  'e',  0x00, // FNAME
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',
      'l',  'o',  ' ',  'w',  'o',  'r',  'l',  'd',
      0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fextra_and_fcomment) {
  // RFC 1952 fixes the order: FEXTRA precedes FCOMMENT
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x14, 0x00, 0x00, 0x00, 0x00,
      0x00, 0xff, 0x02, 0x00, // FEXTRA XLEN = 2
      'A',  'B',              // FEXTRA data
      'h',  'i',  0x00,       // FCOMMENT
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',
      'l',  'o',  ' ',  'w',  'o',  'r',  'l',  'd',
      0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, header_with_fname_and_fcomment) {
  // RFC 1952 fixes the order: FNAME precedes FCOMMENT
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 'a',  '.',
      't',  'x',  't',  0x00, // FNAME
      'c',  'm',  't',  0x00, // FCOMMENT
      0x01, 0x0b, 0x00, 0xf4, 0xff, 'h',  'e',  'l',  'l',  'o',  ' ',  'w',
      'o',  'r',  'l',  'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(decompress_via_stream(stream), "hello world");
}

TEST(GZIP_stream_buffer, reserved_deflate_block_type_throws) {
  // RFC 1951 section 3.2.3: BTYPE=11 is reserved and a compliant decoder
  // must reject it. Deflate packs bits LSB-first, so BFINAL=1 + BTYPE=11
  // is the byte 0b00000111 = 0x07
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
      0x07,                   // BFINAL=1, BTYPE=11 (reserved/invalid)
      0x00, 0x00, 0x00, 0x00, // arbitrary trailing bytes
      0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Reserved deflate block type");
  }
}

TEST(GZIP_stream_buffer, stored_block_with_mismatched_nlen_throws) {
  // RFC 1951 section 3.2.4: NLEN must be the one's complement of LEN
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
      0x01,       // BFINAL=1, BTYPE=00
      0x0b, 0x00, // LEN = 11
      0x00, 0x00, // NLEN corrupted (must be 0xf4 0xff)
      'h',  'e',  'l',  'l',  'o',  ' ',  'w',  'o',  'r',  'l',
      'd',  0x85, 0x11, 0x4a, 0x0d, 0x0b, 0x00, 0x00, 0x00};
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Stored block LEN/NLEN mismatch");
  }
}

TEST(GZIP_stream_buffer, two_concatenated_members) {
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

TEST(GZIP_stream_buffer, three_concatenated_members_with_empty) {
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

TEST(GZIP_stream_buffer, header_only_source_throws) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const std::string header_only{compressed.substr(0, 10)};
  try {
    decompress_via_stream(header_only);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unexpected end of source stream");
  }
}

TEST(GZIP_stream_buffer, truncated_mid_fname_throws) {
  // FLG.FNAME set but the source stream ends before the null terminator
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x08, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xff, 'd',  'a',  't'}; // no null terminator
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unexpected end of source stream");
  }
}

TEST(GZIP_stream_buffer, truncated_mid_fcomment_throws) {
  // FLG.FCOMMENT set but the source stream ends before the null terminator
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x10, 0x00, 0x00, 0x00,
      0x00, 0x00, 0xff, 'c',  'm',  't'}; // no null terminator
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unexpected end of source stream");
  }
}

TEST(GZIP_stream_buffer, truncated_mid_fextra_throws) {
  // FLG.FEXTRA with XLEN=10 but only 3 bytes of extra-field data present
  sourcemeta::core::InputByteStream stream{
      0x1f, 0x8b, 0x08, 0x04, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xff, 0x0a, 0x00, // XLEN = 10
      'A',  'B',  'C'};                   // only 3 of 10 promised bytes present
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Unexpected end of source stream");
  }
}

TEST(GZIP_stream_buffer, fextra_spans_internal_buffer) {
  std::string compressed;
  compressed.append("\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff", 10);
  const std::size_t extra_size{20000};
  compressed.push_back(static_cast<char>(extra_size & 0xff));
  compressed.push_back(static_cast<char>((extra_size >> 8) & 0xff));
  compressed.append(extra_size, '\0');
  compressed.append("\x01\x0b\x00\xf4\xff", 5);
  compressed.append("hello world", 11);
  compressed.append("\x85\x11\x4a\x0d\x0b\x00\x00\x00", 8);
  EXPECT_EQ(decompress_via_stream(compressed), "hello world");
}

TEST(GZIP_stream_buffer, dynamic_block_hlit_above_286_throws) {
  // RFC 1951 section 3.2.7 caps the literal/length alphabet at 286 symbols.
  // The deflate payload starts a dynamic block with HLIT encoding 288 codes
  // (BFINAL=1, BTYPE=10, HLIT field = 31 so 31 + 257 = 288)
  sourcemeta::core::InputByteStream stream{0x1f, 0x8b, 0x08, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0xff,
                                           0xfd, 0x00, 0x00};
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Too many literal/length codes");
  }
}

TEST(GZIP_stream_buffer, dynamic_block_incomplete_code_throws) {
  // RFC 1951 forbids incomplete Huffman codes outside the single-code case.
  // The deflate payload starts a dynamic block whose code-length code tree
  // assigns length two to two symbols, leaving the alphabet incomplete
  // (BFINAL=1, BTYPE=10, HLIT=0, HDIST=0, HCLEN=0, code-length lengths for
  // symbols 16 and 17 set to two)
  sourcemeta::core::InputByteStream stream{0x1f, 0x8b, 0x08, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0xff,
                                           0x05, 0x00, 0x24, 0x00};
  try {
    decompress_via_stream(stream);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Incomplete Huffman code");
  }
}
