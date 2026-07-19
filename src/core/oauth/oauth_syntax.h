#ifndef SOURCEMETA_CORE_OAUTH_SYNTAX_H_
#define SOURCEMETA_CORE_OAUTH_SYNTAX_H_

#include <sourcemeta/core/text.h>

#include <algorithm>   // std::ranges::all_of
#include <string_view> // std::string_view

namespace sourcemeta::core {

// RFC 3986 Section 2.3: "unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"",
// the character set RFC 7636 reuses for the PKCE verifier and challenge
inline auto oauth_is_unreserved(const char character) noexcept -> bool {
  return is_alphanum(character) || character == '-' || character == '.' ||
         character == '_' || character == '~';
}

inline auto oauth_is_unreserved_string(const std::string_view value) noexcept
    -> bool {
  return std::ranges::all_of(value, oauth_is_unreserved);
}

// RFC 7636 Section 4.1: "code-verifier = 43*128unreserved"
inline auto oauth_is_pkce_verifier(const std::string_view value) noexcept
    -> bool {
  return value.size() >= 43 && value.size() <= 128 &&
         oauth_is_unreserved_string(value);
}

// RFC 7636 Section 4.3: "code-challenge = 43*128unreserved"
inline auto oauth_is_pkce_challenge(const std::string_view value) noexcept
    -> bool {
  return value.size() >= 43 && value.size() <= 128 &&
         oauth_is_unreserved_string(value);
}

} // namespace sourcemeta::core

#endif
