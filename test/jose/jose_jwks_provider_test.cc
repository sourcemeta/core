#include <gtest/gtest.h>

#include <sourcemeta/core/jose.h>

#include <array>       // std::array
#include <chrono>      // std::chrono
#include <cstddef>     // std::size_t
#include <memory>      // std::make_shared, std::shared_ptr
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

// A scripted fetcher whose responses are consumed in order, repeating the last
// entry once exhausted, recording how many times it was invoked. The provider
// holds the fetcher by value through a `std::function`, so the call count is
// shared through a `std::shared_ptr` and read back by the assertions
struct ScriptedFetcher {
  std::vector<std::optional<sourcemeta::core::JWKSProvider::FetchResult>>
      responses;
  std::size_t calls{0};

  auto operator()(const std::string_view)
      -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
    const auto index{this->calls < this->responses.size()
                         ? this->calls
                         : this->responses.size() - 1};
    this->calls += 1;
    return this->responses.at(index);
  }
};

auto make_fetcher(std::shared_ptr<ScriptedFetcher> state)
    -> sourcemeta::core::JWKSProvider::Fetcher {
  return [state](const std::string_view url) { return (*state)(url); };
}

auto body(const std::string_view keys,
          const std::optional<std::chrono::seconds> max_age = std::nullopt)
    -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
  return sourcemeta::core::JWKSProvider::FetchResult{std::string{keys},
                                                     max_age};
}

// Locally-signed RS256 token, header typ "at+jwt", iss "acme", aud "client",
// exp 2000000000, and its matching key set (from jose_jwt_verify_test.cc)
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

// RFC 7515 A.2 token re-headed with kid "k1" (its signature therefore no longer
// matches the re-headed signing input), plus kid-bearing key sets
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

// An EC-only set, which cannot serve an RS256 token, so a no-kid token verified
// against it reports UnknownKey (no compatible key)
constexpr std::string_view ELLIPTIC_CURVE_KEYS{
    R"JSON({ "keys": [ { "kty": "EC", "crv": "P-256", "x": "f83OJ3D2xF1Bg8vub9tLe1gHMzV76e8Tus9uPHvRVEU", "y": "x_FEzRu9m36HLN_tue659LNpXW6pCyStikYjKIWI5a0" } ] })JSON"};

constexpr std::array<sourcemeta::core::JWSAlgorithm, 1> ALLOWED_RS256{
    {sourcemeta::core::JWSAlgorithm::RS256}};

// Drives a provider whose first fetch advertises `max_age`, asserts the cached
// set is still served one second before the resulting TTL elapses, and
// refetches at exactly the TTL boundary
auto assert_refetch_boundary(const std::optional<std::chrono::seconds> max_age,
                             const std::chrono::seconds expected_ttl) -> void {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {body(SIGNED_KEYS, max_age)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());
  constexpr auto base{1000000000};

  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(base))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{1});

  EXPECT_FALSE(
      provider
          .verify(token.value(), ALLOWED_RS256, "acme", "client",
                  std::chrono::system_clock::from_time_t(
                      base + static_cast<int>(expected_ttl.count()) - 1))
          .has_value());
  EXPECT_EQ(state->calls, std::size_t{1});

  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(
                               base + static_cast<int>(expected_ttl.count())))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{2});
}

} // namespace

TEST(JOSE_jwks_provider, unreachable_issuer_denies) {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {std::nullopt};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client",
                      std::chrono::system_clock::from_time_t(1000000000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_GE(state->calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, malformed_body_is_treated_as_failure) {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {body("not json at all")};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client",
                      std::chrono::system_clock::from_time_t(1000000000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
}

TEST(JOSE_jwks_provider, cache_hit_does_not_refetch) {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {body(SIGNED_KEYS)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(1000000000))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{1});

  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(1000000000))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, cache_control_absent_uses_fallback) {
  assert_refetch_boundary(std::nullopt, std::chrono::hours{1});
}

TEST(JOSE_jwks_provider, cache_control_above_maximum_clamps_down) {
  assert_refetch_boundary(std::chrono::hours{48}, std::chrono::hours{24});
}

TEST(JOSE_jwks_provider, cache_control_below_minimum_clamps_up) {
  assert_refetch_boundary(std::chrono::seconds{60}, std::chrono::minutes{5});
}

TEST(JOSE_jwks_provider, cache_control_zero_clamps_to_minimum) {
  assert_refetch_boundary(std::chrono::seconds{0}, std::chrono::minutes{5});
}

TEST(JOSE_jwks_provider, unknown_key_triggers_one_guarded_refetch) {
  auto state{std::make_shared<ScriptedFetcher>()};
  // The issuer first publishes a set with no key able to verify the token, then
  // after rotation publishes the matching key
  state->responses = {body(ELLIPTIC_CURVE_KEYS), body(SIGNED_KEYS)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "acme", "client",
                      std::chrono::system_clock::from_time_t(1000000000))};
  EXPECT_FALSE(error.has_value());
  EXPECT_EQ(state->calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, signature_failure_is_not_retried) {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {body(RSA_KEYS_KEY_ID_K1)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  // This token names "k1" and the set contains "k1", but the re-headed token's
  // signature does not match, so jwt_verify returns Signature, not UnknownKey
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  ASSERT_TRUE(token.has_value());

  const auto error{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client",
                      std::chrono::system_clock::from_time_t(1300000000))};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Signature);
  EXPECT_EQ(state->calls, std::size_t{1});
}

