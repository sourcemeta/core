#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

namespace {
auto merge(const char *id_token, const char *userinfo)
    -> std::optional<sourcemeta::core::JSON> {
  return sourcemeta::core::oidc_merge_claims(
      sourcemeta::core::parse_json(id_token),
      sourcemeta::core::parse_json(userinfo));
}
} // namespace

// OpenID Connect Core 1.0 Section 5.3.2: a provider answering twice about one
// person is two halves of one account, and the signed half is authoritative
TEST(merge_claims_userinfo_fills_a_gap) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                          R"JSON({ "sub": "u1", "nickname": "J" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("name").to_string(), "Jane");
  EXPECT_EQ(result.value().at("nickname").to_string(), "J");
}

TEST(merge_claims_id_token_wins_a_conflict) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                          R"JSON({ "sub": "u1", "name": "Impostor" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("name").to_string(), "Jane");
}

// OpenID Connect Core 1.0 Section 5.3.2: "The sub Claim in the UserInfo
// Response MUST be verified to exactly match the sub Claim in the ID Token; if
// they do not match, the UserInfo Response values MUST NOT be used"
TEST(merge_claims_refuses_a_mismatched_subject) {
  EXPECT_FALSE(merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                     R"JSON({ "sub": "u2", "email": "a@b.test" })JSON")
                   .has_value());
}

// Section 5.3.2: "The sub (subject) Claim MUST always be returned in the
// UserInfo Response"
TEST(merge_claims_refuses_a_missing_userinfo_subject) {
  EXPECT_FALSE(
      merge(R"JSON({ "sub": "u1" })JSON", R"JSON({ "email": "a@b.test" })JSON")
          .has_value());
}

// Section 2: the subject is REQUIRED in an ID Token, and without it no match
// can be established
TEST(merge_claims_refuses_a_missing_id_token_subject) {
  EXPECT_FALSE(
      merge(R"JSON({ "name": "Jane" })JSON", R"JSON({ "sub": "u1" })JSON")
          .has_value());
}

TEST(merge_claims_refuses_a_non_object) {
  EXPECT_FALSE(
      merge(R"JSON("nope")JSON", R"JSON({ "sub": "u1" })JSON").has_value());
  EXPECT_FALSE(
      merge(R"JSON({ "sub": "u1" })JSON", R"JSON("nope")JSON").has_value());
}

// Section 5.1: email_verified is "True if the End-User's e-mail address has
// been verified", meaning the address delivered with it, so the pair is only
// ever taken from the answer that carried the address
TEST(merge_claims_takes_the_email_pair_from_the_id_token) {
  const auto result{merge(R"JSON({ "sub": "u1", "email": "signed@b.test",
                     "email_verified": true })JSON",
                          R"JSON({ "sub": "u1", "email": "other@b.test",
                     "email_verified": false })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "signed@b.test");
  EXPECT_TRUE(result.value().at("email_verified").to_boolean());
}

TEST(merge_claims_takes_the_email_pair_from_userinfo_when_absent) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({ "sub": "u1", "email": "a@b.test",
                     "email_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "a@b.test");
  EXPECT_TRUE(result.value().at("email_verified").to_boolean());
}

// An assertion arriving without the address it vouches for would otherwise be
// left to vouch for the other answer's address
TEST(merge_claims_drops_an_orphan_assertion_from_the_id_token) {
  const auto result{merge(R"JSON({ "sub": "u1", "email_verified": true })JSON",
                          R"JSON({ "sub": "u1", "email": "a@b.test" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "a@b.test");
  EXPECT_FALSE(result.value().defines("email_verified"));
}

TEST(merge_claims_drops_an_orphan_assertion_from_userinfo) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "email": "signed@b.test" })JSON",
            R"JSON({ "sub": "u1", "email_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "signed@b.test");
  EXPECT_FALSE(result.value().defines("email_verified"));
}

TEST(merge_claims_drops_an_orphan_assertion_when_neither_carries_an_address) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "email_verified": true })JSON",
            R"JSON({ "sub": "u1", "email_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_FALSE(result.value().defines("email_verified"));
}

// Section 5.3.2: an unreturned claim "SHOULD be omitted ... it SHOULD NOT be
// present with a null or empty string value", so a null address carries none
TEST(merge_claims_treats_a_null_address_as_carrying_nothing) {
  const auto result{merge(R"JSON({ "sub": "u1", "email": null,
                     "email_verified": true })JSON",
                          R"JSON({ "sub": "u1", "email": "a@b.test",
                     "email_verified": false })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "a@b.test");
  EXPECT_FALSE(result.value().at("email_verified").to_boolean());
}

// Section 5.1: phone_number_verified carries the same relationship, and adds
// that "When true, the phone_number Claim MUST be in E.164 format"
TEST(merge_claims_takes_the_phone_pair_from_the_id_token) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "phone_number": "+15551212",
                     "phone_number_verified": true })JSON",
            R"JSON({ "sub": "u1", "phone_number": "+15559999" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("phone_number").to_string(), "+15551212");
  EXPECT_TRUE(result.value().at("phone_number_verified").to_boolean());
}

TEST(merge_claims_drops_an_orphan_phone_assertion) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "phone_number_verified": true })JSON",
            R"JSON({ "sub": "u1", "phone_number": "+15551212" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("phone_number").to_string(), "+15551212");
  EXPECT_FALSE(result.value().defines("phone_number_verified"));
}

TEST(merge_claims_keeps_the_two_pairs_independent) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "email": "signed@b.test" })JSON",
            R"JSON({ "sub": "u1", "phone_number": "+15551212",
                     "phone_number_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("email").to_string(), "signed@b.test");
  EXPECT_TRUE(result.value().at("phone_number_verified").to_boolean());
}

// Section 5.6.2: the member values of _claim_names "are references to the
// member names in the _claim_sources member", so taking one from each answer
// would leave a reference pointing at a source that is not there
TEST(merge_claims_keeps_aggregated_claims_together_from_the_id_token) {
  const auto result{merge(R"JSON({ "sub": "u1",
                     "_claim_names": { "address": "src1" },
                     "_claim_sources": { "src1": { "JWT": "a.b.c" } } })JSON",
                          R"JSON({ "sub": "u1",
                     "_claim_names": { "phone_number": "src2" },
                     "_claim_sources": { "src2": { "JWT": "d.e.f" } } })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().at("_claim_names").defines("address"));
  EXPECT_FALSE(result.value().at("_claim_names").defines("phone_number"));
  EXPECT_TRUE(result.value().at("_claim_sources").defines("src1"));
  EXPECT_FALSE(result.value().at("_claim_sources").defines("src2"));
}

TEST(merge_claims_takes_aggregated_claims_from_userinfo_when_absent) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({ "sub": "u1",
                     "_claim_names": { "phone_number": "src2" },
                     "_claim_sources": { "src2": { "JWT": "d.e.f" } } })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().at("_claim_names").defines("phone_number"));
  EXPECT_TRUE(result.value().at("_claim_sources").defines("src2"));
}

// A reference with nothing to resolve against is no better than none
TEST(merge_claims_does_not_pair_names_with_another_answers_sources) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "_claim_names": { "address": "src1" } })JSON",
            R"JSON({ "sub": "u1",
                     "_claim_sources": { "src2": { "JWT": "d.e.f" } } })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().defines("_claim_names"));
  EXPECT_FALSE(result.value().defines("_claim_sources"));
}

TEST(merge_claims_without_a_userinfo_claim_leaves_the_token_untouched) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                          R"JSON({ "sub": "u1" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().at("name").to_string(), "Jane");
}
