#include <sourcemeta/core/test.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>

#include <optional>    // std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

// The vectors are self-signed with OpenSSL. The signing input is treated as
// opaque bytes and the algorithm is supplied separately, so the same input is
// reused across algorithms, and one case deliberately signs content that is not
// a JSON object
namespace {
constexpr std::string_view RS256_SIGNING_INPUT{
    "eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJhY21lIn0"};
constexpr std::string_view ES256_SIGNING_INPUT{
    "eyJhbGciOiJFUzI1NiJ9.eyJpc3MiOiJhY21lIn0"};
constexpr std::string_view ARBITRARY_SIGNING_INPUT{
    "Generic JWS payload that is not a JSON object"};
constexpr std::string_view RSA_SIGNATURE{
    "LruFc3aFIkE9ooX8G6SfJCFdpAt5Rn3eb_zyNtrJ4oNRoeHnm3pfeEOm1WRv24YH6Z-"
    "LQ25cR86sHqmsF_VHgTjVFj0_"
    "6MbYWbLIkB8sj57UC3OZZ1SmAsItXXPepTIKMzEAnD0fS4KbqlcAbuZ2JG0aS8_"
    "nuKlBezGMAjaQRvpSU2rAhAfW-"
    "3EsjWPikaRgYQklPwRaZo2hPNyKiVu6JgCCoFS1F95SUM78cUe5AaJhnB-"
    "NHbTU155y015py4yaB2HdxOfcYrQqP0wCypmHQuM9z2AvaCOaEjBcp0oY7UvV8nll4KutFadPA"
    "-ncHCNxHgW_7anCnT7E20q617IF2w"};
constexpr std::string_view RSA_ARBITRARY_SIGNATURE{
    "fyh7yY86lH_Sjpc0czE0veQLHadRa6bUo4lWblWjh8VAsPT_EXYHAt3b9JQz0juooM-8v6c-"
    "aWCsLpyXBKA982QCf7BA6ZpN5B8txkkWfqJOSPTlra4Q10sYmkC7SxmakDVSm4GJYc7VGojErd"
    "Sk4Em9IoY4CLcqSINp9_vwP0cg17chsYGYHhuzyiMtEAUWHe3WI55rOHzE1ww4Ya2P7mHbI_"
    "apdBu9_bWOuH8XKldcpjvSCjk3MbmpnzErr_27REDqydqpTZ7lA5U7sC3jmXqv_Xt11-"
    "PUEd7YS2L5ZTkYZaGejL3XOkRXLciAO4-qrz8IH11wztz4hdMLFle-7Q"};
constexpr std::string_view ECDSA_SIGNATURE{
    "VXxKJp11lP3qw8XksZAgpugJbHXGfz76JfMnT1B622YwD9Y5m1Itds7FvGpENf47WFi7fZn36M"
    "fH3LhV0vSY_w"};
constexpr std::string_view RSA_JWK{
    R"JSON({ "kty": "RSA", "n": "g6AMCEh4IMEnWr_9s8s-uUPXOWm1Zt2h4nV2ZCWsZRHnQg-SzmkNDw3SqUF9nLbjCz_HlElABwe9XZ8gfwVGKr3TNHcaTS_QQNGzX6WndznyQKvoEL3BkvMAk-p-CzUpW4XzAl7iwdpOjxh8iFAR-pOcdvCzEcwEVkwlcVL1IDXN_oFxfpldOA94Ljcp4fA0FmsTo74x93el3hzfgHYSt1UeHQkrjQwmfecbjVHpDHmpqcaAmgWpKHYnWa0WZJ5t-cm17UIydct-lEUKne_bqoUHuyqakJG6fLHbunxc0CRxqcV5r_i64D0vMDsdu3I1YehoOj9CDvzE8rKGeSA8Mw", "e": "AQAB" })JSON"};
constexpr std::string_view RSA_JWK_OTHER_ALGORITHM{
    R"JSON({ "kty": "RSA", "n": "g6AMCEh4IMEnWr_9s8s-uUPXOWm1Zt2h4nV2ZCWsZRHnQg-SzmkNDw3SqUF9nLbjCz_HlElABwe9XZ8gfwVGKr3TNHcaTS_QQNGzX6WndznyQKvoEL3BkvMAk-p-CzUpW4XzAl7iwdpOjxh8iFAR-pOcdvCzEcwEVkwlcVL1IDXN_oFxfpldOA94Ljcp4fA0FmsTo74x93el3hzfgHYSt1UeHQkrjQwmfecbjVHpDHmpqcaAmgWpKHYnWa0WZJ5t-cm17UIydct-lEUKne_bqoUHuyqakJG6fLHbunxc0CRxqcV5r_i64D0vMDsdu3I1YehoOj9CDvzE8rKGeSA8Mw", "e": "AQAB", "alg": "RS384" })JSON"};
constexpr std::string_view EC_JWK{
    R"JSON({ "kty": "EC", "crv": "P-256", "x": "uEMPr85yIqQEqUAOF7f-jpo0LA9tnUXj1q6HzanBnJs", "y": "JHRc8vYEaVwcjH20LqwKfehDU2JGg43Sx56GcEgfbXY" })JSON"};
constexpr std::string_view EDDSA_SIGNING_INPUT{
    "eyJhbGciOiJFZERTQSJ9.RXhhbXBsZSBvZiBFZDI1NTE5IHNpZ25pbmc"};
constexpr std::string_view EDDSA_SIGNATURE{
    "hgyY0il_MGCjP0JzlnLWG1PPOt7-09PGcvMg3AIbQR6dWbhijcNR4ki4iylGjg5BhVsPt9g7sV"
    "vpAr_MuM0KAg"};
constexpr std::string_view OKP_JWK{
    R"JSON({ "kty": "OKP", "crv": "Ed25519", "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo" })JSON"};
} // namespace

TEST(rs256_valid) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RS256_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(es256_valid) {
  const auto signature{sourcemeta::core::base64url_decode(ECDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES256, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(arbitrary_signing_input) {
  const auto signature{
      sourcemeta::core::base64url_decode(RSA_ARBITRARY_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, ARBITRARY_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(unrecognized_algorithm) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      std::nullopt, RS256_SIGNING_INPUT, signature.value(), key.value()));
}

TEST(contradicting_key_algorithm) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_JWK_OTHER_ALGORITHM))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RS256_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(key_type_mismatch) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RS256_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(curve_mismatch) {
  const auto signature{sourcemeta::core::base64url_decode(ECDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES512, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(tampered_signature) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  std::string tampered{signature.value()};
  tampered.front() =
      static_cast<char>(static_cast<unsigned char>(tampered.front()) ^ 0x80U);
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RS256_SIGNING_INPUT, tampered,
      key.value()));
}

// The Ed25519 signature is the worked example from RFC 8037 Appendix A.4
TEST(eddsa_ed25519_valid) {
  const auto signature{sourcemeta::core::base64url_decode(EDDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OKP_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::EdDSA, EDDSA_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(eddsa_tampered_signature) {
  const auto signature{sourcemeta::core::base64url_decode(EDDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  std::string tampered{signature.value()};
  tampered.front() =
      static_cast<char>(static_cast<unsigned char>(tampered.front()) ^ 0x80U);
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OKP_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::EdDSA, EDDSA_SIGNING_INPUT, tampered,
      key.value()));
}

TEST(eddsa_key_type_mismatch) {
  const auto signature{sourcemeta::core::base64url_decode(EDDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::EdDSA, EDDSA_SIGNING_INPUT,
      signature.value(), key.value()));
}
