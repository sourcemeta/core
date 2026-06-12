#include <gtest/gtest.h>

#include <sourcemeta/core/crypto.h>

#include <sstream> // std::ostringstream
#include <string>  // std::string

// RFC 4648 Section 10 test vectors
// See https://www.rfc-editor.org/rfc/rfc4648#section-10

TEST(Crypto_base64, encode_rfc4648_empty) {
  EXPECT_EQ(sourcemeta::core::base64_encode(""), "");
}

TEST(Crypto_base64, encode_rfc4648_f) {
  EXPECT_EQ(sourcemeta::core::base64_encode("f"), "Zg==");
}

TEST(Crypto_base64, encode_rfc4648_fo) {
  EXPECT_EQ(sourcemeta::core::base64_encode("fo"), "Zm8=");
}

TEST(Crypto_base64, encode_rfc4648_foo) {
  EXPECT_EQ(sourcemeta::core::base64_encode("foo"), "Zm9v");
}

TEST(Crypto_base64, encode_rfc4648_foob) {
  EXPECT_EQ(sourcemeta::core::base64_encode("foob"), "Zm9vYg==");
}

TEST(Crypto_base64, encode_rfc4648_fooba) {
  EXPECT_EQ(sourcemeta::core::base64_encode("fooba"), "Zm9vYmE=");
}

TEST(Crypto_base64, encode_rfc4648_foobar) {
  EXPECT_EQ(sourcemeta::core::base64_encode("foobar"), "Zm9vYmFy");
}

TEST(Crypto_base64, encode_all_high_bytes) {
  EXPECT_EQ(sourcemeta::core::base64_encode("\xFF\xFF\xFF"), "////");
}

TEST(Crypto_base64, encode_single_high_byte) {
  EXPECT_EQ(sourcemeta::core::base64_encode("\xF8"), "+A==");
}

TEST(Crypto_base64, encode_embedded_nul_byte) {
  const std::string input("a\x00"
                          "b",
                          3);
  EXPECT_EQ(sourcemeta::core::base64_encode(input), "YQBi");
}

TEST(Crypto_base64, encode_stream_overload) {
  std::ostringstream output;
  sourcemeta::core::base64_encode("foobar", output);
  EXPECT_EQ(output.str(), "Zm9vYmFy");
}

TEST(Crypto_base64, encode_stream_overload_empty) {
  std::ostringstream output;
  sourcemeta::core::base64_encode("", output);
  EXPECT_EQ(output.str(), "");
}

TEST(Crypto_base64, decode_rfc4648_empty) {
  const auto result{sourcemeta::core::base64_decode("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(Crypto_base64, decode_rfc4648_f) {
  const auto result{sourcemeta::core::base64_decode("Zg==")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "f");
}

TEST(Crypto_base64, decode_rfc4648_fo) {
  const auto result{sourcemeta::core::base64_decode("Zm8=")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "fo");
}

TEST(Crypto_base64, decode_rfc4648_foo) {
  const auto result{sourcemeta::core::base64_decode("Zm9v")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}

TEST(Crypto_base64, decode_rfc4648_foob) {
  const auto result{sourcemeta::core::base64_decode("Zm9vYg==")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foob");
}

TEST(Crypto_base64, decode_rfc4648_fooba) {
  const auto result{sourcemeta::core::base64_decode("Zm9vYmE=")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "fooba");
}

TEST(Crypto_base64, decode_rfc4648_foobar) {
  const auto result{sourcemeta::core::base64_decode("Zm9vYmFy")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foobar");
}

TEST(Crypto_base64, decode_all_high_bytes) {
  const auto result{sourcemeta::core::base64_decode("////")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF\xFF\xFF");
}

TEST(Crypto_base64, decode_single_high_byte) {
  const auto result{sourcemeta::core::base64_decode("+A==")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xF8");
}

TEST(Crypto_base64, decode_nul_byte) {
  const auto result{sourcemeta::core::base64_decode("AA==")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x00", 1);
  EXPECT_EQ(result.value(), expected);
}

TEST(Crypto_base64, decode_rejects_length_one) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Z").has_value());
}

TEST(Crypto_base64, decode_rejects_missing_padding) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zg").has_value());
}

TEST(Crypto_base64, decode_rejects_partial_padding) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zm8").has_value());
}

TEST(Crypto_base64, decode_rejects_invalid_character) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zm!v").has_value());
}

TEST(Crypto_base64, decode_rejects_interior_space) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zm 9").has_value());
}

TEST(Crypto_base64, decode_rejects_interior_newline) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zm\n9").has_value());
}

TEST(Crypto_base64, decode_rejects_url_safe_minus) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("-A==").has_value());
}

TEST(Crypto_base64, decode_rejects_url_safe_underscore) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("_A==").has_value());
}

TEST(Crypto_base64, decode_rejects_padding_only) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("====").has_value());
}

TEST(Crypto_base64, decode_rejects_interior_padding) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Z=g=").has_value());
}

TEST(Crypto_base64, decode_rejects_triple_padding) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Z===").has_value());
}

TEST(Crypto_base64, decode_rejects_padding_across_groups) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zg==Zg==").has_value());
}

// RFC 4648 Section 3.5: "Implementations MAY chose to reject the encoding if
// the pad bits have not been set to zero". We reject so that every value has
// exactly one accepted encoding
TEST(Crypto_base64, decode_rejects_nonzero_pad_bits_two_characters) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zh==").has_value());
}

TEST(Crypto_base64, decode_rejects_nonzero_pad_bits_three_characters) {
  EXPECT_FALSE(sourcemeta::core::base64_decode("Zm9=").has_value());
}
