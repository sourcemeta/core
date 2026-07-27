#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
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
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_finds_a_bare_token_value) {
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      "Bearer error=invalid_token", "Bearer", "error")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "invalid_token");
}

TEST(challenge_parameter_finds_a_later_parameter_of_the_same_challenge) {
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", error="invalid_token")", "Bearer", "error")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "invalid_token");
}

TEST(challenge_parameter_matches_the_scheme_and_name_case_insensitively) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(bearer REALM="example")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_tolerates_bad_whitespace_around_equals) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm  =  "example")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_selects_the_named_scheme) {
  const auto algs{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", DPoP algs="ES256 PS256")", "DPoP", "algs")};
  EXPECT_TRUE(algs.has_value());
  EXPECT_EQ(algs.value(), "ES256 PS256");
}

TEST(challenge_parameter_does_not_cross_schemes) {
  // realm belongs to Bearer, not to DPoP, so a lookup on DPoP does not find it
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="example", DPoP algs="ES256")", "DPoP", "realm")};
  EXPECT_FALSE(realm.has_value());
}

TEST(challenge_parameter_handles_an_empty_first_challenge) {
  // RFC 9449 Figure 17: a schemes-only Bearer challenge alongside a DPoP one
  const auto algs{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer, DPoP algs="ES256 PS256")", "DPoP", "algs")};
  EXPECT_TRUE(algs.has_value());
  EXPECT_EQ(algs.value(), "ES256 PS256");
}

TEST(challenge_parameter_keeps_a_comma_inside_a_quoted_value) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a,b")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "a,b");
}

TEST(challenge_parameter_unescapes_a_quoted_pair) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="say \"hi\"")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), R"(say "hi")");
}

TEST(challenge_parameter_tolerates_empty_list_elements) {
  const auto point{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="x",, DPoP algs="ES256")", "DPoP", "algs")};
  EXPECT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), "ES256");
}

TEST(challenge_parameter_skips_a_token68_challenge) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate YWJjZGVm==, Bearer realm="example")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "example");
}

TEST(challenge_parameter_finds_a_resource_metadata_url) {
  const auto url{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer resource_metadata="https://rs.example/.well-known/x")",
      "Bearer", "resource_metadata")};
  EXPECT_TRUE(url.has_value());
  EXPECT_EQ(url.value(), "https://rs.example/.well-known/x");
}

TEST(challenge_parameter_returns_nothing_for_an_absent_parameter) {
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example")", "Bearer", "error")
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_an_absent_scheme) {
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example")", "DPoP", "algs")
                   .has_value());
}

TEST(challenge_parameter_rejects_an_unterminated_quoted_string) {
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="example)", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_parses_the_rfc7235_example) {
  // RFC 7235 Section 4.1 example, adapted with the standard schemes
  static constexpr std::string_view header{
      R"(Newauth realm="apps", type=1, title="Login", Basic realm="simple")"};
  EXPECT_EQ(
      sourcemeta::core::oauth_challenge_parameter(header, "Newauth", "realm")
          .value(),
      "apps");
  EXPECT_EQ(
      sourcemeta::core::oauth_challenge_parameter(header, "Newauth", "type")
          .value(),
      "1");
  EXPECT_EQ(
      sourcemeta::core::oauth_challenge_parameter(header, "Newauth", "title")
          .value(),
      "Login");
  EXPECT_EQ(
      sourcemeta::core::oauth_challenge_parameter(header, "Basic", "realm")
          .value(),
      "simple");
}

TEST(challenge_parameter_survives_earlier_escaped_values_in_the_arena) {
  // Two escaped parameters are captured into the arena before the match, so the
  // returned value must remain correct after those appends
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer a="\A", b="\B", realm="\Z")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "Z");
}

TEST(challenge_parameter_finds_a_later_challenge_of_the_same_scheme) {
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a", Bearer error="b")", "Bearer", "error")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "b");
}

TEST(challenge_parameter_keeps_equals_and_colon_in_a_quoted_value) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a=b:c")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "a=b:c");
}

