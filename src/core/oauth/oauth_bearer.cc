#include <sourcemeta/core/oauth_bearer.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

auto oauth_skip_ows(const std::string_view value, std::size_t position) noexcept
    -> std::size_t {
  while (position < value.size() && http_is_ows(value[position])) {
    position += 1;
  }

  return position;
}

} // namespace

auto oauth_bearer_header(const std::string_view token, std::string &sink)
    -> bool {
  if (!http_is_b64token(token)) {
    return false;
  }

  static constexpr std::string_view prefix{"Bearer "};
  sink.reserve(sink.size() + prefix.size() + token.size());
  sink.append(prefix);
  sink.append(token);
  return true;
}

auto oauth_challenge_parameter(const std::string_view header,
                               const std::string_view scheme,
                               const std::string_view name)
    -> std::optional<std::string> {
  // A local arena holds the unescaped bytes of a quoted value. The header is
  // only ever read, never grown, so it can never alias the arena, and the
  // matched value is returned by copy before the arena goes out of scope
  std::string arena;
  std::size_t position{0};
  // The scheme of the challenge currently being read. RFC 7235 shares the comma
  // between the challenge list and each challenge's parameter list, so a token
  // is a new scheme unless it is immediately followed by "=", in which case it
  // is a parameter of the current challenge
  std::string_view current_scheme{};
  // Whether the current challenge has already taken a parameter, and whether
  // the next element must be preceded by a comma. RFC 9110 Section 5.6.1
  // separates list elements with commas, so a parameter or challenge that
  // follows another without one is malformed rather than silently reassociated
  bool scheme_has_parameter{false};
  bool separator_required{false};
  while (position < header.size()) {
    position = oauth_skip_ows(header, position);
    if (position >= header.size()) {
      break;
    }

    if (header[position] == ',') {
      position += 1;
      separator_required = false;
      // A comma before any parameter ends an empty challenge, so a parameter
      // that follows cannot attach to that scheme
      if (!scheme_has_parameter) {
        current_scheme = {};
      }

      continue;
    }

    // The element is not a comma, so if one was required the header is
    // malformed
    if (separator_required) {
      return std::nullopt;
    }

    const std::size_t token_start{position};
    while (position < header.size() && http_is_tchar(header[position])) {
      position += 1;
    }

    if (position == token_start) {
      return std::nullopt;
    }

    const auto token{header.substr(token_start, position - token_start)};
    const auto after_token{position};
    const auto equals{oauth_skip_ows(header, position)};
    if (equals < header.size() && header[equals] == '=') {
      if (current_scheme.empty()) {
        return std::nullopt;
      }

      position = oauth_skip_ows(header, equals + 1);
      const bool match{equals_ignore_case(current_scheme, scheme) &&
                       equals_ignore_case(token, name)};
      std::string_view value{};
      if (position < header.size() && header[position] == '"') {
        arena.clear();
        const auto next{
            http_scan_quoted_string(header, position, arena, value)};
        if (!next.has_value()) {
          return std::nullopt;
        }

        position = next.value();
      } else {
        const std::size_t value_start{position};
        while (position < header.size() && http_is_tchar(header[position])) {
          position += 1;
        }

        if (position == value_start) {
          return std::nullopt;
        }

        value = header.substr(value_start, position - value_start);
      }

      scheme_has_parameter = true;
      separator_required = true;
      if (match) {
        return std::string{value};
      }

      continue;
    }

    // The token is a new scheme. It may be followed by a token68 credential,
    // which is skipped whole so the scan reaches the next challenge
    current_scheme = token;
    scheme_has_parameter = false;
    const auto separator{oauth_skip_ows(header, after_token)};
    if (separator > after_token && separator < header.size()) {
      std::size_t blob{separator};
      while (blob < header.size() && http_is_b64token_char(header[blob])) {
        blob += 1;
      }

      while (blob < header.size() && header[blob] == '=') {
        blob += 1;
      }

      // A token68 must be a valid token68 (RFC 7235 Section 2.1) and must end
      // the challenge, so a bare padding run is not one and a parameter cannot
      // attach to the token68 form of a challenge
      const auto terminator{oauth_skip_ows(header, blob)};
      if ((terminator >= header.size() || header[terminator] == ',') &&
          http_is_b64token(header.substr(separator, blob - separator))) {
        position = blob;
        current_scheme = {};
        separator_required = true;
      }
    }
  }

  return std::nullopt;
}

} // namespace sourcemeta::core
