#ifndef SOURCEMETA_CORE_HTTP_REQUEST_H_
#define SOURCEMETA_CORE_HTTP_REQUEST_H_

#ifndef SOURCEMETA_CORE_HTTP_EXPORT
#include <sourcemeta/core/http_export.h>
#endif

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace sourcemeta::core {

/// @ingroup http
/// Outcome of parsing the head of an HTTP/1.1 request.
enum class HTTPRequestHeadStatus : std::uint8_t {
  /// The request line and all header fields were parsed.
  Complete,
  /// More input is needed to finish a line or find the terminating blank line.
  Incomplete,
  /// The parser rejected a line or its ending.
  Malformed
};

/// @ingroup http
/// The parsed HTTP request line and field lines. String views borrow the input.
struct HTTPRequestHead {
  /// The outcome of the parse.
  HTTPRequestHeadStatus status{HTTPRequestHeadStatus::Incomplete};
  /// Bytes through the terminating blank line, or zero unless complete.
  std::size_t consumed{0};
  /// The method token as sent, or an empty view unless complete.
  std::string_view method{};
  /// The request target as sent, or an empty view unless complete.
  std::string_view target{};
  /// Field names and values in wire order, including repeated names.
  std::vector<std::pair<std::string_view, std::string_view>> fields{};
};

/// @ingroup http
/// Parse one HTTP/1.1 request head using CRLF line endings and single spaces
/// in the request line (RFC 9112 Sections 2, 3 and 5). The target is returned
/// verbatim after checking that it consists of visible ASCII characters.
/// Field values have leading and trailing optional whitespace removed.
/// Request-target forms, field-specific semantics (including Host) and body
/// framing need separate validation before serving a request.
///
/// Complete consumes through the blank line. Incomplete means another line
/// or line ending is needed. Malformed means a line or its ending was rejected.
/// Both failure results have zero bytes consumed and empty fields.
/// Callers can append input and retry from the beginning when incomplete.
/// Keep the input alive and unmodified while using the returned views.
SOURCEMETA_CORE_HTTP_EXPORT
auto http_parse_request_head(std::string_view input) -> HTTPRequestHead;

} // namespace sourcemeta::core

#endif
