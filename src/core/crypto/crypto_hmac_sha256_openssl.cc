#include <sourcemeta/core/crypto_hmac_sha256.h>

#include <openssl/evp.h>  // EVP_sha256
#include <openssl/hmac.h> // HMAC

#include <array>     // std::array
#include <cstdint>   // std::uint8_t
#include <stdexcept> // std::runtime_error

namespace sourcemeta::core {

auto hmac_sha256_digest(const std::string_view key,
                        const std::string_view message)
    -> std::array<std::uint8_t, 32> {
  std::array<std::uint8_t, 32> digest{};
  unsigned int length{0};
  if (HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
           reinterpret_cast<const unsigned char *>(message.data()),
           message.size(), digest.data(), &length) == nullptr) {
    throw std::runtime_error("Could not compute HMAC-SHA256 digest");
  }

  return digest;
}

} // namespace sourcemeta::core
