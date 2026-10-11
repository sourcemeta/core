#ifndef SOURCEMETA_CORE_HTTP_TRACECONTEXT_H_
#define SOURCEMETA_CORE_HTTP_TRACECONTEXT_H_

#ifndef SOURCEMETA_CORE_HTTP_EXPORT
#include <sourcemeta/core/http_export.h>
#endif

#include <sourcemeta/core/http_syntax.h>
#include <sourcemeta/core/text.h>

#include <concepts>    // std::invocable
#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup http
/// The fields of a traceparent field value. Every view borrows the input, so
/// the value these came from must outlive them.
///
/// @see https://www.w3.org/TR/trace-context/#traceparent-header
struct HTTPTraceParent {
  /// The revision of the field format this value was written to.
  std::uint8_t version;
  /// Identifies the trace this request belongs to, as 32 lowercase hexadecimal
  /// characters.
  std::string_view trace_id;
  /// Identifies the operation that made this request, as 16 lowercase
  /// hexadecimal characters.
  std::string_view parent_id;
  /// An eight bit field of which one bit is currently defined.
  std::uint8_t flags;
};

/// @ingroup http
/// Whether the sampled bit is set, which records that the caller may have
/// recorded trace data rather than that it did. The remaining bits are
/// reserved and a reader is told to mask rather than compare: "As this is a
/// bit field, you cannot interpret flags by decoding the hex value and looking
/// at the resulting number". For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_traceparent_sampled(0x01));
/// assert(!sourcemeta::core::http_traceparent_sampled(0x00));
/// assert(sourcemeta::core::http_traceparent_sampled(0x05));
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#sampled-flag
constexpr auto http_traceparent_sampled(const std::uint8_t flags) noexcept
    -> bool {
  return (flags & 0x01U) != 0;
}

/// @ingroup http
/// Whether the input is a well-formed traceparent field value (RFC 5234 ABNF
/// in W3C Trace Context Section 3.2.2).
///
/// A value of exactly 55 characters is well formed whatever its version, since
/// a reader is told to "check that the 2 characters are either the end of the
/// string or a dash", and a longer value must carry a dash where the flags
/// end. Version `ff` is forbidden, and an identifier of all zeroes is
/// "considered an invalid value". For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_is_traceparent(
///     "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));
/// assert(!sourcemeta::core::http_is_traceparent(
///     "ff-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));
/// assert(!sourcemeta::core::http_is_traceparent(
///     "00-00000000000000000000000000000000-b7ad6b7169203331-01"));
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#traceparent-header-field-values
SOURCEMETA_CORE_HTTP_EXPORT
auto http_is_traceparent(std::string_view value) noexcept -> bool;

/// @ingroup http
/// Parse a traceparent field value into its fields, or nothing when it is not
/// well formed. The identifiers borrow the input. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto parent{sourcemeta::core::http_parse_traceparent(
///     "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01")};
/// assert(parent.has_value());
/// assert(parent->version == 0);
/// assert(parent->trace_id == "0af7651916cd43dd8448eb211c80319c");
/// assert(parent->parent_id == "b7ad6b7169203331");
/// assert(sourcemeta::core::http_traceparent_sampled(parent->flags));
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#traceparent-header-field-values
SOURCEMETA_CORE_HTTP_EXPORT
auto http_parse_traceparent(std::string_view value) noexcept
    -> std::optional<HTTPTraceParent>;

/// @ingroup http
/// Whether the input is a well-formed tracestate field value (RFC 5234 ABNF in
/// W3C Trace Context Section 3.3.1).
///
/// The grammar is permissive in ways that surprise: at most 32 members, and
/// "Empty and whitespace-only list members are allowed. Vendors MUST accept
/// empty tracestate headers". A member's value, in contrast, cannot be empty
/// and cannot end in a space, since the production ends in a mandatory
/// non-blank character. Note that baggage differs on every one of those
/// points.
///
/// This rejects a repeated key, which is stricter than the grammar. The
/// grammar does not forbid one, but a key records a position in the trace and
/// "Only one entry per key is allowed", so a repeat is a producer that
/// appended where it should have overwritten. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_is_tracestate(
///     "rojo=00f067aa0ba902b7,congo=t61rcWkgMzE"));
/// assert(sourcemeta::core::http_is_tracestate(""));
/// assert(!sourcemeta::core::http_is_tracestate("foo="));
/// assert(!sourcemeta::core::http_is_tracestate("a=1,a=2"));
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#tracestate-header
SOURCEMETA_CORE_HTTP_EXPORT
auto http_is_tracestate(std::string_view value) noexcept -> bool;

/// @ingroup http
/// Visit each tracestate member in order as a key and a value, skipping the
/// empty members the grammar allows. The whole value is checked before
/// anything is visited, so a caller never acts on part of a malformed value,
/// and nothing is visited when it does not parse. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// std::string flat;
/// assert(sourcemeta::core::http_parse_tracestate(
///     "rojo=1 , , congo=2",
///     [&flat](const auto key, const auto value) {
///       flat += std::string{key} + "=" + std::string{value} + ";";
///     }));
/// assert(flat == "rojo=1;congo=2;");
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#list
template <typename Callback>
  requires std::invocable<Callback, std::string_view, std::string_view>
auto http_parse_tracestate(const std::string_view value, Callback callback)
    -> bool {
  if (!http_is_tracestate(value)) {
    return false;
  }
  auto rest{value};
  while (true) {
    const auto comma{rest.find(',')};
    const auto entry{trim(rest.substr(0, comma), http_is_ows)};
    if (!entry.empty()) {
      const auto equal{entry.find('=')};
      callback(entry.substr(0, equal), entry.substr(equal + 1));
    }
    if (comma == std::string_view::npos) {
      return true;
    }
    rest.remove_prefix(comma + 1);
  }
}

/// @ingroup http
/// Collect each tracestate member in order into a container of pairs. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string_view>
/// #include <utility>
/// #include <vector>
///
/// std::vector<std::pair<std::string_view, std::string_view>> members;
/// assert(sourcemeta::core::http_parse_tracestate("rojo=1,congo=2", members));
/// assert(members.size() == 2);
/// assert(members.front().first == "rojo");
/// ```
///
/// @see https://www.w3.org/TR/trace-context/#list
template <typename Container>
  requires requires(Container container, std::string_view entry) {
    container.emplace_back(entry, entry);
  }
auto http_parse_tracestate(const std::string_view value, Container &members)
    -> bool {
  return http_parse_tracestate(value,
                               [&members](const auto key, const auto entry) {
                                 members.emplace_back(key, entry);
                               });
}

} // namespace sourcemeta::core

#endif
