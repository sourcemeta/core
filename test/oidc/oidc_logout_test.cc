#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstdint>     // std::int64_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

static constexpr std::string_view OCT_JWK{R"JSON({
  "kty": "oct",
  "k": "AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"
})JSON"};

static auto oct_key_set() -> sourcemeta::core::JWKS {
  auto keys{sourcemeta::core::JSON::make_array()};
  keys.push_back(sourcemeta::core::parse_json(OCT_JWK));
  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("keys", std::move(keys));
  return sourcemeta::core::JWKS::from(std::move(document)).value();
}

static auto sign_logout_token(const std::string_view header,
                              const sourcemeta::core::JSON &payload)
    -> std::string {
  return sourcemeta::core::jwt_sign(sourcemeta::core::parse_json(header),
                                    payload,
                                    sourcemeta::core::JWKPrivate::from(
                                        sourcemeta::core::parse_json(OCT_JWK))
                                        .value())
      .value();
}

static auto sign_logout_token(const std::string_view header,
                              const std::string_view payload) -> std::string {
  return sign_logout_token(header, sourcemeta::core::parse_json(payload));
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_HS256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

static const auto REFERENCE_NOW{
    std::chrono::system_clock::from_time_t(1700000000)};

static constexpr std::string_view VALID_HEADER{R"JSON({
  "alg": "HS256",
  "typ": "logout+jwt"
})JSON"};
static constexpr std::string_view VALID_PAYLOAD{R"JSON({
  "iss": "https://issuer.example",
  "aud": "client-id",
  "iat": 1700000000,
  "exp": 2000000000,
  "jti": "logout-1",
  "sub": "user-1",
  "events": {
    "http://schemas.openid.net/event/backchannel-logout": {}
  }
})JSON"};

// The newest whole second the clock can represent and a conversion still
// admits. The tick period differs across standard libraries, so the bound is
// derived rather than written out
static auto highest_representable_second() -> std::int64_t {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::duration::max())
             .count() -
         2;
}

static auto logout_token_expiring_at(const std::int64_t expiration)
    -> sourcemeta::core::JSON {
  auto events{sourcemeta::core::JSON::make_object()};
  events.assign("http://schemas.openid.net/event/backchannel-logout",
                sourcemeta::core::JSON::make_object());
  auto payload{sourcemeta::core::JSON::make_object()};
  payload.assign("iss", sourcemeta::core::JSON{"https://issuer.example"});
  payload.assign("aud", sourcemeta::core::JSON{"client-id"});
  payload.assign("iat", sourcemeta::core::JSON{std::int64_t{1700000000}});
  payload.assign("jti", sourcemeta::core::JSON{"logout-1"});
  payload.assign("sub", sourcemeta::core::JSON{"user-1"});
  payload.assign("events", std::move(events));
  payload.assign("exp", sourcemeta::core::JSON{expiration});
  return payload;
}

TEST(build_logout_url_includes_the_parameters) {
  sourcemeta::core::OIDCLogoutRequest request;
  request.id_token_hint = "eyJ.aaa.bbb";
  request.client_id = "client-id";
  request.post_logout_redirect_uri = "https://client.example/after";
  request.state = "xyz";
  std::string url;
  sourcemeta::core::oidc_build_logout_url("https://server.example/logout",
                                          request, url);
  EXPECT_TRUE(url.find("id_token_hint=eyJ.aaa.bbb") != std::string::npos);
  EXPECT_TRUE(url.find("client_id=client-id") != std::string::npos);
  EXPECT_TRUE(url.find("post_logout_redirect_uri=https%3A%2F%2Fclient.example%"
                       "2Fafter") != std::string::npos);
  EXPECT_TRUE(url.find("state=xyz") != std::string::npos);
}

TEST(validate_logout_token_accepts_a_valid_token) {
  const auto compact{sign_logout_token(VALID_HEADER, VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_wrong_typ) {
  const auto compact{sign_logout_token(R"JSON({
    "alg": "HS256",
    "typ": "JWT"
  })JSON",
                                       VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_accepts_a_missing_typ) {
  const auto compact{sign_logout_token(R"JSON({
    "alg": "HS256"
  })JSON",
                                       VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_accepts_a_mixed_case_typ) {
  const auto compact{sign_logout_token(R"JSON({
    "alg": "HS256",
    "typ": "application/Logout+JWT"
  })JSON",
                                       VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_an_unknown_kid) {
  const auto compact{sign_logout_token(R"JSON({
    "alg": "HS256",
    "typ": "logout+jwt",
    "kid": "unknown"
  })JSON",
                                       VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_nonce) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": "user-1",
    "nonce": "n",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_missing_events) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": "user-1"
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_missing_subject_and_session) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_missing_jti) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_wrong_issuer) {
  const auto compact{sign_logout_token(VALID_HEADER, VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://attacker.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_missing_exp) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_an_expired_token) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1600000000,
    "exp": 1650000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_an_exp_at_the_boundary) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1600000000,
    "exp": 1700000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_accepts_an_exp_near_the_representable_bound) {
  // Adding the skew to a claim this far in the future overflows the tick type,
  // so the skew shifts the server clock instead and the token still validates
  const auto compact{sign_logout_token(
      VALID_HEADER, logout_token_expiring_at(highest_representable_second()))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds{60}));
}