TEST(challenge_parameter_keeps_obs_text_in_a_quoted_value) {
  std::string header{R"(Bearer realm=")"};
  header.push_back('\x80');
  header.append(R"(")");
  const auto realm{
      sourcemeta::core::oauth_challenge_parameter(header, "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value().size(), 1);
  EXPECT_EQ(static_cast<unsigned char>(realm.value().front()), 0x80);
}

TEST(challenge_parameter_skips_an_unpadded_token68_challenge) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate abcdef, Bearer realm="x")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_skips_a_token68_with_plus_and_slash) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Negotiate a/b+c==, Bearer realm="x")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_rejects_a_padding_only_blob) {
  // "==" is not a valid token68 (it lacks the required alphabet character), so
  // the header is malformed
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Foo ==, Bearer realm="x")", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_rejects_a_parameter_after_a_token68) {
  // A token68 form of a challenge cannot also carry parameters (RFC 7235
  // Section 2.1), so the trailing parameter makes the header malformed
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer foo, resource_metadata="https://x")", "Bearer",
                   "resource_metadata")
                   .has_value());
}

TEST(challenge_parameter_rejects_a_parameter_before_any_scheme) {
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(R"(realm="x")",
                                                           "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_rejects_an_empty_bare_value) {
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   "Bearer realm=", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_an_empty_header) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_challenge_parameter("", "Bearer", "realm")
          .has_value());
}

TEST(challenge_parameter_returns_nothing_for_a_whitespace_header) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_challenge_parameter("   ", "Bearer", "realm")
          .has_value());
}

TEST(challenge_parameter_tolerates_leading_and_trailing_commas) {
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(,,Bearer realm="x",,)", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_ignores_malformation_after_the_match) {
  // The lookup returns as soon as the parameter is found, so trailing garbage
  // after the matched value does not change the result
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="x" garbage)", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "x");
}

TEST(challenge_parameter_finds_an_empty_quoted_value) {
  // RFC 9110 Section 5.6.4: a quoted-string may be empty, so an empty value is
  // present and distinct from an absent parameter
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_TRUE(realm.value().empty());
}

TEST(challenge_parameter_rejects_a_parameter_after_a_comma_only_challenge) {
  // The comma ends the empty Bearer challenge, so realm cannot attach to it and
  // there is no scheme for it to belong to (RFC 9110 Section 5.6.1)
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer, realm="x")", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_rejects_a_missing_comma_between_parameters) {
  // RFC 9110 Section 5.6.1: parameters are comma separated, so a second
  // parameter with no comma before it makes the header malformed
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="x" error="y")", "Bearer", "error")
                   .has_value());
}

TEST(challenge_parameter_rejects_a_missing_comma_between_challenges) {
  // A second challenge with no comma before it makes the header malformed, so a
  // lookup that must reach it finds nothing
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="x" DPoP algs="y")", "DPoP", "algs")
                   .has_value());
}

TEST(challenge_parameter_rejects_an_escaped_closing_quote) {
  // RFC 9110 Section 5.6.4: the backslash forms a quoted-pair with the final
  // quote, so the quoted-string is never closed and the header is malformed
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   "Bearer realm=\"abc\\\"", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_keeps_a_literal_space_in_a_quoted_value) {
  // RFC 9110 Section 5.6.4: qdtext admits a bare SP, so a space inside the
  // quotes is part of the value rather than a delimiter
  const auto realm{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="a b")", "Bearer", "realm")};
  EXPECT_TRUE(realm.has_value());
  EXPECT_EQ(realm.value(), "a b");
}

TEST(challenge_parameter_finds_a_bare_value_with_token_punctuation) {
  // RFC 9110 Section 5.6.2: "!" and "#" are tchar, so they belong to a bare
  // token value rather than ending it
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      "Bearer error=a!b#c", "Bearer", "error")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "a!b#c");
}

TEST(challenge_parameter_skips_a_trailing_token68_challenge) {
  // RFC 7235 Section 2.1: a token68 challenge that ends the header is skipped
  // whole, so a lookup that reaches past it simply finds nothing
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer realm="x", Negotiate YWJj==)", "Bearer", "error")
                   .has_value());
}

TEST(challenge_parameter_tolerates_whitespace_before_a_separating_comma) {
  // RFC 9110 Section 5.6.1: optional whitespace may surround the comma, so a
  // space between an empty challenge and the comma is harmless
  const auto algs{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer , DPoP algs="ES256")", "DPoP", "algs")};
  EXPECT_TRUE(algs.has_value());
  EXPECT_EQ(algs.value(), "ES256");
}

