#include <sourcemeta/core/crypto_sha256.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h> // ULONG

#include <bcrypt.h> // BCrypt*, BCRYPT_*

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <limits>      // std::numeric_limits
#include <span>        // std::span
#include <stdexcept>   // std::runtime_error
#include <string_view> // std::string_view

namespace {

auto sha256_hash_data(BCRYPT_HASH_HANDLE hash, const std::string_view input)
    -> bool {
  // The data interface is not const-qualified but never writes through
  // the pointer, and it takes a 32-bit length, so larger inputs must be
  // fed in chunks
  auto *remaining_data{
      reinterpret_cast<unsigned char *>(const_cast<char *>(input.data()))};
  auto remaining_size{input.size()};
  constexpr std::size_t maximum_chunk{std::numeric_limits<ULONG>::max()};
  while (remaining_size > 0) {
    const auto chunk_size{remaining_size > maximum_chunk ? maximum_chunk
                                                         : remaining_size};
    if (!BCRYPT_SUCCESS(BCryptHashData(hash, remaining_data,
                                       static_cast<ULONG>(chunk_size), 0))) {
      return false;
    }

    remaining_data += chunk_size;
    remaining_size -= chunk_size;
  }

  return true;
}

} // namespace

namespace sourcemeta::core {

auto sha256_digest(const std::span<const std::string_view> input)
    -> std::array<std::uint8_t, 32> {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
    throw std::runtime_error("Could not open the CNG SHA-256 provider");
  }

  BCRYPT_HASH_HANDLE hash{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    throw std::runtime_error("Could not create the CNG SHA-256 hash");
  }

  auto success{true};
  for (const auto part : input) {
    success = success && sha256_hash_data(hash, part);
  }

  std::array<std::uint8_t, 32> digest{};
  if (success) {
    success = BCRYPT_SUCCESS(BCryptFinishHash(
        hash, digest.data(), static_cast<ULONG>(digest.size()), 0));
  }

  BCryptDestroyHash(hash);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  if (!success) {
    throw std::runtime_error("Could not compute the CNG SHA-256 digest");
  }

  return digest;
}

auto sha256_digest(const std::string_view input)
    -> std::array<std::uint8_t, 32> {
  return sha256_digest(std::span<const std::string_view>{&input, 1});
}

} // namespace sourcemeta::core
