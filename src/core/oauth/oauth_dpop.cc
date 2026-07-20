#include <sourcemeta/core/oauth_dpop.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose_jwk.h>
#include <sourcemeta/core/jose_jwt.h>
#include <sourcemeta/core/jose_sign.h>
#include <sourcemeta/core/jose_verify.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth_random.h>

#include "oauth_json.h"
#include "oauth_syntax.h"

#include <algorithm> // std::ranges::find, std::ranges::all_of, std::ranges::count_if
#include <array>       // std::array
#include <chrono>      // std::chrono::seconds, std::chrono::duration_cast
#include <cstdint>     // std::int64_t, std::uint8_t
#include <mutex>       // std::scoped_lock
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable
#include <vector>      // std::erase_if

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

const auto HASH_TYP{JSON::Object::hash("typ"sv)};
const auto HASH_ALG{JSON::Object::hash("alg"sv)};
const auto HASH_JWK{JSON::Object::hash("jwk"sv)};
const auto HASH_JTI{JSON::Object::hash("jti"sv)};
const auto HASH_HTM{JSON::Object::hash("htm"sv)};
const auto HASH_HTU{JSON::Object::hash("htu"sv)};
const auto HASH_IAT{JSON::Object::hash("iat"sv)};
const auto HASH_ATH{JSON::Object::hash("ath"sv)};
const auto HASH_NONCE{JSON::Object::hash("nonce"sv)};
const auto HASH_JKT{JSON::Object::hash("jkt"sv)};

// RFC 7515 Section 4.1.1: the "alg" header value each algorithm serializes to
auto dpop_algorithm_name(const JWSAlgorithm algorithm) -> std::string_view {
  switch (algorithm) {
    case JWSAlgorithm::RS256:
      return "RS256";
    case JWSAlgorithm::RS384:
      return "RS384";
    case JWSAlgorithm::RS512:
      return "RS512";
    case JWSAlgorithm::PS256:
      return "PS256";
    case JWSAlgorithm::PS384:
      return "PS384";
    case JWSAlgorithm::PS512:
      return "PS512";
    case JWSAlgorithm::ES256:
      return "ES256";
    case JWSAlgorithm::ES384:
      return "ES384";
    case JWSAlgorithm::ES512:
      return "ES512";
    case JWSAlgorithm::EdDSA:
      return "EdDSA";
    case JWSAlgorithm::HS256:
      return "HS256";
    case JWSAlgorithm::HS384:
      return "HS384";
    case JWSAlgorithm::HS512:
      return "HS512";
  }

  std::unreachable();
}

// RFC 9449 Section 4.2: the DPoP algorithm MUST NOT be a symmetric one, so the
// message authentication code algorithms are the ones to exclude
auto dpop_is_asymmetric(const JWSAlgorithm algorithm) noexcept -> bool {
  switch (algorithm) {
    case JWSAlgorithm::RS256:
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::RS512:
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::PS512:
    case JWSAlgorithm::ES256:
    case JWSAlgorithm::ES384:
    case JWSAlgorithm::ES512:
    case JWSAlgorithm::EdDSA:
      return true;
    case JWSAlgorithm::HS256:
    case JWSAlgorithm::HS384:
    case JWSAlgorithm::HS512:
      return false;
  }

  std::unreachable();
}

// RFC 9449 Section 4.2: the HTTP target URI without query and fragment, with
// the syntax and scheme normalization Section 4.3 recommends applied so the
// same request compares equal on both sides
auto dpop_normalize_target(const std::string_view url)
    -> std::optional<std::string> {
  auto uri{oauth_try_parse_uri(url)};
  if (!uri.has_value()) {
    return std::nullopt;
  }

  // RFC 9449 Section 4.3 recommends the syntax and scheme normalization of
  // RFC 3986 Sections 6.2.2 and 6.2.3, and Section 4.2 excludes the query and
  // fragment. RFC 9110 Section 4.2.1 gives an "http" or "https" target URI no
  // userinfo, so it is dropped rather than signed into the proof or compared
  uri->canonicalize();
  uri->query("");
  uri->userinfo("");
  if (!uri->scheme().has_value() || !uri->host().has_value()) {
    return std::nullopt;
  }

  // RFC 3986 Section 6.2.3: an authority with an empty path is equivalent to
  // one with a path of "/", a scheme-based normalization the general
  // canonicalizer leaves to the caller
  if (!uri->path().has_value() || uri->path().value().empty()) {
    uri->path(std::string{"/"});
  }

  return uri->recompose_without_fragment();
}

