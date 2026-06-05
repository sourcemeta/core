#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_negotiate_encoding, empty_header_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding, gzip_only_returns_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, identity_only_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding,
     gzip_and_identity_tiebreak_to_server_preference_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip, identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding,
     gzip_and_identity_tiebreak_to_server_preference_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip, identity", sourcemeta::core::HTTPContentEncoding::Identity)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding, identity_q_zero_then_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0, gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, gzip_higher_q_wins) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0.3, gzip;q=0.9",
      sourcemeta::core::HTTPContentEncoding::Identity)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, identity_higher_q_wins) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.3, identity;q=0.9",
      sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding, identity_excluded_no_gzip_returns_nullopt) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "identity;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

TEST(HTTP_negotiate_encoding, wildcard_excludes_all_returns_nullopt) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

TEST(HTTP_negotiate_encoding, wildcard_fills_in_for_unlisted_codings) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, x_gzip_alias) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "x-gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, unknown_coding_alone_returns_identity_default) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "br", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding, identity_excluded_via_wildcard_no_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0, gzip", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, identity_excluded_via_wildcard_no_explicit_gzip) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "*;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  EXPECT_FALSE(result.has_value());
}

TEST(HTTP_negotiate_encoding, garbage_q_treated_as_one) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=abc", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, multiple_gzip_entries_take_max_q) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.1, gzip;q=0.9, identity;q=0.5",
      sourcemeta::core::HTTPContentEncoding::Identity)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, gzip_with_identity_q_zero_via_wildcard) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0.5, *;q=0", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::GZIP);
}

TEST(HTTP_negotiate_encoding, only_whitespace_returns_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "   ", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}

TEST(HTTP_negotiate_encoding, gzip_explicit_zero_with_identity) {
  const auto result{sourcemeta::core::http_negotiate_encoding(
      "gzip;q=0, identity", sourcemeta::core::HTTPContentEncoding::GZIP)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::HTTPContentEncoding::Identity);
}
