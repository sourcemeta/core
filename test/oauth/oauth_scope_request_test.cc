#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

namespace {
auto decide(const char *claims, const char *request)
    -> sourcemeta::core::OAuthScopeDecision {
  return sourcemeta::core::oauth_scope_request_accepts(
      sourcemeta::core::parse_json(claims),
      sourcemeta::core::parse_json(request));
}
} // namespace

// OpenID Connect Core 1.0 Section 5.5.1 gives the rule its shape, and RFC 6749
// Section 3.3 makes the granted scope "a list of space-delimited,
// case-sensitive strings" whose "order does not matter"
TEST(scope_request_value_that_is_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read write" })JSON",
                   R"JSON({ "value": "write" })JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_value_that_is_not_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read write" })JSON",
                   R"JSON({ "value": "admin" })JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

// The strings are case-sensitive, so a differing spelling is a different scope
TEST(scope_request_value_differing_in_case) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "value": "READ" })JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

// A scope is a set, so a rule naming several is satisfied by any one of them
TEST(scope_request_values_with_one_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "values": [ "admin", "read" ] })JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_values_with_none_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "values": [ "admin", "write" ] })JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_empty_values_grant_nothing) {
  EXPECT_EQ(
      decide(R"JSON({ "scope": "read" })JSON", R"JSON({ "values": [] })JSON"),
      sourcemeta::core::OAuthScopeDecision::Refused);
}

// A rule naming both is satisfied by either
TEST(scope_request_value_and_values_with_the_value_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "value": "read", "values": [ "admin" ] })JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_value_and_values_with_a_listed_one_granted) {
  EXPECT_EQ(decide(R"JSON({ "scope": "admin" })JSON",
                   R"JSON({ "value": "read", "values": [ "admin" ] })JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

// A rule constraining nothing asks only that a scope be carried at all
TEST(scope_request_unconstrained_rule_with_a_scope) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_unconstrained_rule_carrying_only_essential) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "essential": true })JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_unconstrained_rule_without_a_scope) {
  EXPECT_EQ(decide(R"JSON({ "sub": "u1" })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

// A token carrying no readable scope grants none, whatever the rule asks
TEST(scope_request_absent_scope_claim) {
  EXPECT_EQ(
      decide(R"JSON({ "sub": "u1" })JSON", R"JSON({ "value": "read" })JSON"),
      sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_scope_claim_that_is_not_a_string) {
  EXPECT_EQ(decide(R"JSON({ "scope": [ "read" ] })JSON",
                   R"JSON({ "value": "read" })JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_empty_scope_claim) {
  EXPECT_EQ(
      decide(R"JSON({ "scope": "" })JSON", R"JSON({ "value": "read" })JSON"),
      sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_empty_scope_claim_against_an_unconstrained_rule) {
  EXPECT_EQ(decide(R"JSON({ "scope": "" })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

// A rule this cannot read is neither satisfied nor unsatisfied, so what it
// means is the caller's to decide rather than this predicate's
TEST(scope_request_rule_that_is_not_an_object) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON", R"JSON("read")JSON"),
            sourcemeta::core::OAuthScopeDecision::Unreadable);
}

// OpenID Connect Core 1.0 Section 5.5.1: a null rule "indicates that this
// Claim is being requested in the default manner", so it constrains nothing
TEST(scope_request_null_rule_with_a_scope) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON", R"JSON(null)JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}

TEST(scope_request_null_rule_without_a_scope) {
  EXPECT_EQ(decide(R"JSON({ "sub": "u1" })JSON", R"JSON(null)JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_value_that_is_not_a_string) {
  EXPECT_EQ(
      decide(R"JSON({ "scope": "read" })JSON", R"JSON({ "value": 42 })JSON"),
      sourcemeta::core::OAuthScopeDecision::Unreadable);
}

TEST(scope_request_values_that_is_not_an_array) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "values": "read" })JSON"),
            sourcemeta::core::OAuthScopeDecision::Unreadable);
}

TEST(scope_request_values_carrying_a_member_that_is_not_a_string) {
  EXPECT_EQ(decide(R"JSON({ "scope": "read" })JSON",
                   R"JSON({ "values": [ "admin", 42 ] })JSON"),
            sourcemeta::core::OAuthScopeDecision::Unreadable);
}

// An unreadable rule is unreadable whether or not the token would satisfy a
// readable one, so the decision does not depend on the claims
TEST(scope_request_unreadable_rule_without_a_scope_claim) {
  EXPECT_EQ(decide(R"JSON({ "sub": "u1" })JSON", R"JSON({ "value": 42 })JSON"),
            sourcemeta::core::OAuthScopeDecision::Unreadable);
}

// A rule is read before the token is, so a malformed one is reported as such
// rather than being masked by a token that grants nothing
TEST(scope_request_unreadable_rule_outranks_a_missing_scope) {
  EXPECT_EQ(decide(R"JSON({})JSON", R"JSON("read")JSON"),
            sourcemeta::core::OAuthScopeDecision::Unreadable);
}

// RFC 6749 Section 3.3: scope-token = 1*( %x21 / %x23-5B / %x5D-7E ), so a
// claim of nothing but delimiters names no scope and grants none
TEST(scope_request_whitespace_only_scope_claim) {
  EXPECT_EQ(decide(R"JSON({ "scope": " " })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_several_delimiters_only_scope_claim) {
  EXPECT_EQ(decide(R"JSON({ "scope": "   " })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

TEST(scope_request_whitespace_only_scope_claim_against_a_null_rule) {
  EXPECT_EQ(decide(R"JSON({ "scope": " " })JSON", R"JSON(null)JSON"),
            sourcemeta::core::OAuthScopeDecision::Refused);
}

// A claim padded around a real token still names one
TEST(scope_request_padded_scope_claim_names_a_scope) {
  EXPECT_EQ(decide(R"JSON({ "scope": " read " })JSON", R"JSON({})JSON"),
            sourcemeta::core::OAuthScopeDecision::Accepted);
}
