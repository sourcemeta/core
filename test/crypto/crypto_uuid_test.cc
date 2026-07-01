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
