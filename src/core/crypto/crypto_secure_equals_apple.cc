#include <sourcemeta/core/crypto_secure_equals.h>

#include <cstring> // timingsafe_bcmp

#include <string_view> // std::string_view

namespace sourcemeta::core {

auto secure_equals(const std::string_view left,
                   const std::string_view right) noexcept -> bool {
  if (left.size() != right.size()) {
    return false;
  }

  return timingsafe_bcmp(left.data(), right.data(), left.size()) == 0;
}

} // namespace sourcemeta::core
