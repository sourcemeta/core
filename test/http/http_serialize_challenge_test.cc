#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair

namespace {
using Parameter = std::pair<std::string_view, std::string_view>;
} // namespace

// RFC 6750 §3: "WWW-Authenticate: Bearer realm="example""
TEST(serialize_challenge_bearer_with_a_realm) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"example\"");
}

// RFC 6750 §3: the expired access token example, with its three parameters
TEST(serialize_challenge_bearer_with_an_error) {
  const std::array<Parameter, 3> parameters{
      {{"realm", "example"},
       {"error", "invalid_token"},
       {"error_description", "The access token expired"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"example\", error=\"invalid_token\", "
                           "error_description=\"The access token expired\"");
}

// RFC 9728 §5.1: the protected resource metadata URL parameter
TEST(serialize_challenge_bearer_with_resource_metadata) {
  const std::array<Parameter, 1> parameters{
      {{"resource_metadata",
        "https://resource.example.com/.well-known/oauth-protected-resource"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(),
            "Bearer resource_metadata=\"https://resource.example.com/"
            ".well-known/oauth-protected-resource\"");
}

// RFC 9110 §11.5: "a sender MUST only generate the quoted-string syntax", so
// a realm is quoted even though its value would pass as a bare token
TEST(serialize_challenge_quotes_a_token_shaped_realm) {
  const std::array<Parameter, 1> parameters{{{"realm", "registry"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"registry\"");
}

// RFC 9110 §5.6.4: a double quote within the value is escaped rather than
// closing the quoted-string early, which no guard over an already serialized
// parameter list can achieve
TEST(serialize_challenge_escapes_a_quote_within_a_value) {
  const std::array<Parameter, 1> parameters{{{"realm", "a\"b"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"a\\\"b\"");
}

TEST(serialize_challenge_escapes_a_backslash_within_a_value) {
  const std::array<Parameter, 1> parameters{{{"realm", "a\\b"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"a\\\\b\"");
}

// A comma inside a value is content, and quoting keeps it from reading as the
// separator between two challenges
TEST(serialize_challenge_keeps_a_comma_within_a_value) {
  const std::array<Parameter, 1> parameters{{{"realm", "a,b"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"a,b\"");
}

// RFC 9110 §5.6.4: a control character is outside qdtext and quoted-pair, so
// the value cannot be encoded at all
TEST(serialize_challenge_rejects_a_line_break_within_a_value) {
  const std::array<Parameter, 1> parameters{{{"realm", "a\r\nX-Injected: 1"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 9110 §11.2: "each parameter name MUST only occur once per challenge",
// matched case-insensitively
TEST(serialize_challenge_rejects_a_repeated_parameter) {
  const std::array<Parameter, 2> parameters{
      {{"realm", "one"}, {"realm", "two"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_repeated_parameter_differing_in_case) {
  const std::array<Parameter, 2> parameters{
      {{"realm", "one"}, {"REALM", "two"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 9110 §11.1: auth-scheme = token
TEST(serialize_challenge_rejects_an_empty_scheme) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_challenge({.parameters = parameters})
          .has_value());
}

TEST(serialize_challenge_rejects_a_scheme_that_is_not_a_token) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bea rer", .parameters = parameters})
                   .has_value());
}

// RFC 9110 §11.2: auth-param = token BWS "=" BWS ( token / quoted-string )
TEST(serialize_challenge_rejects_a_parameter_name_that_is_not_a_token) {
  const std::array<Parameter, 1> parameters{{{"re alm", "example"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 9110 §11.3: challenge = auth-scheme [ 1*SP ( token68 / #auth-param ) ],
// so a challenge carries one or the other and never both
TEST(serialize_challenge_token68) {
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Negotiate", .token68 = "YIIFQwYGKwYBBQUCoIIFNzCC"})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Negotiate YIIFQwYGKwYBBQUCoIIFNzCC");
}

TEST(serialize_challenge_token68_with_padding) {
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Negotiate", .token68 = "YWJj=="})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Negotiate YWJj==");
}

TEST(serialize_challenge_rejects_a_token68_alongside_parameters) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_challenge(
          {.scheme = "Negotiate", .token68 = "YWJj", .parameters = parameters})
          .has_value());
}

TEST(serialize_challenge_rejects_a_malformed_token68) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Negotiate", .token68 = "a b"})
                   .has_value());
}

// RFC 9110 §11.3: the parameter list is optional, so a bare scheme is a well
// formed challenge for a scheme that does not require one
TEST(serialize_challenge_bare_scheme) {
  const auto value{
      sourcemeta::core::http_serialize_challenge({.scheme = "Negotiate"})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Negotiate");
}

// RFC 6750 §3: "This scheme MUST be followed by one or more auth-param values"
TEST(serialize_challenge_rejects_a_bare_bearer) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge({.scheme = "Bearer"})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_bearer_carrying_a_token68) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .token68 = "YWJj"})
                   .has_value());
}

// RFC 9110 §11.1: the scheme is matched case-insensitively, so the Bearer
// rules apply however it is spelled
TEST(serialize_challenge_rejects_a_bare_bearer_in_any_case) {
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge({.scheme = "bEaReR"})
                   .has_value());
}

// RFC 6750 §3: scope values exclude every octet outside %x21 / %x23-5B /
// %x5D-7E, with %x20 as the delimiter between them
TEST(serialize_challenge_bearer_scope) {
  const std::array<Parameter, 1> parameters{
      {{"scope", "openid profile email"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer scope=\"openid profile email\"");
}

TEST(serialize_challenge_rejects_a_scope_carrying_a_quote) {
  const std::array<Parameter, 1> parameters{{{"scope", "open\"id"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_scope_carrying_a_backslash) {
  const std::array<Parameter, 1> parameters{{{"scope", "open\\id"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 6750 §3: error and error_description exclude every octet outside
// %x20-21 / %x23-5B / %x5D-7E
TEST(serialize_challenge_rejects_an_error_carrying_a_quote) {
  const std::array<Parameter, 1> parameters{{{"error", "invalid\"token"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_error_description_admits_a_space) {
  const std::array<Parameter, 1> parameters{
      {{"error_description", "The access token expired"}}};
  EXPECT_TRUE(sourcemeta::core::http_serialize_challenge(
                  {.scheme = "Bearer", .parameters = parameters})
                  .has_value());
}

// RFC 6750 §3: error_uri excludes every octet outside %x21 / %x23-5B /
// %x5D-7E, the space among them
TEST(serialize_challenge_rejects_an_error_uri_carrying_a_space) {
  const std::array<Parameter, 1> parameters{
      {{"error_uri", "https://example.com/a b"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_error_uri) {
  const std::array<Parameter, 1> parameters{
      {{"error_uri", "https://example.com/errors/invalid_token"}}};
  EXPECT_TRUE(sourcemeta::core::http_serialize_challenge(
                  {.scheme = "Bearer", .parameters = parameters})
                  .has_value());
}

// The scheme-specific rules belong to the scheme, so another one carrying the
// same parameter name is judged on the general grammar alone
TEST(serialize_challenge_scope_rules_do_not_apply_to_another_scheme) {
  const std::array<Parameter, 1> parameters{{{"scope", "open\"id"}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Custom", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Custom scope=\"open\\\"id\"");
}

// RFC 9110 §11.6.1: WWW-Authenticate = #challenge
TEST(serialize_challenges_several) {
  const std::array<Parameter, 1> bearer{{{"realm", "api"}}};
  const std::array<Parameter, 1> basic{{{"realm", "site"}}};
  const std::array<sourcemeta::core::HTTPChallenge, 2> challenges{
      {{.scheme = "Bearer", .parameters = bearer},
       {.scheme = "Basic", .parameters = basic}}};
  const auto value{sourcemeta::core::http_serialize_challenges(challenges)};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer realm=\"api\", Basic realm=\"site\"");
}

TEST(serialize_challenges_rejects_an_empty_list) {
  const std::array<sourcemeta::core::HTTPChallenge, 0> challenges{};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_challenges(challenges).has_value());
}

TEST(serialize_challenges_rejects_a_list_carrying_an_invalid_challenge) {
  const std::array<Parameter, 1> bearer{{{"realm", "api"}}};
  const std::array<sourcemeta::core::HTTPChallenge, 2> challenges{
      {{.scheme = "Bearer", .parameters = bearer}, {.scheme = "Bearer"}}};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_challenges(challenges).has_value());
}

TEST(serialize_challenge_sink_overload_appends) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  std::string buffer{"WWW-Authenticate: "};
  EXPECT_TRUE(sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters}, buffer));
  EXPECT_EQ(buffer, "WWW-Authenticate: Bearer realm=\"example\"");
}

TEST(serialize_challenge_sink_overload_leaves_the_buffer_alone_on_failure) {
  std::string buffer{"WWW-Authenticate: "};
  EXPECT_FALSE(
      sourcemeta::core::http_serialize_challenge({.scheme = "Bearer"}, buffer));
  EXPECT_EQ(buffer, "WWW-Authenticate: ");
}

TEST(challenge_valid_agrees_with_serialization) {
  const std::array<Parameter, 1> parameters{{{"realm", "example"}}};
  EXPECT_TRUE(sourcemeta::core::http_challenge_valid(
      {.scheme = "Bearer", .parameters = parameters}));
  EXPECT_FALSE(sourcemeta::core::http_challenge_valid({.scheme = "Bearer"}));
}

// RFC 6749 Appendix A.4: scope = scope-token *( SP scope-token ), so the space
// only ever stands between two non-empty values
TEST(serialize_challenge_rejects_an_empty_scope) {
  const std::array<Parameter, 1> parameters{{{"scope", ""}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_scope_with_a_leading_space) {
  const std::array<Parameter, 1> parameters{{{"scope", " openid"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_scope_with_a_trailing_space) {
  const std::array<Parameter, 1> parameters{{{"scope", "openid "}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_a_scope_with_consecutive_spaces) {
  const std::array<Parameter, 1> parameters{{{"scope", "openid  profile"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 6749 Appendix A.7 and A.8: both are 1*NQSCHAR
TEST(serialize_challenge_rejects_an_empty_error) {
  const std::array<Parameter, 1> parameters{{{"error", ""}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_rejects_an_empty_error_description) {
  const std::array<Parameter, 1> parameters{{{"error_description", ""}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

// RFC 6749 Appendix A.9: error-uri = URI-reference, which a charset check
// alone does not decide
TEST(serialize_challenge_rejects_an_error_uri_that_is_not_a_uri_reference) {
  const std::array<Parameter, 1> parameters{{{"error_uri", "://bad"}}};
  EXPECT_FALSE(sourcemeta::core::http_serialize_challenge(
                   {.scheme = "Bearer", .parameters = parameters})
                   .has_value());
}

TEST(serialize_challenge_error_uri_relative_reference) {
  const std::array<Parameter, 1> parameters{{{"error_uri", "/errors/token"}}};
  EXPECT_TRUE(sourcemeta::core::http_serialize_challenge(
                  {.scheme = "Bearer", .parameters = parameters})
                  .has_value());
}

// RFC 6749 Appendix A.9 bounds this by RFC 3986 alone, where §4.1 reaches an
// empty reference through a relative one whose path is empty, so unlike the
// other three parameters it carries no repetition bound to fall foul of. Note
// that only the quoted-string spelling can express it, since a token is 1*tchar
TEST(serialize_challenge_empty_error_uri) {
  const std::array<Parameter, 1> parameters{{{"error_uri", ""}}};
  const auto value{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(value.has_value());
  EXPECT_EQ(value.value(), "Bearer error_uri=\"\"");
}
