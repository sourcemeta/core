#include "crypto_aes.h"

#include <windows.h> // ULONG, PUCHAR
// clang-format off
#include <bcrypt.h> // BCrypt*, BCRYPT_*
// clang-format on

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

// Windows CNG has no RFC 3394 primitive, so AES Key Wrap runs the standard
// integrity register loop over the AES block cipher in Electronic Codebook mode

namespace {
constexpr std::size_t SEMIBLOCK_BYTES{8};
constexpr std::size_t BLOCK_BYTES{16};

// The default integrity value (RFC 3394 Section 2.2.3.1)
constexpr std::array<std::uint8_t, SEMIBLOCK_BYTES> DEFAULT_IV{
    {0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6}};

using Semiblock = std::array<std::uint8_t, SEMIBLOCK_BYTES>;
using Block = std::array<std::uint8_t, BLOCK_BYTES>;

auto as_buffer(const std::string_view value) -> PUCHAR {
  return reinterpret_cast<PUCHAR>(const_cast<char *>(
      value.data())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

auto as_buffer(const wchar_t *const value) -> PUCHAR {
  return reinterpret_cast<PUCHAR>(const_cast<wchar_t *>(
      value)); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

// Fold the round counter into the integrity register as a 64-bit big-endian
// value (RFC 3394 Section 2.2.1)
auto exclusive_or_counter(Semiblock &value, const std::uint64_t counter)
    -> void {
  for (std::size_t index = 0; index < SEMIBLOCK_BYTES; ++index) {
    value[SEMIBLOCK_BYTES - 1 - index] ^=
        static_cast<std::uint8_t>((counter >> (8 * index)) & 0xffu);
  }
}

auto cipher_block(BCRYPT_KEY_HANDLE key, const bool encrypt, const Block &input)
    -> Block {
  Block output{};
  ULONG written{0};
  if (encrypt) {
    BCryptEncrypt(
        key,
        as_buffer(std::string_view{reinterpret_cast<const char *>(input.data()),
                                   input.size()}),
        static_cast<ULONG>(input.size()), nullptr, nullptr, 0, output.data(),
        static_cast<ULONG>(output.size()), &written, 0);
  } else {
    BCryptDecrypt(
        key,
        as_buffer(std::string_view{reinterpret_cast<const char *>(input.data()),
                                   input.size()}),
        static_cast<ULONG>(input.size()), nullptr, nullptr, 0, output.data(),
        static_cast<ULONG>(output.size()), &written, 0);
  }

  return output;
}
} // namespace

namespace sourcemeta::core {

auto aes_wrap(const std::string_view key_encryption_key,
              const std::string_view key) -> std::optional<std::string> {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_AES_ALGORITHM, nullptr, 0))) {
    return std::nullopt;
  }

  std::optional<std::string> result;
  BCRYPT_KEY_HANDLE key_handle{nullptr};
  if (BCRYPT_SUCCESS(BCryptSetProperty(algorithm, BCRYPT_CHAINING_MODE,
                                       as_buffer(BCRYPT_CHAIN_MODE_ECB),
                                       sizeof(BCRYPT_CHAIN_MODE_ECB), 0)) &&
      BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(
          algorithm, &key_handle, nullptr, 0, as_buffer(key_encryption_key),
          static_cast<ULONG>(key_encryption_key.size()), 0))) {
    const auto blocks{key.size() / SEMIBLOCK_BYTES};
    Semiblock integrity{DEFAULT_IV};
    std::string registers{key};
    for (std::size_t round = 0; round < 6; ++round) {
      for (std::size_t index = 1; index <= blocks; ++index) {
        Block input{};
        const auto offset{(index - 1) * SEMIBLOCK_BYTES};
        for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
          input[byte] = integrity[byte];
          input[SEMIBLOCK_BYTES + byte] =
              static_cast<std::uint8_t>(registers[offset + byte]);
        }

        const auto output{cipher_block(key_handle, true, input)};
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
    result = std::move(wrapped);
    BCryptDestroyKey(key_handle);
  }

  BCryptCloseAlgorithmProvider(algorithm, 0);
  return result;
}

auto aes_unwrap(const std::string_view key_encryption_key,
                const std::string_view wrapped_key)
    -> std::optional<std::string> {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_AES_ALGORITHM, nullptr, 0))) {
    return std::nullopt;
  }

  std::optional<std::string> result;
  BCRYPT_KEY_HANDLE key_handle{nullptr};
  if (BCRYPT_SUCCESS(BCryptSetProperty(algorithm, BCRYPT_CHAINING_MODE,
                                       as_buffer(BCRYPT_CHAIN_MODE_ECB),
                                       sizeof(BCRYPT_CHAIN_MODE_ECB), 0)) &&
      BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(
          algorithm, &key_handle, nullptr, 0, as_buffer(key_encryption_key),
          static_cast<ULONG>(key_encryption_key.size()), 0))) {
    const auto blocks{(wrapped_key.size() / SEMIBLOCK_BYTES) - 1};
    Semiblock integrity{};
    for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
      integrity[byte] = static_cast<std::uint8_t>(wrapped_key[byte]);
    }

    std::string registers{wrapped_key.substr(SEMIBLOCK_BYTES)};
    for (std::size_t round = 6; round-- > 0;) {
      for (std::size_t index = blocks; index >= 1; --index) {
        exclusive_or_counter(integrity, (blocks * round) + index);
        Block input{};
        const auto offset{(index - 1) * SEMIBLOCK_BYTES};
        for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
          input[byte] = integrity[byte];
          input[SEMIBLOCK_BYTES + byte] =
              static_cast<std::uint8_t>(registers[offset + byte]);
        }

        const auto output{cipher_block(key_handle, false, input)};
        for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
          integrity[byte] = output[byte];
          registers[offset + byte] =
              static_cast<char>(output[SEMIBLOCK_BYTES + byte]);
        }
      }
    }

    std::uint8_t difference{0};
    for (std::size_t byte = 0; byte < SEMIBLOCK_BYTES; ++byte) {
      difference = static_cast<std::uint8_t>(
          difference | (integrity[byte] ^ DEFAULT_IV[byte]));
    }

    if (difference == 0) {
      result = std::move(registers);
    }

    BCryptDestroyKey(key_handle);
  }

  BCryptCloseAlgorithmProvider(algorithm, 0);
  return result;
}

} // namespace sourcemeta::core
