#ifndef SOURCEMETA_CORE_HTTP_H_
#define SOURCEMETA_CORE_HTTP_H_

#ifndef SOURCEMETA_CORE_HTTP_EXPORT
#include <sourcemeta/core/http_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/http_aws_sigv4.h>
#include <sourcemeta/core/http_error.h>
#include <sourcemeta/core/http_message.h>
#include <sourcemeta/core/http_method.h>
#include <sourcemeta/core/http_problem.h>
#include <sourcemeta/core/http_status.h>
#include <sourcemeta/core/http_syntax.h>
#include <sourcemeta/core/http_system.h>
// NOLINTEND(misc-include-cleaner)

#include <chrono>           // std::chrono::system_clock
#include <cstddef>          // std::size_t
#include <cstdint>          // std::uint8_t
#include <initializer_list> // std::initializer_list
#include <optional>         // std::optional
#include <span>             // std::span
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <utility>          // std::pair

/// @defgroup http HTTP
/// @brief An implementation of HTTP-protocol parsing, formatting, and
/// validation primitives per RFC 9110.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// ```

namespace sourcemeta::core {

/// @ingroup http
/// A content coding supported by this implementation.
enum class HTTPContentEncoding : std::uint8_t {
  /// The identity coding that applies no transformation.
  Identity,
  /// The gzip coding per RFC 9110 §8.4.1.3.
  GZIP,
};

/// @ingroup http
/// Pick the best media-type candidate against an `Accept` header per RFC 9110
/// §12.5.1. Returns an empty value when no candidate is acceptable. The
/// returned view borrows from `candidates`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto best{sourcemeta::core::http_match_accept(
///     "text/html, application/json;q=0.9",
///     {"text/html", "application/json"})};
/// assert(best == "text/html");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_match_accept(const std::string_view accept_header,
                       std::initializer_list<std::string_view> candidates)
    -> std::string_view;

/// @ingroup http
/// Test whether every media type is individually acceptable under an `Accept`
/// header per RFC 9110 §12.5.1. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_accept_includes_all(
///     "text/html, application/json",
///     {"text/html", "application/json"}));
/// assert(!sourcemeta::core::http_accept_includes_all(
///     "text/html;q=0, application/json", {"text/html"}));
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_accept_includes_all(
    const std::string_view accept_header,
    std::initializer_list<std::string_view> media_types) noexcept -> bool;

/// @ingroup http
/// Test whether a `Content-Type` header denotes the given media type per RFC
/// 9110 §8.3.1. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_content_type_matches(
///     "application/json; charset=UTF-8", "application/json"));
/// assert(!sourcemeta::core::http_content_type_matches(
///     "application/xml", "application/json"));
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_content_type_matches(const std::string_view content_type_header,
                               const std::string_view media_type) noexcept
    -> bool;

/// @ingroup http
/// Pick the best language-tag candidate against an `Accept-Language` header
/// per RFC 9110 §12.5.4, which lets an implementation choose its matching
/// scheme. This uses a q-aware Basic-Filtering-style scheme (RFC 4647 §3.3.1,
/// the scheme RFC 9110 §12.5.4 points to) that also honors `q=0` exclusions.
/// Returns an empty value when no candidate is acceptable. The returned view
/// borrows from `candidates`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto best{sourcemeta::core::http_match_accept_language(
///     "fr-CA;q=0.9, en;q=0.8", {"en", "fr"})};
/// assert(best == "fr");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_match_accept_language(
    const std::string_view accept_language_header,
    std::initializer_list<std::string_view> candidates) -> std::string_view;

/// @ingroup http
/// Resolve a content coding against an `Accept-Encoding` header per RFC 9110
/// §12.5.3. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto chosen{sourcemeta::core::http_negotiate_encoding(
///     "gzip, identity;q=0.5", sourcemeta::core::HTTPContentEncoding::GZIP)};
/// assert(chosen.has_value());
/// assert(chosen.value() == sourcemeta::core::HTTPContentEncoding::GZIP);
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_negotiate_encoding(
    const std::string_view accept_encoding_header,
    const HTTPContentEncoding server_preference) noexcept
    -> std::optional<HTTPContentEncoding>;

