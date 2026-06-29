#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// A locally-signed token whose issuer, audience, and far-future expiry let the
// success path run to completion, with its matching key set
constexpr std::string_view SIGNED_TOKEN{
    "eyJhbGciOiJSUzI1NiIsInR5cCI6ImF0K2p3dCJ9."
    "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ."
    "U3ZBo7MvSW0U099gJ_"
    "vIA5T8HJ2XnKSzYmqkx7SDxgxQfmxQyu3QZIeKT68AAH7wQjWRvNWQ7f3Es57UUNUQAMs-"
    "z5TWlVBKtYZf5ZcbYqc4KrQ-ApwpjoFGJxurnd1R_"
    "tz02WssnvrZNKnxNPuGoYIkJKNCl59yLFJwRLf3nK_Jcxs-"
    "1m2MvKsm647PuXqhYOKlZkHOvkIV0RV8cLJ56_gDVjj7TlKQgwbTdW_"
    "71QLwLWRFGftU2EAWuqayTSpPeUA6kB4sfn7JNsweqDs7uev30m6y8BE9uzwzHuuovaN1cZz0o"
    "TAGXcx64sfbPs6HEMp5_FoU0SccxArAbnHSjA"};
constexpr std::string_view SIGNED_KEYS{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "oHTpl-jfNfBuXmBp58sW8s_77UP6j2jA0mjjKjhDkxhp7Agk-xLNGgfPCS_bjdZ6YU6FGeab8uVjkSgo9_0OCJUaF4vzEGwXmNuGawANxnZtiYjWvbJlq-2mn_L7rsqGQcSkMmyM0g4aX7dF8wB6DVrXShJ78fcrNtpeoU72YGEdjehA8qVclDFwBdpCGynxxnWJePk72lQb6gkVMqKMc3jBF8GkWf8oP_sjss-fpOjSUMR1c8_0JlTYWO46KWOZa0EO2t8H1V3imMyzbhoxRd_qZHmo46gJkG-ZdebjX0vGQllaCwu0z4kLcXIfAZhqPEkdssDGhC_txwJuhaPDFQ", "e": "AQAB" } ] })JSON"};

// The RFC 7515 Appendix A.2 worked-example token, re-headed with a key
// identifier so its signature no longer matches the bytes it now covers, plus
// key sets that carry identifiers
constexpr std::string_view RS256_TOKEN_WITH_KEY_ID{
    "eyJhbGciOiJSUzI1NiIsImtpZCI6ImsxIn0."
    "eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS"
    "9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__"
    "dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_"
    "O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_"
    "nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_"
    "dO--xi12jzDwusC-"
    "eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AX"
    "LIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw"};
constexpr std::string_view RSA_KEYS_KEY_ID_K1{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB", "kid": "k1" } ] })JSON"};
constexpr std::string_view RSA_KEYS_KEY_ID_OTHER{
    R"JSON({ "keys": [ { "kty": "RSA", "n": "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ", "e": "AQAB", "kid": "other" } ] })JSON"};

// A set holding only an elliptic-curve key, which cannot serve the RSA token,
// so verifying that token against it reports an unknown key for want of a
// compatible one
constexpr std::string_view ELLIPTIC_CURVE_KEYS{
    R"JSON({ "keys": [ { "kty": "EC", "crv": "P-256", "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU", "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0" } ] })JSON"};

constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_RS256{
    {sourcemeta::core::JWSAlgorithm::RS256}};

} // namespace

