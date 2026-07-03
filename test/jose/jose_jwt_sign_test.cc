#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

static const std::string EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};
static const std::string EC_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"})"};

TEST(jwt_sign_round_trips_through_the_token_layer) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto token_string{sourcemeta::core::jwt_sign(
      sourcemeta::core::parse_json(R"({"alg":"ES256"})"),
      sourcemeta::core::parse_json(R"({"iss":"acme","aud":"client"})"),
      key.value())};
  EXPECT_TRUE(token_string.has_value());

  const auto token{sourcemeta::core::JWT::from(token_string.value())};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().algorithm().has_value());
  EXPECT_EQ(token.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
  EXPECT_TRUE(token.value().issuer().has_value());
  EXPECT_EQ(token.value().issuer().value(), "acme");

  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_TRUE(sourcemeta::core::jwt_verify_signature(token.value(),
                                                     public_key.value()));
}

TEST(jwt_sign_rejects_non_object_payload) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jwt_sign(
                   sourcemeta::core::parse_json(R"({"alg":"ES256"})"),
                   sourcemeta::core::parse_json("42"), key.value())
                   .has_value());
}

TEST(jwt_sign_rejects_header_without_algorithm) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jwt_sign(
                   sourcemeta::core::parse_json(R"({})"),
                   sourcemeta::core::parse_json(R"({"iss":"acme"})"),
                   key.value())
                   .has_value());
}

TEST(jwt_sign_rejects_unsupported_algorithm) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jwt_sign(
                   sourcemeta::core::parse_json(R"({"alg":"none"})"),
                   sourcemeta::core::parse_json(R"({"iss":"acme"})"),
                   key.value())
                   .has_value());
}
