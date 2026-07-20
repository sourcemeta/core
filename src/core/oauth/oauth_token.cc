#include <sourcemeta/core/oauth_token.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/text.h>

#include "oauth_encode.h"

#include <chrono>      // std::chrono::seconds
#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

const auto HASH_ACCESS_TOKEN{JSON::Object::hash("access_token"sv)};
const auto HASH_TOKEN_TYPE{JSON::Object::hash("token_type"sv)};
const auto HASH_EXPIRES_IN{JSON::Object::hash("expires_in"sv)};
const auto HASH_REFRESH_TOKEN{JSON::Object::hash("refresh_token"sv)};
const auto HASH_SCOPE{JSON::Object::hash("scope"sv)};

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

auto oauth_append_resources(SecureString &sink,
                            const std::span<const OAuthParameter> resources)
    -> void {
  for (const auto &resource : resources) {
    oauth_append_form_parameter(sink, resource.name, resource.value);
  }
}

} // namespace

auto oauth_build_token_request_code(
    const std::string_view code, const std::string_view redirect_uri,
    const std::string_view code_verifier,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void {
  oauth_append_form_parameter(sink, "grant_type", "authorization_code");
  oauth_append_form_parameter(sink, "code", code);
  if (!redirect_uri.empty()) {
    oauth_append_form_parameter(sink, "redirect_uri", redirect_uri);
  }

  if (!code_verifier.empty()) {
    oauth_append_form_parameter(sink, "code_verifier", code_verifier);
  }

  oauth_append_resources(sink, resources);
}

auto oauth_build_token_request_refresh(
    const std::string_view refresh_token, const std::string_view scope,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void {
  oauth_append_form_parameter(sink, "grant_type", "refresh_token");
  oauth_append_form_parameter(sink, "refresh_token", refresh_token);
  if (!scope.empty()) {
    oauth_append_form_parameter(sink, "scope", scope);
  }

  oauth_append_resources(sink, resources);
}

auto oauth_build_token_request_client_credentials(
    const std::string_view scope,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void {
  oauth_append_form_parameter(sink, "grant_type", "client_credentials");
  if (!scope.empty()) {
    oauth_append_form_parameter(sink, "scope", scope);
  }

  oauth_append_resources(sink, resources);
}

OAuthTokenResponse::OAuthTokenResponse(const JSON &data) : data_{&data} {}

auto OAuthTokenResponse::access_token() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "access_token"sv, HASH_ACCESS_TOKEN);
}

auto OAuthTokenResponse::token_type() const -> std::optional<std::string_view> {
  return string_member(*this->data_, "token_type"sv, HASH_TOKEN_TYPE);
}

auto OAuthTokenResponse::is_bearer_token_type() const -> bool {
  const auto type{this->token_type()};
  // RFC 6749 Section 5.1: the token type value is compared case insensitively
  return type.has_value() && equals_ignore_case(type.value(), "Bearer");
}

auto OAuthTokenResponse::expires_in() const
    -> std::optional<std::chrono::seconds> {
  if (!this->data_->is_object()) {
    return std::nullopt;
  }

  const auto *member{this->data_->try_at("expires_in"sv, HASH_EXPIRES_IN)};
  if (member == nullptr || !member->is_integer()) {
    return std::nullopt;
  }

  const auto value{member->to_integer()};
  if (value < 0) {
    return std::nullopt;
  }

  return std::chrono::seconds{value};
}

auto OAuthTokenResponse::refresh_token() const
    -> std::optional<std::string_view> {
  return string_member(*this->data_, "refresh_token"sv, HASH_REFRESH_TOKEN);
}

auto OAuthTokenResponse::scope() const -> std::optional<std::string_view> {
  return string_member(*this->data_, "scope"sv, HASH_SCOPE);
}

auto OAuthTokenResponse::has_scope(const std::string_view value) const -> bool {
  const auto granted{this->scope()};
  if (!granted.has_value() || value.empty()) {
    return false;
  }

  // RFC 6749 Section 3.3: scope is a space-delimited, unordered set of tokens,
  // scanned without allocation
  const auto text{granted.value()};
  std::size_t position{0};
  while (position < text.size()) {
    const auto next{text.find(' ', position)};
    const auto token{text.substr(position, next == std::string_view::npos
                                               ? std::string_view::npos
                                               : next - position)};
    if (token == value) {
      return true;
    }

    if (next == std::string_view::npos) {
      break;
    }

    position = next + 1;
  }

  return false;
}

auto OAuthTokenResponse::data() const -> const JSON & { return *this->data_; }

} // namespace sourcemeta::core
