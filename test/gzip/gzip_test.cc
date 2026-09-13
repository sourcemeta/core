#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <array>            // std::array
#include <cstddef>          // std::size_t
#include <cstdint>          // std::uint8_t, std::uint32_t
#include <cstring>          // std::memcmp
#include <initializer_list> // std::initializer_list
#include <istream>          // std::istream
#include <iterator>         // std::istreambuf_iterator
#include <limits>           // std::numeric_limits
#include <random>           // std::mt19937, std::uniform_int_distribution
#include <sstream>          // std::istringstream
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <utility>          // std::move, std::pair
#include <vector>           // std::vector

namespace {

constexpr std::string_view GZIP_HEADER{
    "\x1f\x8b\x08\x00\x00\x00\x00\x00\x00\xff", 10};

// Packs data elements starting at the least-significant bit of each byte and
// Huffman codes starting at their most-significant bit, as per RFC 1951
// section 3.1.1
class DeflateWriter {
public:
  auto bits(const std::uint32_t value, const std::size_t count) -> void {
    for (std::size_t index = 0; index < count; ++index) {
      this->push((value >> index) & 1U);
    }
  }

  auto code(const std::uint32_t value, const std::size_t length) -> void {
    for (std::size_t index = length; index > 0; --index) {
      this->push((value >> (index - 1)) & 1U);
    }
  }

  auto align() -> void { this->bit_position_ = 0; }

  auto bytes(const std::string_view data) -> void {
    this->align();
    this->data_.append(data);
  }

  [[nodiscard]] auto data() const -> const std::string & { return this->data_; }

private:
  auto push(const std::uint32_t bit) -> void {
    if (this->bit_position_ == 0) {
      this->data_.push_back('\0');
    }

    if (bit != 0) {
      this->data_.back() =
          static_cast<char>(static_cast<unsigned char>(this->data_.back()) |
                            (1U << this->bit_position_));
    }

    this->bit_position_ = (this->bit_position_ + 1) % 8;
  }

  std::string data_;
  std::size_t bit_position_{0};
};

// Assigns canonical codes to a sequence of code lengths as per RFC 1951
// section 3.2.2
class HuffmanCode {
public:
  HuffmanCode(std::vector<std::uint8_t> lengths)
      : lengths_{std::move(lengths)} {
    std::array<std::uint32_t, 16> length_count{};
    for (const auto length : this->lengths_) {
      length_count[length] += 1;
    }

    length_count[0] = 0;
    std::array<std::uint32_t, 16> next_code{};
    std::uint32_t code{0};
    for (std::size_t bits = 1; bits < next_code.size(); ++bits) {
      code = (code + length_count[bits - 1]) << 1;
      next_code[bits] = code;
    }

    this->codes_.resize(this->lengths_.size());
    for (std::size_t symbol = 0; symbol < this->lengths_.size(); ++symbol) {
      const auto length{this->lengths_[symbol]};
      if (length != 0) {
        this->codes_[symbol] = next_code[length];
        next_code[length] += 1;
      }
    }
  }

  auto write(DeflateWriter &writer, const std::size_t symbol) const -> void {
    writer.code(this->codes_.at(symbol), this->lengths_.at(symbol));
  }

private:
  std::vector<std::uint8_t> lengths_;
  std::vector<std::uint32_t> codes_;
};

auto code_lengths(
    const std::size_t count,
    const std::initializer_list<std::pair<std::size_t, std::uint8_t>>
        assignments) -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> result;
  result.resize(count);
  for (const auto &assignment : assignments) {
    result.at(assignment.first) = assignment.second;
  }

  return result;
}

auto write_block_header(DeflateWriter &writer, const bool final,
                        const std::uint32_t type) -> void {
  writer.bits(final ? 1U : 0U, 1);
  writer.bits(type, 2);
}

auto write_stored_block(DeflateWriter &writer, const bool final,
                        const std::string_view payload) -> void {
  write_block_header(writer, final, 0);
  writer.align();
  writer.bits(static_cast<std::uint32_t>(payload.size()), 16);
  writer.bits(~static_cast<std::uint32_t>(payload.size()) & 0xffffU, 16);
  writer.bytes(payload);
}

// The fixed literal/length code of RFC 1951 section 3.2.6
auto write_fixed_literal_length(DeflateWriter &writer,
                                const std::uint32_t symbol) -> void {
  if (symbol <= 143) {
    writer.code(0x30 + symbol, 8);
  } else if (symbol <= 255) {
    writer.code(0x190 + symbol - 144, 9);
  } else if (symbol <= 279) {
    writer.code(symbol - 256, 7);
  } else {
    writer.code(0xc0 + symbol - 280, 8);
  }
}

auto write_fixed_literals(DeflateWriter &writer, const std::string_view data)
    -> void {
  for (const auto character : data) {
    write_fixed_literal_length(writer, static_cast<unsigned char>(character));
  }
}

// Transmits every code length explicitly through a code length code that
// assigns four bits to each of the lengths zero to fifteen
auto write_dynamic_header(
    DeflateWriter &writer, const bool final,
    const std::vector<std::uint8_t> &literal_length_lengths,
    const std::vector<std::uint8_t> &distance_lengths) -> void {
  write_block_header(writer, final, 2);
  writer.bits(static_cast<std::uint32_t>(literal_length_lengths.size() - 257),
              5);
  writer.bits(static_cast<std::uint32_t>(distance_lengths.size() - 1), 5);
  writer.bits(15, 4);
  writer.bits(0, 3);
  writer.bits(0, 3);
  writer.bits(0, 3);
  for (std::size_t index = 0; index < 16; ++index) {
    writer.bits(4, 3);
  }

  for (const auto length : literal_length_lengths) {
    writer.code(length, 4);
  }

  for (const auto length : distance_lengths) {
    writer.code(length, 4);
  }
}

auto append_little_endian(std::string &output, const std::uint32_t value)
    -> void {
  output.push_back(static_cast<char>(value & 0xff));
  output.push_back(static_cast<char>((value >> 8) & 0xff));
  output.push_back(static_cast<char>((value >> 16) & 0xff));
  output.push_back(static_cast<char>((value >> 24) & 0xff));
}

auto gzip_member(const std::string_view header, const std::string_view deflate,
                 const std::string_view payload) -> std::string {
  std::string result{header};
  result.append(deflate);
  append_little_endian(result, sourcemeta::core::crc32(payload));
  append_little_endian(result, static_cast<std::uint32_t>(payload.size()));
  return result;
}

auto compress(const std::string_view input, const int level) -> std::string {
  return sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(),
      level);
}

auto decompress(const std::string_view compressed) -> std::string {
  return sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size());
}

auto decompress(const std::string_view compressed,
                const std::size_t output_hint, const std::size_t maximum_size)
    -> std::string {
  return sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), output_hint, maximum_size);
}

// Decodes through the independent streaming decoder of this module
auto decompress_via_stream(const std::string &compressed) -> std::string {
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};
  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  return result;
}

auto random_bytes(const std::size_t size, const std::uint32_t seed)
    -> std::string {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{seed};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string result;
  result.resize(size);
  for (auto &character : result) {
    character = static_cast<char>(distribution(generator));
  }

  return result;
}

