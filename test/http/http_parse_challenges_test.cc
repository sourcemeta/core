#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace {
using Challenges = std::vector<sourcemeta::core::HTTPParsedChallenge>;
} // namespace

// RFC 6750 §3: "WWW-Authenticate: Bearer realm="example""
TEST(parse_challenges_bearer_with_a_realm) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"example\"", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).scheme, "Bearer");
  EXPECT_FALSE(challenges.at(0).token68.has_value());
  EXPECT_EQ(challenges.at(0).parameters.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).parameters.at(0).first, "realm");
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "example");
}

TEST(parse_challenges_several_parameters) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"example\", error=\"invalid_token\", "
      "error_description=\"The access token expired\"",
      challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).parameters.size(), static_cast<std::size_t>(3));
  EXPECT_EQ(challenges.at(0).parameters.at(1).first, "error");
  EXPECT_EQ(challenges.at(0).parameters.at(1).second, "invalid_token");
  EXPECT_EQ(challenges.at(0).parameters.at(2).second,
            "The access token expired");
}

// RFC 9110 §11.6.1 warns that a field value "might contain more than one
// challenge, and each challenge can contain a comma-separated list of
// authentication parameters", so a token that no equals sign follows opens a
// new challenge rather than continuing the current one
TEST(parse_challenges_two_challenges) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"a\", Basic realm=\"b\"", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(2));
  EXPECT_EQ(challenges.at(0).scheme, "Bearer");
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a");
  EXPECT_EQ(challenges.at(1).scheme, "Basic");
  EXPECT_EQ(challenges.at(1).parameters.at(0).second, "b");
}

TEST(parse_challenges_bare_scheme_before_another) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Negotiate, Basic realm=\"b\"", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(2));
  EXPECT_EQ(challenges.at(0).scheme, "Negotiate");
  EXPECT_TRUE(challenges.at(0).parameters.empty());
  EXPECT_EQ(challenges.at(1).scheme, "Basic");
}

TEST(parse_challenges_bare_scheme_alone) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Negotiate", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).scheme, "Negotiate");
  EXPECT_TRUE(challenges.at(0).parameters.empty());
  EXPECT_FALSE(challenges.at(0).token68.has_value());
}

// RFC 9110 §11.2: token68 ends in optional "=" padding, which has to be told
// apart from the equals sign that opens a parameter value
TEST(parse_challenges_token68) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Negotiate YIIFQwYGKwYBBQUCoIIFNzCC", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_TRUE(challenges.at(0).token68.has_value());
  EXPECT_EQ(challenges.at(0).token68.value(), "YIIFQwYGKwYBBQUCoIIFNzCC");
  EXPECT_TRUE(challenges.at(0).parameters.empty());
}

TEST(parse_challenges_token68_with_padding) {
  Challenges challenges;
  EXPECT_TRUE(
      sourcemeta::core::http_parse_challenges("Negotiate YWJj==", challenges));
  EXPECT_TRUE(challenges.at(0).token68.has_value());
  EXPECT_EQ(challenges.at(0).token68.value(), "YWJj==");
}

TEST(parse_challenges_token68_followed_by_another_challenge) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Negotiate YWJj==, Basic realm=\"b\"", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(2));
  EXPECT_EQ(challenges.at(0).token68.value(), "YWJj==");
  EXPECT_EQ(challenges.at(1).scheme, "Basic");
}

// RFC 9110 §11.2: auth-param = token BWS "=" BWS ( token / quoted-string ), so
// an unquoted value is equally well formed
TEST(parse_challenges_token_value) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=example",
                                                      challenges));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "example");
}

TEST(parse_challenges_token_value_before_another_parameter) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=example, error=invalid_token", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).parameters.size(), static_cast<std::size_t>(2));
}

// RFC 9110 §5.6.4: a quoted-pair carries only its second octet
TEST(parse_challenges_unescapes_a_quote) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a\\\"b\"",
                                                      challenges));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a\"b");
}

TEST(parse_challenges_unescapes_a_backslash) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a\\\\b\"",
                                                      challenges));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a\\b");
}

TEST(parse_challenges_keeps_a_comma_within_a_quoted_value) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a,b\"",
                                                      challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a,b");
}

// RFC 9110 §11.2: auth-param tolerates the bad whitespace the grammar names
// around the equals sign, which a recipient still has to accept
TEST(parse_challenges_tolerates_whitespace_around_the_equals_sign) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm = \"example\"", challenges));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "example");
}