// RFC 9449 Section 4.2 and Section 7: the base64url-encoded SHA-256 hash of the
// access token, the value bound into a proof through the hash claim
auto dpop_access_token_hash(const std::string_view access_token)
    -> std::string {
  const auto digest{sha256_digest(access_token)};
  return base64url_encode(std::string_view{
      reinterpret_cast<const char *>(digest.data()), digest.size()});
}

// RFC 9449 Section 4.3 check 7: the public key MUST NOT contain a private key,
// whose presence is marked by any private JSON Web Key member (RFC 7518
// Sections 6.2.2 and 6.3.2, RFC 8037 Section 2, RFC 7518 Section 6.4)
auto dpop_has_private_material(const JSON &key) -> bool {
  return key.defines("d") || key.defines("p") || key.defines("q") ||
         key.defines("dp") || key.defines("dq") || key.defines("qi") ||
         key.defines("oth") || key.defines("k");
}

} // namespace

OAuthDPoPProofer::OAuthDPoPProofer(JWKPrivate key, const JWSAlgorithm algorithm)
    : key_{std::move(key)}, algorithm_{algorithm} {}

auto OAuthDPoPProofer::proof(const std::string_view server,
                             const std::string_view method,
                             const std::string_view url,
                             const std::string_view access_token,
                             const std::chrono::system_clock::time_point now,
                             std::string &sink) -> bool {
  auto target{dpop_normalize_target(url)};
  if (!target.has_value()) {
    return false;
  }

  auto public_key{this->key_.public_jwk()};
  if (!public_key.has_value()) {
    return false;
  }

  auto header{JSON::make_object()};
  header.assign_assume_new("typ", JSON{"dpop+jwt"}, HASH_TYP);
  header.assign_assume_new("alg", JSON{dpop_algorithm_name(this->algorithm_)},
                           HASH_ALG);
  header.assign_assume_new("jwk", std::move(public_key.value()), HASH_JWK);

  auto payload{JSON::make_object()};
  const auto identifier{oauth_random_token()};
  payload.assign_assume_new(
      "jti", JSON{std::string_view{identifier.data(), identifier.size()}},
      HASH_JTI);
  payload.assign_assume_new("htm", JSON{method}, HASH_HTM);
  payload.assign_assume_new("htu", JSON{std::move(target.value())}, HASH_HTU);
  payload.assign_assume_new(
      "iat",
      JSON{static_cast<std::int64_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              now.time_since_epoch())
              .count())},
      HASH_IAT);
  if (!access_token.empty()) {
    payload.assign_assume_new("ath", JSON{dpop_access_token_hash(access_token)},
                              HASH_ATH);
  }

  {
    const std::scoped_lock lock{this->mutex_};
    const auto iterator{this->nonces_.find(server)};
    if (iterator != this->nonces_.end()) {
      payload.assign_assume_new("nonce", JSON{iterator->second}, HASH_NONCE);
    }
  }

  const auto token{jwt_sign(header, payload, this->key_)};
  if (!token.has_value()) {
    return false;
  }

  sink.append(token.value());
  return true;
}

auto OAuthDPoPProofer::observe(const std::string_view server,
                               const std::string_view nonce) -> void {
  const std::scoped_lock lock{this->mutex_};
  this->nonces_.insert_or_assign(std::string{server}, std::string{nonce});
}

auto OAuthDPoPProofer::thumbprint() const -> std::optional<std::string> {
  return this->key_.thumbprint();
}

