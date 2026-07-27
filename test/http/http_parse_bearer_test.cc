#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

TEST(plain_token) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer abc123"), "abc123");
}

TEST(scheme_lowercase) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("bearer abc123"), "abc123");
}

TEST(scheme_uppercase) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("BEARER abc123"), "abc123");
}

TEST(scheme_mixed_case) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("BeArEr abc123"), "abc123");
}

TEST(trailing_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer abc123   "), "abc123");
}

TEST(leading_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer    abc123"), "abc123");
}

TEST(surrounding_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer   abc123   "),
            "abc123");
}

TEST(tab_whitespace_trimmed) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer \tabc123\t"), "abc123");
}

TEST(empty_header) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("").empty());
}

TEST(other_scheme) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Basic abc123").empty());
}

TEST(scheme_only) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer").empty());
}

TEST(scheme_with_space_but_no_token) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ").empty());
}

TEST(scheme_with_only_whitespace) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer   ").empty());
}

TEST(scheme_without_separating_space) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearerabc123").empty());
}

TEST(internal_space_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc def").empty());
}

TEST(internal_tab_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc\tdef").empty());
}

TEST(invalid_character_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer abc@def").empty());
}

TEST(control_character_rejected) {
  EXPECT_TRUE(
      sourcemeta::core::http_parse_bearer("Bearer abc\x01zdef").empty());
}

TEST(b64token_alphabet_accepted) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer aZ09-._~+/"),
            "aZ09-._~+/");
}

TEST(base64_padding_accepted) {
  EXPECT_EQ(sourcemeta::core::http_parse_bearer("Bearer YWJjMTIz=="),
            "YWJjMTIz==");
}

TEST(jwt_like_accepted) {
  EXPECT_EQ(
      sourcemeta::core::http_parse_bearer("Bearer eyJhbGc.eyJzdWIi.SflKxwRJ"),
      "eyJhbGc.eyJzdWIi.SflKxwRJ");
}

TEST(only_padding_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ===").empty());
}

TEST(padding_before_alphabet_rejected) {
  EXPECT_TRUE(sourcemeta::core::http_parse_bearer("Bearer ab=cd").empty());
}
