#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <array>    // std::array
#include <chrono>   // std::chrono::system_clock, std::chrono::seconds
#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::move

static constexpr std::string_view EC_PRIVATE_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ",)"
    R"("d":"ttepxcp-OwXCj4-v4sGcxRxQRXA8D5Svu02yhcHvbd0"})"};

static constexpr std::string_view EC_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"2TARGSWq8F97iq3Ng48wCEN26cxzy8OCzbFa-6ZfnaI",)"
    R"("y":"5lkzZqhWOkc2m2zJOotl6K3_x6-TSs9OnzQKwb35DxQ"})"};

static constexpr std::string_view OTHER_PUBLIC_JWK{
    R"({"kty":"EC","crv":"P-256",)"
    R"("x":"l8tFrhx-34tV3hRICRDY9zCkDlpBhF42UQUfWVAWBFs",)"
    R"("y":"9VE4jf_Ok_o64zbTTlcuNJajHmt6v9TDVrU0CdvGRDA"})"};

static constexpr std::string_view OCT_JWK{
    R"({"kty":"oct","k":"AyM1SysPpbyDfgZld3umj1qzKObwVMkoqQ-EstJQLr_T)"
    R"(-1qS0gZH75aKtMN3Yj0iPS4hcgUuTwjAzZr1Z9CAow"})"};

// RFC 9449 Section 6.1: the thumbprint of the Section 5 example public key
static constexpr std::string_view OTHER_THUMBPRINT{
    "0ZcOCORZNYy-DWpqq30jZyJGHTN0d2HglBV3uiguA4I"};

static const std::string RSA_PRIVATE_JWK{
    R"({"kty":"RSA",)"
    R"("n":"n-ivWLN0CTSLXc7GNcHGOuZHdu6QcGT3SlFTaf2ckxLVEDcAU4fIez7rKtvZaCzp)"
    R"(KJ6cb5KZnDcIn-gOCrE0FI6OZdCvgAGNqoFH6D1AANk8lWSFc1bWoti5xzXls2IN4fDRDa)"
    R"(BoXeXBniurOF8zTMlpyFiX4sgmaPpaJuKGH97AyXkHg4AYAFOUxOMPDM4EglexkzWRo0z_)"
    R"(qwRfj4qKbaNqF2P24dLOclnojEqD1i-ZKZn4PzD4plw85DSjc6Sm90OOpFW54zxiX2Eff5)"
    R"(9ez46j91_2d6SlSzFhNwrvaZYKBJl-vNegJzxsHWPwrGcNR31jEXa6kepjbAsp0FOBJw",)"
    R"("e":"AQAB",)"
    R"("d":"DdJY8FEijcvNjWAwCw7NXuNEo2e0yGI8awoN1wglc85EPaXjZf-YjpPo_nKzn1OHx)"
    R"(mEJ-ILdZX6Zmj4T6KIEdGUWCIA54ICXNCOp3s2x2IuZDE38qEmF59wqbVOf9RoGmn7sMuy)"
    R"(og2U2j8C1M1GB56Mz0i4GLYNLU_Y_u0NdKyfnhYRijuGOIb_TcC0MOy269XhsAVr-Ntat0)"
    R"(OzlITbaGKGW0yOr96hUY2Ocax99xbtjMyiYhB5Hu99yHn7ZPRtfATnmAujjWPV7Knd9n1e)"
    R"(O-hC6X8f-eKl9v-iqLuhNnFBGHhd3So_FZ0W3qXTXfC0Oq4TUsRjf4k4EtOcGf80SoQ",)"
    R"("p":"zYCJC70qUjNIlxUZ4u_XmDF8FiIsEo5A09eRNQchLnGClp_pVQ9z77mSicWsyUJOt)"
    R"(I9YTKWKCwniN0RIPZM2O3cV4ZC8hdWP5noTxitnbLYeyfoZDr-i8uPqHHu3vaW-VJhGeJj)"
    R"(5yELFrOxXAlikA2QQkecn94hs3iQ9tdFFc-8",)"
    R"("q":"xzQIWpdrkeQnOk3_YoO7XY8ZyRiQwv-8kqQ8oae3Njo06oMWC0VbV8o5HPj-pCMWB)"
    R"(vwAfMdZznOhMGjV2Ajla-z2ANteWebFVYt1xu2bxsMDIPqMfXTxWUftHtmFi6PSS9ZN5mI)"
    R"(22AarR0nLDuFHr3KG3lWm173-RfgvhH_Xrkk",)"
    R"("dp":"WSWfTfZbu5j_rnq65hWBg0ZEPB3K3KnVOZDULxrOrCUVr13jjMDNXHs2NIoKMKyR)"
    R"(FAbzGRzey3cYKT130S5hYl6AoX92KODCMgtXNKpzjVdb9-aEpD9B4vg4AO8ygBS8glokiA)"
    R"(Bkqxk8Q42rGRt22vm3rnOGhP0rrRovowYLiQU",)"
    R"("dq":"o08lCh_ZMGG7RzFqjXkxwiHvIc3h3_uIvS-oBV9Z9DsD5r5Q9CyIFhDTgc0f9bBN)"
    R"(_qvaOnG0Tmy9WNKZfeLNMw2xIEK3tzxZyyfqBowFiY2WoxLE2pVkx60P2Jq7wR8s6L9oXd)"
    R"(dm3vOYt3jn3-sQueVKbDwL7BL2wqYVTqsARwE",)"
    R"("qi":"qNjXTrOsMEqKYSJNsk6Z0sjDB2yuHS47CbkWZjikg4TIR4WmpYVY7EOfidoboen5)"
    R"(jGKFMuXAJo4pz6IBgS5eN6uzPvfvqJUiMGc6lG_zBFKsc-WnveUIzYqqz3gNrA_gwN872o)"
    R"(JCzSsEiFwMKFkui2fIAH8-c7Z5Tpo63qroRxk"})"};