auto oauth_dpop_confirmation(const std::string_view thumbprint) -> JSON {
  auto confirmation{JSON::make_object()};
  confirmation.assign_assume_new("jkt", JSON{thumbprint}, HASH_JKT);
  return confirmation;
}

auto oauth_dpop_proof_thumbprint(const std::string_view proof)
    -> std::optional<std::string> {
  const auto token{JWT::from(proof)};
  if (!token.has_value()) {
    return std::nullopt;
  }

  const auto &header{token.value().header()};
  if (!header.is_object()) {
    return std::nullopt;
  }

  const auto *key{header.try_at("jwk"sv, HASH_JWK)};
  if (key == nullptr || !key->is_object()) {
    return std::nullopt;
  }

  const auto parsed{JWK::from(*key)};
  if (!parsed.has_value()) {
    return std::nullopt;
  }

  return parsed.value().thumbprint();
}

auto oauth_dpop_verify(const std::string_view proof,
                       const std::string_view method,
                       const std::string_view url,
                       const std::chrono::system_clock::time_point now,
                       const OAuthDPoPVerifyOptions &options)
    -> std::optional<OAuthDPoPError> {
  // RFC 9449 Section 4.3 check 1: exactly one DPoP header field
  if (options.proof_count != 1) {
    return OAuthDPoPError::ProofCount;
  }

  // Check 2: a single well-formed JSON Web Token
  const auto token{JWT::from(proof)};
  if (!token.has_value()) {
    return OAuthDPoPError::Malformed;
  }

  const auto &parsed{token.value()};

  // Check 4: the token type is dpop+jwt
  const auto type{parsed.type()};
  if (!type.has_value() || type.value() != "dpop+jwt") {
    return OAuthDPoPError::UnexpectedType;
  }

  // Check 5: an asymmetric algorithm, not none, within local policy
  const auto algorithm{parsed.algorithm()};
  if (!algorithm.has_value() || !dpop_is_asymmetric(algorithm.value())) {
    return OAuthDPoPError::UnsupportedAlgorithm;
  }

  if (!options.allowed_algorithms.empty() &&
      std::ranges::find(options.allowed_algorithms, algorithm.value()) ==
          options.allowed_algorithms.end()) {
    return OAuthDPoPError::UnsupportedAlgorithm;
  }

  // Check 3: all required header parameters and claims are present
  const auto &header{parsed.header()};
  const auto *key{header.is_object() ? header.try_at("jwk"sv, HASH_JWK)
                                     : nullptr};
  if (key == nullptr || !key->is_object()) {
    return OAuthDPoPError::MissingClaim;
  }

  const auto identifier{parsed.token_id()};
  const auto method_claim{
      oauth_json_string_member(parsed.payload(), "htm"sv, HASH_HTM)};
  const auto target_claim{
      oauth_json_string_member(parsed.payload(), "htu"sv, HASH_HTU)};
  const auto issued{parsed.issued_at()};
  if (!identifier.has_value() || !method_claim.has_value() ||
      !target_claim.has_value() || !issued.has_value()) {
    return OAuthDPoPError::MissingClaim;
  }

  // Check 7: the embedded key carries no private material
  if (dpop_has_private_material(*key)) {
    return OAuthDPoPError::PrivateKey;
  }

  // Check 6: the signature verifies with the embedded key
  const auto public_key{JWK::from(*key)};
  if (!public_key.has_value() ||
      !jwt_verify_signature(parsed, public_key.value())) {
    return OAuthDPoPError::Signature;
  }

  // Check 8: the method claim matches the request method
  if (method_claim.value() != method) {
    return OAuthDPoPError::MethodMismatch;
  }

  // Check 9: the target claim matches the request target
  const auto normalized_claim{dpop_normalize_target(target_claim.value())};
  const auto normalized_request{dpop_normalize_target(url)};
  if (!normalized_claim.has_value() || !normalized_request.has_value() ||
      normalized_claim.value() != normalized_request.value()) {
    return OAuthDPoPError::TargetMismatch;
  }

  // Check 10: the nonce claim matches the nonce the server issued
  const auto nonce_claim{
      oauth_json_string_member(parsed.payload(), "nonce"sv, HASH_NONCE)};
  if (options.expected_nonce.has_value()) {
    if (!nonce_claim.has_value()) {
      return OAuthDPoPError::MissingNonce;
    }

    if (!secure_equals(nonce_claim.value(), options.expected_nonce.value())) {
      return OAuthDPoPError::NonceMismatch;
    }
  }

  // Check 11: the creation time is within the acceptable window
  if (issued.value() < now - options.past_window ||
      issued.value() > now + options.future_window) {
    return OAuthDPoPError::Expired;
  }

  // Check 12: the access token hash and the key binding, when a token is
  // presented
  if (options.access_token.has_value()) {
    const auto hash_claim{
        oauth_json_string_member(parsed.payload(), "ath"sv, HASH_ATH)};
    if (!hash_claim.has_value()) {
      return OAuthDPoPError::AccessTokenMismatch;
    }

    const auto expected{dpop_access_token_hash(options.access_token.value())};
    if (!secure_equals(hash_claim.value(), expected)) {
      return OAuthDPoPError::AccessTokenMismatch;
    }

    // RFC 9449 Section 7.1: a protected resource MUST confirm the proof key
    // matches the key the token is bound to, so presenting a token without a
    // binding to check against fails closed rather than skipping that
    // confirmation
    if (!options.bound_thumbprint.has_value()) {
      return OAuthDPoPError::KeyMismatch;
    }
  }

  if (options.bound_thumbprint.has_value()) {
    const auto proof_thumbprint{public_key.value().thumbprint()};
    if (!proof_thumbprint.has_value() ||
        !secure_equals(proof_thumbprint.value(),
                       options.bound_thumbprint.value())) {
      return OAuthDPoPError::KeyMismatch;
    }
  }

  return std::nullopt;
}