/// @ingroup http
/// Parse an HTTP-date string per RFC 9110 §5.6.7. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_from_date(
///     "Sun, 06 Nov 1994 08:49:37 GMT").has_value());
/// assert(sourcemeta::core::http_from_date(
///     "Sunday, 06-Nov-94 08:49:37 GMT").has_value());
/// assert(sourcemeta::core::http_from_date(
///     "Sun Nov  6 08:49:37 1994").has_value());
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_from_date(const std::string_view value) noexcept
    -> std::optional<std::chrono::system_clock::time_point>;

/// @ingroup http
/// Read the `max-age` response directive from a `Cache-Control` header value
/// per RFC 9111 §5.2.2.1. Returns an empty value when the directive is absent
/// or malformed. A value larger than the cache can represent saturates to
/// `2147483648` seconds as mandated by RFC 9111 §1.2.2. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <chrono>
///
/// const auto max_age{sourcemeta::core::http_cache_control_max_age(
///     "public, max-age=600")};
/// assert(max_age.has_value());
/// assert(max_age.value() == std::chrono::seconds{600});
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_cache_control_max_age(const std::string_view cache_control) noexcept
    -> std::optional<std::chrono::seconds>;

/// @ingroup http
/// A typed RFC 8288 §3 link-value. The caller owns the backing storage for
/// every field, must URI-escape `target`, and must ensure parameter values are
/// valid `quoted-string` content.
struct HTTPLink {
  /// The link target reference
  std::string_view target;
  /// The link relation type
  std::string_view rel;
  /// The additional target attributes of the link
  std::span<const std::pair<std::string_view, std::string_view>> parameters{};
};

/// @ingroup http
/// Append an RFC 8288 §3 link-value to `out`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// std::string buffer{"prefix:"};
/// sourcemeta::core::http_format_link(
///     {.target = "/schema.json", .rel = "describedby"}, buffer);
/// assert(buffer == "prefix:</schema.json>; rel=\"describedby\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_link(const HTTPLink &link, std::string &out) -> void;

/// @ingroup http
/// Format an RFC 8288 §3 link-value. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto value{sourcemeta::core::http_format_link(
///     {.target = "https://example.com/schema.json", .rel = "describedby"})};
/// assert(value ==
///   "<https://example.com/schema.json>; rel=\"describedby\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_link(const HTTPLink &link) -> std::string;

/// @ingroup http
/// Append an RFC 8288 §3.5 comma-separated multi-link value to `out`. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// const sourcemeta::core::HTTPLink links[]{
///     {.target = "/here", .rel = "self"},
///     {.target = "/next", .rel = "next"}};
/// std::string buffer;
/// sourcemeta::core::http_format_links(links, buffer);
/// assert(buffer ==
///   "</here>; rel=\"self\", </next>; rel=\"next\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_links(std::span<const HTTPLink> links, std::string &out)
    -> void;

/// @ingroup http
/// Format an RFC 8288 §3.5 comma-separated multi-link value. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const sourcemeta::core::HTTPLink links[]{
///     {.target = "/here", .rel = "self"},
///     {.target = "/next", .rel = "next"}};
/// const auto value{sourcemeta::core::http_format_links(links)};
/// assert(value ==
///   "</here>; rel=\"self\", </next>; rel=\"next\"");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_links(std::span<const HTTPLink> links) -> std::string;

/// @ingroup http
/// The `SameSite` attribute of a cookie per RFC 6265bis §5.2.
enum class HTTPCookieSameSite : std::uint8_t {
  /// The cookie is withheld from every cross-site request.
  Strict,
  /// The cookie is sent on top-level cross-site navigations only.
  Lax,
  /// The cookie is sent on every cross-site request.
  None
};

