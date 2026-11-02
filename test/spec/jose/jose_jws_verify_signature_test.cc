#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

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
// The RFC 7515 Appendix A.1 example key and signing input
constexpr std::string_view OCT_JWK{
    R"JSON({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})JSON"};
constexpr std::string_view RFC7515_A1_SIGNING_INPUT{
    "eyJ0eXAiOiJKV1QiLA0KICJhbGciOiJIUzI1NiJ9."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlL"
    "mNvbS9pc19yb290Ijp0cnVlfQ"};
constexpr std::string_view EDDSA_SIGNING_INPUT{
    "eyJhbGciOiJFZERTQSJ9.RXhhbXBsZSBvZiBFZDI1NTE5IHNpZ25pbmc"};
constexpr std::string_view EDDSA_SIGNATURE{
    "hgyY0il_MGCjP0JzlnLWG1PPOt7-09PGcvMg3AIbQR6dWbhijcNR4ki4iylGjg5BhVsPt9g7sV"
    "vpAr_MuM0KAg"};
constexpr std::string_view OKP_JWK{
    R"JSON({ "kty": "OKP", "crv": "Ed25519", "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo" })JSON"};
constexpr std::string_view RSA_JWK_MATCHING_ALGORITHM{
    R"JSON({ "kty": "RSA", "n": "g6AMCEh4IMEnWr_9s8s-uUPXOWm1Zt2h4nV2ZCWsZRHnQg-SzmkNDw3SqUF9nLbjCz_HlElABwe9XZ8gfwVGKr3TNHcaTS_QQNGzX6WndznyQKvoEL3BkvMAk-p-CzUpW4XzAl7iwdpOjxh8iFAR-pOcdvCzEcwEVkwlcVL1IDXN_oFxfpldOA94Ljcp4fA0FmsTo74x93el3hzfgHYSt1UeHQkrjQwmfecbjVHpDHmpqcaAmgWpKHYnWa0WZJ5t-cm17UIydct-lEUKne_bqoUHuyqakJG6fLHbunxc0CRxqcV5r_i64D0vMDsdu3I1YehoOj9CDvzE8rKGeSA8Mw", "e": "AQAB", "alg": "RS256" })JSON"};
// A P-384 public key, so that a curve the algorithm does not name can be told
// apart from a key type it does not name
constexpr std::string_view EC_P384_JWK{
    R"JSON({ "kty": "EC", "crv": "P-384", "x": "2vNb4tkbgqO0zEAs2uZ7sfAud-yiG24vhu8f_n7t1bGuAk6Ri1cGMIqUv8vcaDFf", "y": "Y1aaM4Jna_mH2L_KgEA5SIrhaq5XbASmay0UnPTw2F3-znyZ-fZ8yCwtj3aPGUGv" })JSON"};
// Secrets below what RFC 7518 Section 3.2 requires of each MAC, which asks for
// a key of at least the size of the hash output
constexpr std::string_view OCT_JWK_16_BYTES{
    R"JSON({"kty":"oct","k":"QUFBQUFBQUFBQUFBQUFBQQ"})JSON"};
constexpr std::string_view OCT_JWK_32_BYTES{
    R"JSON({"kty":"oct","k":"QUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUE"})JSON"};
constexpr std::string_view EC_P256_OTHER_JWK{
    R"JSON({ "kty": "EC", "crv": "P-256", "x": "f5qz3NpV9TWR9WS-LMxoQ85jXz1oJafYhP0dhHOYjgE", "y": "Gn_pneRFnAQcsd3piv9ddYAOfz6kOgvKev7Eh6Naupg" })JSON"};
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

TEST(jws_verify_signature_hs256_known_answer) {
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(jws_verify_signature_hs256_rejects_tampered_input) {
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, "eyJhbGciOiJIUzI1NiJ9.tampered",
      signature.value(), key.value()));
}

TEST(jws_verify_signature_hs256_rejects_truncated_signature) {
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, RFC7515_A1_SIGNING_INPUT,
      std::string_view{signature.value()}.substr(0, 16), key.value()));
}

TEST(jws_verify_signature_hs256_rejects_rsa_key) {
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(jws_verify_signature_rs256_rejects_oct_key) {
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(jws_verify_signature_hs512_rejects_short_secret) {
  const auto key{sourcemeta::core::JWK::from(sourcemeta::core::parse_json(
      R"({"kty":"oct","k":"hJtXIZ2uSN5kbQfbtTNWbpdmhkV8FJG-Onbc6mxCcYg"})"))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS512, RFC7515_A1_SIGNING_INPUT,
      std::string(64, 'x'), key.value()));
}

TEST(jws_verify_signature_hs256_key_declaring_other_algorithm) {
  const auto key{sourcemeta::core::JWK::from(sourcemeta::core::parse_json(
      R"({"kty":"oct","alg":"HS512","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ)"
      R"(-EstJQLr_T-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})"))};
  EXPECT_TRUE(key.has_value());
  const auto signature{sourcemeta::core::base64url_decode(
      "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")};
  EXPECT_TRUE(signature.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

// RFC 7518 Section 3.5 gives PSS an RSA key
// A signature the wrong verifier would accept does not exist, so what this
// pins is the refusal rather than the comparison that follows it
TEST(ps256_rejects_an_ec_key) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::PS256, RS256_SIGNING_INPUT,
      signature.value(), key.value()));
}