auto mixed_corpus() -> std::string {
  std::string result;
  for (std::size_t index = 0; index < 2000; ++index) {
    result += "The quick brown fox jumps over the lazy dog. ";
  }

  result += random_bytes(40000, 7);
  result.append(70000, '\0');
  for (std::size_t index = 0; index < 5000; ++index) {
    result += "ab";
  }

  for (std::size_t index = 0; index < 256; ++index) {
    result.append(index + 1, static_cast<char>(index));
  }

  return result;
}

} // namespace

TEST(compress_empty_input) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_hello_world) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());
  EXPECT_LT(compressed.size(), input.size() + 30);

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_round_trip_binary_data) {
  std::string input;
  input.resize(256);
  for (unsigned int index = 0; index < 256; ++index) {
    input[index] = static_cast<char>(index);
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed.size(), input.size());
  EXPECT_EQ(std::memcmp(decompressed.data(), input.data(), input.size()), 0);
}

TEST(compress_round_trip_large_input) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());
  EXPECT_LT(compressed.size(), input.size());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(decompress_with_output_hint) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), input.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_with_explicit_level_round_trips) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 9)};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_higher_level_is_not_larger) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto fastest{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 1)};
  const auto smallest{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 12)};
  EXPECT_LE(smallest.size(), fastest.size());
}

TEST(decompress_invalid_input_throws) {
  const std::string garbage{"this is not gzip data"};
  try {
    sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(garbage.data()), garbage.size());
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_beyond_maximum_size_throws) {
  const std::string input{"Hello, World! Highly compressible content here."};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  try {
    sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(compressed.data()),
        compressed.size(), 0, 4);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_within_maximum_size_succeeds) {
  const std::string input{"Hello, World! Highly compressible content here."};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), 0, 1024)};
  EXPECT_EQ(decompressed, input);
}

TEST(decompress_concatenated_members) {
  const std::string first{"hello "};
  const std::string second{"world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(first.data()), first.size())};
  compressed += sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(second.data()), second.size());
  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, "hello world");
}

TEST(decompress_concatenated_members_grows_across_members) {
  // A tiny hint forces the second member to grow the buffer that already holds
  // the first, exercising the multi-member growth path
  const std::string first{"hello "};
  const std::string second{"world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(first.data()), first.size())};
  compressed += sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(second.data()), second.size());
  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), 1, 1024)};
  EXPECT_EQ(decompressed, "hello world");
}

TEST(decompress_ignores_trailing_data) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed += "not a gzip member";
  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(decompress_corrupt_trailing_member_throws) {
  const std::string input{"hello world"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed.push_back('\x1F');
  compressed.push_back('\x8B');
  compressed.append("corrupt");
  try {
    sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(compressed.data()),
        compressed.size());
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(compress_level_0_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 0)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_1_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_2_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 2)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_3_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 3)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_4_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 4)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_5_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 5)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_6_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 6)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_7_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 7)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_8_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 8)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_9_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 9)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_10_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 10)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_11_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 11)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_12_round_trips) {
  const auto input{mixed_corpus()};
  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_above_maximum_throws) {
  try {
    compress("hello world", 13);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not allocate compressor");
  }
}

TEST(compress_negative_level_throws) {
  try {
    compress("hello world", -2);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not allocate compressor");
  }
}

TEST(compress_largest_integer_level_throws) {
  try {
    compress("hello world", std::numeric_limits<int>::max());
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not allocate compressor");
  }
}

TEST(compress_smallest_integer_level_throws) {
  try {
    compress("hello world", std::numeric_limits<int>::min());
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not allocate compressor");
  }
}

TEST(compress_output_header_identifies_deflate_member) {
  const auto compressed{compress("hello world", 6)};
  EXPECT_GE(compressed.size(), 18);
  EXPECT_EQ(compressed.at(0), '\x1f');
  EXPECT_EQ(compressed.at(1), '\x8b');
  EXPECT_EQ(compressed.at(2), '\x08');
  EXPECT_EQ(static_cast<unsigned char>(compressed.at(3)) & 0xe0U, 0U);
}

TEST(compress_empty_input_output_header_identifies_deflate_member) {
  const auto compressed{compress("", 0)};
  EXPECT_GE(compressed.size(), 18);
  EXPECT_EQ(compressed.at(0), '\x1f');
  EXPECT_EQ(compressed.at(1), '\x8b');
  EXPECT_EQ(compressed.at(2), '\x08');
  EXPECT_EQ(static_cast<unsigned char>(compressed.at(3)) & 0xe0U, 0U);
}

TEST(compress_output_trailer_of_empty_input) {
  const auto compressed{compress("", 1)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8), std::string(8, '\0'));
}

TEST(compress_output_trailer_of_hello_world_at_level_0) {
  const auto compressed{compress("hello world", 0)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8),
            std::string("\x85\x11\x4a\x0d\x0b\x00\x00\x00", 8));
}

TEST(compress_output_trailer_of_hello_world_at_level_1) {
  const auto compressed{compress("hello world", 1)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8),
            std::string("\x85\x11\x4a\x0d\x0b\x00\x00\x00", 8));
}

TEST(compress_output_trailer_of_hello_world_at_level_12) {
  const auto compressed{compress("hello world", 12)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8),
            std::string("\x85\x11\x4a\x0d\x0b\x00\x00\x00", 8));
}

TEST(compress_output_trailer_of_crc32_check_string) {
  const auto compressed{compress("123456789", 6)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8),
            std::string("\x26\x39\xf4\xcb\x09\x00\x00\x00", 8));
}

TEST(compress_output_trailer_of_pangram) {
  const auto compressed{
      compress("The quick brown fox jumps over the lazy dog", 9)};
  EXPECT_EQ(compressed.substr(compressed.size() - 8),
            std::string("\x39\xa3\x4f\x41\x2b\x00\x00\x00", 8));
}

TEST(compress_output_trailer_size_spans_multiple_bytes) {
  const std::string input(70000, 'z');
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(compressed.substr(compressed.size() - 4),
            std::string("\x70\x11\x01\x00", 4));
}