/// @ingroup http
/// A cookie to serialise into an RFC 6265 §4.1 `Set-Cookie` response header
/// value. The caller owns the backing storage for every field. A valid cookie
/// has a name that is an RFC 9110 §5.6.2 token and a value made of RFC 6265
/// §4.1.1 cookie-octets. RFC 6265bis §5.7 requires a cookie with a same-site
/// mode of none to also be secure.
struct HTTPCookie {
  /// The cookie name
  std::string_view name{};
  /// The cookie value
  std::string_view value{};
  /// The path the cookie is scoped to
  std::optional<std::string_view> path{};
  /// The host the cookie is scoped to
  std::optional<std::string_view> domain{};
  /// The cookie lifetime
  std::optional<std::chrono::seconds> max_age{};
  /// Whether the cookie is withheld from scripts
  bool http_only{false};
  /// Whether the cookie is only sent over secure channels
  bool secure{false};
  /// The cross-site request policy for the cookie
  std::optional<HTTPCookieSameSite> same_site{};
};

/// @ingroup http
/// The RFC 6265bis §5.7 ceiling on the sum of the lengths of a cookie name and
/// its value, past which a user agent ignores the cookie entirely. Note that
/// the RFC 6265 §6.1 minimum capability names the same number but measures the
/// attributes into it as well, so a cookie sized against this one is not
/// necessarily within that older bound.
inline constexpr std::size_t HTTP_COOKIE_MAXIMUM_NAME_VALUE_LENGTH{4096};

/// @ingroup http
/// The RFC 6265bis §5.6 ceiling on the length of a single cookie attribute
/// value, past which a user agent ignores that attribute while keeping the
/// cookie, silently widening or narrowing the scope the server asked for.
inline constexpr std::size_t HTTP_COOKIE_MAXIMUM_ATTRIBUTE_VALUE_LENGTH{1024};

/// @ingroup http
/// Test whether a cookie can be serialised into a valid RFC 6265 §4.1
/// `Set-Cookie` header value: the name is a non-empty RFC 9110 §5.6.2 token,
/// the value is made of RFC 6265 §4.1.1 cookie-octets, any path is made of RFC
/// 6265bis av-octets, any domain is a valid RFC 1123 host name allowing an
/// ignorable leading dot, a present `max_age` is not negative, per RFC 6265bis
/// §5.7 a `HTTPCookieSameSite::None` cookie is also `secure`, and the RFC
/// 6265bis §4.1.3 `__Secure-` and `__Host-` name prefixes carry their required
/// attributes. The RFC 6265bis §5.6 and §5.7 length ceilings are enforced too,
/// since a cookie past either is ignored in whole or in part by the user
/// agent. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_cookie_valid({.name = "a", .value = "b"}));
/// assert(!sourcemeta::core::http_cookie_valid({.name = "a", .value = "b;c"}));
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_cookie_valid(const HTTPCookie &cookie) -> bool;

/// @ingroup http
/// Append an RFC 6265 §4.1 `Set-Cookie` header value to `out`, returning `true`
/// on success. When the cookie is not `http_cookie_valid`, `out` is left
/// unchanged and this returns `false`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
/// #include <string>
///
/// std::string buffer;
/// const auto ok{sourcemeta::core::http_serialize_cookie(
///     {.name = "session", .value = "abc", .http_only = true}, buffer)};
/// assert(ok);
/// assert(buffer == "session=abc; HttpOnly");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_serialize_cookie(const HTTPCookie &cookie, std::string &out) -> bool;

/// @ingroup http
/// Serialise an RFC 6265 §4.1 `Set-Cookie` header value, returning no value
/// when the cookie is not `http_cookie_valid`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto value{sourcemeta::core::http_serialize_cookie(
///     {.name = "session", .value = "abc", .secure = true})};
/// assert(value == "session=abc; Secure");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_serialize_cookie(const HTTPCookie &cookie)
    -> std::optional<std::string>;

