#include <sourcemeta/core/http.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(build_a_bearer_header) {
  std::string header;
  EXPECT_TRUE(sourcemeta::core::oauth_bearer_header("mF_9.B5f-4.1JqM", header));
  EXPECT_EQ(header, "Bearer mF_9.B5f-4.1JqM");
}

TEST(build_appends_to_the_sink_without_clearing) {
  std::string header{"prefix:"};
  EXPECT_TRUE(sourcemeta::core::oauth_bearer_header("abc123", header));
  EXPECT_EQ(header, "prefix:Bearer abc123");
}

TEST(build_accepts_base64_padding) {
  std::string header;
  EXPECT_TRUE(sourcemeta::core::oauth_bearer_header("YWJj==", header));
  EXPECT_EQ(header, "Bearer YWJj==");
}

TEST(build_rejects_a_token_with_a_space) {
  std::string header;
  EXPECT_FALSE(sourcemeta::core::oauth_bearer_header("has space", header));
  EXPECT_TRUE(header.empty());
}

TEST(build_rejects_an_empty_token) {
  std::string header;
  EXPECT_FALSE(sourcemeta::core::oauth_bearer_header("", header));
  EXPECT_TRUE(header.empty());
}

TEST(build_then_http_parse_round_trips) {
  // Token parsing is provided by http_parse_bearer, so the built header must
  // round-trip through it
  std::string header;
  EXPECT_TRUE(sourcemeta::core::oauth_bearer_header("abc123", header));
  EXPECT_EQ(sourcemeta::core::http_parse_bearer(header), "abc123");
}

TEST(challenge_parameter_finds_a_quoted_value) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_finds_a_bare_token_value) {
  std::string storage;
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      "Bearer error=invalid_token", "Bearer", "error", storage)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "invalid_token");
}

TEST(challenge_parameter_finds_a_later_parameter_of_the_same_challenge) {
  std::string storage;
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", error="invalid_token")", "Bearer", "error",
      storage)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "invalid_token");
}

TEST(challenge_parameter_matches_the_scheme_and_name_case_insensitively) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(bearer REALM="example")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_tolerates_bad_whitespace_around_equals) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm  =  "example")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_selects_the_named_scheme) {
  std::string storage;
  const auto algs{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", DPoP algs="ES256 PS256")", "DPoP", "algs",
      storage)};
  EXPECT_TRUE(algs.has_value());
  EXPECT_EQ(algs.value(), "ES256 PS256");
}

TEST(challenge_parameter_does_not_cross_schemes) {
  std::string storage;
  // realm belongs to Bearer, not to DPoP, so a lookup on DPoP does not find it
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", DPoP algs="ES256")", "DPoP", "realm",
      storage)};
  EXPECT_FALSE(realm.has_value());
}

TEST(challenge_parameter_handles_an_empty_first_challenge) {
  std::string storage;
  // RFC 9449 Figure 17: a schemes-only Bearer challenge alongside a DPoP one
  const auto algs{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer, DPoP algs="ES256 PS256")", "DPoP", "algs", storage)};
  EXPECT_TRUE(algs.has_value());
  EXPECT_EQ(algs.value(), "ES256 PS256");
}

TEST(challenge_parameter_keeps_a_comma_inside_a_quoted_value) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a,b")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "a,b");
}

TEST(challenge_parameter_unescapes_a_quoted_pair) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="say \"hi\"")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), R"(say "hi")");
}

TEST(challenge_parameter_tolerates_empty_list_elements) {
  std::string storage;
  const auto point{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="x",, DPoP algs="ES256")", "DPoP", "algs", storage)};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), "ES256");
}

TEST(challenge_parameter_skips_a_token68_challenge) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate YWJjZGVm==, Bearer realm="example")", "Bearer", "realm",
      storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_finds_a_resource_metadata_url) {
  std::string storage;
  const auto url{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer resource_metadata="https://rs.example/.well-known/x")",
      "Bearer", "resource_metadata", storage)};
  EXPECT_TRUE(url.has_value());
  EXPECT_EQ(url.value(), "https://rs.example/.well-known/x");
}