TEST(compress_single_zero_byte_round_trips) {
  const std::string input(1, '\0');
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_minimum_match_length_input_round_trips) {
  const std::string input{"abcabc"};
  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_run_of_maximum_match_length_round_trips) {
  const std::string input(259, 'q');
  const auto compressed{compress(input, 6)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_run_one_past_maximum_match_length_round_trips) {
  const std::string input(260, 'q');
  const auto compressed{compress(input, 6)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_input_one_below_window_size_round_trips) {
  const auto input{random_bytes(32767, 1)};
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_input_of_window_size_round_trips) {
  const auto input{random_bytes(32768, 1)};
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_input_one_above_window_size_round_trips) {
  const auto input{random_bytes(32769, 1)};
  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_0_input_of_maximum_stored_block_length_round_trips) {
  const auto input{random_bytes(65535, 2)};
  const auto compressed{compress(input, 0)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_0_input_one_above_maximum_stored_block_length_round_trips) {
  const auto input{random_bytes(65536, 2)};
  const auto compressed{compress(input, 0)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_level_0_input_of_two_maximum_stored_blocks_round_trips) {
  const auto input{random_bytes(131070, 2)};
  const auto compressed{compress(input, 0)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_incompressible_input_at_level_0_barely_expands) {
  const auto input{random_bytes(1048576, 3)};
  const auto compressed{compress(input, 0)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_incompressible_input_at_level_1_barely_expands) {
  const auto input{random_bytes(1048576, 3)};
  const auto compressed{compress(input, 1)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_incompressible_input_at_level_6_barely_expands) {
  const auto input{random_bytes(1048576, 3)};
  const auto compressed{compress(input, 6)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_incompressible_input_at_level_12_barely_expands) {
  const auto input{random_bytes(1048576, 3)};
  const auto compressed{compress(input, 12)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_eight_megabytes_of_zeros_at_level_1) {
  const std::string input(8388608, '\0');
  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_four_megabytes_of_zeros_at_level_12) {
  const std::string input(4194304, '\0');
  const auto compressed{compress(input, 12)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_alternating_bytes_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 524288; ++index) {
    input += "\x01\xfe";
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_block_repeated_at_window_size_distance_at_level_6) {
  const auto block{random_bytes(32768, 4)};
  const auto input{block + block + block + block};
  const auto compressed{compress(input, 6)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_block_repeated_at_window_size_distance_at_level_12) {
  const auto block{random_bytes(32768, 4)};
  const auto input{block + block + block + block};
  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_block_repeated_beyond_window_size_distance) {
  const auto block{random_bytes(32769, 5)};
  const auto input{block + block + block + block};
  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_block_repeated_at_maximum_match_length_period) {
  const auto block{random_bytes(258, 6)};
  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += block;
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_random_block_repeated_beyond_maximum_match_length_period) {
  const auto block{random_bytes(259, 6)};
  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += block;
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_runs_of_every_match_length_round_trips) {
  std::string input;
  for (std::size_t length = 1; length <= 300; ++length) {
    input.append(length, static_cast<char>(length & 0xff));
    input.append(length, static_cast<char>(~length & 0xff));
  }

  const auto compressed{compress(input, 9)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_fibonacci_word_at_level_1) {
  std::string previous{"b"};
  std::string input{"a"};
  while (input.size() < 1048576) {
    auto next{input + previous};
    previous = std::move(input);
    input = std::move(next);
  }

  const auto compressed{compress(input, 1)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_fibonacci_word_at_level_12) {
  std::string previous{"b"};
  std::string input{"a"};
  while (input.size() < 1048576) {
    auto next{input + previous};
    previous = std::move(input);
    input = std::move(next);
  }

  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_counter_sequence_round_trips) {
  std::string input;
  for (std::uint32_t index = 0; index < 262144; ++index) {
    append_little_endian(input, index);
  }

  const auto compressed{compress(input, 6)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_already_compressed_input_round_trips) {
  const auto input{compress(mixed_corpus(), 12)};
  const auto compressed{compress(input, 12)};
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_sixteen_megabytes_round_trips) {
  const auto corpus{mixed_corpus()};
  std::string input;
  while (input.size() < 16777216) {
    input += corpus;
  }

  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size());
  EXPECT_EQ(decompress(compressed), input);
  EXPECT_EQ(decompress_via_stream(compressed), input);
}

TEST(compress_members_of_different_levels_concatenate) {
  const auto first{random_bytes(70000, 8)};
  const std::string second(100000, 'x');
  const auto compressed{compress(first, 0) + compress(second, 12) +
                        compress("", 6) + compress("end", 1)};
  EXPECT_EQ(decompress(compressed), first + second + "end");
  EXPECT_EQ(decompress_via_stream(compressed), first + second + "end");
}

TEST(decompress_header_with_ftext) {
  const std::string header{"\x1f\x8b\x08\x01\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_valid_fhcrc) {
  const std::string header{"\x1f\x8b\x08\x02\x00\x00\x00\x00\x00\xff\x90\xc9",
                           12};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fextra) {
  std::string header{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff\x05\x00", 12};
  header += "EXTRA";
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_empty_fextra) {
  const std::string header{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff\x00\x00",
                           12};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_maximum_length_fextra) {
  std::string header{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff\xff\xff", 12};
  header += random_bytes(65535, 9);
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fextra_subfields) {
  std::string header{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff\x0a\x00", 12};
  header += "AB";
  header += std::string{"\x02\x00\x01\x02", 4};
  header += "CD";
  header += std::string{"\x00\x00", 2};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fextra_of_null_bytes_before_fname) {
  std::string header{"\x1f\x8b\x08\x0c\x00\x00\x00\x00\x00\xff\x03\x00", 12};
  header += std::string(3, '\0');
  header += "name";
  header.push_back('\0');
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fname) {
  std::string header{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\xff", 10};
  header += "data.txt";
  header.push_back('\0');
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_empty_fname) {
  const std::string header{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\xff\x00", 11};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_one_megabyte_fname) {
  std::string header{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\xff", 10};
  header.append(1048576, 'n');
  header.push_back('\0');
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fname_containing_gzip_magic) {
  std::string header{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\xff", 10};
  header += std::string{"\x1f\x8b\x08\x1f\x8b\x00", 6};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_fcomment) {
  std::string header{"\x1f\x8b\x08\x10\x00\x00\x00\x00\x00\xff", 10};
  header += "a comment\nwith two lines";
  header.push_back('\0');
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_empty_fcomment) {
  const std::string header{"\x1f\x8b\x08\x10\x00\x00\x00\x00\x00\xff\x00", 11};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_one_megabyte_fcomment) {
  std::string header{"\x1f\x8b\x08\x10\x00\x00\x00\x00\x00\xff", 10};
  header.append(1048576, 'c');
  header.push_back('\0');
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_every_optional_field) {
  std::string header{"\x1f\x8b\x08\x1f\x00\x00\x00\x00\x00\xff\x04\x00", 12};
  header += std::string{"\xaa\xbb\xcc\xdd", 4};
  header += "name.txt";
  header.push_back('\0');
  header += "comment";
  header.push_back('\0');
  header += std::string{"\x90\x3e", 2};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_header_with_nonzero_mtime_xfl_and_os) {
  const std::string header{"\x1f\x8b\x08\x00\x78\x56\x34\x12\x02\x03", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  EXPECT_EQ(decompress(gzip_member(header, writer.data(), "hello world")),
            "hello world");
}

TEST(decompress_wrong_first_identification_byte_throws) {
  const std::string header{"\x1e\x8b\x08\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_wrong_second_identification_byte_throws) {
  const std::string header{"\x1f\x8c\x08\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_swapped_identification_bytes_throws) {
  const std::string header{"\x8b\x1f\x08\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_compression_method_0_throws) {
  const std::string header{"\x1f\x8b\x00\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_compression_method_7_throws) {
  const std::string header{"\x1f\x8b\x07\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_unknown_compression_method_9_throws) {
  const std::string header{"\x1f\x8b\x09\x00\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_flag_bit_5_throws) {
  const std::string header{"\x1f\x8b\x08\x20\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_flag_bit_6_throws) {
  const std::string header{"\x1f\x8b\x08\x40\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_flag_bit_7_throws) {
  const std::string header{"\x1f\x8b\x08\x80\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_flag_bit_alongside_ftext_throws) {
  const std::string header{"\x1f\x8b\x08\x21\x00\x00\x00\x00\x00\xff", 10};
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  try {
    decompress(gzip_member(header, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_empty_input_throws) {
  try {
    decompress("");
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_single_identification_byte_throws) {
  try {
    decompress("\x1f");
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_identification_bytes_only_throws) {
  try {
    decompress("\x1f\x8b");
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_header_only_throws) {
  try {
    decompress(GZIP_HEADER);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_header_and_trailer_without_blocks_throws) {
  try {
    decompress(gzip_member(GZIP_HEADER, "", ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_truncated_fextra_throws) {
  std::string compressed{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff\x64\x00",
                         12};
  compressed.append(20, 'x');
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_fextra_swallowing_rest_of_member_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  const auto body{gzip_member("", writer.data(), "hello world")};
  std::string compressed{"\x1f\x8b\x08\x04\x00\x00\x00\x00\x00\xff", 10};
  append_little_endian(compressed, static_cast<std::uint32_t>(body.size()));
  compressed.resize(compressed.size() - 2);
  compressed += body;
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_unterminated_fname_throws) {
  std::string compressed{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\xff", 10};
  compressed.append(30, 'n');
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_unterminated_fcomment_throws) {
  std::string compressed{"\x1f\x8b\x08\x10\x00\x00\x00\x00\x00\xff", 10};
  compressed.append(30, 'c');
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_truncated_fhcrc_throws) {
  try {
    decompress(std::string{"\x1f\x8b\x08\x02\x00\x00\x00\x00\x00\xff\x90", 11});
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_truncated_stored_block_payload_throws) {
  const auto payload{random_bytes(1000, 10)};
  DeflateWriter writer;
  write_stored_block(writer, true, payload);
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), payload)};
  compressed.resize(compressed.size() - 508);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_truncated_compressed_blocks_throws) {
  auto compressed{compress(mixed_corpus(), 6)};
  compressed.resize(compressed.size() / 2);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_missing_trailer_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), "hello world")};
  compressed.resize(compressed.size() - 8);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_trailer_missing_last_byte_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), "hello world")};
  compressed.pop_back();
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_crc32_lowest_bit_mismatch_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), "hello world")};
  compressed[compressed.size() - 8] =
      static_cast<char>(compressed[compressed.size() - 8] ^ 0x01);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_crc32_highest_bit_mismatch_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), "hello world")};
  compressed[compressed.size() - 5] =
      static_cast<char>(compressed[compressed.size() - 5] ^ 0x80);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_isize_off_by_one_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  std::string compressed{GZIP_HEADER};
  compressed += writer.data();
  append_little_endian(compressed, sourcemeta::core::crc32("hello world"));
  append_little_endian(compressed, 12);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_isize_highest_byte_mismatch_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  auto compressed{gzip_member(GZIP_HEADER, writer.data(), "hello world")};
  compressed.back() = '\x01';
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_empty_payload_with_nonzero_crc32_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "");
  std::string compressed{GZIP_HEADER};
  compressed += writer.data();
  append_little_endian(compressed, 1);
  append_little_endian(compressed, 0);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_swapped_trailer_fields_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello world");
  std::string compressed{GZIP_HEADER};
  compressed += writer.data();
  append_little_endian(compressed, 11);
  append_little_endian(compressed, sourcemeta::core::crc32("hello world"));
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_stored_block_with_empty_final_block) {
  DeflateWriter writer;
  write_stored_block(writer, true, "");
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "")), "");
}

TEST(decompress_stored_block_of_maximum_length) {
  const auto payload{random_bytes(65535, 11)};
  DeflateWriter writer;
  write_stored_block(writer, true, payload);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_consecutive_stored_blocks_of_maximum_length) {
  const auto first{random_bytes(65535, 12)};
  const auto second{random_bytes(65535, 13)};
  DeflateWriter writer;
  write_stored_block(writer, false, first);
  write_stored_block(writer, true, second);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), first + second)),
            first + second);
}

TEST(decompress_many_empty_stored_blocks_before_data) {
  DeflateWriter writer;
  for (std::size_t index = 0; index < 10000; ++index) {
    write_stored_block(writer, false, "");
  }

  write_stored_block(writer, true, "data");
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "data")),
            "data");
}

TEST(decompress_stored_block_ignores_nonzero_bits_before_byte_boundary) {
  DeflateWriter writer;
  write_block_header(writer, false, 1);
  write_fixed_literals(writer, "a");
  write_fixed_literal_length(writer, 256);
  write_block_header(writer, true, 0);
  writer.bits(7, 3);
  writer.bits(1, 16);
  writer.bits(0xfffe, 16);
  writer.bytes("b");
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "ab")), "ab");
}

TEST(decompress_stored_block_nlen_mismatch_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 0);
  writer.align();
  writer.bits(11, 16);
  writer.bits(0, 16);
  writer.bytes("hello world");
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_stored_block_nlen_equal_to_len_throws) {
  const std::string payload(255, 'a');
  DeflateWriter writer;
  write_block_header(writer, true, 0);
  writer.align();
  writer.bits(255, 16);
  writer.bits(255, 16);
  writer.bytes(payload);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), payload));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_stored_block_len_beyond_input_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 0);
  writer.align();
  writer.bits(100, 16);
  writer.bits(0xff9b, 16);
  writer.bytes("0123456789");
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "0123456789"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_block_type_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 3);
  writer.bytes("hello world");
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_reserved_block_type_after_valid_block_throws) {
  DeflateWriter writer;
  write_stored_block(writer, false, "hello");
  write_block_header(writer, true, 3);
  writer.bytes(" world");
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "hello world"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_missing_final_block_throws) {
  DeflateWriter writer;
  write_stored_block(writer, false, "hello");
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "hello"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_fixed_block_with_every_literal_value) {
  std::string payload;
  for (std::size_t index = 0; index < 256; ++index) {
    payload.push_back(static_cast<char>(index));
  }

  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, payload);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_fixed_block_with_only_end_of_block) {
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "")), "");
}

TEST(decompress_fixed_block_with_boundary_match_lengths) {
  const std::string payload(1146, 'a');
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  write_fixed_literal_length(writer, 257);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 264);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 265);
  writer.bits(0, 1);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 265);
  writer.bits(1, 1);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 268);
  writer.bits(1, 1);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 269);
  writer.bits(0, 2);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 272);
  writer.bits(3, 2);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 273);
  writer.bits(0, 3);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 280);
  writer.bits(15, 4);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 281);
  writer.bits(0, 5);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 284);
  writer.bits(0, 5);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 284);
  writer.bits(30, 5);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 285);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_fixed_block_with_boundary_distances) {
  const auto history{random_bytes(32768, 14)};
  std::string expected{history};
  expected.append(3, expected.back());
  expected += expected.substr(expected.size() - 4, 3);
  expected += expected.substr(expected.size() - 5, 3);
  expected += expected.substr(expected.size() - 6, 3);
  expected += expected.substr(expected.size() - 12, 3);
  expected += expected.substr(expected.size() - 128, 3);
  expected += expected.substr(expected.size() - 129, 3);
  expected += expected.substr(expected.size() - 1024, 3);
  expected += expected.substr(expected.size() - 1025, 3);
  expected += expected.substr(expected.size() - 12288, 3);
  expected += expected.substr(expected.size() - 12289, 3);
  expected += expected.substr(expected.size() - 24577, 3);
  expected += expected.substr(expected.size() - 32768, 3);

  DeflateWriter writer;
  write_stored_block(writer, false, history);
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 257);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 257);
  writer.code(3, 5);
  write_fixed_literal_length(writer, 257);
  writer.code(4, 5);
  writer.bits(0, 1);
  write_fixed_literal_length(writer, 257);
  writer.code(4, 5);
  writer.bits(1, 1);
  write_fixed_literal_length(writer, 257);
  writer.code(6, 5);
  writer.bits(3, 2);
  write_fixed_literal_length(writer, 257);
  writer.code(13, 5);
  writer.bits(31, 5);
  write_fixed_literal_length(writer, 257);
  writer.code(14, 5);
  writer.bits(0, 6);
  write_fixed_literal_length(writer, 257);
  writer.code(19, 5);
  writer.bits(255, 8);
  write_fixed_literal_length(writer, 257);
  writer.code(20, 5);
  writer.bits(0, 9);
  write_fixed_literal_length(writer, 257);
  writer.code(26, 5);
  writer.bits(4095, 12);
  write_fixed_literal_length(writer, 257);
  writer.code(27, 5);
  writer.bits(0, 12);
  write_fixed_literal_length(writer, 257);
  writer.code(29, 5);
  writer.bits(0, 13);
  write_fixed_literal_length(writer, 257);
  writer.code(29, 5);
  writer.bits(8191, 13);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), expected)),
            expected);
}

