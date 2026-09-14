#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t, std::uint32_t
#include <istream>  // std::istream
#include <iterator> // std::istreambuf_iterator
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <sstream>  // std::istringstream
#include <string>   // std::string
#include <utility>  // std::move
#include <vector>   // std::vector

namespace {

auto compress(const std::string &input, const int level)
    -> std::vector<std::uint8_t> {
  const auto output{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(),
      level)};
  return {output.cbegin(), output.cend()};
}

auto decompress_one_shot(const std::vector<std::uint8_t> &input,
                         const std::size_t output_hint = 0,
                         const std::size_t maximum_size = 268435456)
    -> std::string {
  return sourcemeta::core::gunzip(input.data(), input.size(), output_hint,
                                  maximum_size);
}

auto decompress_stream(const std::vector<std::uint8_t> &input) -> std::string {
  std::istringstream stream{std::string{input.cbegin(), input.cend()}};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};
  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  return result;
}

} // namespace

// Both decompression mechanisms must produce the same output for every input
#define EXPECT_GZIP_DECOMPRESS(input, expected)                                \
  EXPECT_EQ(decompress_one_shot(input), expected);                             \
  EXPECT_EQ(decompress_stream(input), expected)

TEST(compress_empty_input) {
  const std::string input;
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_hello_world) {
  const std::string input{"hello world"};
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_every_byte_value) {
  std::string input;
  for (std::size_t index = 0; index < 256; ++index) {
    input.push_back(static_cast<char>(index));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_0_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 0)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_1_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_2_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 2)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_3_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 3)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_4_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 4)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_5_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 5)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_6_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 6)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_7_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 7)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_8_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 8)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_9_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 9)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_10_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 10)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_11_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 11)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_12_round_trips) {
  std::string input;
  for (std::size_t index = 0; index < 2000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 12)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_single_byte) {
  const std::string input{"X"};
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_two_bytes) {
  const std::string input{"AB"};
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_single_zero_byte) {
  const std::string input(1, '\0');
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_minimum_match_length_input) {
  const std::string input{"abcabc"};
  const auto compressed{compress(input, 12)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_run_of_maximum_match_length) {
  const std::string input(259, 'q');
  const auto compressed{compress(input, 6)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_run_one_past_maximum_match_length) {
  const std::string input(260, 'q');
  const auto compressed{compress(input, 6)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_bytes_one_below_stream_buffer_size) {
  std::string input;
  for (std::size_t index = 0; index < 16383; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_bytes_of_stream_buffer_size) {
  std::string input;
  for (std::size_t index = 0; index < 16384; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_bytes_one_above_stream_buffer_size) {
  std::string input;
  for (std::size_t index = 0; index < 16385; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_bytes_of_window_size) {
  std::string input;
  for (std::size_t index = 0; index < 32768; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_bytes_of_one_megabyte) {
  std::string input;
  for (std::size_t index = 0; index < 1048576; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_input_one_below_window_size) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{1};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 32767; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_input_of_window_size) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{1};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 32768; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_input_one_above_window_size) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{1};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 32769; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_0_random_input_of_maximum_stored_block_length) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{2};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 65535; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 0)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_0_random_input_one_above_maximum_stored_block_length) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{2};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 65536; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 0)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_level_0_random_input_of_two_maximum_stored_blocks) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{2};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 131070; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 0)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_incompressible_random_input) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{42};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 65536; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_run_wrapping_the_window_many_times) {
  const std::string input(200000, 'a');
  const auto compressed{compress(input, 1)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_block_repeated_at_window_size_distance_at_level_6) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{4};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string block;
  for (std::size_t index = 0; index < 32768; ++index) {
    block.push_back(static_cast<char>(distribution(generator)));
  }

  const auto input{block + block + block + block};
  const auto compressed{compress(input, 6)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_block_repeated_at_window_size_distance_at_level_12) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{4};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string block;
  for (std::size_t index = 0; index < 32768; ++index) {
    block.push_back(static_cast<char>(distribution(generator)));
  }

  const auto input{block + block + block + block};
  const auto compressed{compress(input, 12)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_block_repeated_beyond_window_size_distance) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{5};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string block;
  for (std::size_t index = 0; index < 32769; ++index) {
    block.push_back(static_cast<char>(distribution(generator)));
  }

  const auto input{block + block + block + block};
  const auto compressed{compress(input, 12)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_runs_of_every_match_length) {
  std::string input;
  for (std::size_t length = 1; length <= 300; ++length) {
    input.append(length, static_cast<char>(length & 0xff));
    input.append(length, static_cast<char>(~length & 0xff));
  }

  const auto compressed{compress(input, 9)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
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
  EXPECT_GZIP_DECOMPRESS(compressed, input);
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
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_counter_sequence) {
  std::string input;
  for (std::uint32_t index = 0; index < 262144; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
    input.push_back(static_cast<char>((index >> 8) & 0xff));
    input.push_back(static_cast<char>((index >> 16) & 0xff));
    input.push_back(static_cast<char>((index >> 24) & 0xff));
  }

  const auto compressed{compress(input, 6)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_already_compressed_input) {
  std::string text;
  for (std::size_t index = 0; index < 2000; ++index) {
    text += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto inner{compress(text, 12)};
  const std::string input{inner.cbegin(), inner.cend()};
  const auto compressed{compress(input, 12)};
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_members_of_different_levels_concatenate) {
  const std::string first(70000, 'f');
  const std::string second{"second"};
  auto compressed{compress(first, 0)};
  const auto second_compressed{compress(second, 12)};
  const auto empty_compressed{compress("", 6)};
  compressed.insert(compressed.end(), second_compressed.cbegin(),
                    second_compressed.cend());
  compressed.insert(compressed.end(), empty_compressed.cbegin(),
                    empty_compressed.cend());
  EXPECT_GZIP_DECOMPRESS(compressed, first + second);
}
