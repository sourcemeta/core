#include <sourcemeta/core/crypto_secure_equals.h>

#include <openssl/crypto.h> // CRYPTO_memcmp

#include <string_view> // std::string_view

namespace sourcemeta::core {

auto secure_equals(const std::string_view left,
                   const std::string_view right) noexcept -> bool {
  if (left.size() != right.size()) {
    return false;
  }

  return CRYPTO_memcmp(left.data(), right.data(), left.size()) == 0;
}

} // namespace sourcemeta::core
