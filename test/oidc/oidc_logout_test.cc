#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view OCT_JWK{
    R"JSON({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})JSON"};

static auto oct_key_set() -> sourcemeta::core::JWKS {
  return sourcemeta::core::JWKS::from(
             sourcemeta::core::parse_json(std::string{R"({"keys":[)"} +
                                          std::string{OCT_JWK} + R"(]})"))
      .value();
}

static auto sign_logout_token(const std::string_view header,
                              const std::string_view payload) -> std::string {
  return sourcemeta::core::jwt_sign(sourcemeta::core::parse_json(header),
                                    sourcemeta::core::parse_json(payload),
                                    sourcemeta::core::JWKPrivate::from(
                                        sourcemeta::core::parse_json(OCT_JWK))
                                        .value())
      .value();
}

static constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> allowed_hs256{
    {sourcemeta::core::JWSAlgorithm::HS256}};

static const auto reference_now{
    std::chrono::system_clock::from_time_t(1700000000)};

static constexpr std::string_view VALID_HEADER{
    R"({"alg":"HS256","typ":"logout+jwt"})"};
static constexpr std::string_view VALID_PAYLOAD{
    R"({"iss":"https://issuer.example","aud":"client-id","iat":1700000000,"jti":"logout-1","sub":"user-1","events":{"http://schemas.openid.net/event/backchannel-logout":{}}})"};

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
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_wrong_typ) {
  const auto compact{
      sign_logout_token(R"({"alg":"HS256","typ":"JWT"})", VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_nonce) {
  const auto compact{sign_logout_token(
      VALID_HEADER,
      R"({"iss":"https://issuer.example","aud":"client-id","iat":1700000000,"jti":"logout-1","sub":"user-1","nonce":"n","events":{"http://schemas.openid.net/event/backchannel-logout":{}}})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_missing_events) {
  const auto compact{sign_logout_token(
      VALID_HEADER,
      R"({"iss":"https://issuer.example","aud":"client-id","iat":1700000000,"jti":"logout-1","sub":"user-1"})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_missing_subject_and_session) {
  const auto compact{sign_logout_token(
      VALID_HEADER,
      R"({"iss":"https://issuer.example","aud":"client-id","iat":1700000000,"jti":"logout-1","events":{"http://schemas.openid.net/event/backchannel-logout":{}}})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_missing_jti) {
  const auto compact{sign_logout_token(
      VALID_HEADER,
      R"({"iss":"https://issuer.example","aud":"client-id","iat":1700000000,"sub":"user-1","events":{"http://schemas.openid.net/event/backchannel-logout":{}}})")};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://issuer.example",
      "client-id", reference_now));
}

TEST(validate_logout_token_rejects_a_wrong_issuer) {
  const auto compact{sign_logout_token(VALID_HEADER, VALID_PAYLOAD)};
  const auto token{sourcemeta::core::JWT::from(compact)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(sourcemeta::core::oidc_validate_logout_token(
      token.value(), oct_key_set(), allowed_hs256, "https://attacker.example",
      "client-id", reference_now));
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