static const auto FIXED_TIME{
    std::chrono::system_clock::from_time_t(1562262616)};

TEST(dpop_token_type_constant) {
  EXPECT_EQ(sourcemeta::core::OAUTH_TOKEN_TYPE_DPOP, "DPoP");
}

TEST(proofer_builds_a_verifiable_proof) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(proof_carries_the_expected_header_and_claims) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().type().value(), "dpop+jwt");
  EXPECT_EQ(token.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::ES256);
  EXPECT_TRUE(token.value().header().defines("jwk"));
  EXPECT_TRUE(token.value().token_id().has_value());
  EXPECT_TRUE(token.value().issued_at().has_value());
  EXPECT_EQ(token.value().payload().at("htm").to_string(), "POST");
  EXPECT_EQ(token.value().payload().at("htu").to_string(),
            "https://server.example.com/token");
  EXPECT_FALSE(token.value().payload().defines("ath"));
  EXPECT_FALSE(token.value().payload().defines("nonce"));
}

TEST(proof_includes_the_access_token_hash) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "the-token",
                            FIXED_TIME, proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().payload().defines("ath"));

  const auto thumbprint{proofer.thumbprint().value()};
  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "the-token";
  options.bound_thumbprint = thumbprint;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "GET", "https://resource.example.org/data",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(proofer_thumbprint_matches_the_key) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};

  const auto public_key{
      sourcemeta::core::JWK::from(sourcemeta::core::parse_json(EC_PUBLIC_JWK))};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_EQ(proofer.thumbprint().value(),
            public_key.value().thumbprint().value());
}

TEST(proof_thumbprint_extracts_the_key) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  EXPECT_EQ(sourcemeta::core::oauth_dpop_proof_thumbprint(proof).value(),
            proofer.thumbprint().value());
}

TEST(proof_thumbprint_rejects_a_malformed_proof) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_dpop_proof_thumbprint("not-a-jwt").has_value());
}

TEST(confirmation_binds_the_thumbprint) {
  const auto confirmation{sourcemeta::core::oauth_dpop_confirmation(
      "0ZcOCORZNYy-DWpqq30jZyJGHTN0d2HglBV3uiguA4I")};
  EXPECT_TRUE(confirmation.is_object());
  EXPECT_EQ(confirmation.at("jkt").to_string(),
            "0ZcOCORZNYy-DWpqq30jZyJGHTN0d2HglBV3uiguA4I");
}

