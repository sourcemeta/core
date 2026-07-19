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
