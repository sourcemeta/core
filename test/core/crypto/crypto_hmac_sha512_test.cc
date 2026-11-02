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
  EXPECT_EQ(sourcemeta::core::hmac_sha512(key, "Hi There"),
            "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cde"
            "daa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(rfc4231_case_2) {
  EXPECT_EQ(
      sourcemeta::core::hmac_sha512("Jefe", "what do ya want for nothing?"),
      "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea250554"
      "9758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737");
}

// RFC 4231 Section 4.4 (Test Case 3)
TEST(rfc4231_case_3) {
  const std::string key(20, '\xaa');
  const std::string message(50, '\xdd');
  EXPECT_EQ(sourcemeta::core::hmac_sha512(key, message),
            "fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39"
            "bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb");
}

// RFC 4231 Section 4.5 (Test Case 4)
TEST(rfc4231_case_4) {
  const auto key{sourcemeta::core::hex_to_bytes(
                     "0102030405060708090a0b0c0d0e0f10111213141516171819")
                     .value()};
  const std::string message(50, '\xcd');
  EXPECT_EQ(sourcemeta::core::hmac_sha512(key, message),
            "b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3db"
            "a91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd");
}

// RFC 4231 Section 4.7 (Test Case 6), a key longer than the block size
TEST(rfc4231_case_6) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha512(
                key, "Test Using Larger Than Block-Size Key - Hash Key First"),
            "80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f352"
            "6b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598");
}

// RFC 4231 Section 4.8 (Test Case 7), a key longer than the block size
TEST(rfc4231_case_7) {
  const std::string key(131, '\xaa');
  EXPECT_EQ(sourcemeta::core::hmac_sha512(
                key, "This is a test using a larger than block-size key and a "
                     "larger than block-size data. The key needs to be hashed "
                     "before being used by the HMAC algorithm."),
            "e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944"
            "b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58");
}

TEST(ostream_case_2) {
  std::ostringstream result;
  sourcemeta::core::hmac_sha512("Jefe", "what do ya want for nothing?", result);
  EXPECT_EQ(result.str(),
            "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea250554"
            "9758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737");
}

// RFC 4231 Section 4.3 (Test Case 2)
TEST(digest_case_2) {
  const std::array<std::uint8_t, 64> expected{
      {0x16, 0x4b, 0x7a, 0x7b, 0xfc, 0xf8, 0x19, 0xe2, 0xe3, 0x95, 0xfb,
       0xe7, 0x3b, 0x56, 0xe0, 0xa3, 0x87, 0xbd, 0x64, 0x22, 0x2e, 0x83,
       0x1f, 0xd6, 0x10, 0x27, 0x0c, 0xd7, 0xea, 0x25, 0x05, 0x54, 0x97,
       0x58, 0xbf, 0x75, 0xc0, 0x5a, 0x99, 0x4a, 0x6d, 0x03, 0x4f, 0x65,
       0xf8, 0xf0, 0xe6, 0xfd, 0xca, 0xea, 0xb1, 0xa3, 0x4d, 0x4a, 0x6b,
       0x4b, 0x63, 0x6e, 0x07, 0x0a, 0x38, 0xbc, 0xe7, 0x37}};
  EXPECT_EQ(sourcemeta::core::hmac_sha512_digest(
                "Jefe", "what do ya want for nothing?"),
            expected);
}
