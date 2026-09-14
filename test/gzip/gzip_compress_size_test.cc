#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t, std::uint32_t
#include <istream>  // std::istream
#include <iterator> // std::istreambuf_iterator
#include <random>   // std::mt19937, std::uniform_int_distribution
#include <sstream>  // std::istringstream
#include <string>   // std::string
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

TEST(compress_repeated_pangram) {
  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size());
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_higher_level_is_not_larger) {
  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += "The quick brown fox jumps over the lazy dog. ";
  }

  EXPECT_LE(compress(input, 12).size(), compress(input, 1).size());
}

TEST(compress_highly_compressible_zeros) {
  const std::string input(65536, '\0');
  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_highly_compressible_repeated_byte) {
  const std::string input(65536, static_cast<char>(0xff));
  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_incompressible_input_at_level_0_barely_expands) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{3};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 1048576; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 0)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_incompressible_input_at_level_1_barely_expands) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{3};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 1048576; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_incompressible_input_at_level_6_barely_expands) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{3};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 1048576; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_incompressible_input_at_level_12_barely_expands) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{3};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string input;
  for (std::size_t index = 0; index < 1048576; ++index) {
    input.push_back(static_cast<char>(distribution(generator)));
  }

  const auto compressed{compress(input, 12)};
  EXPECT_LE(compressed.size(), input.size() + input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_eight_megabytes_of_zeros_at_level_1) {
  const std::string input(8388608, '\0');
  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_four_megabytes_of_zeros_at_level_12) {
  const std::string input(4194304, '\0');
  const auto compressed{compress(input, 12)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_alternating_bytes) {
  std::string input;
  for (std::size_t index = 0; index < 524288; ++index) {
    input += "\x01\xfe";
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 100);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_block_repeated_at_maximum_match_length_period) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{6};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string block;
  for (std::size_t index = 0; index < 258; ++index) {
    block.push_back(static_cast<char>(distribution(generator)));
  }

  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += block;
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_random_block_repeated_beyond_maximum_match_length_period) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp,bugprone-random-generator-seed)
  std::mt19937 generator{6};
  std::uniform_int_distribution<int> distribution{0, 255};
  std::string block;
  for (std::size_t index = 0; index < 259; ++index) {
    block.push_back(static_cast<char>(distribution(generator)));
  }

  std::string input;
  for (std::size_t index = 0; index < 1000; ++index) {
    input += block;
  }

  const auto compressed{compress(input, 6)};
  EXPECT_LT(compressed.size(), input.size() / 10);
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}

TEST(compress_sixteen_megabytes) {
  std::string input;
  for (std::uint32_t index = 0; index < 4194304; ++index) {
    input.push_back(static_cast<char>(index & 0xff));
    input.push_back(static_cast<char>((index >> 8) & 0xff));
    input.push_back(static_cast<char>((index >> 16) & 0xff));
    input.push_back(static_cast<char>((index >> 24) & 0xff));
  }

  const auto compressed{compress(input, 1)};
  EXPECT_LT(compressed.size(), input.size());
  EXPECT_GZIP_DECOMPRESS(compressed, input);
}
