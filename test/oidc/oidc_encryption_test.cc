#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view OCT_JWK{
    R"JSON({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})JSON"};

static auto inner_signed_token() -> std::string {
  return sourcemeta::core::jwt_sign(
             sourcemeta::core::parse_json(R"({"alg":"HS256"})"),
             sourcemeta::core::parse_json(
                 R"({"iss":"https://issuer.example","sub":"user-1","aud":"client-id","exp":2000000000})"),
             sourcemeta::core::JWKPrivate::from(
                 sourcemeta::core::parse_json(OCT_JWK))
                 .value())
      .value();
}

static auto encrypt(const std::string_view plaintext) -> std::string {
  const auto key{sourcemeta::core::JWK::from_octets(std::string(32, 'k'))};
  return sourcemeta::core::jwe_encrypt(
             sourcemeta::core::parse_json(R"({"alg":"dir","enc":"A256GCM"})"),
             plaintext, key)
      .value();
}

TEST(decrypt_nested_jwt_recovers_the_inner_token) {
  const auto inner{inner_signed_token()};
  const auto jwe{encrypt(inner)};
  const auto recovered{sourcemeta::core::oidc_decrypt_nested_jwt(
      jwe, sourcemeta::core::JWKPrivate::from_octets(std::string(32, 'k')))};
  EXPECT_TRUE(recovered.has_value());
  EXPECT_EQ(recovered.value(), inner);

  const auto token{sourcemeta::core::JWT::from(recovered.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().subject().has_value());
  EXPECT_EQ(token.value().subject().value(), "user-1");
}

TEST(decrypt_nested_jwt_rejects_a_wrong_key) {
  const auto jwe{encrypt(inner_signed_token())};
  const auto recovered{sourcemeta::core::oidc_decrypt_nested_jwt(
      jwe, sourcemeta::core::JWKPrivate::from_octets(std::string(32, 'x')))};
  EXPECT_FALSE(recovered.has_value());
}

TEST(decrypt_nested_jwt_rejects_a_non_jwe_input) {
  const auto recovered{sourcemeta::core::oidc_decrypt_nested_jwt(
      "not-a-jwe",
      sourcemeta::core::JWKPrivate::from_octets(std::string(32, 'k')))};
  EXPECT_FALSE(recovered.has_value());
}
