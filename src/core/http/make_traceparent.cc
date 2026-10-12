#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <span>    // std::span

#include "helpers.h"

namespace sourcemeta::core {

auto http_make_traceparent(const bool sampled) -> std::array<char, 55> {
  // The two identifiers are drawn in one go, sixteen bytes for the trace and
  // eight for the operation within it
  std::array<std::uint8_t, 24> entropy{};
  random_bytes(entropy);

  std::array<char, 55> result{};
  result[0] = '0';
  result[1] = '0';
  result[2] = '-';
  http_write_hex(std::span{entropy}.first(16),
                 std::span{result}.subspan(3, 32));
  result[35] = '-';
  http_write_hex(std::span{entropy}.last(8), std::span{result}.subspan(36, 16));
  result[52] = '-';

  // Only the lowest bit carries a meaning, and the rest are left clear because
  // "Vendors MUST set those to zero"
  // https://www.w3.org/TR/trace-context/#other-flags
  result[53] = '0';
  result[54] = sampled ? '1' : '0';
  return result;
}

} // namespace sourcemeta::core