TEST(verify_accepts_a_bound_thumbprint) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "the-token",
                            FIXED_TIME, proof));

  const auto thumbprint{proofer.thumbprint().value()};
  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "the-token";
  options.bound_thumbprint = thumbprint;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "GET", "https://resource.example.org/data",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_accepts_an_allowed_algorithm) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.allowed_algorithms = allowed;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(proofer_includes_an_observed_nonce) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  proofer.observe("https://server.example.com", "the-nonce");
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().payload().at("nonce").to_string(), "the-nonce");

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.expected_nonce = "the-nonce";
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_rejects_a_proof_count_other_than_one) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.proof_count = 2;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::ProofCount);
}

TEST(verify_rejects_a_malformed_proof) {
  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                "not-a-jwt", "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Malformed);
}

TEST(verify_rejects_a_wrong_method) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(
      sourcemeta::core::oauth_dpop_verify(
          proof, "GET", "https://server.example.com/token", FIXED_TIME, options)
          .value(),
      sourcemeta::core::OAuthDPoPError::MethodMismatch);
}

TEST(verify_rejects_a_wrong_target) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/introspect",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::TargetMismatch);
}

TEST(verify_normalizes_the_target_before_comparing) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "HTTPS://Server.Example.COM:443/token",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_rejects_a_stale_proof) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token",
                FIXED_TIME + std::chrono::seconds{600}, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Expired);
}

TEST(verify_rejects_a_proof_from_the_future) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token",
                FIXED_TIME - std::chrono::seconds{60}, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Expired);
}

TEST(verify_rejects_an_unexpected_type) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{sourcemeta::core::parse_json(R"({"typ":"jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::UnexpectedType);
}

TEST(verify_rejects_a_symmetric_algorithm) {
  const auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(OCT_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"HS256"})")};
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::UnsupportedAlgorithm);
}

TEST(verify_rejects_an_algorithm_outside_policy) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::ES384}};
  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.allowed_algorithms = allowed;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::UnsupportedAlgorithm);
}

TEST(verify_rejects_a_private_key_in_the_header) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PRIVATE_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::PrivateKey);
}

TEST(verify_rejects_a_missing_claim) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PUBLIC_JWK));
  const auto payload{
      sourcemeta::core::parse_json(R"({"jti":"x","htm":"POST","iat":1})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingClaim);
}

TEST(verify_rejects_a_missing_jwk_header) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingClaim);
}

TEST(verify_rejects_a_broken_signature) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(OTHER_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Signature);
}

TEST(verify_rejects_a_missing_required_nonce) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.expected_nonce = "the-nonce";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingNonce);
}

TEST(verify_rejects_a_nonce_mismatch) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  proofer.observe("https://server.example.com", "stale-nonce");
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.expected_nonce = "fresh-nonce";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::NonceMismatch);
}

TEST(verify_rejects_a_missing_access_token_hash) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "", FIXED_TIME,
                            proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "the-token";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "GET", "https://resource.example.org/data", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::AccessTokenMismatch);
}

TEST(verify_rejects_an_access_token_hash_mismatch) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "one-token",
                            FIXED_TIME, proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "another-token";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "GET", "https://resource.example.org/data", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::AccessTokenMismatch);
}

TEST(verify_rejects_a_key_binding_mismatch) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "the-token",
                            FIXED_TIME, proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "the-token";
  options.bound_thumbprint = "0ZcOCORZNYy-DWpqq30jZyJGHTN0d2HglBV3uiguA4I";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "GET", "https://resource.example.org/data", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::KeyMismatch);
}

TEST(replay_store_accepts_then_rejects) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_TRUE(store.check_and_insert("id", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{300}));
  EXPECT_FALSE(store.check_and_insert("id", "https://server.example.com/token",
                                      FIXED_TIME, std::chrono::seconds{300}));
}

TEST(replay_store_separates_targets) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_TRUE(store.check_and_insert("id", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{300}));
  EXPECT_TRUE(store.check_and_insert("id",
                                     "https://server.example.com/introspect",
                                     FIXED_TIME, std::chrono::seconds{300}));
}

TEST(replay_store_prunes_an_expired_entry) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_TRUE(store.check_and_insert("id", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{10}));
  EXPECT_EQ(store.size(FIXED_TIME), 1);
  EXPECT_TRUE(store.check_and_insert("id", "https://server.example.com/token",
                                     FIXED_TIME + std::chrono::seconds{20},
                                     std::chrono::seconds{10}));
  EXPECT_EQ(store.size(FIXED_TIME + std::chrono::seconds{20}), 1);
}