/// @ingroup http
/// Derive the cookie that removes a cookie, carrying its name and every
/// attribute across so that the RFC 6265 §3.1 requirement to match the path
/// and the domain of the original is met by construction. The value is
/// dropped, and the lifetime is set to zero, which RFC 6265bis §5.6.2 maps to
/// "the earliest representable date and time". For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// const auto value{sourcemeta::core::http_serialize_cookie(
///     sourcemeta::core::http_expire_cookie(
///         {.name = "session", .value = "abc", .http_only = true}))};
/// assert(value == "session=; Max-Age=0; HttpOnly");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_expire_cookie(const HTTPCookie &cookie) -> HTTPCookie;

/// @ingroup http
/// Test whether a comma-separated header value per RFC 9110 §5.6.1 lists any
/// of the given tokens. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_field_list_contains_any(
///     "\"abc\", W/\"def\", *", {"*"}));
/// assert(!sourcemeta::core::http_field_list_contains_any(
///     "\"abc\", \"def\"", {"\"xyz\""}));
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_field_list_contains_any(
    const std::string_view header_value,
    std::initializer_list<std::string_view> tokens) noexcept -> bool;

/// @ingroup http
/// Append an RFC 9110 §12.5.5 `Vary` header value to `out`, returning `true`
/// on success. Every member must be a non-empty RFC 9110 §5.6.2 token, which
/// the wildcard also is, since §5.1 defines a field name as one and §5.6.1.1
/// forbids a sender from generating an empty list element. An empty list has
/// nothing to send, so the header is left to be omitted rather than emitted
/// blank. The spelling of each name is kept as given, as §5.1 makes field
/// names case-insensitive. Nothing is appended and this returns `false` when
/// any member is refused, which keeps a name from injecting a further field.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <array>
/// #include <cassert>
/// #include <string>
/// #include <string_view>
///
/// const std::array<std::string_view, 2> names{{"Accept", "Accept-Encoding"}};
/// std::string buffer;
/// const auto ok{sourcemeta::core::http_format_vary(names, buffer)};
/// assert(ok);
/// assert(buffer == "Accept, Accept-Encoding");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_vary(std::span<const std::string_view> field_names,
                      std::string &out) -> bool;

/// @ingroup http
/// Compose an RFC 9110 §12.5.5 `Vary` header value, returning no value when
/// any member is not an RFC 9110 §5.6.2 token or when there are none.
///
/// Note that RFC 9110 §12.5.5 forbids a proxy from generating the wildcard
/// member, so only an origin server may compose one. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <array>
/// #include <cassert>
/// #include <string_view>
///
/// const std::array<std::string_view, 2> names{{"Accept", "Accept-Encoding"}};
/// const auto value{sourcemeta::core::http_format_vary(names)};
/// assert(value == "Accept, Accept-Encoding");
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_format_vary(std::span<const std::string_view> field_names)
    -> std::optional<std::string>;

/// @ingroup http
/// Extract the credential from an `Authorization` header that uses the Bearer
/// scheme per RFC 6750 §2.1, matching the scheme case-insensitively per RFC
/// 9110 §11.1 and tolerating optional whitespace around the token. Returns an
/// empty view when the header is absent, uses another scheme, or does not carry
/// a well-formed `b64token` credential. For example:
///
/// ```cpp
/// #include <sourcemeta/core/http.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::http_parse_bearer("Bearer abc123") == "abc123");
/// assert(sourcemeta::core::http_parse_bearer("Basic abc123").empty());
/// ```
SOURCEMETA_CORE_HTTP_EXPORT
auto http_parse_bearer(const std::string_view authorization) noexcept
    -> std::string_view;

} // namespace sourcemeta::core

#endif
