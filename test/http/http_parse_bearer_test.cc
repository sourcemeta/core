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