TEST(decompress_fixed_block_overlapping_match) {
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "XY");
  write_fixed_literal_length(writer, 259);
  writer.code(1, 5);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "XYXYXYX")),
            "XYXYXYX");
}

TEST(decompress_fixed_block_maximum_length_match_at_distance_one) {
  const std::string payload(259, 'x');
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "x");
  write_fixed_literal_length(writer, 285);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_fixed_block_maximum_length_match_at_maximum_distance) {
  const auto history{random_bytes(32768, 15)};
  const auto expected{history + history.substr(0, 258)};
  DeflateWriter writer;
  write_stored_block(writer, false, history);
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 285);
  writer.code(29, 5);
  writer.bits(8191, 13);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), expected)),
            expected);
}

TEST(decompress_fixed_block_distance_beyond_output_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  write_fixed_literal_length(writer, 257);
  writer.code(1, 5);
  write_fixed_literal_length(writer, 256);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "aaaa"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_fixed_block_maximum_distance_one_beyond_output_throws) {
  const auto history{random_bytes(32767, 16)};
  DeflateWriter writer;
  write_stored_block(writer, false, history);
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 257);
  writer.code(29, 5);
  writer.bits(8191, 13);
  write_fixed_literal_length(writer, 256);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), history));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_fixed_block_match_before_any_output_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 257);
  writer.code(0, 5);
  write_fixed_literal_length(writer, 256);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_match_across_stored_and_fixed_blocks) {
  DeflateWriter writer;
  write_stored_block(writer, false, "hello ");
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 259);
  writer.code(4, 5);
  writer.bits(1, 1);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "hello hello")),
            "hello hello");
}

