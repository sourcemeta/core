#include <sourcemeta/core/crypto.h>

#include "crypto_random.h"

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t
#include <span>    // std::span
#include <string>  // std::string

namespace sourcemeta::core {

auto random_bytes(std::span<std::uint8_t> buffer) -> void {
  if (buffer.empty()) {
    return;
  }

  fill_random_bytes(buffer);
}

auto random_bytes(const std::size_t length) -> std::string {
  std::string result(length, '\0');
  random_bytes(std::span<std::uint8_t>{
      reinterpret_cast<std::uint8_t *>(result.data()), length});
  return result;
}

} // namespace sourcemeta::core
