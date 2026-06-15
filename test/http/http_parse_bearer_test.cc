#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_parse_bearer, plain_token) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer abc123"), "abc123");
}

TEST(HTTP_parse_bearer, scheme_lowercase) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("bearer abc123"), "abc123");
}

TEST(HTTP_parse_bearer, scheme_uppercase) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("BEARER abc123"), "abc123");
}

TEST(HTTP_parse_bearer, scheme_mixed_case) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("BeArEr abc123"), "abc123");
}

TEST(HTTP_parse_bearer, trailing_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer abc123   "), "abc123");
}

TEST(HTTP_parse_bearer, leading_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer    abc123"), "abc123");
}

TEST(HTTP_parse_bearer, surrounding_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer   abc123   "),
            "abc123");
}

TEST(HTTP_parse_bearer, tab_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer \tabc123\t"), "abc123");
}

TEST(HTTP_parse_bearer, empty_header) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("").empty());
}

TEST(HTTP_parse_bearer, other_scheme) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Basic abc123").empty());
}

TEST(HTTP_parse_bearer, scheme_only) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer").empty());
}

TEST(HTTP_parse_bearer, scheme_with_space_but_no_token) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ").empty());
}

TEST(HTTP_parse_bearer, scheme_with_only_whitespace) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer   ").empty());
}

TEST(HTTP_parse_bearer, scheme_without_separating_space) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearerabc123").empty());
}

TEST(HTTP_parse_bearer, internal_space_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc def").empty());
}

TEST(HTTP_parse_bearer, internal_tab_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc\tdef").empty());
}

TEST(HTTP_parse_bearer, invalid_character_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc@def").empty());
}

TEST(HTTP_parse_bearer, control_character_rejected) {
  EXPECT_TRUE(
      sourcemeta::core::http_parse_bearer("Bearer abc\x01zdef").empty());
}

TEST(HTTP_parse_bearer, b64token_alphabet_accepted) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer aZ09-._~+/"),
            "aZ09-._~+/");
}

TEST(HTTP_parse_bearer, base64_padding_accepted) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer YWJjMTIz=="),
            "YWJjMTIz==");
}

TEST(HTTP_parse_bearer, jwt_like_accepted) {
  EXPECT_EQ(
      sourcemeta::core::http_parse_bearer("Bearer eyJhbGc.eyJzdWIi.SflKxwRJ"),
      "eyJhbGc.eyJzdWIi.SflKxwRJ");
}

TEST(HTTP_parse_bearer, only_padding_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ===").empty());
}

TEST(HTTP_parse_bearer, padding_before_alphabet_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ab=cd").empty());
}
