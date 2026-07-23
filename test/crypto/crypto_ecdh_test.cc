#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {
// A pair of P-256 keys and their agreed secret, generated out of band
const std::string A_SCALAR{
    "2f2ca554ea1dabab39212f78b4238238b64b246ba7573cd76ef3f8d6705dfde0"};
const std::string A_COORDINATE_X{
    "7ba511d9f36a052945e3229573db7d5ed17141897e58e9f64abb5464264b7d56"};
const std::string A_COORDINATE_Y{
    "d5ee7f014ea98dadab10cb47e6f7ff423a10e02540194e642354b1f56bc17c94"};
const std::string B_SCALAR{
    "a80aeab3dca90d227049a8197f3198160b1f38bcfbc08955411e1c254858ca6a"};
const std::string B_COORDINATE_X{
    "6b307d19db9d33d779644c95aca80f23738254deeee8266bc1e4b134686f2c4b"};
const std::string B_COORDINATE_Y{
    "b1aa1b614cd45a38b79f75ace6489ff60852cb6c05369034d1729f2d2ec911ac"};
const std::string SHARED_SECRET{
    "07915976af1a99f773825059cd8ca4b1ee2c7dde4bac4d8d0b4175d9760b14e0"};
const std::string P384_COORDINATE_X{
    "fbc48e6c25b8b4a04f9fe830dc558906c972118856aef6dd154c7f2d8d0beddb"
    "d7a91430e0ef3a583eaad396d8d27651"};
const std::string P384_COORDINATE_Y{
    "b941e926ead50d193eae772510b2baca2cf0866eb438938b4c38a2b7f124fae4"
    "9c698ac2524b3e4ded5d1098d0e5fac5"};

auto p256_private_key(const std::string_view scalar, const std::string_view x,
                      const std::string_view y)
    -> std::optional<sourcemeta::core::PrivateKey> {
  return sourcemeta::core::make_ec_private_key(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::hex_to_bytes(scalar).value(),
      sourcemeta::core::hex_to_bytes(x).value(),
      sourcemeta::core::hex_to_bytes(y).value());
}

auto p256_public_key(const std::string_view x, const std::string_view y)
    -> std::optional<sourcemeta::core::PublicKey> {
  return sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P256,
      sourcemeta::core::hex_to_bytes(x).value(),
      sourcemeta::core::hex_to_bytes(y).value());
}
} // namespace

TEST(kdf_concat_derives_the_rfc7518_appendix_c_key) {
  // RFC 7518 Appendix C worked example, deriving a 128-bit key from the shared
  // secret Z with the algorithm identifier "A128GCM" and the party information
  // "Alice" and "Bob"
  const auto shared_secret{sourcemeta::core::hex_to_bytes(
      "9e56d91d817135d372834283bf84269cfb316ea3da806a48f6daa7798cfe90c4")};
  EXPECT_TRUE(shared_secret.has_value());
  const auto derived{sourcemeta::core::kdf_concat(
      shared_secret.value(), "A128GCM", "Alice", "Bob", 16)};
  const auto expected{
      sourcemeta::core::hex_to_bytes("56aa8deaf8236d205c2228cd71a7101a")};
  EXPECT_TRUE(expected.has_value());
  EXPECT_EQ(derived, expected.value());
}

TEST(kdf_concat_produces_the_requested_length) {
  const std::string shared_secret(32, '\x2a');
  const auto derived{
      sourcemeta::core::kdf_concat(shared_secret, "A256GCM", "", "", 32)};
  EXPECT_EQ(derived.size(), std::string::size_type{32});
}

TEST(kdf_concat_spans_multiple_hash_blocks) {
  // A derived length beyond the SHA-256 output forces the counter past its
  // first value, so the result must still match the requested length and stay
  // deterministic
  const std::string shared_secret(32, '\x2a');
  const auto first{
      sourcemeta::core::kdf_concat(shared_secret, "A256CBC-HS512", "", "", 48)};
  const auto second{
      sourcemeta::core::kdf_concat(shared_secret, "A256CBC-HS512", "", "", 48)};
  EXPECT_EQ(first.size(), std::string::size_type{48});
  EXPECT_EQ(first, second);
}

TEST(kdf_concat_diverges_on_the_algorithm_identifier) {
  const std::string shared_secret(32, '\x2a');
  const auto first{
      sourcemeta::core::kdf_concat(shared_secret, "A128GCM", "", "", 16)};
  const auto second{
      sourcemeta::core::kdf_concat(shared_secret, "A192GCM", "", "", 16)};
  EXPECT_NE(first, second);
}

TEST(kdf_concat_diverges_on_the_party_information) {
  const std::string shared_secret(32, '\x2a');
  const auto first{sourcemeta::core::kdf_concat(shared_secret, "A128GCM",
                                                "Alice", "Bob", 16)};
  const auto second{sourcemeta::core::kdf_concat(shared_secret, "A128GCM",
                                                 "Bob", "Alice", 16)};
  EXPECT_NE(first, second);
}

TEST(ecdh_derive_matches_a_known_p256_shared_secret) {
  const auto private_key{
      p256_private_key(A_SCALAR, A_COORDINATE_X, A_COORDINATE_Y)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{p256_public_key(B_COORDINATE_X, B_COORDINATE_Y)};
  EXPECT_TRUE(public_key.has_value());
  const auto secret{
      sourcemeta::core::ecdh_derive(private_key.value(), public_key.value())};
  EXPECT_TRUE(secret.has_value());
  EXPECT_EQ(secret.value(),
            sourcemeta::core::hex_to_bytes(SHARED_SECRET).value());
}

TEST(ecdh_derive_is_symmetric) {
  const auto private_key{
      p256_private_key(B_SCALAR, B_COORDINATE_X, B_COORDINATE_Y)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{p256_public_key(A_COORDINATE_X, A_COORDINATE_Y)};
  EXPECT_TRUE(public_key.has_value());
  const auto secret{
      sourcemeta::core::ecdh_derive(private_key.value(), public_key.value())};
  EXPECT_TRUE(secret.has_value());
  EXPECT_EQ(secret.value(),
            sourcemeta::core::hex_to_bytes(SHARED_SECRET).value());
}

TEST(ecdh_derive_rejects_a_mismatched_curve) {
  const auto private_key{
      p256_private_key(A_SCALAR, A_COORDINATE_X, A_COORDINATE_Y)};
  EXPECT_TRUE(private_key.has_value());
  const auto public_key{sourcemeta::core::make_ec_public_key(
      sourcemeta::core::EllipticCurve::P384,
      sourcemeta::core::hex_to_bytes(P384_COORDINATE_X).value(),
      sourcemeta::core::hex_to_bytes(P384_COORDINATE_Y).value())};
  EXPECT_TRUE(public_key.has_value());
  EXPECT_FALSE(
      sourcemeta::core::ecdh_derive(private_key.value(), public_key.value())
          .has_value());
}
