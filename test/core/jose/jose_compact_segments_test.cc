#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

// RFC 7515 §7.1: the JWS Compact Serialization is the protected header, the
// payload, and the signature, each base64url encoded and joined by dots
TEST(jose_compact_segments_jws_compact_serialization) {
  const auto segments{sourcemeta::core::jose_compact_segments<3>(
      "eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJhY21lIn0.c2ln")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_EQ(segments.value().at(0), "eyJhbGciOiJSUzI1NiJ9");
  EXPECT_EQ(segments.value().at(1), "eyJpc3MiOiJhY21lIn0");
  EXPECT_EQ(segments.value().at(2), "c2ln");
}

// RFC 7516 §7.1: the JWE Compact Serialization is the protected header, the
// encrypted key, the initialization vector, the ciphertext, and the
// authentication tag
TEST(jose_compact_segments_jwe_compact_serialization) {
  const auto segments{
      sourcemeta::core::jose_compact_segments<5>("header.key.iv.body.tag")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_EQ(segments.value().at(0), "header");
  EXPECT_EQ(segments.value().at(1), "key");
  EXPECT_EQ(segments.value().at(2), "iv");
  EXPECT_EQ(segments.value().at(3), "body");
  EXPECT_EQ(segments.value().at(4), "tag");
}

TEST(jose_compact_segments_too_few) {
  EXPECT_FALSE(
      sourcemeta::core::jose_compact_segments<3>("header.payload").has_value());
}

TEST(jose_compact_segments_too_many) {
  EXPECT_FALSE(sourcemeta::core::jose_compact_segments<3>(
                   "header.payload.signature.extra")
                   .has_value());
}

TEST(jose_compact_segments_trailing_separator) {
  EXPECT_FALSE(
      sourcemeta::core::jose_compact_segments<3>("header.payload.signature.")
          .has_value());
}

// RFC 7515 Appendix F: a detached content JWS omits the payload from the
// serialization, leaving an empty middle segment
TEST(jose_compact_segments_empty_middle_segment) {
  const auto segments{
      sourcemeta::core::jose_compact_segments<3>("header..signature")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_EQ(segments.value().at(0), "header");
  EXPECT_TRUE(segments.value().at(1).empty());
  EXPECT_EQ(segments.value().at(2), "signature");
}

// RFC 7516 §5.1: direct encryption produces no encrypted key, so that segment
// is the empty octet sequence
TEST(jose_compact_segments_empty_encrypted_key) {
  const auto segments{
      sourcemeta::core::jose_compact_segments<5>("header..iv.body.tag")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_TRUE(segments.value().at(1).empty());
}

TEST(jose_compact_segments_every_segment_empty) {
  const auto segments{sourcemeta::core::jose_compact_segments<3>("..")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_TRUE(segments.value().at(0).empty());
  EXPECT_TRUE(segments.value().at(1).empty());
  EXPECT_TRUE(segments.value().at(2).empty());
}

TEST(jose_compact_segments_empty_input) {
  EXPECT_FALSE(sourcemeta::core::jose_compact_segments<3>("").has_value());
}

TEST(jose_compact_segments_single_segment) {
  const auto segments{sourcemeta::core::jose_compact_segments<1>("header")};
  EXPECT_TRUE(segments.has_value());
  EXPECT_EQ(segments.value().at(0), "header");
}

TEST(jose_compact_segments_single_segment_with_a_separator) {
  EXPECT_FALSE(
      sourcemeta::core::jose_compact_segments<1>("header.payload").has_value());
}

TEST(jose_compact_segments_borrow_from_the_input) {
  const std::string_view input{"header.payload.signature"};
  const auto segments{sourcemeta::core::jose_compact_segments<3>(input)};
  EXPECT_TRUE(segments.has_value());
  EXPECT_TRUE(segments.value().at(0).data() == input.data());
  EXPECT_TRUE(segments.value().at(1).data() == input.data() + 7);
  EXPECT_TRUE(segments.value().at(2).data() == input.data() + 15);
}
