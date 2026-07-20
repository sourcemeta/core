#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

TEST(client_secret_basic_encodes_the_credential) {
  sourcemeta::core::SecureString header;
  sourcemeta::core::oauth_client_secret_basic("s6BhdRkqt3", "gX1fBat3bV",
                                              header);
  EXPECT_TRUE(header == "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW");
}

TEST(client_secret_basic_percent_encodes_each_half) {
  // RFC 6749 Section 2.3.1: the colon in the identifier and the space in the
  // secret are percent-encoded before the halves are joined and Base64 encoded
  sourcemeta::core::SecureString header;
  sourcemeta::core::oauth_client_secret_basic("a:b", "c d", header);
  EXPECT_TRUE(header == "Basic YSUzQWI6YyUyMGQ=");
}

TEST(client_secret_post_emits_both_parameters) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_client_secret_post("s6BhdRkqt3", "gX1fBat3bV", body);
  EXPECT_TRUE(body == "client_id=s6BhdRkqt3&client_secret=gX1fBat3bV");
}

TEST(client_secret_post_escapes_the_secret) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_client_secret_post("abc", "s p@ce", body);
  EXPECT_TRUE(body == "client_id=abc&client_secret=s%20p%40ce");
}

TEST(client_id_only_emits_the_identifier) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_client_id_only("s6BhdRkqt3", body);
  EXPECT_TRUE(body == "client_id=s6BhdRkqt3");
}

TEST(client_id_only_composes_after_a_grant) {
  sourcemeta::core::SecureString body;
  sourcemeta::core::oauth_build_token_request_code("abc", "", "verifier", {},
                                                   body);
  sourcemeta::core::oauth_client_id_only("s6BhdRkqt3", body);
  EXPECT_TRUE(body == "grant_type=authorization_code&code=abc"
                      "&code_verifier=verifier&client_id=s6BhdRkqt3");
}
