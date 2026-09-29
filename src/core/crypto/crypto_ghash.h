#ifndef SOURCEMETA_CORE_CRYPTO_GHASH_H_
#define SOURCEMETA_CORE_CRYPTO_GHASH_H_

// Finite field multiplication in GF(2^128) and GHASH (NIST SP 800-38D Section
// 6.3 and 6.4) for the reference AES-GCM backend. This is not constant-time,
// which is acceptable only because this backend is the non-production fallback.

#include "crypto_aes_block.h"

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <string_view> // std::string_view

namespace sourcemeta::core {

// Load an unsigned 64-bit big-endian integer from a byte pointer
inline auto load_u64_be(const std::uint8_t *bytes) -> std::uint64_t {
  return (static_cast<std::uint64_t>(bytes[0]) << 56u) |
         (static_cast<std::uint64_t>(bytes[1]) << 48u) |
         (static_cast<std::uint64_t>(bytes[2]) << 40u) |
         (static_cast<std::uint64_t>(bytes[3]) << 32u) |
         (static_cast<std::uint64_t>(bytes[4]) << 24u) |
         (static_cast<std::uint64_t>(bytes[5]) << 16u) |
         (static_cast<std::uint64_t>(bytes[6]) << 8u) |
         (static_cast<std::uint64_t>(bytes[7]));
}

// Store an unsigned 64-bit big-endian integer into a byte pointer
inline auto store_u64_be(std::uint8_t *bytes, const std::uint64_t value)
    -> void {
  bytes[0] = static_cast<std::uint8_t>((value >> 56u) & 0xffu);
  bytes[1] = static_cast<std::uint8_t>((value >> 48u) & 0xffu);
  bytes[2] = static_cast<std::uint8_t>((value >> 40u) & 0xffu);
  bytes[3] = static_cast<std::uint8_t>((value >> 32u) & 0xffu);
  bytes[4] = static_cast<std::uint8_t>((value >> 24u) & 0xffu);
  bytes[5] = static_cast<std::uint8_t>((value >> 16u) & 0xffu);
  bytes[6] = static_cast<std::uint8_t>((value >> 8u) & 0xffu);
  bytes[7] = static_cast<std::uint8_t>(value & 0xffu);
}

// Multiply two blocks in GF(2^128) with the GCM reduction polynomial (NIST SP
// 800-38D Section 6.3) using 64-bit word operations
inline auto gf_multiply(const AesBlock &left, const AesBlock &right)
    -> AesBlock {
  // Reduction polynomial R = 11100001 || 0^120 (NIST SP 800-38D Section 6.3)
  constexpr std::uint64_t reduction{0xe100000000000000ULL};

  std::uint64_t product0{0};
  std::uint64_t product1{0};
  std::uint64_t value0{load_u64_be(left.data())};
  std::uint64_t value1{load_u64_be(left.data() + 8)};
  const std::uint64_t right0{load_u64_be(right.data())};
  const std::uint64_t right1{load_u64_be(right.data() + 8)};

  for (std::size_t bit = 0; bit < 64; ++bit) {
    const auto bit_mask{0ULL - ((right0 >> (63u - bit)) & 1u)};
    product0 ^= (value0 & bit_mask);
    product1 ^= (value1 & bit_mask);

    const auto carry_mask{0ULL - (value1 & 1u)};
    value1 = (value1 >> 1u) | (value0 << 63u);
    value0 = (value0 >> 1u) ^ (reduction & carry_mask);
  }

  for (std::size_t bit = 0; bit < 64; ++bit) {
    const auto bit_mask{0ULL - ((right1 >> (63u - bit)) & 1u)};
    product0 ^= (value0 & bit_mask);
    product1 ^= (value1 & bit_mask);

    const auto carry_mask{0ULL - (value1 & 1u)};
    value1 = (value1 >> 1u) | (value0 << 63u);
    value0 = (value0 >> 1u) ^ (reduction & carry_mask);
  }

  AesBlock result{};
  store_u64_be(result.data(), product0);
  store_u64_be(result.data() + 8, product1);
  return result;
}

// GHASH the data padded to whole blocks (NIST SP 800-38D Section 6.4)
inline auto ghash(const AesBlock &key, const std::string_view data,
                  AesBlock accumulator) -> AesBlock {
  for (std::size_t offset = 0; offset < data.size(); offset += 16) {
    for (std::size_t index = 0; index < 16 && offset + index < data.size();
         ++index) {
      accumulator[index] ^= static_cast<std::uint8_t>(data[offset + index]);
    }

    accumulator = gf_multiply(accumulator, key);
  }

  return accumulator;
}

} // namespace sourcemeta::core

#endif
