#include <sourcemeta/core/oidc_userinfo.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include "oidc_verify.h"

#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

constexpr auto HASH_SUB{JSON::Object::hash("sub"sv)};

} // namespace

auto oidc_build_userinfo(const std::string_view subject,
                         const JSON &additional_claims) -> JSON {
  auto document{additional_claims.is_object() ? additional_claims
                                              : JSON::make_object()};
  // OpenID Connect Core 1.0 Section 5.3.2: the sub claim is REQUIRED in a
  // UserInfo response
  document.assign("sub", JSON{subject});
  return document;
}

auto oidc_userinfo_matches_subject(const JSON &userinfo,
                                   const std::string_view expected_subject)
    -> bool {
  if (!userinfo.is_object()) {
    return false;
  }

  const auto *subject{userinfo.try_at("sub"sv, HASH_SUB)};
  return subject != nullptr && subject->is_string() &&
         secure_equals(subject->to_string(), expected_subject);
}

auto oidc_verify_userinfo(
    const JWT &token, const JWKS &keys,
    const std::span<const JWSAlgorithm> allowed_algorithms,
    const std::string_view expected_subject) -> std::optional<JSON> {
  // OpenID Connect Core 1.0 Section 5.3.2: the algorithm is pinned and the key
  // selected by identifier, never taken from the token header alone
  if (!oidc_verify_selected_signature(token, keys, allowed_algorithms)) {
    return std::nullopt;
  }

  // OpenID Connect Core 1.0 Section 5.3.2: the sub must match the ID Token sub
  if (!oidc_userinfo_matches_subject(token.payload(), expected_subject)) {
    return std::nullopt;
  }

  return token.payload();
}

} // namespace sourcemeta::core
