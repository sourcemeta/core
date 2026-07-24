#include <sourcemeta/core/oidc_registration.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc_error.h>

#include <chrono>      // std::chrono::seconds
#include <cstddef>     // std::size_t
#include <limits>      // std::numeric_limits
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

constexpr auto HASH_REDIRECT_URIS{JSON::Object::hash("redirect_uris"sv)};
constexpr auto HASH_APPLICATION_TYPE{JSON::Object::hash("application_type"sv)};
constexpr auto HASH_SUBJECT_TYPE{JSON::Object::hash("subject_type"sv)};
constexpr auto HASH_SECTOR_IDENTIFIER_URI{
    JSON::Object::hash("sector_identifier_uri"sv)};
constexpr auto HASH_ID_TOKEN_SIGNED_ALG{
    JSON::Object::hash("id_token_signed_response_alg"sv)};
constexpr auto HASH_ID_TOKEN_ENCRYPTED_ALG{
    JSON::Object::hash("id_token_encrypted_response_alg"sv)};
constexpr auto HASH_USERINFO_SIGNED_ALG{
    JSON::Object::hash("userinfo_signed_response_alg"sv)};
constexpr auto HASH_DEFAULT_MAX_AGE{JSON::Object::hash("default_max_age"sv)};
constexpr auto HASH_REQUIRE_AUTH_TIME{
    JSON::Object::hash("require_auth_time"sv)};
constexpr auto HASH_INITIATE_LOGIN_URI{
    JSON::Object::hash("initiate_login_uri"sv)};
constexpr auto HASH_POST_LOGOUT_REDIRECT_URIS{
    JSON::Object::hash("post_logout_redirect_uris"sv)};

auto string_member(const JSON &data, const JSON::StringView name,
                   const JSON::Object::hash_type hash)
    -> std::optional<std::string_view> {
  const auto *member{data.try_at(name, hash)};
  if (member == nullptr || !member->is_string()) {
    return std::nullopt;
  }

  return std::string_view{member->to_string()};
}

auto validate_client_metadata(const OAuthClientMetadata &oauth) -> void {
  // OpenID Connect Dynamic Client Registration 1.0 Section 2: redirect_uris is
  // REQUIRED, which OpenID Connect tightens from the OAuth OPTIONAL
  const auto &data{oauth.data()};
  const auto *redirect_uris{data.try_at("redirect_uris"sv, HASH_REDIRECT_URIS)};
  if (redirect_uris == nullptr || !redirect_uris->is_array() ||
      redirect_uris->empty()) {
    throw OIDCRegistrationParseError{};
  }

  for (const auto &element : redirect_uris->as_array()) {
    if (!element.is_string()) {
      throw OIDCRegistrationParseError{};
    }
  }
}

} // namespace

OIDCClientMetadata::OIDCClientMetadata(JSON &&data) : oauth_{std::move(data)} {
  validate_client_metadata(this->oauth_);
}

auto OIDCClientMetadata::from(JSON &&data)
    -> std::optional<OIDCClientMetadata> {
  try {
    return OIDCClientMetadata{std::move(data)};
  } catch (const OAuthRegistrationParseError &) {
    return std::nullopt;
  } catch (const OIDCRegistrationParseError &) {
    return std::nullopt;
  }
}

auto OIDCClientMetadata::has_redirect_uri(const std::string_view value) const
    -> bool {
  return this->oauth_.has_redirect_uri(value);
}

auto OIDCClientMetadata::application_type() const -> std::string_view {
  // OpenID Connect Dynamic Client Registration 1.0 Section 2: the default is
  // web
  const auto value{string_member(this->oauth_.data(), "application_type"sv,
                                 HASH_APPLICATION_TYPE)};
  return value.value_or("web");
}

auto OIDCClientMetadata::subject_type() const
    -> std::optional<std::string_view> {
  return string_member(this->oauth_.data(), "subject_type"sv,
                       HASH_SUBJECT_TYPE);
}

auto OIDCClientMetadata::sector_identifier_uri() const
    -> std::optional<std::string_view> {
  return string_member(this->oauth_.data(), "sector_identifier_uri"sv,
                       HASH_SECTOR_IDENTIFIER_URI);
}

auto OIDCClientMetadata::id_token_signed_response_alg() const
    -> std::string_view {
  // OpenID Connect Dynamic Client Registration 1.0 Section 2: the default is
  // RS256
  const auto value{string_member(this->oauth_.data(),
                                 "id_token_signed_response_alg"sv,
                                 HASH_ID_TOKEN_SIGNED_ALG)};
  return value.value_or("RS256");
}

auto OIDCClientMetadata::id_token_encrypted_response_alg() const
    -> std::optional<std::string_view> {
  return string_member(this->oauth_.data(), "id_token_encrypted_response_alg"sv,
                       HASH_ID_TOKEN_ENCRYPTED_ALG);
}

auto OIDCClientMetadata::userinfo_signed_response_alg() const
    -> std::optional<std::string_view> {
  return string_member(this->oauth_.data(), "userinfo_signed_response_alg"sv,
                       HASH_USERINFO_SIGNED_ALG);
}

auto OIDCClientMetadata::default_max_age() const
    -> std::optional<std::chrono::seconds> {
  const auto *member{
      this->oauth_.data().try_at("default_max_age"sv, HASH_DEFAULT_MAX_AGE)};
  if (member == nullptr || !member->is_integer() || member->to_integer() < 0 ||
      member->to_integer() >
          std::numeric_limits<std::chrono::seconds::rep>::max()) {
    return std::nullopt;
  }

  return std::chrono::seconds{member->to_integer()};
}

auto OIDCClientMetadata::require_auth_time() const -> bool {
  const auto *member{this->oauth_.data().try_at("require_auth_time"sv,
                                                HASH_REQUIRE_AUTH_TIME)};
  return member != nullptr && member->is_boolean() && member->to_boolean();
}

auto OIDCClientMetadata::initiate_login_uri() const
    -> std::optional<std::string_view> {
  return string_member(this->oauth_.data(), "initiate_login_uri"sv,
                       HASH_INITIATE_LOGIN_URI);
}

auto OIDCClientMetadata::has_post_logout_redirect_uri(
    const std::string_view value) const -> bool {
  return this->oauth_.data().array_member_contains(
      "post_logout_redirect_uris"sv, HASH_POST_LOGOUT_REDIRECT_URIS, value);
}

auto OIDCClientMetadata::oauth() const -> const OAuthClientMetadata & {
  return this->oauth_;
}

auto OIDCClientMetadata::data() const -> const JSON & {
  return this->oauth_.data();
}

auto oidc_sector_identifier_contains(
    const JSON &sector_document,
    const std::span<const std::string_view> redirect_uris) -> bool {
  // OpenID Connect Dynamic Client Registration 1.0 Section 5: the document is a
  // JSON array of redirection URIs, and every registered URI must appear in it
  if (!sector_document.is_array()) {
    return false;
  }

  for (const auto redirect_uri : redirect_uris) {
    bool found{false};
    for (const auto &element : sector_document.as_array()) {
      if (element.is_string() && element.to_string() == redirect_uri) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}

} // namespace sourcemeta::core
