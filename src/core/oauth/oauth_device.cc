#include <sourcemeta/core/oauth_device.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/text.h>

#include "oauth_encode.h"

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int64_t, std::uint8_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

// RFC 8628 Section 6.1: the recommended base-20 user code alphabet, the
// uppercase letters A to Z with the vowels and Y removed so a code cannot form
// a word
constexpr std::string_view USER_CODE_ALPHABET{"BCDFGHJKLMNPQRSTVWXZ"};
constexpr std::chrono::seconds DEFAULT_INTERVAL{5};

const auto HASH_DEVICE_CODE{JSON::Object::hash("device_code"sv)};
const auto HASH_USER_CODE{JSON::Object::hash("user_code"sv)};
const auto HASH_VERIFICATION_URI{JSON::Object::hash("verification_uri"sv)};
const auto HASH_VERIFICATION_URI_COMPLETE{
    JSON::Object::hash("verification_uri_complete"sv)};
const auto HASH_EXPIRES_IN{JSON::Object::hash("expires_in"sv)};
const auto HASH_INTERVAL{JSON::Object::hash("interval"sv)};

auto string_member(const JSON &data, const JSON::StringView name,
                   const JSON::Object::hash_type hash)
    -> std::optional<std::string_view> {
  if (!data.is_object()) {
    return std::nullopt;
  }

  const auto *member{data.try_at(name, hash)};
  if (member == nullptr || !member->is_string()) {
    return std::nullopt;
  }

  return std::string_view{member->to_string()};
}

auto seconds_member(const JSON &data, const JSON::StringView name,
                    const JSON::Object::hash_type hash)
    -> std::optional<std::chrono::seconds> {
  if (!data.is_object()) {
    return std::nullopt;
  }

  const auto *member{data.try_at(name, hash)};
  if (member == nullptr || !member->is_integer()) {
    return std::nullopt;
  }

  return std::chrono::seconds{member->to_integer()};
}

auto normalize_user_code(const std::string_view value) -> std::string {
  std::string result;
  result.reserve(value.size());
  for (const auto character : value) {
    if (!is_alphanum(character)) {
      continue;
    }

    // The alphabet is uppercase, so a lowercase ASCII letter is folded up
    if (character >= 'a' && character <= 'z') {
      result.push_back(static_cast<char>(character - ('a' - 'A')));
    } else {
      result.push_back(character);
    }
  }

  return result;
}

} // namespace

auto oauth_build_device_authorization_request(
    const std::string_view client_id, const std::string_view scope,
    const std::span<const OAuthParameter> resources, std::string &sink)
    -> void {
  if (!client_id.empty()) {
    oauth_append_form_parameter(sink, "client_id", client_id);
  }

  if (!scope.empty()) {
    oauth_append_form_parameter(sink, "scope", scope);
  }

  for (const auto &resource : resources) {
    oauth_append_form_parameter(sink, resource.name, resource.value);
  }
}

auto oauth_build_token_request_device(
    const std::string_view device_code,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void {
  oauth_append_form_parameter(sink, "grant_type",
                              "urn:ietf:params:oauth:grant-type:device_code");
  oauth_append_form_parameter(sink, "device_code", device_code);
  for (const auto &resource : resources) {
    oauth_append_form_parameter(sink, resource.name, resource.value);
  }
}

OAuthDeviceAuthorizationResponse::OAuthDeviceAuthorizationResponse(
    const JSON &data)
    : data_{&data} {}

auto OAuthDeviceAuthorizationResponse::device_code() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "device_code"sv, HASH_DEVICE_CODE);
}

auto OAuthDeviceAuthorizationResponse::user_code() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "user_code"sv, HASH_USER_CODE);
}

auto OAuthDeviceAuthorizationResponse::verification_uri() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "verification_uri"sv,
                       HASH_VERIFICATION_URI);
}

auto OAuthDeviceAuthorizationResponse::verification_uri_complete() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "verification_uri_complete"sv,
                       HASH_VERIFICATION_URI_COMPLETE);
}

auto OAuthDeviceAuthorizationResponse::expires_in() const
    -> std::optional<std::chrono::seconds> {
  return seconds_member(*this->data_, "expires_in"sv, HASH_EXPIRES_IN);
}

auto OAuthDeviceAuthorizationResponse::interval() const
    -> std::chrono::seconds {
  // RFC 8628 Section 3.2: absent interval means the default polling interval
  return seconds_member(*this->data_, "interval"sv, HASH_INTERVAL)
      .value_or(DEFAULT_INTERVAL);
}