TEST(challenge_parameter_tolerates_whitespace_around_a_separating_comma) {
  // RFC 9110 Section 5.6.1: optional whitespace may surround the comma between
  // two parameters of the same challenge
  const auto error{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer realm="x" , error="y")", "Bearer", "error")};
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error.value(), "y");
}

TEST(challenge_parameter_treats_a_second_scheme_without_a_comma_as_a_scheme) {
  // RFC 7235 Section 4.1: a token that is not followed by "=" is a new scheme
  // rather than a parameter, so the parameter binds to the second scheme
  const auto basic{sourcemeta::core::oauth_challenge_parameter(
      R"(Bearer Basic realm="x")", "Basic", "realm")};
  EXPECT_TRUE(basic.has_value());
  EXPECT_EQ(basic.value(), "x");
  EXPECT_FALSE(sourcemeta::core::oauth_challenge_parameter(
                   R"(Bearer Basic realm="x")", "Bearer", "realm")
                   .has_value());
}

TEST(challenge_parameter_returns_nothing_for_a_scheme_only_header) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_challenge_parameter("Bearer", "Bearer", "realm")
          .has_value());
}

TEST(challenge_parameter_returns_nothing_for_a_comma_only_header) {
  EXPECT_FALSE(
      sourcemeta::core::oauth_challenge_parameter(" , , ", "Bearer", "realm")
          .has_value());
}

TEST(build_challenge_bearer_realm_and_error) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  challenge.error = "invalid_token";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(header, R"(Bearer realm="example", error="invalid_token")");
}

TEST(build_challenge_full_bearer) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  challenge.error = "invalid_token";
  challenge.error_description = "The access token expired";
  challenge.error_uri = "https://server.example/errors/token";
  challenge.resource_metadata =
      "https://resource.example/.well-known/oauth-protected-resource";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(
      header,
      R"(Bearer realm="example", error="invalid_token", )"
      R"(error_description="The access token expired", )"
      R"(error_uri="https://server.example/errors/token", )"
      R"(resource_metadata="https://resource.example/.well-known/oauth-protected-resource")");
}

TEST(build_challenge_dpop_with_algs) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.error = "invalid_token";
  challenge.algs = "ES256 PS256";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("DPoP", challenge, header));
  EXPECT_EQ(header, R"(DPoP error="invalid_token", algs="ES256 PS256")");
}

TEST(build_challenge_scope_is_quoted) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.scope = "read write";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(header, R"(Bearer scope="read write")");
}

TEST(build_challenge_escapes_quote_and_backslash) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = R"(say "hi" a\b)";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(header, R"(Bearer realm="say \"hi\" a\\b")");
}

TEST(build_challenge_rejects_a_control_character) {
  sourcemeta::core::OAuthChallenge challenge;
  std::string realm{"a"};
  realm.push_back('\r');
  realm.push_back('\n');
  realm.append("b");
  challenge.realm = realm;
  std::string header;
  EXPECT_FALSE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_TRUE(header.empty());
}

TEST(build_challenge_emits_a_bare_scheme_for_an_empty_challenge) {
  // RFC 9449 Section 7.1: a DPoP challenge may carry no parameters, so a bare
  // scheme is a valid challenge rather than a rejected one
  const sourcemeta::core::OAuthChallenge challenge;
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("DPoP", challenge, header));
  EXPECT_EQ(header, "DPoP");
}

TEST(build_challenge_emits_a_bare_scheme_into_an_existing_sink) {
  const sourcemeta::core::OAuthChallenge challenge;
  std::string header{"Bearer realm=\"x\", "};
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("DPoP", challenge, header));
  EXPECT_EQ(header, R"(Bearer realm="x", DPoP)");
}

TEST(build_challenge_rejects_a_non_token_scheme) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  std::string header;
  EXPECT_FALSE(
      sourcemeta::core::oauth_build_challenge("Bea rer", challenge, header));
  EXPECT_TRUE(header.empty());
}

