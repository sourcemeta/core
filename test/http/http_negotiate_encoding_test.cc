#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

TEST(empty_header_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(gzip_only_returns_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(identity_only_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(gzip_and_identity_tiebreak_to_server_preference_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip, identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(gzip_and_identity_tiebreak_to_server_preference_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip, identity", sourcemeta::core::HTTPContentEncoding::Identity)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(identity_q_zero_then_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0, gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(gzip_higher_q_wins) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0.3, gzip;q=0.9",
      sourcemeta::core::HTTPContentEncoding::Identity)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(identity_higher_q_wins) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.3, identity;q=0.9",
      sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(identity_excluded_no_gzip_returns_nullopt) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

TEST(wildcard_excludes_all_returns_nullopt) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

TEST(wildcard_fills_in_for_unlisted_codings) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(x_gzip_alias) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "x-gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(unknown_coding_alone_returns_identity_default) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "br", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(identity_excluded_via_wildcard_no_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0, gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(identity_excluded_via_wildcard_no_explicit_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

// RFC 9110 §12.4.2: a malformed weight is a fail-safe refusal, treated as 0,
// so gzip is refused and the implicit identity encoding is selected
TEST(garbage_q_treated_as_zero) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=abc", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(multiple_gzip_entries_take_max_q) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.1, gzip;q=0.9, identity;q=0.5",
      sourcemeta::core::HTTPContentEncoding::Identity)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(gzip_with_identity_q_zero_via_wildcard) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.5, *;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(only_whitespace_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "   ", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(gzip_explicit_zero_with_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0, identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(case_insensitive_uppercase_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "GZIP", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(case_insensitive_mixed_case_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "GzIp", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(case_insensitive_uppercase_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "IDENTITY", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(case_insensitive_uppercase_x_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "X-GZIP", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(wildcard_zero_then_wildcard_one_takes_higher_q) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0, *;q=1", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(identity_zero_then_identity_one_takes_higher_q) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0, identity;q=1",
      sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(gzip_zero_with_wildcard_one_uses_wildcard) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0, *;q=1", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(q_value_one_no_decimal) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=1", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(q_value_zero_no_decimal_excludes) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(q_value_three_decimal_digits) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.999, identity;q=0.998",
      sourcemeta::core::HTTPContentEncoding::Identity)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(leading_and_trailing_comma_tolerated) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      ",gzip,", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}