// RFC 7518 Section 3.4 ties this algorithm to the P-256 curve alone. The
// signature is a real one over this input, made with the P-384 key below under
// this algorithm's own hash, so it would verify if the curve were not checked
// and the refusal can only come from the curve
TEST(es256_rejects_a_p384_key) {
  const auto signature{sourcemeta::core::base64url_decode(
      "PtKwAViIAQoBqIK2D3Xflpd7eaS9yqGWyuBiKClC5f0yqG1hEc4ClwxHorkP"
      "IpiqmWrKZ8E9g98MnLawdqEM4C3bmwrkyxW_grAD-NZaQBnEC0220-9mfrJi"
      "h4zCNM3a")};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_P384_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES256, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

// RFC 7518 Section 3.4 gives every ECDSA algorithm an elliptic curve key
// A signature the wrong verifier would accept does not exist, so what this
// pins is the refusal rather than the comparison that follows it
TEST(es384_rejects_an_rsa_key) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES384, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

// RFC 7518 Section 3.4 ties this algorithm to the P-384 curve alone. As above,
// the signature is a real one made with the P-256 key below under this
// algorithm's own hash, so only the curve stands between it and acceptance
TEST(es384_rejects_a_p256_key) {
  const auto signature{sourcemeta::core::base64url_decode(
      "zqMLn9iNimNYZJbUJ4a6oLVgwKGlYS1idat2HCcDPX-P07ZDfsJ5U2sBcUz5"
      "YUkbjkHHvdNr6W8UCzU_NjOwWQ")};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(EC_P256_OTHER_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES384, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

// A signature the wrong verifier would accept does not exist, so what this
// pins is the refusal rather than the comparison that follows it
TEST(es512_rejects_an_rsa_key) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::ES512, ES256_SIGNING_INPUT,
      signature.value(), key.value()));
}

// A signature the wrong verifier would accept does not exist, so what this
// pins is the refusal rather than the comparison that follows it
TEST(eddsa_rejects_an_rsa_key) {
  const auto signature{sourcemeta::core::base64url_decode(EDDSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(RSA_JWK))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::EdDSA, EDDSA_SIGNING_INPUT,
      signature.value(), key.value()));
}

// RFC 7518 Section 3.2 asks for a key of at least the hash output size
TEST(hs256_rejects_a_secret_below_the_hash_size) {
  // The tag is the one this key and input really produce, computed outside
  // this library, so the refusal can only come from the size of the key and
  // not from a signature that was never going to match
  const auto signature{sourcemeta::core::base64url_decode(
      "ChFflhqtBAe4-Nn-P5U2RRzl1RB181AbzNxaNku9ftk")};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(OCT_JWK_16_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS256, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(hs384_rejects_a_secret_below_the_hash_size) {
  // The tag is the one this key and input really produce, computed outside
  // this library, so the refusal can only come from the size of the key and
  // not from a signature that was never going to match
  const auto signature{sourcemeta::core::base64url_decode(
      "cLISYhgbinnpCk4MlYVbwxRscu4a8Uan8FrU4hMjeYdI_BtseA2_3vUlEmVT"
      "kWnN")};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS384, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

TEST(hs512_rejects_a_secret_below_the_hash_size) {
  // The tag is the one this key and input really produce, computed outside
  // this library, so the refusal can only come from the size of the key and
  // not from a signature that was never going to match
  const auto signature{sourcemeta::core::base64url_decode(
      "nROtnrTtvHMh8h-pNXtUZUpulAZvRzeFjPAomkVhKBASJAffuQZmDBG9F7JZ"
      "80orxd2-wK4c7TdcbXXyQCB2gQ")};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(OCT_JWK_32_BYTES))};
  EXPECT_TRUE(key.has_value());
  EXPECT_FALSE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::HS512, RFC7515_A1_SIGNING_INPUT,
      signature.value(), key.value()));
}

// The key names the same algorithm the caller asked for, which is the one
// reading of the hint that has never been taken
TEST(rs256_accepts_a_key_whose_algorithm_hint_matches) {
  const auto signature{sourcemeta::core::base64url_decode(RSA_SIGNATURE)};
  EXPECT_TRUE(signature.has_value());
  const auto key{sourcemeta::core::JWK::from(
      sourcemeta::core::parse_json(RSA_JWK_MATCHING_ALGORITHM))};
  EXPECT_TRUE(key.has_value());
  EXPECT_TRUE(sourcemeta::core::jws_verify_signature(
      sourcemeta::core::JWSAlgorithm::RS256, RS256_SIGNING_INPUT,
      signature.value(), key.value()));
}