TEST(build_challenge_appends_to_an_existing_sink) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  std::string header{"WWW-Authenticate: "};
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(header, R"(WWW-Authenticate: Bearer realm="example")");
}

TEST(build_challenge_round_trips_through_the_parser) {
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  challenge.resource_metadata = "https://resource.example/.well-known/x";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  const auto resource_metadata{sourcemeta::core::oauth_challenge_parameter(
      header, "Bearer", "resource_metadata")};
  EXPECT_TRUE(resource_metadata.has_value());
  EXPECT_EQ(resource_metadata.value(),
            "https://resource.example/.well-known/x");
}

TEST(has_audience_matches_a_single_string) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": "https://api.example.com"
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_a_different_single_string) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": "https://api.example.com"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_has_audience(
      claims, "https://other.example.com"));
}

TEST(has_audience_matches_a_member_of_an_array) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": [ "https://a.example.com", "https://api.example.com" ]
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_a_non_member_of_an_array) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": [ "https://a.example.com", "https://b.example.com" ]
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_when_the_claim_is_absent) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "sub": "user"
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_when_the_claims_are_not_an_object) {
  const auto claims{sourcemeta::core::parse_json(R"JSON([ "aud" ])JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_compares_by_code_points) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": "https://api.example.com/"
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(is_dpop_bound_detects_a_confirmation_thumbprint) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": { "jkt": "0ZcOCORZNYy-DWpqq30jZyJGHTN0d2HglBV3uiguA4I" }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_a_missing_confirmation) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "sub": "user"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_a_confirmation_without_a_thumbprint) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": { "x5t#S256": "abc" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_a_non_string_thumbprint) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": { "jkt": 42 }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_a_non_object_confirmation) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": "opaque"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_when_the_claims_are_not_an_object) {
  const auto claims{sourcemeta::core::parse_json(R"JSON("cnf")JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(build_challenge_rejects_when_a_later_value_is_invalid) {
  // Every value is validated before anything is appended, so a valid realm
  // does not leak into the sink when a later value is rejected
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = "example";
  std::string algs{"ES256"};
  algs.push_back('\n');
  challenge.algs = algs;
  std::string header;
  EXPECT_FALSE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_TRUE(header.empty());
}

TEST(build_challenge_emits_a_whitespace_only_value) {
  // A single space is not empty and is a valid quoted-string byte, so it is a
  // present parameter rather than an omitted one
  sourcemeta::core::OAuthChallenge challenge;
  challenge.realm = " ";
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  EXPECT_EQ(header, R"(Bearer realm=" ")");
}

TEST(build_challenge_allows_obs_text_in_a_value) {
  // RFC 9110 Section 5.6.4: obs-text is admitted in a quoted-string, so a byte
  // above 0x7F is emitted verbatim rather than rejected
  sourcemeta::core::OAuthChallenge challenge;
  std::string realm{"a"};
  realm.push_back('\x80');
  challenge.realm = realm;
  std::string header;
  EXPECT_TRUE(
      sourcemeta::core::oauth_build_challenge("Bearer", challenge, header));
  std::string expected{R"(Bearer realm="a)"};
  expected.push_back('\x80');
  expected.push_back('"');
  EXPECT_EQ(header, expected);
}

TEST(has_audience_matches_despite_a_non_string_array_element) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": [ 42, "https://api.example.com" ]
  })JSON")};
  EXPECT_TRUE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_an_empty_array) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": []
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_a_numeric_claim) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": 42
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_an_object_claim) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": { "value": "https://api.example.com" }
  })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::oauth_has_audience(claims, "https://api.example.com"));
}

TEST(has_audience_rejects_an_empty_audience_argument) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "aud": ""
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_has_audience(claims, ""));
}

TEST(is_dpop_bound_treats_an_empty_thumbprint_as_present) {
  // The check is presence and type only, so an empty confirmation thumbprint
  // still counts as DPoP-bound
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": { "jkt": "" }
  })JSON")};
  EXPECT_TRUE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_an_array_confirmation) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": [ "jkt" ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}

TEST(is_dpop_bound_rejects_an_empty_confirmation) {
  const auto claims{sourcemeta::core::parse_json(R"JSON({
    "cnf": {}
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oauth_is_dpop_bound(claims));
}
