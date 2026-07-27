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

TEST(parse_client_authentication_reads_a_basic_credential) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW", "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
  EXPECT_EQ(credentials.client_secret, "gX1fBat3bV");
}

TEST(parse_client_authentication_matches_the_basic_scheme_case_insensitively) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "basic czZCaGRSa3F0MzpnWDFmQmF0M2JW", "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
}

TEST(parse_client_authentication_percent_decodes_basic_halves) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic YSUyQmI6cyUyRmM=", "", storage, credentials));
  EXPECT_EQ(credentials.client_id, "a+b");
  EXPECT_EQ(credentials.client_secret, "s/c");
}

TEST(parse_client_authentication_rejects_a_basic_without_a_colon) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  // "Zm9v" decodes to "foo", which has no colon separator
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic Zm9v", "", storage, credentials));
}

TEST(parse_client_authentication_rejects_a_malformed_basic_base64) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic not$base64", "", storage, credentials));
}

TEST(parse_client_authentication_reads_a_post_secret) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_id=s6BhdRkqt3&client_secret=gX1fBat3bV", storage,
      credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Post);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
  EXPECT_EQ(credentials.client_secret, "gX1fBat3bV");
}

TEST(parse_client_authentication_reads_a_public_client_id) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_id=s6BhdRkqt3", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Public);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
  EXPECT_TRUE(credentials.client_secret.empty());
}

TEST(parse_client_authentication_reads_an_assertion) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "",
      "client_assertion_type=urn%3Aietf%3Aparams%3Aoauth%3Aclient-assertion-"
      "type"
      "%3Ajwt-bearer&client_assertion=ey.ey.sig",
      storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Assertion);
  EXPECT_EQ(credentials.assertion, "ey.ey.sig");
  EXPECT_EQ(credentials.assertion_type,
            "urn:ietf:params:oauth:client-assertion-type:jwt-bearer");
}

TEST(parse_client_authentication_rejects_a_partial_assertion) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_assertion=ey.ey.sig", storage, credentials));
}

TEST(parse_client_authentication_rejects_multiple_mechanisms) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW", "client_secret=other", storage,
      credentials));
}

TEST(parse_client_authentication_rejects_a_basic_body_id_mismatch) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW", "client_id=different", storage,
      credentials));
}

TEST(parse_client_authentication_allows_a_matching_basic_body_id) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW", "client_id=s6BhdRkqt3", storage,
      credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
}

TEST(parse_client_authentication_reports_no_credentials) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", "grant_type=client_credentials", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::None);
}

TEST(parse_client_authentication_ignores_a_non_basic_scheme) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Bearer sometoken", "client_id=s6BhdRkqt3", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Public);
}

TEST(parse_client_authentication_rejects_a_duplicate_body_parameter) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_id=a&client_id=b", storage, credentials));
}

TEST(parse_client_authentication_basic_with_an_empty_secret) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic aWQ6", "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
  EXPECT_EQ(credentials.client_id, "id");
  EXPECT_TRUE(credentials.client_secret.empty());
}

TEST(parse_client_authentication_basic_with_an_empty_id) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic OnNlY3JldA==", "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
  EXPECT_TRUE(credentials.client_id.empty());
  EXPECT_EQ(credentials.client_secret, "secret");
}

TEST(parse_client_authentication_basic_splits_at_the_first_colon) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic aWQ6c2U6Y3JldA==", "", storage, credentials));
  EXPECT_EQ(credentials.client_id, "id");
  EXPECT_EQ(credentials.client_secret, "se:cret");
}

TEST(parse_client_authentication_rejects_basic_plus_assertion) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW",
      "client_assertion_type=urn%3Aietf%3Aparams%3Aoauth%3Aclient-assertion-"
      "type%3Ajwt-bearer&client_assertion=ey.ey.sig",
      storage, credentials));
}

TEST(parse_client_authentication_rejects_an_assertion_type_without_assertion) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "",
      "client_assertion_type=urn%3Aietf%3Aparams%3Aoauth%3Aclient-assertion-"
      "type%3Ajwt-bearer",
      storage, credentials));
}

TEST(parse_client_authentication_tolerates_multiple_spaces_after_basic) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "Basic   czZCaGRSa3F0MzpnWDFmQmF0M2JW", "", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Basic);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
}

TEST(parse_client_authentication_treats_an_empty_secret_as_no_post) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  // An empty client_secret is treated as omitted (RFC 6749 Section 3.2), so
  // the request is a public client identification, not a Post mechanism
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_id=s6BhdRkqt3&client_secret=", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Public);
}

TEST(parse_client_authentication_rejects_a_secret_without_an_id) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  EXPECT_FALSE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client_secret=gX1fBat3bV", storage, credentials));
}

TEST(parse_client_authentication_decodes_a_percent_encoded_name) {
  sourcemeta::core::SecureString storage;
  sourcemeta::core::OAuthClientCredentials credentials;
  // client%5Fid decodes to client_id
  EXPECT_TRUE(sourcemeta::core::oauth_parse_client_authentication(
      "", "client%5Fid=s6BhdRkqt3", storage, credentials));
  EXPECT_EQ(credentials.method,
            sourcemeta::core::OAuthClientAuthenticationMethod::Public);
  EXPECT_EQ(credentials.client_id, "s6BhdRkqt3");
}
