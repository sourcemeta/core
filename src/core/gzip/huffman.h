#ifndef SOURCEMETA_CORE_GZIP_HUFFMAN_H_
#define SOURCEMETA_CORE_GZIP_HUFFMAN_H_

#include "bit_reader.h"

#include <sourcemeta/core/gzip_error.h>

#include <algorithm> // std::ranges::fill
#include <array>     // std::array
#include <cstddef>   // std::size_t
#include <cstdint>   // std::uint8_t, std::uint16_t

namespace sourcemeta::core {

// Maximum Huffman code length per RFC 1951 section 3.2.7
inline constexpr unsigned int MAX_HUFFMAN_BITS{15};

// Largest alphabet is the literal/length alphabet (288 symbols) per RFC 1951
// section 3.2.5
inline constexpr std::size_t MAX_HUFFMAN_SYMBOLS{288};

class HuffmanDecoder {
public:
  HuffmanDecoder() = default;

  auto build(const std::uint8_t *lengths, const std::size_t length_count)
      -> void {
    std::ranges::fill(this->count_, 0);

    for (std::size_t symbol = 0; symbol < length_count; ++symbol) {
      if (lengths[symbol] > MAX_HUFFMAN_BITS) {
        throw GZIPError{"Huffman code length out of range"};
      }
      this->count_[lengths[symbol]]++;
    }

    if (this->count_[0] == length_count) {
      return;
    }

    // Verify the alphabet is complete or single-symbol per RFC 1951
    int left{1};
    for (unsigned int bits = 1; bits <= MAX_HUFFMAN_BITS; ++bits) {
      left <<= 1;
      left -= this->count_[bits];
      if (left < 0) {
        throw GZIPError{"Over-subscribed Huffman code"};
      }
    }

    std::array<std::uint16_t, MAX_HUFFMAN_BITS + 1> offsets{};
    offsets[1] = 0;
    for (unsigned int bits = 1; bits < MAX_HUFFMAN_BITS; ++bits) {
      offsets[bits + 1] =
          static_cast<std::uint16_t>(offsets[bits] + this->count_[bits]);
    }

    for (std::size_t symbol = 0; symbol < length_count; ++symbol) {
      if (lengths[symbol] != 0) {
        this->symbols_[offsets[lengths[symbol]]++] =
            static_cast<std::uint16_t>(symbol);
      }
    }
  }

  auto decode(BitReader &reader) const -> std::uint16_t {
    int code{0};
    int first{0};
    int index{0};
    for (unsigned int bits = 1; bits <= MAX_HUFFMAN_BITS; ++bits) {
      code |= static_cast<int>(reader.read_bits(1));
      const auto entries{static_cast<int>(this->count_[bits])};
      if (code - entries < first) {
        const auto position{static_cast<std::size_t>(index) +
                            static_cast<std::size_t>(code - first)};
        return this->symbols_[position];
      }
      index += entries;
      first = (first + entries) << 1;
      code <<= 1;
    }
    throw GZIPError{"Invalid Huffman code"};
  }

private:
  std::array<std::uint16_t, MAX_HUFFMAN_BITS + 1> count_{};
  std::array<std::uint16_t, MAX_HUFFMAN_SYMBOLS> symbols_{};
};

} // namespace sourcemeta::core

#endif
