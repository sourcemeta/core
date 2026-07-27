#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/regex.h>
#include <sourcemeta/core/test.h>

TEST(regex) {
  const auto uuid{sourcemeta::core::uuidv4()};
  EXPECT_TRUE(sourcemeta::core::matches_if_valid(
      "^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$",
      uuid));
  EXPECT_TRUE(sourcemeta::core::is_uuid_like(uuid));
}

TEST(version_and_variant) {
  for (auto count = 0; count < 100; ++count) {
    const auto uuid{sourcemeta::core::uuidv4()};
    EXPECT_EQ(uuid[14], '4');
    EXPECT_TRUE(uuid[19] == '8' || uuid[19] == '9' || uuid[19] == 'a' ||
                uuid[19] == 'b');
    EXPECT_TRUE(sourcemeta::core::is_uuid_like(uuid));
  }
}

TEST(idempotent) {
  const auto uuid_1{sourcemeta::core::uuidv4()};
  const auto uuid_2{sourcemeta::core::uuidv4()};
  const auto uuid_3{sourcemeta::core::uuidv4()};
  EXPECT_NE(uuid_1, uuid_2);
  EXPECT_NE(uuid_2, uuid_3);
  EXPECT_NE(uuid_3, uuid_1);
  EXPECT_TRUE(sourcemeta::core::is_uuid_like(uuid_1));
  EXPECT_TRUE(sourcemeta::core::is_uuid_like(uuid_2));
  EXPECT_TRUE(sourcemeta::core::is_uuid_like(uuid_3));
}

TEST(valid_all_upper_case) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2EB8AA08-AA98-11EA-B4AA-73B441D16380"));
}

TEST(valid_all_lower_case) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(valid_mixed_case) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-AA98-11ea-B4Aa-73B441D16380"));
}

TEST(valid_all_zeroes) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("00000000-0000-0000-0000-000000000000"));
}

TEST(valid_version_4) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("98d80576-482e-427f-8434-7f86890ab222"));
}

TEST(valid_version_5) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("99c17cbb-656f-564a-940f-1a4568f03487"));
}

TEST(valid_hypothetical_version_6) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("99c17cbb-656f-664a-940f-1a4568f03487"));
}

TEST(valid_hypothetical_version_15) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("99c17cbb-656f-f64a-940f-1a4568f03487"));
}

TEST(invalid_wrong_length) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d1638"));
}

TEST(invalid_missing_section) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-73b441d16380"));
}

TEST(invalid_non_hex_character) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4ga-73b441d16380"));
}

TEST(invalid_no_dashes) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08aa9811eab4aa73b441d16380"));
}

TEST(invalid_too_few_dashes) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08aa98-11ea-b4aa73b441d16380"));
}

TEST(invalid_too_many_dashes) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8-aa08-aa98-11ea-b4aa73b44-1d16380"));
}

TEST(invalid_dashes_in_wrong_spot) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08aa9811eab4aa73b441d16380----"));
}

TEST(invalid_shifted_dashes) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa0-8aa98-11e-ab4aa7-3b441d16380"));
}

TEST(invalid_empty) { EXPECT_FALSE(sourcemeta::core::is_uuid_like("")); }

TEST(invalid_trailing_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d16380 "));
}

TEST(invalid_leading_space) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like(" 2eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(invalid_braces) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("{2eb8aa08-aa98-11ea-b4aa-73b441d16380}"));
}

TEST(invalid_urn_prefix) {
  EXPECT_FALSE(sourcemeta::core::is_uuid_like(
      "urn:uuid:2eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(valid_max) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("ffffffff-ffff-ffff-ffff-ffffffffffff"));
}

// RFC 9562 Section 4.2: the version nibble (index 14) is a semantic field, not
// a syntactic constraint - every nibble 0-f is a valid string representation.
// RFC 9562 Section 4.1: the variant nibble (index 19) is likewise
// unconstrained.
TEST(valid_version_nibble_0) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-01ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_1) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_2) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-21ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_3) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-31ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_4) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-41ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_5) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-51ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_6) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-61ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_7) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-71ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_8) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-81ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_9) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-91ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_a) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-a1ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_b) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-b1ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_c) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-c1ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_d) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-d1ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_e) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-e1ea-b4aa-73b441d16380"));
}

TEST(valid_version_nibble_f) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-f1ea-b4aa-73b441d16380"));
}

TEST(valid_variant_nibble_0) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-04aa-73b441d16380"));
}

TEST(valid_variant_nibble_1) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-14aa-73b441d16380"));
}

TEST(valid_variant_nibble_2) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-24aa-73b441d16380"));
}

TEST(valid_variant_nibble_3) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-34aa-73b441d16380"));
}

TEST(valid_variant_nibble_4) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-44aa-73b441d16380"));
}

TEST(valid_variant_nibble_5) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-54aa-73b441d16380"));
}

TEST(valid_variant_nibble_6) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-64aa-73b441d16380"));
}

TEST(valid_variant_nibble_7) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-74aa-73b441d16380"));
}

TEST(valid_variant_nibble_8) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-84aa-73b441d16380"));
}

TEST(valid_variant_nibble_9) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-94aa-73b441d16380"));
}

TEST(valid_variant_nibble_a) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-a4aa-73b441d16380"));
}

TEST(valid_variant_nibble_b) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(valid_variant_nibble_c) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-c4aa-73b441d16380"));
}

TEST(valid_variant_nibble_d) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-d4aa-73b441d16380"));
}

TEST(valid_variant_nibble_e) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-e4aa-73b441d16380"));
}

TEST(valid_variant_nibble_f) {
  EXPECT_TRUE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-f4aa-73b441d16380"));
}

TEST(invalid_underscore_separator) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d1_380"));
}

TEST(invalid_leading_plus) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("+eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(invalid_hex_radix_prefix) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("0x000000-0000-0000-0000-000000000000"));
}

// U+0968 DEVANAGARI DIGIT TWO is not a HEXDIG.
TEST(invalid_non_ascii_digit) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("\xe0\xa5\xa8"
                                     "eb8aa08-aa98-11ea-b4aa-73b441d16380"));
}

TEST(invalid_trailing_newline) {
  EXPECT_FALSE(
      sourcemeta::core::is_uuid_like("2eb8aa08-aa98-11ea-b4aa-73b441d16380\n"));
}
