#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstdint>     // std::uint8_t
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view

// RFC 4231 Section 4.2 (Test Case 1)
TEST(rfc4231_case_1) {
  const std::string key(20, '\x0b');
  EXPECT_EQ(sourcemeta::core::hmac_sha384(key, "Hi There"),
            "afd03944d84895626b0825f4ab46907f15f9dadbe4101ec682aa034c7cebc59c"
            "faea9ea9076ede7f4af152e8b2fa9cb6");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(rfc4231_case_2) {
  EXPECT_EQ(
      sourcemeta::core::hmac_sha384("Jefe", "what do ya want for nothing?"),
      "af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e"
      "8e2240ca5e69e2c78b3239ecfab21649");
}

// RFC 4231 Section 4.4 (Test Case 3)
TEST(rfc4231_case_3) {
  const std::string key(20, '\xaa');
  const std::string message(50, '\xdd');
  EXPECT_EQ(sourcemeta::core::hmac_sha384(key, message),
            "88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b"
            "2a5ab39dc13814b94e3ab6e101a34f27");
}

// RFC 4231 Section 4.5 (Test Case 4)
TEST(rfc4231_case_4) {
  const auto key{sourcemeta::core::hex_to_bytes(
                     "0102030405060708090a0b0c0d0e0f10111213141516171819")
                     .value()};
  const std::string message(50, '\xcd');
  EXPECT_EQ(sourcemeta::core::hmac_sha384(key, message),
            "3e8a69b7783c25851933ab6290af6ca77a9981480850009cc5577c6e1f573b4e"
            "6801dd23c4a7d679ccf8a386c674cffb");
}

// RFC 4231 Section 4.7 (Test Case 6), a key longer than the block size
TEST(rfc4231_case_6) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha384(
                key, "Test Using Larger Than Block-Size Key - Hash Key First"),
            "4ece084485813e9088d2c63a041bc5b44f9ef1012a2b588f3cd11f05033ac4c6"
            "0c2ef6ab4030fe8296248df163f44952");
}

// RFC 4231 Section 4.8 (Test Case 7), a key longer than the block size
TEST(rfc4231_case_7) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha384(
                key, "This is a test using a larger than block-size key and a "
                     "larger than block-size data. The key needs to be hashed "
                     "before being used by the HMAC algorithm."),
            "6617178e941f020d351e2f254e8fd32c602420feb0b8fb9adccebb82461e99c5"
            "a678cc31e799176d3860e6110c46523e");
}

TEST(ostream_case_2) {
  std::ostringstream result;
  sourcemeta::core::hmac_sha384("Jefe", "what do ya want for nothing?", result);
  EXPECT_EQ(result.str(),
            "af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e"
            "8e2240ca5e69e2c78b3239ecfab21649");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(digest_case_2) {
  const std::array<std::uint8_t, 48> expected{
      {0xaf, 0x45, 0xd2, 0xe3, 0x76, 0x48, 0x40, 0x31, 0x61, 0x7f, 0x78, 0xd2,
       0xb5, 0x8a, 0x6b, 0x1b, 0x9c, 0x7e, 0xf4, 0x64, 0xf5, 0xa0, 0x1b, 0x47,
       0xe4, 0x2e, 0xc3, 0x73, 0x63, 0x22, 0x44, 0x5e, 0x8e, 0x22, 0x40, 0xca,
       0x5e, 0x69, 0xe2, 0xc7, 0x8b, 0x32, 0x39, 0xec, 0xfa, 0xb2, 0x16, 0x49}};
  EXPECT_EQ(sourcemeta::core::hmac_sha384_digest(
                "Jefe", "what do ya want for nothing?"),
            expected);
}