TEST(replay_store_size_counts_live_entries) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_EQ(store.size(FIXED_TIME), 0);
  EXPECT_TRUE(store.check_and_insert("one", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{10}));
  EXPECT_TRUE(store.check_and_insert("two", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{10}));
  EXPECT_EQ(store.size(FIXED_TIME), 2);
  EXPECT_EQ(store.size(FIXED_TIME + std::chrono::seconds{11}), 0);
}

TEST(valid_dpop_nonce_accepts_the_nonce_character_set) {
  EXPECT_TRUE(
      sourcemeta::core::oauth_is_valid_dpop_nonce("eyJ7S_zG.eyJH0-Z.HX4w-7v"));
  EXPECT_TRUE(sourcemeta::core::oauth_is_valid_dpop_nonce("!"));
}

TEST(valid_dpop_nonce_rejects_empty_and_disallowed_characters) {
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce(""));
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce("has space"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce("has\"quote"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce("has\\backslash"));
}

TEST(valid_dpop_nonce_boundary_characters) {
  EXPECT_TRUE(sourcemeta::core::oauth_is_valid_dpop_nonce("#"));
  EXPECT_TRUE(sourcemeta::core::oauth_is_valid_dpop_nonce("~"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce("\x7F"));
  EXPECT_FALSE(sourcemeta::core::oauth_is_valid_dpop_nonce("\x80"));
}

TEST(proof_thumbprint_matches_the_rfc9449_vector) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(OTHER_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());
  EXPECT_EQ(
      sourcemeta::core::oauth_dpop_proof_thumbprint(proof.value()).value(),
      OTHER_THUMBPRINT);
}

TEST(proof_embeds_only_the_public_key) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  const auto &embedded{token.value().header().at("jwk")};
  EXPECT_FALSE(embedded.defines("d"));
  EXPECT_FALSE(embedded.defines("p"));
  EXPECT_FALSE(embedded.defines("q"));
  EXPECT_TRUE(embedded.defines("kty"));
  EXPECT_TRUE(embedded.defines("crv"));
  EXPECT_TRUE(embedded.defines("x"));
  EXPECT_TRUE(embedded.defines("y"));
}

TEST(proofer_builds_a_verifiable_rsa_proof) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(RSA_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::RS256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME, options)
                   .has_value());
}

TEST(verify_normalizes_an_empty_path) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().payload().at("htu").to_string(),
            "https://server.example.com/");

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_FALSE(
      sourcemeta::core::oauth_dpop_verify(
          proof, "POST", "https://server.example.com/", FIXED_TIME, options)
          .has_value());
}

TEST(proof_strips_userinfo_from_the_target) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://user:pass@server.example.com/token", "",
                            FIXED_TIME, proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().payload().at("htu").to_string(),
            "https://server.example.com/token");
}

TEST(verify_accepts_iat_at_the_window_edges) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME + std::chrono::seconds{300}, options)
                   .has_value());
  EXPECT_FALSE(sourcemeta::core::oauth_dpop_verify(
                   proof, "POST", "https://server.example.com/token",
                   FIXED_TIME - std::chrono::seconds{5}, options)
                   .has_value());
}

TEST(verify_rejects_iat_one_second_past_the_window_edges) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token",
                FIXED_TIME + std::chrono::seconds{301}, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Expired);
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "POST", "https://server.example.com/token",
                FIXED_TIME - std::chrono::seconds{6}, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::Expired);
}

TEST(verify_rejects_a_missing_jti) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"htm":"POST","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingClaim);
}

TEST(verify_rejects_a_missing_htm) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htu":"https://server.example.com/token",)"
      R"("iat":1562262616})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingClaim);
}

TEST(verify_rejects_a_missing_iat) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PUBLIC_JWK));
  const auto payload{sourcemeta::core::parse_json(
      R"({"jti":"x","htm":"POST","htu":"https://server.example.com/token"})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());

  const sourcemeta::core::OAuthDPoPVerifyOptions options;
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof.value(), "POST", "https://server.example.com/token",
                FIXED_TIME, options)
                .value(),
            sourcemeta::core::OAuthDPoPError::MissingClaim);
}