TEST(parse_challenges_tolerates_whitespace_around_the_separator) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"a\"  ,   error=\"b\"", challenges));
  EXPECT_EQ(challenges.at(0).parameters.size(), static_cast<std::size_t>(2));
}

// RFC 9110 §5.6.1.2: "A recipient MUST parse and ignore a reasonable number of
// empty list elements"
TEST(parse_challenges_ignores_empty_list_elements) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"a\",, Basic realm=\"b\"", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(2));
}

TEST(parse_challenges_ignores_a_leading_separator) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges(", Bearer realm=\"a\"",
                                                      challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
}

TEST(parse_challenges_ignores_a_trailing_separator) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a\", ",
                                                      challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
}

TEST(parse_challenges_empty_input) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges("", challenges));
}

TEST(parse_challenges_only_separators) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges(",,", challenges));
}

TEST(parse_challenges_unterminated_quoted_string) {
  Challenges challenges;
  EXPECT_FALSE(
      sourcemeta::core::http_parse_challenges("Bearer realm=\"a", challenges));
}

// RFC 9110 §11.2: a run of token characters followed by an equals sign and
// nothing else is a token68 with its padding, not a parameter missing a value,
// so the grammar settles the ambiguity in favour of the credential
TEST(parse_challenges_trailing_equals_sign_reads_as_a_credential) {
  Challenges challenges;
  EXPECT_TRUE(
      sourcemeta::core::http_parse_challenges("Bearer realm=", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_TRUE(challenges.at(0).token68.has_value());
  EXPECT_EQ(challenges.at(0).token68.value(), "realm=");
  EXPECT_TRUE(challenges.at(0).parameters.empty());
}

TEST(parse_challenges_missing_parameter_value_before_a_separator) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a\", b=",
                                                       challenges));
}

TEST(parse_challenges_scheme_that_is_not_a_token) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges("\"Bearer\" realm=\"a\"",
                                                       challenges));
}

TEST(parse_challenges_clears_the_container_first) {
  Challenges challenges;
  challenges.emplace_back();
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Negotiate", challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).scheme, "Negotiate");
}

// A recipient judges nothing beyond the grammar, so a challenge a sender would
// have been refused for still parses
TEST(parse_challenges_accepts_a_bare_bearer) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer", challenges));
  EXPECT_EQ(challenges.at(0).scheme, "Bearer");
  EXPECT_TRUE(challenges.at(0).parameters.empty());
}

// What this module serialises, it reads back unchanged
TEST(parse_challenges_round_trips_a_serialized_challenge) {
  const std::array<std::pair<std::string_view, std::string_view>, 2> parameters{
      {{"realm", "a\"b"}, {"error", "invalid_token"}}};
  const auto serialized{sourcemeta::core::http_serialize_challenge(
      {.scheme = "Bearer", .parameters = parameters})};
  EXPECT_TRUE(serialized.has_value());
  Challenges challenges;
  EXPECT_TRUE(
      sourcemeta::core::http_parse_challenges(serialized.value(), challenges));
  EXPECT_EQ(challenges.size(), static_cast<std::size_t>(1));
  EXPECT_EQ(challenges.at(0).scheme, "Bearer");
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a\"b");
  EXPECT_EQ(challenges.at(0).parameters.at(1).second, "invalid_token");
}

// RFC 9110 §5.6.4: qdtext and the escaped octet of a quoted-pair admit HTAB,
// SP, the visible characters and obs-text, so any other control character
// makes the value unreadable rather than carrying it through
TEST(parse_challenges_rejects_a_control_character_within_a_value) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"a\r\nb\"", challenges));
}

TEST(parse_challenges_admits_a_horizontal_tab_within_a_value) {
  Challenges challenges;
  EXPECT_TRUE(sourcemeta::core::http_parse_challenges("Bearer realm=\"a\tb\"",
                                                      challenges));
  EXPECT_EQ(challenges.at(0).parameters.at(0).second, "a\tb");
}

// RFC 9110 §11.6.1: WWW-Authenticate = #challenge, so a comma delimits two
// challenges and one cannot simply abut the other
TEST(parse_challenges_rejects_a_missing_separator_between_challenges) {
  Challenges challenges;
  EXPECT_FALSE(sourcemeta::core::http_parse_challenges(
      "Bearer realm=\"a\" Basic realm=\"b\"", challenges));
}

TEST(parse_challenges_rejects_a_token_that_opens_no_parameter) {
  Challenges challenges;
  EXPECT_FALSE(
      sourcemeta::core::http_parse_challenges("Bearer abc def", challenges));
}
