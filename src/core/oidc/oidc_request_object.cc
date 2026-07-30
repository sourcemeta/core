#include <sourcemeta/core/oidc_request_object.h>

#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include "oidc_verify.h"

#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

constexpr auto HASH_ALG{JSON::Object::hash("alg"sv)};
constexpr auto HASH_ISS{JSON::Object::hash("iss"sv)};
constexpr auto HASH_AUD{JSON::Object::hash("aud"sv)};

} // namespace

auto oidc_request_object_pairing_is_valid(const std::string_view request,
                                          const std::string_view request_uri)
    -> bool {
  // OpenID Connect Core 1.0 Section 6: the request and request_uri parameters
  // MUST NOT both be used in the same request
  return request.empty() || request_uri.empty();
}

auto oidc_build_request_object(const JSON &parameters, const JWKPrivate &key,
                               const JWSAlgorithm algorithm)
    -> std::optional<std::string> {
  if (!parameters.is_object()) {
    return std::nullopt;
  }

  auto header{JSON::make_object()};
  header.assign_assume_new("alg", JSON{jws_algorithm_name(algorithm)},
                           HASH_ALG);
  return jwt_sign(header, parameters, key);
}

auto oidc_verify_request_object(
    const JWT &token, const JWKS &keys,
    const std::span<const JWSAlgorithm> allowed_algorithms,
    const std::string_view client_id, const std::string_view provider_issuer)
    -> std::optional<JSON> {
  // OpenID Connect Core 1.0 Section 6.3: the algorithm is pinned and the key is
  // selected by identifier, never taken from the header alone
  if (!oidc_verify_selected_signature(token, keys, allowed_algorithms)) {
    return std::nullopt;
  }

  // OpenID Connect Core 1.0 Section 6.1: "If signed, the Request Object SHOULD
  // contain the Claims iss (issuer) and aud (audience) as members". Being only
  // a SHOULD, an absent iss or aud is accepted, but when either is present it
  // is validated: the iss must be the client and the aud must include the
  // provider, which rejects an object minted by or addressed to a different
  // party
  const auto &payload{token.payload()};
  const auto *issuer{payload.try_at("iss"sv, HASH_ISS)};
  if (issuer != nullptr &&
      (!issuer->is_string() || issuer->to_string() != client_id)) {
    return std::nullopt;
  }

  const auto *audience{payload.try_at("aud"sv, HASH_AUD)};
  if (audience != nullptr && !token.has_audience(provider_issuer)) {
    return std::nullopt;
  }

  return payload;
}

} // namespace sourcemeta::core
