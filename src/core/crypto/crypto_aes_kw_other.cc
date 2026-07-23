#include "crypto_aes.h"
#include "crypto_aes_block.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

// A from-scratch AES Key Wrap (RFC 3394) for the reference backend, over the
// shared AES block cipher. This is not constant-time, which is acceptable only
// because this backend is the non-production fallback

namespace sourcemeta::core {
namespace {
constexpr std::size_t SEMIBLOCK_BYTES{8};

// The default integrity value (RFC 3394 Section 2.2.3.1)
constexpr std::array<std::uint8_t, SEMIBLOCK_BYTES> DEFAULT_IV{
    {0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6}};

using Semiblock = std::array<std::uint8_t, SEMIBLOCK_BYTES>;

// Fold the round counter into the integrity register as a 64-bit big-endian
// value (RFC 3394 Section 2.2.1)
auto exclusive_or_counter(Semiblock &value, const std::uint64_t counter)
    -> void {
  for (std::size_t index = 0; index < SEMIBLOCK_BYTES; ++index) {
    value[SEMIBLOCK_BYTES - 1 - index] ^=
        static_cast<std::uint8_t>((counter >> (8 * index)) & 0xffu);
  }
}
} // namespace

auto aes_wrap(const std::string_view key_encryption_key,
              const std::string_view key) -> std::optional<std::string> {
  const auto schedule{aes_expand_key(key_encryption_key)};
  const auto blocks{key.size() / SEMIBLOCK_BYTES};
  Semiblock integrity{DEFAULT_IV};
  std::string registers{key};
  for (std::size_t round = 0; round < 6; ++round) {
    for (std::size_t index = 1; index <= blocks; ++index) {
      AesBlock input{};
      const auto offset{(index - 1) * SEMIBLOCK_BYTES};
      for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
        input[byte] = integrity[byte];
        input[SEMIBLOCK_BYTES + byte] =
            static_cast<std::uint8_t>(registers[offset + byte]);
      }

      const auto output{aes_encrypt_block(schedule, input)};
      for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
        integrity[byte] = output[byte];
        registers[offset + byte] =
            static_cast<char>(output[SEMIBLOCK_BYTES + byte]);
      }

      exclusive_or_counter(integrity, (blocks * round) + index);
    }
  }

  std::string wrapped;
  wrapped.reserve(key.size() + SEMIBLOCK_BYTES);
  wrapped.append(reinterpret_cast<const char *>(integrity.data()),
                 SEMIBLOCK_BYTES);
  wrapped.append(registers);
  return wrapped;
}

auto aes_unwrap(const std::string_view key_encryption_key,
                const std::string_view wrapped_key)
    -> std::optional<std::string> {
  const auto schedule{aes_expand_key(key_encryption_key)};
  const auto blocks{(wrapped_key.size() / SEMIBLOCK_BYTES) - 1};
  Semiblock integrity{};
  for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
    integrity[byte] = static_cast<std::uint8_t>(wrapped_key[byte]);
  }

  std::string registers{wrapped_key.substr(SEMIBLOCK_BYTES)};
  for (std::size_t round = 6; round-- > 0;) {
    for (std::size_t index = blocks; index >= 1; --index) {
      exclusive_or_counter(integrity, (blocks * round) + index);
      AesBlock input{};
      const auto offset{(index - 1) * SEMIBLOCK_BYTES};
      for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
        input[byte] = integrity[byte];
        input[SEMIBLOCK_BYTES + byte] =
            static_cast<std::uint8_t>(registers[offset + byte]);
      }

      const auto output{aes_decrypt_block(schedule, input)};
      for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
        integrity[byte] = output[byte];
        registers[offset + byte] =
            static_cast<char>(output[SEMIBLOCK_BYTES + byte]);
      }
    }
  }

  // The integrity register must return to the default value, checked without an
  // early exit so the comparison does not leak where a mismatch occurred
  std::uint8_t difference{0};
  for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
    difference = static_cast<std::uint8_t>(
        difference | (integrity[byte] ^ DEFAULT_IV[byte]));
  }

  if (difference != 0) {
    return std::nullopt;
  }

  return registers;
}

} // namespace sourcemeta::core
