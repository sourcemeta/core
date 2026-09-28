#ifndef SOURCEMETA_CORE_OAUTH_DURATION_H_
#define SOURCEMETA_CORE_OAUTH_DURATION_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <chrono>   // std::chrono::seconds
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt

namespace sourcemeta::core {

/// @ingroup oauth
/// Read an integer member of a JSON object as a duration in seconds, rejecting
/// a negative value as malformed, and one past the range of the duration so a
/// bad lifetime or interval cannot reach a caller narrowed. The OAuth and
/// OpenID Connect documents spell every lifetime and interval this way, so the
/// two families read them through this one function rather than each deciding
/// for itself what a malformed member looks like. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <sourcemeta/core/json.h>
/// #include <cassert>
///
/// const auto document{sourcemeta::core::parse_json("{ \"expires_in\": 1800
/// }")}; const auto hash{sourcemeta::core::JSON::Object::hash("expires_in")};
/// const auto value{
///     sourcemeta::core::oauth_json_seconds_member(document, "expires_in",
///     hash)};
/// assert(value.has_value());
/// assert(value.value() == std::chrono::seconds{1800});
/// ```
inline auto oauth_json_seconds_member(const JSON &data,
                                      const JSON::StringView name,
                                      const JSON::Object::hash_type hash)
    -> std::optional<std::chrono::seconds> {
  if (!data.is_object()) {
    return std::nullopt;
  }

  const auto *member{data.try_at(name, hash)};
  if (member == nullptr || !member->is_integer() || member->to_integer() < 0) {
    return std::nullopt;
  }

  // A JSON integer is a signed 64 bit value, whereas a duration is only
  // promised 35 bits, so a platform whose duration is the narrower of the two
  // has to range check the value before narrowing it. Where the two are the
  // same width the comparison cannot fail, and asking for it at compile time
  // discards it rather than leaving an unreachable branch behind
  if constexpr (std::numeric_limits<JSON::Integer>::max() >
                std::numeric_limits<std::chrono::seconds::rep>::max()) {
    if (member->to_integer() >
        std::numeric_limits<std::chrono::seconds::rep>::max()) {
      return std::nullopt;
    }
  }

  return std::chrono::seconds{member->to_integer()};
}

} // namespace sourcemeta::core

#endif