auto OAuthDeviceAuthorizationResponse::data() const -> const JSON & {
  return *this->data_;
}

OAuthDevicePoller::OAuthDevicePoller(
    const std::chrono::seconds interval, const std::chrono::seconds lifetime,
    const std::chrono::system_clock::time_point start) noexcept
    : interval_{interval > std::chrono::seconds{0} ? interval
                                                   : DEFAULT_INTERVAL},
      lifetime_{lifetime}, start_{start} {}

auto OAuthDevicePoller::interval() const noexcept -> std::chrono::seconds {
  return this->interval_;
}

auto OAuthDevicePoller::expired(
    const std::chrono::system_clock::time_point now) const noexcept -> bool {
  return now >= this->start_ + this->lifetime_;
}

auto OAuthDevicePoller::observe(const OAuthTokenError error) noexcept
    -> OAuthDevicePollDecision {
  switch (error) {
    case OAuthTokenError::SlowDown:
      // RFC 8628 Section 3.5: slow_down permanently increases the interval by
      // five seconds
      this->interval_ += std::chrono::seconds{5};
      return OAuthDevicePollDecision::Continue;
    case OAuthTokenError::AuthorizationPending:
      return OAuthDevicePollDecision::Continue;
    case OAuthTokenError::AccessDenied:
      return OAuthDevicePollDecision::Denied;
    case OAuthTokenError::ExpiredToken:
      return OAuthDevicePollDecision::Expired;
    default:
      return OAuthDevicePollDecision::Error;
  }
}

auto oauth_make_device_authorization_response(
    const std::string_view device_code, const std::string_view user_code,
    const std::string_view verification_uri,
    const std::string_view verification_uri_complete,
    const std::chrono::seconds expires_in, const std::chrono::seconds interval)
    -> JSON {
  auto response{JSON::make_object()};
  response.assign_assume_new("device_code", JSON{device_code},
                             HASH_DEVICE_CODE);
  response.assign_assume_new("user_code", JSON{user_code}, HASH_USER_CODE);
  response.assign_assume_new("verification_uri", JSON{verification_uri},
                             HASH_VERIFICATION_URI);
  if (!verification_uri_complete.empty()) {
    response.assign_assume_new("verification_uri_complete",
                               JSON{verification_uri_complete},
                               HASH_VERIFICATION_URI_COMPLETE);
  }

  response.assign_assume_new(
      "expires_in", JSON{static_cast<std::int64_t>(expires_in.count())},
      HASH_EXPIRES_IN);
  // RFC 8628 Section 3.2: the interval is emitted only when it differs from the
  // default the client would otherwise assume
  if (interval != DEFAULT_INTERVAL) {
    response.assign_assume_new(
        "interval", JSON{static_cast<std::int64_t>(interval.count())},
        HASH_INTERVAL);
  }

  return response;
}

auto oauth_device_user_code() -> std::array<char, 8> {
  std::array<char, 8> result{};
  const auto alphabet{static_cast<std::uint8_t>(USER_CODE_ALPHABET.size())};
  // Reject bytes in the biased tail so each character is drawn uniformly from
  // the alphabet, preserving the RFC 8628 Section 6.1 entropy of the code
  const auto limit{static_cast<std::uint8_t>(256U / alphabet * alphabet)};
  std::size_t filled{0};
  while (filled < result.size()) {
    std::array<std::uint8_t, 8> entropy{};
    random_bytes(entropy);
    for (const auto byte : entropy) {
      if (byte >= limit) {
        continue;
      }

      result[filled] = USER_CODE_ALPHABET[byte % alphabet];
      filled += 1;
      if (filled == result.size()) {
        break;
      }
    }
  }

  return result;
}

auto oauth_device_user_code_matches(const std::string_view presented,
                                    const std::string_view stored) -> bool {
  const auto normalized_presented{normalize_user_code(presented)};
  const auto normalized_stored{normalize_user_code(stored)};
  // A code has content, so an empty normalized value on either side, from an
  // empty or punctuation-only input, never matches, since secure_equals treats
  // two empty strings as equal
  if (normalized_presented.empty() || normalized_stored.empty()) {
    return false;
  }

  return secure_equals(normalized_presented, normalized_stored);
}

} // namespace sourcemeta::core
