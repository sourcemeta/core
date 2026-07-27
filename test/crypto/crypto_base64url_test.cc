#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

// RFC 4648 Section 10 test vectors
// See https://www.rfc-editor.org/rfc/rfc4648#section-10

TEST(encode_rfc4648_empty) {
  EXPECT_EQ(sourcemeta::core::base64url_encode(""), "");
}

TEST(encode_rfc4648_f) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("f"), "Zg");
}

TEST(encode_rfc4648_fo) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("fo"), "Zm8");
}

TEST(encode_rfc4648_foo) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("foo"), "Zm9v");
}

TEST(encode_rfc4648_foob) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("foob"), "Zm9vYg");
}

TEST(encode_rfc4648_fooba) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("fooba"), "Zm9vYmE");
}

TEST(encode_rfc4648_foobar) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("foobar"), "Zm9vYmFy");
}

// RFC 7515 Appendix C example octets
// See https://www.rfc-editor.org/rfc/rfc7515#appendix-C
TEST(encode_rfc7515_appendix_c) {
  const std::string input("\x03\xEC\xFF\xE0\xC1", 5);
  EXPECT_EQ(sourcemeta::core::base64url_encode(input), "A-z_4ME");
}

TEST(encode_all_high_bytes) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("\xFF\xFF\xFF"), "____");
}

TEST(encode_single_high_byte) {
  EXPECT_EQ(sourcemeta::core::base64url_encode("\xF8"), "-A");
}

TEST(encode_stream_overload) {
  std::ostringstream output;
  sourcemeta::core::base64url_encode("foobar", output);
  EXPECT_EQ(output.str(), "Zm9vYmFy");
}

TEST(encode_stream_overload_empty) {
  std::ostringstream output;
  sourcemeta::core::base64url_encode("", output);
  EXPECT_EQ(output.str(), "");
}

TEST(encode_secure_string_overload_appends) {
  sourcemeta::core::SecureString output{"prefix:"};
  sourcemeta::core::base64url_encode("foobar", output);
  EXPECT_TRUE(output == "prefix:Zm9vYmFy");
}

TEST(encode_secure_string_overload_empty) {
  sourcemeta::core::SecureString output;
  sourcemeta::core::base64url_encode("", output);
  EXPECT_TRUE(output.empty());
}

TEST(encode_bytes_overload_foobar) {
  const std::array<std::uint8_t, 6> input{{'f', 'o', 'o', 'b', 'a', 'r'}};
  EXPECT_EQ(sourcemeta::core::base64url_encode(input), "Zm9vYmFy");
}

TEST(encode_bytes_overload_empty) {
  const std::array<std::uint8_t, 0> input{};
  EXPECT_EQ(sourcemeta::core::base64url_encode(input), "");
}

TEST(encode_bytes_overload_high_bytes) {
  const std::array<std::uint8_t, 3> input{{0xFF, 0xFF, 0xFF}};
  EXPECT_EQ(sourcemeta::core::base64url_encode(input), "____");
}

TEST(encode_bytes_overload_matches_the_string_view_form) {
  const auto digest{sourcemeta::core::sha256_digest("fo")};
  EXPECT_EQ(sourcemeta::core::base64url_encode(digest),
            sourcemeta::core::base64url_encode(std::string_view{
                reinterpret_cast<const char *>(digest.data()), digest.size()}));
}

TEST(decode_rfc4648_empty) {
  const auto result{sourcemeta::core::base64url_decode("")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(decode_rfc4648_f) {
  const auto result{sourcemeta::core::base64url_decode("Zg")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "f");
}

TEST(decode_rfc4648_fo) {
  const auto result{sourcemeta::core::base64url_decode("Zm8")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "fo");
}

TEST(decode_rfc4648_foo) {
  const auto result{sourcemeta::core::base64url_decode("Zm9v")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foo");
}

TEST(decode_rfc4648_foob) {
  const auto result{sourcemeta::core::base64url_decode("Zm9vYg")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foob");
}

TEST(decode_rfc4648_fooba) {
  const auto result{sourcemeta::core::base64url_decode("Zm9vYmE")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "fooba");
}

TEST(decode_rfc4648_foobar) {
  const auto result{sourcemeta::core::base64url_decode("Zm9vYmFy")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "foobar");
}

// RFC 7515 Appendix C example octets
// See https://www.rfc-editor.org/rfc/rfc7515#appendix-C
TEST(decode_rfc7515_appendix_c) {
  const auto result{sourcemeta::core::base64url_decode("A-z_4ME")};
  EXPECT_TRUE(result.has_value());
  const std::string expected("\x03\xEC\xFF\xE0\xC1", 5);
  EXPECT_EQ(result.value(), expected);
}

TEST(decode_all_high_bytes) {
  const auto result{sourcemeta::core::base64url_decode("____")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xFF\xFF\xFF");
}

TEST(decode_single_high_byte) {
  const auto result{sourcemeta::core::base64url_decode("-A")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "\xF8");
}

TEST(decode_rejects_length_one) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Z").has_value());
}

TEST(decode_rejects_padding) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Zg==").has_value());
}

TEST(decode_rejects_single_padding_character) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Zm8=").has_value());
}

TEST(decode_rejects_plus) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("+A").has_value());
}

TEST(decode_rejects_slash) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("AA//").has_value());
}

TEST(decode_rejects_invalid_character) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Z!").has_value());
}

TEST(decode_rejects_interior_space) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Zm 9").has_value());
}

// RFC 4648 Section 3.5: "Implementations MAY chose to reject the encoding if
// the pad bits have not been set to zero". We reject so that every value has
// exactly one accepted encoding
TEST(decode_rejects_nonzero_pad_bits_two_characters) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Zh").has_value());
}

TEST(decode_rejects_nonzero_pad_bits_three_characters) {
  EXPECT_FALSE(sourcemeta::core::base64url_decode("Zm9").has_value());
}