TEST(JOSE_jwks_provider, unknown_key_refetch_is_bounded_by_cooldown) {
  auto state{std::make_shared<ScriptedFetcher>()};
  // The published set never contains "k1", so every verify hits UnknownKey
  state->responses = {body(RSA_KEYS_KEY_ID_OTHER)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(RS256_TOKEN_WITH_KEY_ID)};
  ASSERT_TRUE(token.has_value());

  // Cold fetch (1) then one guarded refetch (2); the kid is still absent
  const auto first{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client",
                      std::chrono::system_clock::from_time_t(1300000000))};
  ASSERT_TRUE(first.has_value());
  EXPECT_EQ(first.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(state->calls, std::size_t{2});

  // Within the cooldown window no further refetch happens
  const auto second{
      provider.verify(token.value(), ALLOWED_RS256, "joe", "client",
                      std::chrono::system_clock::from_time_t(1300000000))};
  ASSERT_TRUE(second.has_value());
  EXPECT_EQ(second.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(state->calls, std::size_t{2});

  // After the cooldown elapses a new guarded refetch is allowed
  const auto third{provider.verify(
      token.value(), ALLOWED_RS256, "joe", "client",
      std::chrono::system_clock::from_time_t(1300000000 + 301))};
  ASSERT_TRUE(third.has_value());
  EXPECT_EQ(third.value(), sourcemeta::core::JWTVerificationError::UnknownKey);
  EXPECT_EQ(state->calls, std::size_t{3});
}

TEST(JOSE_jwks_provider, failed_refresh_serves_stale_keys) {
  auto state{std::make_shared<ScriptedFetcher>()};
  // Good keys first, then the issuer becomes unreachable on every later fetch
  state->responses = {body(SIGNED_KEYS), std::nullopt};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(1000000000))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{1});

  // Past the 1h fallback TTL the refresh is attempted and fails, so the
  // retained keys still verify the token
  EXPECT_FALSE(
      provider
          .verify(token.value(), ALLOWED_RS256, "acme", "client",
                  std::chrono::system_clock::from_time_t(1000000000 + 7200))
          .has_value());
  EXPECT_EQ(state->calls, std::size_t{2});

  // A failed refresh backs off by minimum_ttl, so an immediate retry does not
  // hammer the unreachable issuer
  EXPECT_FALSE(provider
                   .verify(token.value(), ALLOWED_RS256, "acme", "client",
                           std::chrono::system_clock::from_time_t(1000000000 +
                                                                  7200 + 60))
                   .has_value());
  EXPECT_EQ(state->calls, std::size_t{2});
}

TEST(JOSE_jwks_provider, expected_type_is_forwarded) {
  auto state{std::make_shared<ScriptedFetcher>()};
  state->responses = {body(SIGNED_KEYS)};
  sourcemeta::core::JWKSProvider provider{"https://issuer.test/jwks",
                                          make_fetcher(state)};
  const auto token{sourcemeta::core::JWT::from(SIGNED_TOKEN)};
  ASSERT_TRUE(token.has_value());

  // SIGNED_TOKEN carries typ "at+jwt", so pinning a different type is rejected,
  // proving the expected_type argument reaches jwt_verify
  const auto error{provider.verify(
      token.value(), ALLOWED_RS256, "acme", "client",
      std::chrono::system_clock::from_time_t(1000000000),
      std::chrono::seconds{0}, std::nullopt, "application/example")};
  ASSERT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), sourcemeta::core::JWTVerificationError::Type);
}