TEST(validate_logout_token_accepts_an_exp_near_the_bound_without_skew) {
  const auto compact{sign_logout_token(
      VALID_HEADER, logout_token_expiring_at(highest_representable_second()))};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_an_expired_token_under_a_skew) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1600000000,
    "exp": 1699999000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds{60}));
  // The same token is inside the window once the tolerance covers the gap
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds{2000}));
}

TEST(validate_logout_token_rejects_a_future_iat_beyond_the_skew) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700001000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds{60}));
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds{2000}));
}

TEST(validate_logout_token_tolerates_a_saturating_skew) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, std::chrono::seconds::max()));
}

TEST(validate_logout_token_rejects_a_non_string_subject) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": 42,
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_non_string_session) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sid": {},
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(validate_logout_token_rejects_a_non_string_jti) {
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "exp": 2000000000,
    "jti": 7,
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}

TEST(build_logout_url_without_parameters_leaves_the_endpoint_untouched) {
  const sourcemeta::core::OIDCLogoutRequest request;
  std::string url;
  sourcemeta::core::oidc_build_logout_url("https://server.example/logout",
                                          request, url);
  EXPECT_EQ(url, "https://server.example/logout");
}

TEST(build_logout_url_continues_an_existing_query) {
  sourcemeta::core::OIDCLogoutRequest request;
  request.state = "xyz";
  std::string url;
  sourcemeta::core::oidc_build_logout_url(
      "https://server.example/logout?foo=bar", request, url);
  EXPECT_EQ(url, "https://server.example/logout?foo=bar&state=xyz");
}

TEST(build_logout_url_opens_a_fresh_query) {
  sourcemeta::core::OIDCLogoutRequest request;
  request.state = "xyz";
  std::string url;
  sourcemeta::core::oidc_build_logout_url("https://server.example/logout",
                                          request, url);
  EXPECT_EQ(url, "https://server.example/logout?state=xyz");
}

TEST(front_channel_pairing_requires_both_or_neither) {
  EXPECT_TRUE(sourcemeta::core::oidc_front_channel_pairing_is_valid(
      "https://issuer.example", "session-1"));
  EXPECT_TRUE(sourcemeta::core::oidc_front_channel_pairing_is_valid("", ""));
  EXPECT_FALSE(sourcemeta::core::oidc_front_channel_pairing_is_valid(
      "https://issuer.example", ""));
  EXPECT_FALSE(
      sourcemeta::core::oidc_front_channel_pairing_is_valid("", "session-1"));
}

TEST(session_state_is_stable_and_salted) {
  const auto first{sourcemeta::core::oidc_session_state(
      "client-id", "https://client.example", "browser-state", "salt")};
  const auto second{sourcemeta::core::oidc_session_state(
      "client-id", "https://client.example", "browser-state", "salt")};
  EXPECT_EQ(first, second);
  EXPECT_TRUE(first.find(".salt") != std::string::npos);
  const auto different{sourcemeta::core::oidc_session_state(
      "client-id", "https://client.example", "browser-state", "other")};
  EXPECT_FALSE(first == different);
}

TEST(validate_logout_token_bounds_the_clock_skew_like_the_base_check) {
  // The same tolerance must mean the same thing whichever path validates a
  // token, so a skew beyond the grace period cannot revive a token here that
  // the base JSON Web Token check would refuse
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1600000000,
    "exp": 1650000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  const std::chrono::seconds two_years{63113904};
  EXPECT_TRUE(sourcemeta::core::jwt_check_claims(
                  token.value(), "https://issuer.example", "client-id",
                  REFERENCE_NOW, two_years)
                  .has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW, two_years));
}

TEST(validate_logout_token_rejects_a_future_nbf) {
  // Delegating to the base check also picks up the not-before rule, which the
  // previous reimplementation of these claims never applied
  const auto compact{sign_logout_token(VALID_HEADER, R"JSON({
    "iss": "https://issuer.example",
    "aud": "client-id",
    "iat": 1700000000,
    "nbf": 1900000000,
    "exp": 2000000000,
    "jti": "logout-1",
    "sub": "user-1",
    "events": {
      "http://schemas.openid.net/event/backchannel-logout": {}
    }
  })JSON")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), ALLOWED_HS256, "https://issuer.example",
      "client-id", REFERENCE_NOW));
}