TEST(decompress_match_spanning_several_blocks) {
  DeflateWriter writer;
  write_stored_block(writer, false, "ab");
  write_block_header(writer, false, 1);
  write_fixed_literals(writer, "cd");
  write_fixed_literal_length(writer, 256);
  write_stored_block(writer, false, "ef");
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 260);
  writer.code(4, 5);
  writer.bits(1, 1);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "abcdefabcdef")),
            "abcdefabcdef");
}

TEST(decompress_match_into_previous_member_throws) {
  DeflateWriter first;
  write_stored_block(first, true, "hello");
  DeflateWriter second;
  write_block_header(second, true, 1);
  write_fixed_literal_length(second, 259);
  second.code(4, 5);
  second.bits(0, 1);
  write_fixed_literal_length(second, 256);
  const auto compressed{gzip_member(GZIP_HEADER, first.data(), "hello") +
                        gzip_member(GZIP_HEADER, second.data(), "hello")};
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_many_single_literal_fixed_blocks) {
  std::string payload;
  DeflateWriter writer;
  for (std::size_t index = 0; index < 10000; ++index) {
    const auto character{static_cast<char>('a' + (index % 26))};
    payload.push_back(character);
    write_block_header(writer, false, 1);
    write_fixed_literal_length(writer, static_cast<unsigned char>(character));
    write_fixed_literal_length(writer, 256);
  }

  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_single_fixed_block_with_maximum_compression_ratio) {
  std::string payload;
  payload.append(16908289, 'a');
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  for (std::size_t index = 0; index < 65536; ++index) {
    write_fixed_literal_length(writer, 285);
    writer.code(0, 5);
  }

  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_ignores_nonzero_padding_bits_after_final_block) {
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  write_fixed_literal_length(writer, 256);
  writer.bits(63, 6);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "a")), "a");
}

TEST(decompress_dynamic_block_with_literals_and_match) {
  const auto literal_lengths{
      code_lengths(258, {{97, 2}, {98, 2}, {256, 2}, {257, 2}})};
  const auto distance_lengths{code_lengths(1, {{0, 1}})};
  const HuffmanCode literals{literal_lengths};
  const HuffmanCode distances{distance_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, distance_lengths);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 257);
  distances.write(writer, 0);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "ababbbb")),
            "ababbbb");
}

TEST(decompress_dynamic_block_without_distance_codes) {
  const auto literal_lengths{code_lengths(257, {{97, 1}, {256, 1}})};
  const HuffmanCode literals{literal_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, code_lengths(1, {}));
  literals.write(writer, 97);
  literals.write(writer, 97);
  literals.write(writer, 97);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "aaa")), "aaa");
}

TEST(decompress_dynamic_block_with_maximum_literal_length_codes) {
  const std::string payload(259, 'a');
  const auto literal_lengths{code_lengths(286, {{97, 1}, {256, 2}, {285, 2}})};
  const auto distance_lengths{code_lengths(1, {{0, 1}})};
  const HuffmanCode literals{literal_lengths};
  const HuffmanCode distances{distance_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, distance_lengths);
  literals.write(writer, 97);
  literals.write(writer, 285);
  distances.write(writer, 0);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_dynamic_block_with_maximum_distance_codes) {
  const auto literal_lengths{
      code_lengths(258, {{97, 2}, {98, 2}, {256, 2}, {257, 2}})};
  const auto distance_lengths{code_lengths(32, {{0, 1}, {1, 1}})};
  const HuffmanCode literals{literal_lengths};
  const HuffmanCode distances{distance_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, distance_lengths);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 257);
  distances.write(writer, 1);
  literals.write(writer, 257);
  distances.write(writer, 0);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "ababaaaa")),
            "ababaaaa");
}

