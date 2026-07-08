#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <chrono>   // std::chrono::system_clock, std::chrono::milliseconds
#include <optional> // std::optional
#include <string>   // std::string

// Build a compact JSON Web Token from its three component parts, base64url
// encoding each so the inputs are exact rather than hand-encoded
static auto make_token(const std::string_view header,
                       const std::string_view payload,
                       const std::string_view signature) -> std::string {
  return sourcemeta::core::base64url_encode(header) + "." +
         sourcemeta::core::base64url_encode(payload) + "." +
         sourcemeta::core::base64url_encode(signature);
}

TEST(parses_minimal_token) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().algorithm().has_value());
  EXPECT_EQ(token.value().algorithm().value(),
            sourcemeta::core::JWSAlgorithm::RS256);
  EXPECT_TRUE(token.value().header().is_object());
  EXPECT_TRUE(token.value().payload().is_object());
  EXPECT_EQ(token.value().signature(), "sig");
}

TEST(rejects_duplicate_header_parameter) {
  // RFC 7515 Section 4: the header parameter names must be unique
  const auto input{
      make_token(R"({ "alg": "RS256", "alg": "none" })", R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_duplicate_header_parameter_non_alg) {
  const auto input{make_token(R"({ "alg": "RS256", "kid": "a", "kid": "b" })",
                              R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(signing_input_is_verbatim) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "iss": "acme" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto expected{
      sourcemeta::core::base64url_encode(R"({ "alg": "RS256" })") + "." +
      sourcemeta::core::base64url_encode(R"({ "iss": "acme" })")};
  EXPECT_EQ(token.value().signing_input(), expected);
}

TEST(unrecognized_algorithm_still_parses) {
  const auto input{make_token(R"({ "alg": "none" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().algorithm().has_value());
}

TEST(key_id) {
  const auto input{
      make_token(R"({ "alg": "RS256", "kid": "key-1" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().key_id().has_value());
  EXPECT_EQ(token.value().key_id().value(), "key-1");
}

TEST(absent_key_id) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().key_id().has_value());
}

TEST(type) {
  const auto input{
      make_token(R"({ "alg": "RS256", "typ": "at+jwt" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().type().has_value());
  EXPECT_EQ(token.value().type().value(), "at+jwt");
}

TEST(absent_type) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().type().has_value());
}

TEST(issuer) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "iss": "acme" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().issuer().has_value());
  EXPECT_EQ(token.value().issuer().value(), "acme");
}

TEST(subject) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "sub": "user-1" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().subject().has_value());
  EXPECT_EQ(token.value().subject().value(), "user-1");
}

TEST(token_id) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "jti": "abc" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().token_id().has_value());
  EXPECT_EQ(token.value().token_id().value(), "abc");
}

TEST(absent_string_claim) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().issuer().has_value());
  EXPECT_FALSE(token.value().subject().has_value());
  EXPECT_FALSE(token.value().token_id().has_value());
}

TEST(non_string_claim_is_absent) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "iss": 123 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().issuer().has_value());
}

TEST(has_audience_string) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "aud": "api" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().has_audience("api"));
  EXPECT_FALSE(token.value().has_audience("other"));
}

TEST(has_audience_array) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "aud": [ "api", "web" ] })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().has_audience("api"));
  EXPECT_TRUE(token.value().has_audience("web"));
  EXPECT_FALSE(token.value().has_audience("other"));
}

TEST(has_audience_absent) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().has_audience("api"));
}

TEST(expires_at_integer) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "exp": 1700000000 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().expires_at().has_value());
  EXPECT_EQ(token.value().expires_at().value(),
            std::chrono::system_clock::from_time_t(1700000000));
}

TEST(expires_at_fractional) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "exp": 1700000000.5 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().expires_at().has_value());
  EXPECT_EQ(token.value().expires_at().value(),
            std::chrono::system_clock::from_time_t(1700000000) +
                std::chrono::milliseconds{500});
}

TEST(not_before_and_issued_at) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "nbf": 1700000000, "iat": 1699999999 })",
                              "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().not_before().has_value());
  EXPECT_EQ(token.value().not_before().value(),
            std::chrono::system_clock::from_time_t(1700000000));
  EXPECT_TRUE(token.value().issued_at().has_value());
  EXPECT_EQ(token.value().issued_at().value(),
            std::chrono::system_clock::from_time_t(1699999999));
}

