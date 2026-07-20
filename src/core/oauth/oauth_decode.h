#ifndef SOURCEMETA_CORE_OAUTH_DECODE_H_
#define SOURCEMETA_CORE_OAUTH_DECODE_H_

#include <sourcemeta/core/uri.h>

#include <cassert>     // assert
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

// Decode one "application/x-www-form-urlencoded" value against the caller's
// arena following design convention 1: borrow from the input when it carries no
// escape, otherwise append the decoded bytes to the arena and view them there.
// The arena MUST be reserved up front to at least the total raw length of every
// value that will be decoded into it, since decoding only shrinks, so that no
// append reallocates and a prior borrowed view into it stays valid. Reserving
// the whole input length once satisfies this
inline auto oauth_form_decode_into(const std::string_view value,
                                   std::string &arena, std::string_view &result)
    -> bool {
  if (value.find_first_of("+%") == std::string_view::npos) {
    result = value;
    return true;
  }

  // The caller must have reserved enough headroom for the raw value, since
  // decoding only shrinks, so this append never reallocates and a prior
  // borrowed view into the arena stays valid
  assert(arena.capacity() - arena.size() >= value.size());
  const auto base{arena.size()};
  if (!URI::unescape_form(value, arena)) {
    return false;
  }

  result = std::string_view{arena}.substr(base);
  return true;
}

} // namespace sourcemeta::core

#endif
