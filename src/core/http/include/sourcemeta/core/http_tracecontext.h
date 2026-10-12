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
  /// The revision of the field format this value was written to. Note that a
  /// value being forwarded is not sent on under this revision but under "the
  /// highest version of the specification known to the implementation", so a
  /// higher one read here is reported rather than endorsed.
  ///
  /// @see https://www.w3.org/TR/trace-context/#versioning-of-traceparent
  std::uint8_t version{0};
  /// Identifies the trace this request belongs to, as 32 lowercase hexadecimal
  /// characters
  std::string_view trace_id;
  /// Identifies the operation that made this request, as 16 lowercase
  /// hexadecimal characters
  std::string_view parent_id;
  /// An eight bit field of which one bit is currently defined
  std::uint8_t flags{0};
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
/// @see https://www.w3.org/TR/trace-context/#trace-flags
/// @see https://www.w3.org/TR/trace-context/#sampled-flag
constexpr auto http_traceparent_sampled(const std::uint8_t flags) noexcept
    -> bool {
  return (flags & 0x01U) != 0;
}

/// @ingroup http
/// Whether the input is a well-formed traceparent field value. The grammar in
/// W3C Trace Context Section 3.2.2 is written for version `00`, and a higher
/// version is admitted under the rules for reading one in Section 3.2.4.
///
/// Version `00` is exactly 55 characters and admits nothing further. A later
/// version may append, since a reader is told to "check that the 2 characters
/// are either the end of the string or a dash", so a longer value must carry a
/// dash where the flags end. Whatever follows that dash is left alone, because
/// "Vendors MUST NOT parse or assume anything about unknown fields for this
/// version", and refusing a value over those bytes would discard a trace the
/// same section says to keep. Version `ff` is invalid, and an identifier of
/// all zeroes is "considered an invalid value". For example:
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
auto http_is_traceparent(const std::string_view value) noexcept -> bool;

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
auto http_parse_traceparent(const std::string_view value) noexcept
    -> std::optional<HTTPTraceParent>;

/// @ingroup http
/// Whether the input is a well-formed tracestate field value (RFC 5234 ABNF in
/// W3C Trace Context Section 3.3.1).
///
/// The grammar is permissive in ways that surprise: at most 32 members, and
/// "Empty and whitespace-only list members are allowed. Vendors MUST accept
/// empty tracestate headers". Since "Spaces and horizontal tabs surrounding
/// list-members are ignored", a member padded on either side is accepted and
/// so is one made only of padding. Padding at the two outer ends of the whole
/// value is a little more than the list production writes down, and is
/// accepted on that same sentence. A member's value, in contrast, cannot be
/// empty. Note that baggage differs on every one of those points.
///
/// This rejects a repeated key, which the grammar alone does not forbid, on
/// the rule that "Only one entry per key is allowed because the entry
/// represents that last position in the trace". A reader is offered the
/// alternative of dropping just the repeated entry, but only in a section
/// marked non-normative. For example:
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
auto http_is_tracestate(const std::string_view value) noexcept -> bool;

/// @ingroup http
/// Visit each tracestate member in order as a key and a value, skipping the
/// empty members the grammar allows. The whole value is checked before
/// anything is visited, so a caller never acts on part of a malformed value,
/// and nothing is visited when it does not parse. Every key and value borrows
/// the input, so the value they came from must outlive them. For example:
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
inline auto http_parse_tracestate(const std::string_view value,
                                  Callback callback) -> bool {
  if (!http_is_tracestate(value)) {
    return false;
  }
  auto rest{value};
  while (true) {
    const auto comma{rest.find(',')};
    const auto entry{trim(rest.substr(0, comma), http_is_ows)};
    const auto equal{entry.find('=')};
    if (equal != std::string_view::npos) {
      callback(entry.substr(0, equal), entry.substr(equal + 1));
    }
    if (comma == std::string_view::npos) {
      return true;
    }
    rest.remove_prefix(comma + 1);
  }
}

/// @ingroup http
/// Collect each tracestate member in order into a container of pairs. Every
/// key and value borrows the input, so the value they came from must outlive
/// the container. For example:
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
inline auto http_parse_tracestate(const std::string_view value,
                                  Container &members) -> bool {
  return http_parse_tracestate(
      value,
      [&members](const std::string_view key, const std::string_view entry)
          -> void { members.emplace_back(key, entry); });
}

} // namespace sourcemeta::core

#endif
