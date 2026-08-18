#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

// OpenID Connect Core 1.0 Section 5.5.1: a scalar is compared as it always was
TEST(multi_valued_scalar_matching_the_requested_value) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, sourcemeta::core::JSON{"admin"}));
}

TEST(multi_valued_scalar_not_matching_the_requested_value) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, sourcemeta::core::JSON{"guest"}));
}

// RFC 7643 Section 2.4: a multi-valued attribute is a list, so belonging to
// the set it carries is what the rule asks about
TEST(multi_valued_array_with_one_member_matching) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{
      sourcemeta::core::parse_json(R"JSON([ "guest", "admin" ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

TEST(multi_valued_array_with_no_member_matching) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{
      sourcemeta::core::parse_json(R"JSON([ "guest", "auditor" ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_empty_array_matches_nothing) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_array_against_a_values_request) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "values": [ "admin", "auditor" ] })JSON")};
  const auto value{
      sourcemeta::core::parse_json(R"JSON([ "guest", "auditor" ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

// RFC 7643 Section 2.4: "value" is "the attribute's significant value", so it
// is the sub-attribute a rule is compared against
TEST(multi_valued_scim_member_matching_on_its_value) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "value": "e9e30dba-f08f-4109-8486-d5c6a331660a" })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([
    { "value": "e9e30dba-f08f-4109-8486-d5c6a331660a",
      "$ref": "https://example.com/v2/Groups/e9e30dba",
      "display": "Tour Guides" }
  ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

// RFC 7643 Section 2.4: "display" is "a human-readable name, primarily used
// for display purposes", so admitting on it would let whoever can rename a
// group grant themselves access
TEST(multi_valued_scim_member_never_matches_on_its_display) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "Tour Guides" })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([
    { "value": "e9e30dba-f08f-4109-8486-d5c6a331660a",
      "display": "Tour Guides" }
  ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_scim_member_never_matches_on_its_reference) {
  const auto request{sourcemeta::core::parse_json(
      R"JSON({ "value": "https://example.com/v2/Groups/e9e30dba" })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([
    { "value": "e9e30dba-f08f-4109-8486-d5c6a331660a",
      "$ref": "https://example.com/v2/Groups/e9e30dba" }
  ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_scim_member_without_a_value_matches_nothing) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{sourcemeta::core::parse_json(
      R"JSON([ { "display": "Administrators" } ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_bare_scim_member_outside_an_array) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{sourcemeta::core::parse_json(
      R"JSON({ "value": "admin", "display": "Administrators" })JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

// RFC 7643 Section 2.4: elements are "primitive values, or objects with a set
// of sub-attributes", so one list may carry both
TEST(multi_valued_array_mixing_primitives_and_scim_members) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{sourcemeta::core::parse_json(
      R"JSON([ "guest", { "value": "admin" } ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

// A rule constraining nothing asks only that the claim carry something
TEST(multi_valued_unconstrained_request_admits_any_member) {
  const auto request{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([ "anything" ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}

TEST(multi_valued_unconstrained_request_against_an_empty_array) {
  const auto request{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

// A malformed request permits nothing rather than silently opening up
TEST(multi_valued_malformed_request_admits_nothing) {
  const auto request{sourcemeta::core::parse_json(R"JSON("admin")JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([ "admin" ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

// A nested array is not a member shape the schema defines
TEST(multi_valued_nested_array_member_matches_nothing) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([ [ "admin" ] ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

// The scalar comparison stays whole-value, so an array never matches a rule
// naming the array itself
TEST(multi_valued_does_not_compare_the_array_as_a_whole) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": [ "admin" ] })JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([ "admin" ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

// RFC 7643 Section 2.5: an unassigned attribute, the null value, and the empty
// array "SHALL be considered to be equivalent in state", so a null claim
// carries no membership any more than an empty one does
TEST(multi_valued_null_claim_matches_nothing) {
  const auto request{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_null_member_matches_nothing) {
  const auto request{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  const auto value{sourcemeta::core::parse_json(R"JSON([ null ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

TEST(multi_valued_scim_member_with_a_null_value_matches_nothing) {
  const auto request{sourcemeta::core::parse_json(R"JSON(null)JSON")};
  const auto value{sourcemeta::core::parse_json(
      R"JSON([ { "value": null, "display": "Administrators" } ])JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(
      request, value));
}

// A null member alongside a real one does not spoil the set
TEST(multi_valued_null_member_beside_a_matching_one) {
  const auto request{
      sourcemeta::core::parse_json(R"JSON({ "value": "admin" })JSON")};
  const auto value{
      sourcemeta::core::parse_json(R"JSON([ null, "admin" ])JSON")};
  EXPECT_TRUE(sourcemeta::core::oidc_claim_request_accepts_multi_valued(request,
                                                                        value));
}
