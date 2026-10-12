#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>

#include <algorithm> // std::ranges::copy
#include <array>     // std::array
#include <cstdint>   // std::uint8_t
#include <span>      // std::span

#include "helpers.h"

namespace sourcemeta::core {

auto http_continue_traceparent(const HTTPTraceParent &parent,
                               const bool sampled) -> std::array<char, 55> {
  std::array<std::uint8_t, 8> entropy{};
  random_bytes(entropy);

  // The revision is the one this library implements rather than the one that
  // was read, since a value is reconstructed "according to the highest version
  // of the specification known to the implementation"
  // https://www.w3.org/TR/trace-context/#versioning-of-traceparent
  std::array<char, 55> result{};
  result[0] = '0';
  result[1] = '0';
  result[2] = '-';

  // The trace carries on, so its identifier is the one that arrived
  std::ranges::copy(parent.trace_id, result.begin() + 3);
  result[35] = '-';

  // The operation is this one rather than the caller's, and drawing a fresh
  // identifier is also what lets the sampling decision differ from the one
  // that arrived: "The parent-id field MUST be set to a new value with the
  // sampled flag update"
  // https://www.w3.org/TR/trace-context/#mutating-the-traceparent-field
  http_write_hex(entropy, std::span{result}.subspan(36, 16));
  result[52] = '-';
  result[53] = '0';
  result[54] = sampled ? '1' : '0';
  return result;
}

} // namespace sourcemeta::core
