#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// RFC 7636 Appendix B: the worked example verifier and its S256 challenge.
static constexpr std::string_view APPENDIX_B_VERIFIER{
    "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk"};
static constexpr std::string_view APPENDIX_B_CHALLENGE{
    "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM"};

// A second valid verifier, distinct from the Appendix B one, for mismatch
// coverage (43 unreserved characters).
static constexpr std::string_view OTHER_VERIFIER{
    "Zm9vYmFyZm9vYmFyZm9vYmFyZm9vYmFyZm9vYmFyMDA"};

TEST(challenge_of_rfc7636_appendix_b) {
  const auto challenge{
      sourcemeta::core::oauth_pkce_challenge(APPENDIX_B_VERIFIER)};
  EXPECT_EQ((std::string_view{challenge.data(), challenge.size()}),
            APPENDIX_B_CHALLENGE);
}

TEST(minted_verifier_and_challenge_round_trip) {
  const auto verifier{sourcemeta::core::oauth_pkce_verifier()};
  const std::string_view verifier_view{verifier.data(), verifier.size()};
  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier_view)};
  const std::string_view challenge_view{challenge.data(), challenge.size()};
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  verifier_view, challenge_view,
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(minted_verifiers_differ_between_calls) {
  const auto first{sourcemeta::core::oauth_pkce_verifier()};
  const auto second{sourcemeta::core::oauth_pkce_verifier()};
  EXPECT_FALSE((std::string_view{first.data(), first.size()}) ==
               (std::string_view{second.data(), second.size()}));
}

TEST(verify_matches_rfc7636_appendix_b_under_strict) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, APPENDIX_B_CHALLENGE,
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(verify_rejects_a_mismatched_verifier) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  OTHER_VERIFIER, APPENDIX_B_CHALLENGE,
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::Mismatch);
}

TEST(verify_reports_a_missing_verifier) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  "", APPENDIX_B_CHALLENGE,
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MissingVerifier);
}

TEST(verify_reports_a_missing_challenge) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, "",
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MissingChallenge);
}

TEST(verify_reports_not_used_when_neither_is_present) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  "", "", sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::NotUsed);
}

TEST(verify_rejects_plain_under_strict) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, APPENDIX_B_VERIFIER,
                  sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MethodNotAllowed);
}

TEST(verify_accepts_plain_under_compatible) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, APPENDIX_B_VERIFIER,
                  sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Compatible) ==
              sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(verify_rejects_a_plain_mismatch_under_compatible) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, OTHER_VERIFIER,
                  sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Compatible) ==
              sourcemeta::core::OAuthPKCEOutcome::Mismatch);
}

TEST(verify_rejects_a_malformed_verifier) {
  // A 42 character value is one short of the minimum length (RFC 7636
  // Section 4.1).
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjX",
                  APPENDIX_B_CHALLENGE, sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedVerifier);
}

TEST(verify_rejects_an_s256_challenge_of_the_wrong_length) {
  // A valid S256 challenge is always exactly 43 characters (RFC 7636
  // Section 4.2), so a 44 character value cannot be one.
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER,
                  "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cMA",
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedChallenge);
}

TEST(verify_rejects_a_verifier_with_an_invalid_character) {
  // 43 characters, but the "+" is outside the RFC 3986 unreserved set the
  // verifier draws from (RFC 7636 Section 4.1).
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  "dBjftJeZ4CVP+mB92K27uhbUJU1p1r_wW1gFWFOEjXk",
                  APPENDIX_B_CHALLENGE, sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedVerifier);
}

TEST(verify_rejects_an_s256_challenge_with_an_invalid_character) {
  // 43 characters, but "+" is not in the unreserved set (RFC 7636 Section 4.2).
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER,
                  "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-c+",
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedChallenge);
}

TEST(verify_treats_an_unreserved_non_base64url_s256_challenge_as_a_mismatch) {
  // The final "." is unreserved, so the challenge is a syntactically valid code
  // challenge that simply cannot match the S256 output, which is a Mismatch
  // rather than a MalformedChallenge.
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER,
                  "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-c.",
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::Mismatch);
}

TEST(verify_rejects_a_malformed_plain_challenge_under_compatible) {
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER,
                  "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-c+",
                  sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Compatible) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedChallenge);
}

TEST(verify_rejects_a_plain_challenge_of_the_wrong_length) {
  // A plain challenge is the verifier itself, so it must also satisfy the
  // 43 character minimum (RFC 7636 Section 4.2), which a 42 character value
  // fails
  const std::string challenge(42, 'a');
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  APPENDIX_B_VERIFIER, challenge,
                  sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Compatible) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedChallenge);
}

TEST(verify_accepts_a_verifier_with_all_unreserved_punctuation) {
  // RFC 7636 Section 4.1 draws the verifier from the RFC 3986 unreserved set,
  // so
  // "-", ".", "_", and "~" are all valid, none of which the Appendix B vector
  // exercises together
  const std::string verifier{"abcdefghij-klmnopqrst.uvwxyzABCDEFG_HIJKL~M"};
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  verifier, verifier, sourcemeta::core::OAuthPKCEMethod::Plain,
                  sourcemeta::core::OAuthProfile::Compatible) ==
              sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(verify_accepts_a_verifier_at_the_maximum_length) {
  const std::string verifier(128, 'a');
  const auto challenge{sourcemeta::core::oauth_pkce_challenge(verifier)};
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  verifier,
                  std::string_view{challenge.data(), challenge.size()},
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::Match);
}

TEST(verify_rejects_a_verifier_over_the_maximum_length) {
  const std::string verifier(129, 'a');
  EXPECT_TRUE(sourcemeta::core::oauth_pkce_verify(
                  verifier, APPENDIX_B_CHALLENGE,
                  sourcemeta::core::OAuthPKCEMethod::S256,
                  sourcemeta::core::OAuthProfile::Strict) ==
              sourcemeta::core::OAuthPKCEOutcome::MalformedVerifier);
}

TEST(serialize_pkce_methods) {
  EXPECT_EQ(sourcemeta::core::oauth_pkce_method_code(
                sourcemeta::core::OAuthPKCEMethod::S256),
            "S256");
  EXPECT_EQ(sourcemeta::core::oauth_pkce_method_code(
                sourcemeta::core::OAuthPKCEMethod::Plain),
            "plain");
}

TEST(parse_pkce_methods) {
  const auto s256{sourcemeta::core::to_oauth_pkce_method("S256")};
  EXPECT_TRUE(s256.has_value());
  EXPECT_TRUE(s256.value() == sourcemeta::core::OAuthPKCEMethod::S256);
  const auto plain{sourcemeta::core::to_oauth_pkce_method("plain")};
  EXPECT_TRUE(plain.has_value());
  EXPECT_TRUE(plain.value() == sourcemeta::core::OAuthPKCEMethod::Plain);
}

TEST(parse_pkce_method_rejects_an_unknown_value) {
  EXPECT_FALSE(sourcemeta::core::to_oauth_pkce_method("plaintext").has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_pkce_method("s256").has_value());
  EXPECT_FALSE(sourcemeta::core::to_oauth_pkce_method("").has_value());
}
