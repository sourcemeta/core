#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <optional> // std::optional

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
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(
                                R"JSON({
    "sub": "u1", "name": "Jane", "nickname": "J"
  })JSON"));
}

TEST(merge_claims_id_token_wins_a_conflict) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                          R"JSON({ "sub": "u1", "name": "Impostor" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(
                                R"JSON({ "sub": "u1", "name": "Jane" })JSON"));
}

// Section 5.3.2: "The sub Claim in the UserInfo Response MUST be verified to
// exactly match the sub Claim in the ID Token; if they do not match, the
// UserInfo Response values MUST NOT be used"
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

TEST(merge_claims_refuses_a_non_object_id_token) {
  EXPECT_FALSE(
      merge(R"JSON("nope")JSON", R"JSON({ "sub": "u1" })JSON").has_value());
}

TEST(merge_claims_refuses_a_non_object_userinfo) {
  EXPECT_FALSE(
      merge(R"JSON({ "sub": "u1" })JSON", R"JSON("nope")JSON").has_value());
}

// Section 5.1: email_verified is "True if the End-User's e-mail address has
// been verified", meaning the address delivered with it, so the pair is only
// ever taken from the answer that carried the address
TEST(merge_claims_takes_the_email_pair_from_the_id_token) {
  const auto result{merge(R"JSON({
    "sub": "u1", "email": "signed@b.test", "email_verified": true
  })JSON",
                          R"JSON({
    "sub": "u1", "email": "other@b.test", "email_verified": false
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "signed@b.test", "email_verified": true
  })JSON"));
}

TEST(merge_claims_takes_the_email_pair_from_userinfo_when_absent) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": true
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": true
  })JSON"));
}

// An assertion arriving without the address it vouches for would otherwise be
// left to vouch for the other answer's address
TEST(merge_claims_drops_an_orphan_assertion_from_the_id_token) {
  const auto result{merge(R"JSON({ "sub": "u1", "email_verified": true })JSON",
                          R"JSON({ "sub": "u1", "email": "a@b.test" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "a@b.test"
  })JSON"));
}

TEST(merge_claims_drops_an_orphan_assertion_from_userinfo) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "email": "signed@b.test" })JSON",
            R"JSON({ "sub": "u1", "email_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "signed@b.test"
  })JSON"));
}

TEST(merge_claims_drops_an_orphan_assertion_when_neither_carries_an_address) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "email_verified": true })JSON",
            R"JSON({ "sub": "u1", "email_verified": true })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::parse_json(R"JSON({ "sub": "u1" })JSON"));
}

// Section 5.3.2: an unreturned claim "SHOULD be omitted ... it SHOULD NOT be
// present with a null or empty string value", so neither shape delivers an
// address an assertion could speak for
TEST(merge_claims_treats_a_null_address_as_carrying_nothing) {
  const auto result{merge(R"JSON({
    "sub": "u1", "email": null, "email_verified": true
  })JSON",
                          R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": false
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": false
  })JSON"));
}

TEST(merge_claims_treats_an_empty_address_as_carrying_nothing) {
  const auto result{merge(R"JSON({
    "sub": "u1", "email": "", "email_verified": true
  })JSON",
                          R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": false
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": false
  })JSON"));
}

TEST(merge_claims_drops_an_assertion_carrying_nothing_beside_an_address) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({
    "sub": "u1", "email": "a@b.test", "email_verified": null
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "a@b.test"
  })JSON"));
}

// Section 5.1 relates the phone pair the same way, adding that a verified
// number "MUST be in E.164 format"
TEST(merge_claims_takes_the_phone_pair_from_the_id_token) {
  const auto result{merge(R"JSON({
    "sub": "u1", "phone_number": "+15551212", "phone_number_verified": true
  })JSON",
                          R"JSON({
    "sub": "u1", "phone_number": "+15559999"
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "phone_number": "+15551212", "phone_number_verified": true
  })JSON"));
}

TEST(merge_claims_drops_an_orphan_phone_assertion) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "phone_number_verified": true })JSON",
            R"JSON({ "sub": "u1", "phone_number": "+15551212" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "phone_number": "+15551212"
  })JSON"));
}

TEST(merge_claims_keeps_the_two_pairs_independent) {
  const auto result{merge(R"JSON({
    "sub": "u1", "email": "signed@b.test"
  })JSON",
                          R"JSON({
    "sub": "u1", "phone_number": "+15551212", "phone_number_verified": true
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "email": "signed@b.test",
    "phone_number": "+15551212", "phone_number_verified": true
  })JSON"));
}

