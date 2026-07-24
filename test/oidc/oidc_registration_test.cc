#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <chrono>      // std::chrono::seconds
#include <string_view> // std::string_view

TEST(from_parses_a_valid_client_document) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "subject_type": "pairwise",
    "sector_identifier_uri": "https://client.example/sector.json",
    "id_token_signed_response_alg": "ES256",
    "default_max_age": 3600,
    "require_auth_time": true
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().has_redirect_uri("https://client.example/cb"));
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_TRUE(metadata.value().subject_type().has_value());
  EXPECT_EQ(metadata.value().subject_type().value(), "pairwise");
  EXPECT_TRUE(metadata.value().sector_identifier_uri().has_value());
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "ES256");
  EXPECT_TRUE(metadata.value().default_max_age().has_value());
  EXPECT_EQ(metadata.value().default_max_age().value(),
            std::chrono::seconds{3600});
  EXPECT_TRUE(metadata.value().require_auth_time());
}

TEST(from_applies_the_defaults) {
  auto document{sourcemeta::core::parse_json(
      R"JSON({ "redirect_uris": [ "https://client.example/cb" ] })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_EQ(metadata.value().application_type(), "web");
  EXPECT_EQ(metadata.value().id_token_signed_response_alg(), "RS256");
  EXPECT_FALSE(metadata.value().require_auth_time());
  EXPECT_FALSE(metadata.value().subject_type().has_value());
  EXPECT_FALSE(metadata.value().default_max_age().has_value());
}

TEST(from_rejects_a_missing_redirect_uris) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "subject_type": "public" })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_rejects_an_empty_redirect_uris) {
  auto document{
      sourcemeta::core::parse_json(R"JSON({ "redirect_uris": [] })JSON")};
  EXPECT_FALSE(sourcemeta::core::OIDCClientMetadata::from(std::move(document))
                   .has_value());
}

TEST(from_exposes_post_logout_redirect_uris) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "redirect_uris": [ "https://client.example/cb" ],
    "post_logout_redirect_uris": [ "https://client.example/after" ]
  })JSON")};
  const auto metadata{
      sourcemeta::core::OIDCClientMetadata::from(std::move(document))};
  EXPECT_TRUE(metadata.has_value());
  EXPECT_TRUE(metadata.value().has_post_logout_redirect_uri(
      "https://client.example/after"));
  EXPECT_FALSE(metadata.value().has_post_logout_redirect_uri(
      "https://attacker.example/after"));
}

TEST(sector_identifier_contains_all_registered_uris) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "https://client.example/cb", "https://client.example/cb2" ])JSON")};
  const std::array<std::string_view, 2> registered{
      {"https://client.example/cb", "https://client.example/cb2"}};
  EXPECT_TRUE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(sector_identifier_rejects_a_missing_uri) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON([ "https://client.example/cb" ])JSON")};
  const std::array<std::string_view, 2> registered{
      {"https://client.example/cb", "https://client.example/other"}};
  EXPECT_FALSE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}

TEST(sector_identifier_rejects_a_non_array_document) {
  const auto document{sourcemeta::core::parse_json(
      R"JSON({ "uris": [ "https://client.example/cb" ] })JSON")};
  const std::array<std::string_view, 1> registered{
      {"https://client.example/cb"}};
  EXPECT_FALSE(
      sourcemeta::core::oidc_sector_identifier_contains(document, registered));
}