TEST(expires_at_exponent_form) {
  // Exponent notation parses to a decimal-backed number, which must still
  // yield the correct timestamp (1e9 seconds since the epoch)
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "exp": 1e9 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().expires_at().has_value());
  EXPECT_EQ(token.value().expires_at().value(),
            std::chrono::system_clock::from_time_t(1000000000));
}

TEST(expires_at_high_precision_decimal) {
  // More than fifteen significant digits parses to a decimal-backed number
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "exp": 1700000000.000000001 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().expires_at().has_value());
  EXPECT_EQ(token.value().expires_at().value(),
            std::chrono::system_clock::from_time_t(1700000000));
}

TEST(expires_at_out_of_range_decimal_is_absent) {
  // A decimal beyond the range of a double cannot represent a timestamp
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "exp": 1e400 })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().expires_at().has_value());
}

TEST(absent_date_claim) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"({})", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().expires_at().has_value());
}

TEST(non_numeric_date_claim_is_absent) {
  const auto input{
      make_token(R"({ "alg": "RS256" })", R"({ "exp": "soon" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  EXPECT_FALSE(token.value().expires_at().has_value());
}

TEST(payload_exposes_unregistered_claims) {
  const auto input{make_token(R"({ "alg": "RS256" })",
                              R"({ "scope": "read write" })", "sig")};
  const auto token{sourcemeta::core::JWT::from(input)};
  EXPECT_TRUE(token.has_value());
  const auto *scope{token.value().payload().try_at(
      sourcemeta::core::JSON::StringView{"scope"})};
  EXPECT_NE(scope, nullptr);
  EXPECT_TRUE(scope->is_string());
  EXPECT_EQ(scope->to_string(), "read write");
}

TEST(rejects_one_segment) {
  EXPECT_FALSE(sourcemeta::core::JWT::from("abc").has_value());
}

TEST(rejects_two_segments) {
  EXPECT_FALSE(sourcemeta::core::JWT::from("abc.def").has_value());
}

TEST(rejects_four_segments) {
  EXPECT_FALSE(sourcemeta::core::JWT::from("a.b.c.d").has_value());
}

TEST(rejects_invalid_base64url_header) {
  const std::string input{"!!!." + sourcemeta::core::base64url_encode(R"({})") +
                          "." + sourcemeta::core::base64url_encode("sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_invalid_base64url_signature) {
  const std::string input{
      sourcemeta::core::base64url_encode(R"({ "alg": "RS256" })") + "." +
      sourcemeta::core::base64url_encode(R"({})") + ".!!!"};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_non_object_header) {
  const auto input{make_token(R"([])", R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_non_object_payload) {
  const auto input{make_token(R"({ "alg": "RS256" })", R"([])", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_missing_algorithm) {
  const auto input{make_token(R"({})", R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_non_string_algorithm) {
  const auto input{make_token(R"({ "alg": 123 })", R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(rejects_critical_header) {
  const auto input{
      make_token(R"({ "alg": "RS256", "crit": [ "exp" ] })", R"({})", "sig")};
  EXPECT_FALSE(sourcemeta::core::JWT::from(input).has_value());
}

TEST(constructor_throws_on_invalid_input) {
  try {
    sourcemeta::core::JWT{"abc.def"};
    FAIL();
  } catch (const sourcemeta::core::JWTParseError &error) {
    EXPECT_STREQ(error.what(), "The input is not a valid JSON Web Token");
  }
}

TEST(owns_decoded_data) {
  std::optional<sourcemeta::core::JWT> token;
  {
    const auto scoped{
        make_token(R"({ "alg": "RS256" })", R"({ "iss": "acme" })", "sig")};
    token = sourcemeta::core::JWT::from(scoped);
  }

  // The decoded header, payload, and signature are owned, so they survive the
  // destruction of the buffers they were parsed from
  EXPECT_TRUE(token.has_value());
  EXPECT_TRUE(token.value().issuer().has_value());
  EXPECT_EQ(token.value().issuer().value(), "acme");
  EXPECT_EQ(token.value().signature(), "sig");
}