// Section 5.6.2: the member values of _claim_names "are references to the
// member names in the _claim_sources member", so taking one from each answer
// would leave a reference pointing at a source that is not there
TEST(merge_claims_keeps_aggregated_claims_together_from_the_id_token) {
  const auto result{merge(R"JSON({
    "sub": "u1",
    "_claim_names": { "address": "src1" },
    "_claim_sources": { "src1": { "JWT": "a.b.c" } }
  })JSON",
                          R"JSON({
    "sub": "u1",
    "_claim_names": { "phone_number": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1",
    "_claim_names": { "address": "src1" },
    "_claim_sources": { "src1": { "JWT": "a.b.c" } }
  })JSON"));
}

TEST(merge_claims_takes_aggregated_claims_from_userinfo_when_absent) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({
    "sub": "u1",
    "_claim_names": { "phone_number": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1",
    "_claim_names": { "phone_number": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON"));
}

// A reference with nothing to resolve against is no better than none
TEST(merge_claims_does_not_pair_names_with_another_answers_sources) {
  const auto result{
      merge(R"JSON({ "sub": "u1", "_claim_names": { "address": "src1" } })JSON",
            R"JSON({
    "sub": "u1", "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "_claim_names": { "address": "src1" }
  })JSON"));
}

TEST(merge_claims_aggregated_names_carrying_nothing_do_not_block_the_other) {
  const auto result{merge(R"JSON({ "sub": "u1", "_claim_names": null })JSON",
                          R"JSON({
    "sub": "u1",
    "_claim_names": { "phone_number": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1",
    "_claim_names": { "phone_number": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON"));
}

TEST(merge_claims_without_a_userinfo_claim_leaves_the_token_untouched) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "Jane" })JSON",
                          R"JSON({ "sub": "u1" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(
                                R"JSON({ "sub": "u1", "name": "Jane" })JSON"));
}

// An ordinary claim carrying nothing neither blocks the other answer from
// filling it nor travels across on its own
TEST(merge_claims_null_id_token_claim_does_not_block_the_fill) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": null })JSON",
                          R"JSON({ "sub": "u1", "name": "Jane" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(
                                R"JSON({ "sub": "u1", "name": "Jane" })JSON"));
}

TEST(merge_claims_empty_id_token_claim_does_not_block_the_fill) {
  const auto result{merge(R"JSON({ "sub": "u1", "name": "" })JSON",
                          R"JSON({ "sub": "u1", "name": "Jane" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(
                                R"JSON({ "sub": "u1", "name": "Jane" })JSON"));
}

TEST(merge_claims_does_not_carry_over_a_null_userinfo_claim) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({ "sub": "u1", "nickname": null })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::parse_json(R"JSON({ "sub": "u1" })JSON"));
}

TEST(merge_claims_does_not_carry_over_an_empty_userinfo_claim) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({ "sub": "u1", "nickname": "" })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            sourcemeta::core::parse_json(R"JSON({ "sub": "u1" })JSON"));
}

// A claim that is genuinely false or zero carries a value, so the rule reaches
// only the two shapes Section 5.3.2 names
TEST(merge_claims_carries_a_false_or_zero_value) {
  const auto result{merge(R"JSON({ "sub": "u1" })JSON",
                          R"JSON({
    "sub": "u1", "flag": false, "count": 0
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1", "flag": false, "count": 0
  })JSON"));
}

// Every group and the ordinary fill at once, so that no rule quietly consumes
// a claim another was meant to carry
TEST(merge_claims_all_groups_and_the_fill_together) {
  const auto result{merge(R"JSON({
    "sub": "u1",
    "name": "Jane",
    "email_verified": true,
    "phone_number": "+15551212",
    "phone_number_verified": true,
    "_claim_names": { "address": "src1" },
    "_claim_sources": { "src1": { "JWT": "a.b.c" } }
  })JSON",
                          R"JSON({
    "sub": "u1",
    "name": "Impostor",
    "nickname": "J",
    "email": "a@b.test",
    "email_verified": false,
    "phone_number": "+15559999",
    "_claim_names": { "gender": "src2" },
    "_claim_sources": { "src2": { "JWT": "d.e.f" } }
  })JSON")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), sourcemeta::core::parse_json(R"JSON({
    "sub": "u1",
    "name": "Jane",
    "nickname": "J",
    "email": "a@b.test",
    "email_verified": false,
    "phone_number": "+15551212",
    "phone_number_verified": true,
    "_claim_names": { "address": "src1" },
    "_claim_sources": { "src1": { "JWT": "a.b.c" } }
  })JSON"));
}