TEST(proof_thumbprint_rejects_a_proof_without_a_jwk) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  const auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  const auto payload{sourcemeta::core::parse_json(R"({"jti":"x"})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());
  EXPECT_FALSE(
      sourcemeta::core::oauth_dpop_proof_thumbprint(proof.value()).has_value());
}

TEST(verify_rejects_a_presented_token_without_a_binding) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://resource.example.org", "GET",
                            "https://resource.example.org/data", "the-token",
                            FIXED_TIME, proof));

  sourcemeta::core::OAuthDPoPVerifyOptions options;
  options.access_token = "the-token";
  EXPECT_EQ(sourcemeta::core::oauth_dpop_verify(
                proof, "GET", "https://resource.example.org/data", FIXED_TIME,
                options)
                .value(),
            sourcemeta::core::OAuthDPoPError::KeyMismatch);
}

TEST(replay_store_detects_an_equivalent_target) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_TRUE(store.check_and_insert("id", "https://server.example.com/token",
                                     FIXED_TIME, std::chrono::seconds{300}));
  EXPECT_FALSE(store.check_and_insert("id",
                                      "HTTPS://Server.Example.COM:443/token",
                                      FIXED_TIME, std::chrono::seconds{300}));
}

TEST(replay_store_separator_prevents_ambiguity) {
  sourcemeta::core::OAuthDPoPReplayStore store;
  EXPECT_TRUE(store.check_and_insert("ab", "https://server.example.com/c",
                                     FIXED_TIME, std::chrono::seconds{300}));
  EXPECT_TRUE(store.check_and_insert("a", "https://server.example.com/bc",
                                     FIXED_TIME, std::chrono::seconds{300}));
}

TEST(confirmation_has_a_single_member) {
  const auto confirmation{
      sourcemeta::core::oauth_dpop_confirmation(OTHER_THUMBPRINT)};
  EXPECT_EQ(confirmation.size(), 1);
}

TEST(proofer_uses_the_latest_observed_nonce) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  proofer.observe("https://server.example.com", "first-nonce");
  proofer.observe("https://server.example.com", "second-nonce");
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_EQ(token.value().payload().at("nonce").to_string(), "second-nonce");
}

TEST(proofer_rejects_an_unparseable_target) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  std::string proof;
  EXPECT_FALSE(proofer.proof("https://server.example.com", "POST", "not a url",
                             "", FIXED_TIME, proof));
}

TEST(proofer_rejects_a_symmetric_algorithm) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::HS256};
  std::string proof;
  EXPECT_FALSE(proofer.proof("https://server.example.com", "POST",
                             "https://server.example.com/token", "", FIXED_TIME,
                             proof));
  EXPECT_TRUE(proof.empty());
}

TEST(proofer_ignores_a_malformed_observed_nonce) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  sourcemeta::core::OAuthDPoPProofer proofer{
      std::move(key.value()), sourcemeta::core::JWSAlgorithm::ES256};
  proofer.observe("https://server.example.com", "bad nonce");
  std::string proof;
  EXPECT_TRUE(proofer.proof("https://server.example.com", "POST",
                            "https://server.example.com/token", "", FIXED_TIME,
                            proof));

  const auto token{sourcemeta::core::JWT::from(proof)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().payload().defines("nonce"));
}

TEST(proof_thumbprint_rejects_a_private_key_in_the_header) {
  auto key{sourcemeta::core::JWKPrivate::from(
      sourcemeta::core::parse_json(EC_PRIVATE_JWK))};
  EXPECT_TRUE(key.has_value());
  auto header{
      sourcemeta::core::parse_json(R"({"typ":"dpop+jwt","alg":"ES256"})")};
  header.assign("jwk", sourcemeta::core::parse_json(EC_PRIVATE_JWK));
  const auto payload{sourcemeta::core::parse_json(R"({"jti":"x"})")};
  const auto proof{sourcemeta::core::jwt_sign(header, payload, key.value())};
  EXPECT_TRUE(proof.has_value());
  EXPECT_FALSE(
      sourcemeta::core::oauth_dpop_proof_thumbprint(proof.value()).has_value());
}