TEST(JOSE_jwks_provider, unreachable_issuer_denies) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return std::nullopt;
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, malformed_body_is_treated_as_failure) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{"not json at all",
                                                           std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(JOSE_jwks_provider, cache_hit_does_not_refetch) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, cache_control_absent_uses_fallback) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Still fresh one second before the 1 hour fallback lifetime elapses
  now = std::chrono::system_clock::from_time_t(1000000000 + 3599);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Stale at exactly the fallback lifetime
  now = std::chrono::system_clock::from_time_t(1000000000 + 3600);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, cache_control_above_maximum_clamps_down) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::chrono::hours{48}};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Still fresh one second before the 24 hour maximum lifetime elapses
  now = std::chrono::system_clock::from_time_t(1000000000 + 86399);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Stale at exactly the maximum lifetime, never at the advertised 48 hours
  now = std::chrono::system_clock::from_time_t(1000000000 + 86400);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, cache_control_below_minimum_clamps_up) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::chrono::seconds{60}};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Still fresh one second before the 5 minute minimum lifetime elapses
  now = std::chrono::system_clock::from_time_t(1000000000 + 299);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Stale at exactly the minimum lifetime, never at the advertised 60 seconds
  now = std::chrono::system_clock::from_time_t(1000000000 + 300);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, cache_control_zero_clamps_to_minimum) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        // A real, advertised lifetime of zero, distinct from no advertisement
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::chrono::seconds{0}};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Still fresh one second before the 5 minute minimum lifetime elapses
  now = std::chrono::system_clock::from_time_t(1000000000 + 299);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Stale at exactly the minimum lifetime, and zero does not collapse into the
  // fallback
  now = std::chrono::system_clock::from_time_t(1000000000 + 300);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, unknown_key_triggers_one_guarded_refetch) {
  std::size_t calls{0};
  // The issuer first publishes a set with no key able to verify the token, then
  // after rotation publishes the matching key
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{calls == 1 ? ELLIPTIC_CURVE_KEYS : SIGNED_KEYS},
            std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, signature_failure_is_not_retried) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{RSA_KEYS_KEY_ID_K1}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1300000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  // The token names a key the set also contains, but the token was re-headed so
  // its signature no longer matches, so verification reports a signature
  // failure rather than an unknown key, and a signature failure is never
  // retried
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Signature);
  EXPECT_EQ(calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, unknown_key_refetch_is_bounded_by_cooldown) {
  std::size_t calls{0};
  // The published set never contains the token's key, so every verification
  // hits an unknown key
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{RSA_KEYS_KEY_ID_OTHER}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1300000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  ASSERT_TRUE(token.has_value());

  // Cold fetch then one guarded refetch, and the named key is still absent
  const auto first{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client")};
  ASSERT_TRUE(first.has_value());
  EXPECT_EQ(first.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(calls, std::size_t{2});

  // Within the cooldown window no further refetch happens
  const auto second{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client")};
  ASSERT_TRUE(second.has_value());
  EXPECT_EQ(second.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(calls, std::size_t{2});

  // After the cooldown elapses a new guarded refetch is allowed
  now = std::chrono::system_clock::from_time_t(1300000000 + 301);
  const auto third{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client")};
  ASSERT_TRUE(third.has_value());
  EXPECT_EQ(third.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(calls, std::size_t{3});
}

TEST(JOSE_jwks_provider, failed_refresh_serves_stale_keys) {
  std::size_t calls{0};
  // Good keys first, then the issuer becomes unreachable on every later fetch
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        if (calls == 1) {
          return sourcemeta::core::JWKSProvider::FetchResult{
              std::string{SIGNED_KEYS}, std::nullopt};
        }
        return std::nullopt;
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Past the fallback lifetime the refresh is attempted and fails, so the
  // retained keys still verify the token
  now = std::chrono::system_clock::from_time_t(1000000000 + 7200);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});

  // A failed refresh backs off by the minimum lifetime, so an immediate retry
  // does not hammer the unreachable issuer
  now = std::chrono::system_clock::from_time_t(1000000000 + 7200 + 60);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, expected_type_is_forwarded) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  // The token carries an access-token type, so pinning a different expected
  // type is rejected, proving that argument reaches the underlying verification
  const auto error{provider.verify(token.value(), ALLOWED_RS256, "acme",
                                   "client", std::nullopt,
                                   "application/example")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Type);
}

TEST(JOSE_jwks_provider, clock_skew_tolerates_recent_expiry) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  // Thirty seconds past the token's expiry, but a sixty second skew absorbs it,
  // proving the configured tolerance reaches the underlying verification
  auto now{std::chrono::system_clock::from_time_t(2000000030)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{.clock_skew =
                                                  std::chrono::seconds{60}},
      [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
}

TEST(JOSE_jwks_provider, fetcher_exception_is_treated_as_failure) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        throw std::runtime_error{"transport failure"};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  // A throwing fetcher must not escape verification, and it denies like any
  // failed fetch
  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(JOSE_jwks_provider, fetcher_recovers_after_exception) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        if (calls == 1) {
          throw std::runtime_error{"transport failure"};
        }
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  // The first fetch throws and denies, leaving the provider usable
  const auto first{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(first.has_value());
  EXPECT_EQ(first.value(), sourcemeta::core::JWTVerificationError::UnknownKey);

  // The next call obtains keys and verifies, proving no state was corrupted
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, fetcher_exception_serves_stale_keys) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        if (calls == 1) {
          return sourcemeta::core::JWKSProvider::FetchResult{
              std::string{SIGNED_KEYS}, std::nullopt};
        }
        throw std::runtime_error{"transport failure"};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Past the fallback lifetime the refresh throws, so the retained keys are
  // still served rather than failing closed
  now = std::chrono::system_clock::from_time_t(1000000000 + 7200);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, empty_key_set_is_treated_as_failure) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{R"({ "keys": [] })",
                                                           std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(JOSE_jwks_provider, disallowed_algorithm_denies) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  // The token's signing algorithm is absent from the allow-list
  const std::array<sourcemeta::core::JWSAlgorithm, 1> allowed{
      {sourcemeta::core::JWSAlgorithm::ES256}};
  const auto error{provider.verify(token.value(), allowed, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(),
            sourcemeta::core::JWTVerificationError::AlgorithmNotAllowed);
  // A rejected algorithm is final, so there is no unknown-key refetch
  EXPECT_EQ(calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, expired_token_denies) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  // One second past the token's expiry of 2000000000
  auto now{std::chrono::system_clock::from_time_t(2000000001)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Expiration);
}

TEST(JOSE_jwks_provider, wrong_audience_denies) {
  const auto fetcher{
      [](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::nullopt};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  sourcemeta::core::JWKSProvider provider{
      "https://issuer.test/jwks", fetcher,
      sourcemeta::core::JWKSProvider::Options{}, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "unexpected")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Audience);
}

TEST(JOSE_jwks_provider, inverted_lifetime_bounds_stay_well_defined) {
  std::size_t calls{0};
  const auto fetcher{
      [&calls](const std::string_view)
          -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
        calls += 1;
        return sourcemeta::core::JWKSProvider::FetchResult{
            std::string{SIGNED_KEYS}, std::chrono::hours{1}};
      }};
  auto now{std::chrono::system_clock::from_time_t(1000000000)};
  // A misconfiguration with the minimum above the maximum must not be undefined
  // behaviour; the minimum wins, so the advertised one hour becomes one day
  sourcemeta::core::JWKSProvider::Options options;
  options.minimum_ttl = std::chrono::hours{24};
  options.maximum_ttl = std::chrono::minutes{5};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks", fetcher,
                                          options, [&now] { return now; }};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Still fresh one second before the minimum elapses
  now = std::chrono::system_clock::from_time_t(1000000000 + 86399);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{1});

  // Stale at exactly the minimum
  now = std::chrono::system_clock::from_time_t(1000000000 + 86400);
  EXPECT_FALSE(provider.verify(token.value(), ALLOWED_RS256, "acme", "client")
                   .has_value());
  EXPECT_EQ(calls, std::size_t{2});
}
