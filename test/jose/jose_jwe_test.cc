#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// A structurally valid compact object carrying the given protected header and
// placeholder segments, so the parser can be exercised without real ciphertext
static auto compact_from_header(const std::string_view header_json)
    -> std::string {
  std::string result{sourcemeta::core::base64url_encode(header_json)};
  result.append(".QUFB.QUFB.QUFB.QUFB");
  return result;
}

TEST(from_parses_a_valid_object) {
  const auto object{sourcemeta::core::JWE::from(
      compact_from_header(R"({"alg":"dir","enc":"A128GCM"})"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_TRUE(object.value().algorithm().has_value());
  EXPECT_EQ(object.value().algorithm().value(),
            sourcemeta::core::JWEAlgorithm::DIR);
  EXPECT_TRUE(object.value().encryption().has_value());
  EXPECT_EQ(object.value().encryption().value(),
            sourcemeta::core::JWEEncryption::A128GCM);
}

TEST(from_exposes_the_key_id) {
  const auto object{sourcemeta::core::JWE::from(
      compact_from_header(R"({"alg":"dir","enc":"A128GCM","kid":"key-1"})"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_TRUE(object.value().key_id().has_value());
  EXPECT_EQ(object.value().key_id().value(), "key-1");
}

TEST(from_decodes_the_segments) {
  const auto object{sourcemeta::core::JWE::from(
      compact_from_header(R"({"alg":"dir","enc":"A128GCM"})"))};
  EXPECT_TRUE(object.has_value());
  // The placeholder segments each base64url-decode to the bytes "AAA"
  EXPECT_EQ(object.value().encrypted_key(), "AAA");
  EXPECT_EQ(object.value().initialization_vector(), "AAA");
  EXPECT_EQ(object.value().ciphertext(), "AAA");
  EXPECT_EQ(object.value().tag(), "AAA");
}

TEST(from_retains_the_encoded_protected_header) {
  const auto encoded{
      sourcemeta::core::base64url_encode(R"({"alg":"dir","enc":"A128GCM"})")};
  const auto object{sourcemeta::core::JWE::from(
      compact_from_header(R"({"alg":"dir","enc":"A128GCM"})"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_EQ(object.value().protected_header(), encoded);
}

TEST(from_keeps_an_unknown_algorithm_unresolved) {
  const auto object{sourcemeta::core::JWE::from(
      compact_from_header(R"({"alg":"RSA1_5","enc":"A128GCM"})"))};
  EXPECT_TRUE(object.has_value());
  EXPECT_FALSE(object.value().algorithm().has_value());
}

TEST(from_rejects_too_few_segments) {
  EXPECT_FALSE(sourcemeta::core::JWE::from("QUFB.QUFB.QUFB.QUFB").has_value());
}

TEST(from_rejects_too_many_segments) {
  EXPECT_FALSE(
      sourcemeta::core::JWE::from("QUFB.QUFB.QUFB.QUFB.QUFB.QUFB").has_value());
}

TEST(from_rejects_invalid_base64url) {
  EXPECT_FALSE(
      sourcemeta::core::JWE::from("*.QUFB.QUFB.QUFB.QUFB").has_value());
}

TEST(from_rejects_a_non_object_header) {
  EXPECT_FALSE(
      sourcemeta::core::JWE::from(compact_from_header("123")).has_value());
}

TEST(from_rejects_a_missing_algorithm) {
  EXPECT_FALSE(
      sourcemeta::core::JWE::from(compact_from_header(R"({"enc":"A128GCM"})"))
          .has_value());
}

TEST(from_rejects_a_missing_encryption) {
  EXPECT_FALSE(
      sourcemeta::core::JWE::from(compact_from_header(R"({"alg":"dir"})"))
          .has_value());
}

TEST(from_rejects_a_critical_header) {
  EXPECT_FALSE(sourcemeta::core::JWE::from(
                   compact_from_header(
                       R"({"alg":"dir","enc":"A128GCM","crit":["exp"]})"))
                   .has_value());
}

TEST(from_rejects_a_duplicate_header_member) {
  EXPECT_FALSE(sourcemeta::core::JWE::from(
                   compact_from_header(
                       R"({"alg":"dir","enc":"A128GCM","enc":"A256GCM"})"))
                   .has_value());
}
