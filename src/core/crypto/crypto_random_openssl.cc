#include "crypto_random.h"

#include <openssl/rand.h> // RAND_bytes

#include <cstdint>   // std::uint8_t
#include <span>      // std::span
#include <stdexcept> // std::runtime_error

namespace sourcemeta::core {

auto fill_random_bytes(std::span<std::uint8_t> bytes) -> void {
  if (RAND_bytes(bytes.data(), static_cast<int>(bytes.size())) != 1) {
    throw std::runtime_error("Could not generate random bytes with OpenSSL");
  }
}

} // namespace sourcemeta::core