TEST(challenge_parameter_returns_nothing_for_an_absent_parameter) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example")", "Bearer", "error", storage)
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_an_absent_scheme) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example")", "DPoP", "algs", storage)
                   .has_value());
}

TEST(challenge_parameter_rejects_an_unterminated_quoted_string) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example)", "Bearer", "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_parses_the_rfc7235_example) {
  // RFC 7235 Section 4.1 example, adapted with the standard schemes
  static constexpr std::string_view header{
      R"(Newauth realm="apps", type=1, title="Login", Basic realm="simple")"};
  std::string storage;
  EXPECT_EQ(sourcemeta::core::oauth_challenge_parameter(header, "Newauth",
                                                        "realm", storage)
                .value(),
            "apps");
  std::string type_storage;
  EXPECT_EQ(sourcemeta::core::oauth_challenge_parameter(header, "Newauth",
                                                        "type", type_storage)
                .value(),
            "1");
  std::string title_storage;
  EXPECT_EQ(sourcemeta::core::oauth_challenge_parameter(header, "Newauth",
                                                        "title", title_storage)
                .value(),
            "Login");
  std::string basic_storage;
  EXPECT_EQ(sourcemeta::core::oauth_challenge_parameter(header, "Basic",
                                                        "realm", basic_storage)
                .value(),
            "simple");
}

TEST(challenge_parameter_survives_earlier_escaped_values_in_the_arena) {
  // Two escaped parameters are captured into the arena before the match, so the
  // returned view must remain valid after those appends
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer a="\A", b="\B", realm="\Z")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "Z");
}

TEST(challenge_parameter_finds_a_later_challenge_of_the_same_scheme) {
  std::string storage;
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a", Bearer error="b")", "Bearer", "error", storage)};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "b");
}

TEST(challenge_parameter_keeps_equals_and_colon_in_a_quoted_value) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a=b:c")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "a=b:c");
}

TEST(challenge_parameter_keeps_obs_text_in_a_quoted_value) {
  std::string header{R"(Bearer realm=")"};
  header.push_back('\x80');
  header.append(R"(")");
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      header, "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value().size(), 1);
  EXPECT_EQ(static_cast<unsigned char>(realm.value().front()), 0x80);
}

TEST(challenge_parameter_skips_an_unpadded_token68_challenge) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate abcdef, Bearer realm="x")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_skips_a_token68_with_plus_and_slash) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate a/b+c==, Bearer realm="x")", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_rejects_a_padding_only_blob) {
  // "==" is not a valid token68 (it lacks the required alphabet character), so
  // the header is malformed
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Foo ==, Bearer realm="x")", "Bearer", "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_rejects_a_parameter_after_a_token68) {
  // A token68 form of a challenge cannot also carry parameters (RFC 7235
  // Section 2.1), so the trailing parameter makes the header malformed
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer foo, resource_metadata="https://x")", "Bearer",
                   "resource_metadata", storage)
                   .has_value());
}

TEST(challenge_parameter_rejects_a_parameter_before_any_scheme) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(realm="x")", "Bearer", "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_rejects_an_empty_bare_value) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   "Bearer realm=", "Bearer", "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_an_empty_header) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter("", "Bearer",
                                                           "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_a_whitespace_header) {
  std::string storage;
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter("   ", "Bearer",
                                                           "realm", storage)
                   .has_value());
}

TEST(challenge_parameter_tolerates_leading_and_trailing_commas) {
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(,,Bearer realm="x",,)", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_ignores_malformation_after_the_match) {
  // The lookup returns as soon as the parameter is found, so trailing garbage
  // after the matched value does not change the result
  std::string storage;
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="x" garbage)", "Bearer", "realm", storage)};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}