TEST(decompress_dynamic_block_with_fifteen_bit_literal_length_codes) {
  const auto literal_lengths{code_lengths(257, {{97, 1},
                                                {98, 2},
                                                {99, 3},
                                                {100, 4},
                                                {101, 5},
                                                {102, 6},
                                                {103, 7},
                                                {104, 8},
                                                {105, 9},
                                                {106, 10},
                                                {107, 11},
                                                {108, 12},
                                                {109, 13},
                                                {110, 14},
                                                {111, 15},
                                                {256, 15}})};
  const HuffmanCode literals{literal_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, code_lengths(1, {}));
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 99);
  literals.write(writer, 100);
  literals.write(writer, 101);
  literals.write(writer, 102);
  literals.write(writer, 103);
  literals.write(writer, 104);
  literals.write(writer, 105);
  literals.write(writer, 106);
  literals.write(writer, 107);
  literals.write(writer, 108);
  literals.write(writer, 109);
  literals.write(writer, 110);
  literals.write(writer, 111);
  literals.write(writer, 111);
  literals.write(writer, 256);
  EXPECT_EQ(
      decompress(gzip_member(GZIP_HEADER, writer.data(), "abcdefghijklmnoo")),
      "abcdefghijklmnoo");
}

TEST(decompress_dynamic_block_with_fifteen_bit_distance_codes) {
  std::string expected;
  for (std::size_t index = 0; index < 256; ++index) {
    expected.push_back(static_cast<char>(index));
  }

  const auto history{expected};
  expected += expected.substr(expected.size() - 193, 3);
  expected += expected.substr(expected.size() - 192, 3);
  expected.append(3, expected.back());

  const auto literal_lengths{code_lengths(258, {{256, 1}, {257, 1}})};
  const auto distance_lengths{code_lengths(16, {{0, 1},
                                                {1, 2},
                                                {2, 3},
                                                {3, 4},
                                                {4, 5},
                                                {5, 6},
                                                {6, 7},
                                                {7, 8},
                                                {8, 9},
                                                {9, 10},
                                                {10, 11},
                                                {11, 12},
                                                {12, 13},
                                                {13, 14},
                                                {14, 15},
                                                {15, 15}})};
  const HuffmanCode literals{literal_lengths};
  const HuffmanCode distances{distance_lengths};
  DeflateWriter writer;
  write_stored_block(writer, false, history);
  write_dynamic_header(writer, true, literal_lengths, distance_lengths);
  literals.write(writer, 257);
  distances.write(writer, 15);
  writer.bits(0, 6);
  literals.write(writer, 257);
  distances.write(writer, 14);
  writer.bits(63, 6);
  literals.write(writer, 257);
  distances.write(writer, 0);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), expected)),
            expected);
}

TEST(decompress_dynamic_block_with_seven_bit_code_length_codes) {
  const HuffmanCode precode{code_lengths(
      19, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 7}})};
  const auto literal_lengths{code_lengths(257, {{97, 1},
                                                {98, 2},
                                                {99, 3},
                                                {100, 4},
                                                {101, 5},
                                                {102, 6},
                                                {103, 7},
                                                {256, 7}})};
  const HuffmanCode literals{literal_lengths};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(0, 5);
  writer.bits(0, 5);
  writer.bits(14, 4);
  // Code length code lengths in the order of RFC 1951 section 3.2.7
  writer.bits(0, 3);
  writer.bits(0, 3);
  writer.bits(0, 3);
  writer.bits(1, 3);
  writer.bits(0, 3);
  writer.bits(7, 3);
  writer.bits(0, 3);
  writer.bits(7, 3);
  writer.bits(0, 3);
  writer.bits(6, 3);
  writer.bits(0, 3);
  writer.bits(5, 3);
  writer.bits(0, 3);
  writer.bits(4, 3);
  writer.bits(0, 3);
  writer.bits(3, 3);
  writer.bits(0, 3);
  writer.bits(2, 3);
  for (const auto length : literal_lengths) {
    precode.write(writer, length);
  }

  precode.write(writer, 0);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 99);
  literals.write(writer, 100);
  literals.write(writer, 101);
  literals.write(writer, 102);
  literals.write(writer, 103);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "abcdefg")),
            "abcdefg");
}

TEST(decompress_dynamic_block_repeat_previous_code_length_across_alphabets) {
  const HuffmanCode precode{code_lengths(19, {{2, 1}, {16, 2}, {18, 2}})};
  const HuffmanCode literals{
      code_lengths(258, {{97, 2}, {98, 2}, {256, 2}, {257, 2}})};
  const HuffmanCode distances{
      code_lengths(4, {{0, 2}, {1, 2}, {2, 2}, {3, 2}})};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(1, 5);
  writer.bits(3, 5);
  writer.bits(12, 4);
  writer.bits(2, 3);
  writer.bits(0, 3);
  writer.bits(2, 3);
  writer.bits(0, 36);
  writer.bits(1, 3);
  precode.write(writer, 18);
  writer.bits(86, 7);
  precode.write(writer, 2);
  precode.write(writer, 2);
  precode.write(writer, 18);
  writer.bits(127, 7);
  precode.write(writer, 18);
  writer.bits(8, 7);
  precode.write(writer, 2);
  precode.write(writer, 16);
  writer.bits(2, 2);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 257);
  distances.write(writer, 3);
  literals.write(writer, 257);
  distances.write(writer, 2);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "abababaaba")),
            "abababaaba");
}

TEST(decompress_dynamic_block_repeat_zero_code_length_across_alphabets) {
  const HuffmanCode precode{
      code_lengths(19, {{1, 2}, {2, 2}, {17, 2}, {18, 2}})};
  const HuffmanCode literals{
      code_lengths(260, {{97, 2}, {98, 2}, {256, 2}, {257, 2}})};
  const HuffmanCode distances{code_lengths(4, {{2, 1}, {3, 1}})};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(3, 5);
  writer.bits(3, 5);
  writer.bits(14, 4);
  writer.bits(0, 3);
  writer.bits(2, 3);
  writer.bits(2, 3);
  writer.bits(0, 36);
  writer.bits(2, 3);
  writer.bits(0, 3);
  writer.bits(2, 3);
  precode.write(writer, 18);
  writer.bits(86, 7);
  precode.write(writer, 2);
  precode.write(writer, 2);
  precode.write(writer, 18);
  writer.bits(127, 7);
  precode.write(writer, 18);
  writer.bits(8, 7);
  precode.write(writer, 2);
  precode.write(writer, 2);
  precode.write(writer, 17);
  writer.bits(1, 3);
  precode.write(writer, 1);
  precode.write(writer, 1);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 97);
  literals.write(writer, 98);
  literals.write(writer, 257);
  distances.write(writer, 3);
  literals.write(writer, 257);
  distances.write(writer, 2);
  literals.write(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), "abababaaba")),
            "abababaaba");
}