auto OAuthDPoPReplayStore::check_and_insert(
    const std::string_view identifier, const std::string_view target,
    const std::chrono::system_clock::time_point now,
    const std::chrono::seconds window) -> bool {
  // RFC 9449 Section 11.1: the identifier is tracked in the context of the
  // target URI, so the digest covers both and one store safely spans targets.
  // The target is normalized the same way the verifier compares the htu claim
  // (Section 4.3 check 9), so a replay to an equivalent but differently spelled
  // target is still detected rather than admitted as a fresh identifier
  const auto normalized{dpop_normalize_target(target)};
  const std::string_view effective{
      normalized.has_value() ? std::string_view{normalized.value()} : target};
  std::string material;
  material.reserve(identifier.size() + 1 + effective.size());
  material.append(identifier);
  material.push_back('\0');
  material.append(effective);
  const auto digest{sha256_digest(material)};

  const std::scoped_lock lock{this->mutex_};
  std::erase_if(this->entries_, [now](const Entry &entry) -> bool {
    return entry.expiry <= now;
  });
  for (const auto &entry : this->entries_) {
    if (entry.digest == digest) {
      return false;
    }
  }

  this->entries_.push_back(Entry{.digest = digest, .expiry = now + window});
  return true;
}

auto OAuthDPoPReplayStore::size(
    const std::chrono::system_clock::time_point now) const -> std::size_t {
  const std::scoped_lock lock{this->mutex_};
  return static_cast<std::size_t>(
      std::ranges::count_if(this->entries_, [now](const Entry &entry) -> bool {
        return entry.expiry > now;
      }));
}

auto oauth_is_valid_dpop_nonce(const std::string_view value) noexcept -> bool {
  if (value.empty()) {
    return false;
  }

  // RFC 6749 Appendix A: NQCHAR = %x21 / %x23-5B / %x5D-7E, the printable ASCII
  // set excluding the space, the double quote, and the backslash
  return std::ranges::all_of(value, [](const char character) -> bool {
    const auto byte{static_cast<unsigned char>(character)};
    return byte == 0x21 || (byte >= 0x23 && byte <= 0x5B) ||
           (byte >= 0x5D && byte <= 0x7E);
  });
}

} // namespace sourcemeta::core
