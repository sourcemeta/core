#ifndef SOURCEMETA_CORE_GZIP_BIT_READER_H_
#define SOURCEMETA_CORE_GZIP_BIT_READER_H_

#include <sourcemeta/core/gzip_error.h>

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t, std::uint32_t, std::uint64_t
#include <istream> // std::istream

namespace sourcemeta::core {

class BitReader {
public:
  BitReader(std::istream &source) : source_{&source} {}

  auto read_bits(const unsigned int count) -> std::uint32_t {
    while (this->bits_available_ < count) {
      char byte{};
      this->source_->read(&byte, 1);
      if (this->source_->gcount() != 1) {
        throw GZIPError{"Unexpected end of source stream"};
      }
      this->accumulator_ |=
          static_cast<std::uint64_t>(static_cast<std::uint8_t>(byte))
          << this->bits_available_;
      this->bits_available_ += 8;
    }

    const auto mask{(static_cast<std::uint64_t>(1) << count) - 1};
    const auto value{static_cast<std::uint32_t>(this->accumulator_ & mask)};
    this->accumulator_ >>= count;
    this->bits_available_ -= count;
    return value;
  }

  auto align_to_byte() -> void {
    const auto trailing_bits{this->bits_available_ % 8};
    this->accumulator_ >>= trailing_bits;
    this->bits_available_ -= trailing_bits;
  }

  auto read_byte() -> std::uint8_t {
    if (this->bits_available_ >= 8) {
      const auto value{static_cast<std::uint8_t>(this->accumulator_ & 0xff)};
      this->accumulator_ >>= 8;
      this->bits_available_ -= 8;
      return value;
    }

    char byte{};
    this->source_->read(&byte, 1);
    if (this->source_->gcount() != 1) {
      throw GZIPError{"Unexpected end of source stream"};
    }
    return static_cast<std::uint8_t>(byte);
  }

  auto read_bytes(std::uint8_t *destination, const std::size_t count) -> void {
    for (std::size_t index = 0; index < count; ++index) {
      destination[index] = this->read_byte();
    }
  }

private:
  std::istream *source_;
  std::uint64_t accumulator_{0};
  unsigned int bits_available_{0};
};

} // namespace sourcemeta::core

#endif