TEST(decompress_dynamic_block_over_subscribed_literal_length_code_throws) {
  DeflateWriter writer;
  write_dynamic_header(writer, true,
                       code_lengths(257, {{97, 1}, {98, 1}, {256, 1}}),
                       code_lengths(1, {}));
  writer.bits(0, 16);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "a"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_over_subscribed_distance_code_throws) {
  DeflateWriter writer;
  write_dynamic_header(
      writer, true, code_lengths(258, {{97, 2}, {98, 2}, {256, 2}, {257, 2}}),
      code_lengths(3, {{0, 1}, {1, 1}, {2, 1}}));
  writer.bits(0, 16);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), "a"));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_over_subscribed_code_length_code_throws) {
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(0, 5);
  writer.bits(0, 5);
  writer.bits(0, 4);
  writer.bits(1, 3);
  writer.bits(1, 3);
  writer.bits(1, 3);
  writer.bits(1, 3);
  writer.bits(0, 64);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_repeat_previous_without_previous_throws) {
  const HuffmanCode precode{code_lengths(19, {{0, 1}, {16, 1}})};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(0, 5);
  writer.bits(0, 5);
  writer.bits(0, 4);
  writer.bits(1, 3);
  writer.bits(0, 3);
  writer.bits(0, 3);
  writer.bits(1, 3);
  precode.write(writer, 16);
  writer.bits(3, 2);
  writer.bits(0, 64);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_repeat_zero_beyond_code_length_count_throws) {
  const HuffmanCode precode{code_lengths(19, {{0, 1}, {18, 1}})};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(0, 5);
  writer.bits(0, 5);
  writer.bits(0, 4);
  writer.bits(0, 3);
  writer.bits(0, 3);
  writer.bits(1, 3);
  writer.bits(1, 3);
  precode.write(writer, 18);
  writer.bits(127, 7);
  precode.write(writer, 18);
  writer.bits(127, 7);
  writer.bits(0, 64);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_repeat_previous_beyond_code_length_count_throws) {
  const HuffmanCode precode{code_lengths(19, {{16, 1}, {18, 1}})};
  DeflateWriter writer;
  write_block_header(writer, true, 2);
  writer.bits(0, 5);
  writer.bits(0, 5);
  writer.bits(0, 4);
  writer.bits(1, 3);
  writer.bits(0, 3);
  writer.bits(1, 3);
  writer.bits(0, 3);
  precode.write(writer, 18);
  writer.bits(127, 7);
  precode.write(writer, 18);
  writer.bits(108, 7);
  precode.write(writer, 16);
  writer.bits(0, 2);
  writer.bits(0, 64);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), ""));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_dynamic_block_without_end_of_block_code_throws) {
  const auto literal_lengths{code_lengths(257, {{97, 1}, {98, 1}})};
  const HuffmanCode literals{literal_lengths};
  DeflateWriter writer;
  write_dynamic_header(writer, true, literal_lengths, code_lengths(1, {}));
  for (std::size_t index = 0; index < 64; ++index) {
    literals.write(writer, 97);
  }

  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), std::string(64, 'a')));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_member_mixing_every_block_type) {
  const auto literal_lengths{code_lengths(257, {{32, 3},
                                                {97, 3},
                                                {99, 3},
                                                {100, 3},
                                                {105, 3},
                                                {109, 3},
                                                {110, 3},
                                                {121, 4},
                                                {256, 4}})};
  const HuffmanCode literals{literal_lengths};
  DeflateWriter writer;
  write_stored_block(writer, false, "stored ");
  write_block_header(writer, false, 1);
  write_fixed_literals(writer, "fixed ");
  write_fixed_literal_length(writer, 256);
  write_dynamic_header(writer, false, literal_lengths, code_lengths(1, {}));
  literals.write(writer, 100);
  literals.write(writer, 121);
  literals.write(writer, 110);
  literals.write(writer, 97);
  literals.write(writer, 109);
  literals.write(writer, 105);
  literals.write(writer, 99);
  literals.write(writer, 32);
  literals.write(writer, 256);
  write_block_header(writer, true, 1);
  write_fixed_literal_length(writer, 261);
  writer.code(8, 5);
  writer.bits(4, 3);
  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(),
                                   "stored fixed dynamic stored ")),
            "stored fixed dynamic stored ");
}

TEST(decompress_many_dynamic_blocks_with_distinct_codes) {
  std::string payload;
  DeflateWriter writer;
  for (std::size_t index = 0; index < 1000; ++index) {
    const std::size_t symbol{97 + (index % 26)};
    const auto literal_lengths{code_lengths(257, {{symbol, 1}, {256, 1}})};
    const HuffmanCode literals{literal_lengths};
    write_dynamic_header(writer, index == 999, literal_lengths,
                         code_lengths(1, {}));
    literals.write(writer, symbol);
    literals.write(writer, 256);
    payload.push_back(static_cast<char>(symbol));
  }

  EXPECT_EQ(decompress(gzip_member(GZIP_HEADER, writer.data(), payload)),
            payload);
}

TEST(decompress_output_exactly_at_maximum_size_succeeds) {
  const std::string input(1000, 'm');
  EXPECT_EQ(decompress(compress(input, 6), 0, 1000), input);
}

TEST(decompress_output_one_byte_over_maximum_size_throws) {
  const std::string input(1000, 'm');
  try {
    decompress(compress(input, 6), 0, 999);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_empty_payload_with_zero_maximum_size_succeeds) {
  EXPECT_EQ(decompress(compress("", 1), 0, 0), "");
}

TEST(decompress_single_byte_payload_with_zero_maximum_size_throws) {
  try {
    decompress(compress("a", 1), 0, 0);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_single_byte_payload_with_single_byte_maximum_size_succeeds) {
  EXPECT_EQ(decompress(compress("a", 1), 0, 1), "a");
}

TEST(decompress_two_byte_payload_with_single_byte_maximum_size_throws) {
  try {
    decompress(compress("ab", 1), 0, 1);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_output_hint_larger_than_output) {
  EXPECT_EQ(decompress(compress("hello world", 1), 1048576, 268435456),
            "hello world");
}

TEST(decompress_output_hint_above_maximum_size_with_fitting_output) {
  const std::string input(16, 'h');
  EXPECT_EQ(decompress(compress(input, 1), 1048576, 16), input);
}

TEST(decompress_output_hint_above_maximum_size_with_exceeding_output_throws) {
  const std::string input(17, 'h');
  try {
    decompress(compress(input, 1), 1048576, 16);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_output_hint_of_one_grows_to_one_megabyte) {
  const auto input{random_bytes(1048576, 17)};
  EXPECT_EQ(decompress(compress(input, 1), 1, 268435456), input);
}

TEST(decompress_with_largest_possible_maximum_size) {
  const auto input{mixed_corpus()};
  EXPECT_EQ(decompress(compress(input, 6), 1,
                       std::numeric_limits<std::size_t>::max()),
            input);
}

TEST(decompress_input_larger_than_quarter_of_maximum_size) {
  const auto input{random_bytes(1000, 18)};
  EXPECT_EQ(decompress(compress(input, 0), 0, 2000), input);
}

TEST(decompress_growth_clamps_to_uneven_maximum_size) {
  const std::string input(1000, 'k');
  EXPECT_EQ(decompress(compress(input, 6), 3, 1000), input);
}

TEST(decompress_growth_beyond_uneven_maximum_size_throws) {
  const std::string input(1000, 'k');
  try {
    decompress(compress(input, 6), 3, 999);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_crafted_bomb_at_maximum_size_succeeds) {
  const std::string payload(1056769, 'a');
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  for (std::size_t index = 0; index < 4096; ++index) {
    write_fixed_literal_length(writer, 285);
    writer.code(0, 5);
  }

  write_fixed_literal_length(writer, 256);
  EXPECT_EQ(
      decompress(gzip_member(GZIP_HEADER, writer.data(), payload), 0, 1056769),
      payload);
}

TEST(decompress_crafted_bomb_beyond_maximum_size_throws) {
  const std::string payload(1056769, 'a');
  DeflateWriter writer;
  write_block_header(writer, true, 1);
  write_fixed_literals(writer, "a");
  for (std::size_t index = 0; index < 4096; ++index) {
    write_fixed_literal_length(writer, 285);
    writer.code(0, 5);
  }

  write_fixed_literal_length(writer, 256);
  try {
    decompress(gzip_member(GZIP_HEADER, writer.data(), payload), 0, 1056768);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_compressed_zeros_beyond_maximum_size_throws) {
  const std::string input(8388608, '\0');
  try {
    decompress(compress(input, 1), 0, 1048576);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_corrupted_stored_payload_byte_throws) {
  auto compressed{compress(random_bytes(1000, 19), 0)};
  compressed[compressed.size() / 2] =
      static_cast<char>(compressed[compressed.size() / 2] ^ 0x10);
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_members_totaling_exactly_maximum_size_succeeds) {
  const auto compressed{compress("aaaa", 1) + compress("bbbb", 1)};
  EXPECT_EQ(decompress(compressed, 0, 8), "aaaabbbb");
}

TEST(decompress_members_totaling_one_byte_over_maximum_size_throws) {
  const auto compressed{compress("aaaa", 1) + compress("bbbb", 1) +
                        compress("c", 1)};
  try {
    decompress(compressed, 0, 8);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_second_member_alone_exceeding_maximum_size_throws) {
  const auto compressed{compress("a", 1) + compress(std::string(100, 'b'), 1)};
  try {
    decompress(compressed, 0, 50);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()},
              "Decompressed output exceeds the maximum allowed size");
  }
}

TEST(decompress_many_empty_members_before_data) {
  const auto empty{compress("", 1)};
  std::string compressed;
  for (std::size_t index = 0; index < 1000; ++index) {
    compressed += empty;
  }

  compressed += compress("x", 1);
  EXPECT_EQ(decompress(compressed), "x");
}

TEST(decompress_ten_thousand_single_byte_members) {
  std::string payload;
  std::string compressed;
  for (std::size_t index = 0; index < 10000; ++index) {
    const std::string character(1, static_cast<char>(index & 0xff));
    DeflateWriter writer;
    write_stored_block(writer, true, character);
    compressed += gzip_member(GZIP_HEADER, writer.data(), character);
    payload += character;
  }

  EXPECT_EQ(decompress(compressed, 1, 268435456), payload);
}

TEST(decompress_empty_member_between_members) {
  const auto compressed{compress("left", 1) + compress("", 1) +
                        compress("right", 1)};
  EXPECT_EQ(decompress(compressed), "leftright");
}

TEST(decompress_large_member_after_small_member) {
  const auto input{random_bytes(1048576, 20)};
  const auto compressed{compress("x", 1) + compress(input, 6)};
  EXPECT_EQ(decompress(compressed, 1, 268435456), "x" + input);
}

TEST(decompress_members_with_different_optional_header_fields) {
  std::string named{"\x1f\x8b\x08\x08\x00\x00\x00\x00\x00\x03", 10};
  named += "a.txt";
  named.push_back('\0');
  std::string annotated{"\x1f\x8b\x08\x1f\x00\x00\x00\x00\x00\xff\x04\x00", 12};
  annotated += std::string{"\xaa\xbb\xcc\xdd", 4};
  annotated += "name.txt";
  annotated.push_back('\0');
  annotated += "comment";
  annotated.push_back('\0');
  annotated += std::string{"\x90\x3e", 2};
  DeflateWriter first;
  write_stored_block(first, true, "hello ");
  DeflateWriter second;
  write_stored_block(second, true, "world");
  const auto compressed{gzip_member(named, first.data(), "hello ") +
                        gzip_member(annotated, second.data(), "world")};
  EXPECT_EQ(decompress(compressed), "hello world");
}

TEST(decompress_ignores_trailing_zero_bytes) {
  const auto compressed{compress("hello world", 1) + std::string(1024, '\0')};
  EXPECT_EQ(decompress(compressed), "hello world");
}

TEST(decompress_ignores_single_trailing_identification_byte) {
  const auto compressed{compress("hello world", 1) + "\x1f"};
  EXPECT_EQ(decompress(compressed), "hello world");
}

TEST(decompress_ignores_trailing_first_identification_byte_without_second) {
  const auto compressed{compress("hello world", 1) + "\x1f\x8c" + "more"};
  EXPECT_EQ(decompress(compressed), "hello world");
}

TEST(decompress_ignores_member_after_trailing_garbage) {
  const auto compressed{compress("hello", 1) + "junk" + compress("world", 1)};
  EXPECT_EQ(decompress(compressed), "hello");
}

TEST(decompress_ignores_trailing_garbage_after_empty_member) {
  const auto compressed{compress("", 1) + "junk"};
  EXPECT_EQ(decompress(compressed), "");
}

TEST(decompress_trailing_identification_bytes_only_throws) {
  const auto compressed{compress("hello world", 1) + "\x1f\x8b"};
  try {
    decompress(compressed);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_second_member_crc32_mismatch_throws) {
  auto second{compress("world", 1)};
  second[second.size() - 8] = static_cast<char>(second[second.size() - 8] ^ 1);
  try {
    decompress(compress("hello", 1) + second);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_second_member_truncated_throws) {
  auto second{compress(mixed_corpus(), 6)};
  second.resize(second.size() / 2);
  try {
    decompress(compress("hello", 1) + second);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_second_member_reserved_flag_throws) {
  auto second{compress("world", 1)};
  second[3] = '\x20';
  try {
    decompress(compress("hello", 1) + second);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_second_member_unsupported_compression_method_throws) {
  auto second{compress("world", 1)};
  second[2] = '\x09';
  try {
    decompress(compress("hello", 1) + second);
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_member_without_trailer_followed_by_member_throws) {
  DeflateWriter writer;
  write_stored_block(writer, true, "hello");
  auto first{gzip_member(GZIP_HEADER, writer.data(), "hello")};
  first.resize(first.size() - 8);
  try {
    decompress(first + compress("world", 1));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}

TEST(decompress_valid_member_after_corrupt_member_throws) {
  auto first{compress("hello", 1)};
  first[first.size() - 1] = '\x7f';
  try {
    decompress(first + compress("world", 1));
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}
