#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

auto bytes(const std::string_view hexadecimal) -> std::string {
  return sourcemeta::core::hex_to_bytes(hexadecimal).value();
}

// RFC 5869 §2.3: the expand step emits at most 255 blocks of the hash length
constexpr std::size_t MAXIMUM_SHA256_OUTPUT{std::size_t{255} * 32};
constexpr std::size_t MAXIMUM_SHA384_OUTPUT{std::size_t{255} * 48};
constexpr std::size_t MAXIMUM_SHA512_OUTPUT{std::size_t{255} * 64};

template <std::size_t Size>
auto to_hex(const std::array<std::uint8_t, Size> &digest) -> std::string {
  return sourcemeta::core::bytes_to_hex(
      {reinterpret_cast<const char *>(digest.data()), digest.size()});
}

// RFC 5869 Appendix A.1: basic test case with SHA-256
constexpr std::string_view CASE_1_IKM{
    "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"};
constexpr std::string_view CASE_1_SALT{"000102030405060708090a0b0c"};
constexpr std::string_view CASE_1_INFO{"f0f1f2f3f4f5f6f7f8f9"};
constexpr std::string_view CASE_1_PRK{"077709362c2e32df0ddc3f0dc47bba63"
                                      "90b6c73bb50f9c3122ec844ad7c2b3e5"};
constexpr std::string_view CASE_1_OKM{"3cb25f25faacd57a90434f64d0362f2a"
                                      "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
                                      "34007208d5b887185865"};

// RFC 5869 Appendix A.2: test with SHA-256 and longer inputs/outputs
constexpr std::string_view CASE_2_IKM{"000102030405060708090a0b0c0d0e0f"
                                      "101112131415161718191a1b1c1d1e1f"
                                      "202122232425262728292a2b2c2d2e2f"
                                      "303132333435363738393a3b3c3d3e3f"
                                      "404142434445464748494a4b4c4d4e4f"};
constexpr std::string_view CASE_2_SALT{"606162636465666768696a6b6c6d6e6f"
                                       "707172737475767778797a7b7c7d7e7f"
                                       "808182838485868788898a8b8c8d8e8f"
                                       "909192939495969798999a9b9c9d9e9f"
                                       "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"};
constexpr std::string_view CASE_2_INFO{"b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
                                       "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
                                       "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
                                       "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
                                       "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"};
constexpr std::string_view CASE_2_PRK{"06a6b88c5853361a06104c9ceb35b45c"
                                      "ef760014904671014a193f40c15fc244"};
constexpr std::string_view CASE_2_OKM{"b11e398dc80327a1c8e7f78c596a4934"
                                      "4f012eda2d4efad8a050cc4c19afa97c"
                                      "59045a99cac7827271cb41c65e590e09"
                                      "da3275600c2f09b8367793a9aca3db71"
                                      "cc30c58179ec3e87c14c01d5c1f3434f"
                                      "1d87"};

// RFC 5869 Appendix A.3: test with SHA-256 and zero-length salt/info
constexpr std::string_view CASE_3_IKM{
    "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"};
constexpr std::string_view CASE_3_PRK{"19ef24a32c717b167f33a91d6f648bdf"
                                      "96596776afdb6377ac434c1c293ccb04"};
constexpr std::string_view CASE_3_OKM{"8da4e775a563c18f715f802a063c5a31"
                                      "b8a11f5c5ee1879ec3454e5f3c738d2d"
                                      "9d201395faa4b61a96c8"};

} // namespace

TEST(hkdf_sha256_rfc5869_case_1_extract) {
  const auto prk{sourcemeta::core::hkdf_sha256_extract(bytes(CASE_1_SALT),
                                                       bytes(CASE_1_IKM))};
  EXPECT_EQ(to_hex(prk), CASE_1_PRK);
}

TEST(hkdf_sha256_rfc5869_case_1_expand) {
  const auto okm{sourcemeta::core::hkdf_sha256_expand(bytes(CASE_1_PRK),
                                                      bytes(CASE_1_INFO), 42)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_1_OKM);
}

TEST(hkdf_sha256_rfc5869_case_1) {
  const auto okm{sourcemeta::core::hkdf_sha256(
      bytes(CASE_1_IKM), bytes(CASE_1_SALT), bytes(CASE_1_INFO), 42)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_1_OKM);
}

TEST(hkdf_sha256_rfc5869_case_2_extract) {
  const auto prk{sourcemeta::core::hkdf_sha256_extract(bytes(CASE_2_SALT),
                                                       bytes(CASE_2_IKM))};
  EXPECT_EQ(to_hex(prk), CASE_2_PRK);
}

TEST(hkdf_sha256_rfc5869_case_2_expand) {
  const auto okm{sourcemeta::core::hkdf_sha256_expand(bytes(CASE_2_PRK),
                                                      bytes(CASE_2_INFO), 82)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_2_OKM);
}

TEST(hkdf_sha256_rfc5869_case_2) {
  const auto okm{sourcemeta::core::hkdf_sha256(
      bytes(CASE_2_IKM), bytes(CASE_2_SALT), bytes(CASE_2_INFO), 82)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_2_OKM);
}

// RFC 5869 §2.2: an absent salt is set to a string of HashLen zeros, which the
// empty salt of this case exercises
TEST(hkdf_sha256_rfc5869_case_3_extract) {
  const auto prk{sourcemeta::core::hkdf_sha256_extract("", bytes(CASE_3_IKM))};
  EXPECT_EQ(to_hex(prk), CASE_3_PRK);
}

TEST(hkdf_sha256_rfc5869_case_3_expand) {
  const auto okm{
      sourcemeta::core::hkdf_sha256_expand(bytes(CASE_3_PRK), "", 42)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_3_OKM);
}

TEST(hkdf_sha256_rfc5869_case_3) {
  const auto okm{sourcemeta::core::hkdf_sha256(bytes(CASE_3_IKM), "", "", 42)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(sourcemeta::core::bytes_to_hex(okm.value()), CASE_3_OKM);
}

// RFC 5869 §2.2: an absent salt is equivalent to HashLen zeros, so spelling
// those zeros out reaches the same pseudorandom key
TEST(hkdf_sha256_absent_salt_equals_a_zero_filled_one) {
  const std::string zeros(32, '\x00');
  EXPECT_EQ(
      to_hex(sourcemeta::core::hkdf_sha256_extract("", bytes(CASE_3_IKM))),
      to_hex(sourcemeta::core::hkdf_sha256_extract(zeros, bytes(CASE_3_IKM))));
}

// RFC 5869 §2.3: L is at most 255*HashLen octets
TEST(hkdf_sha256_maximum_output_length) {
  const auto okm{sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "",
                                               MAXIMUM_SHA256_OUTPUT)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(okm.value().size(), MAXIMUM_SHA256_OUTPUT);
}

TEST(hkdf_sha256_output_length_past_the_maximum) {
  EXPECT_FALSE(sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "",
                                             MAXIMUM_SHA256_OUTPUT + 1)
                   .has_value());
}

TEST(hkdf_sha256_expand_output_length_past_the_maximum) {
  EXPECT_FALSE(sourcemeta::core::hkdf_sha256_expand(bytes(CASE_1_PRK), "",
                                                    MAXIMUM_SHA256_OUTPUT + 1)
                   .has_value());
}

// RFC 5869 §2.3: the pseudorandom key is at least HashLen octets
TEST(hkdf_sha256_expand_rejects_a_short_pseudorandom_key) {
  const std::string key(31, '\x0b');
  EXPECT_FALSE(sourcemeta::core::hkdf_sha256_expand(key, "", 42).has_value());
}

TEST(hkdf_sha256_zero_output_length) {
  const auto okm{sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "", 0)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_TRUE(okm.value().empty());
}

// RFC 5869 §3.2: the info input separates derivations from one secret
TEST(hkdf_sha256_info_separates_derivations) {
  const auto first{
      sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "encryption", 32)};
  const auto second{
      sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "signing", 32)};
  EXPECT_TRUE(first.has_value());
  EXPECT_TRUE(second.has_value());
  EXPECT_NE(first.value(), second.value());
}

TEST(hkdf_sha384_extract_produces_a_digest_sized_key) {
  const auto prk{sourcemeta::core::hkdf_sha384_extract(bytes(CASE_1_SALT),
                                                       bytes(CASE_1_IKM))};
  EXPECT_EQ(prk.size(), static_cast<std::size_t>(48));
}

TEST(hkdf_sha384_maximum_output_length) {
  const auto okm{sourcemeta::core::hkdf_sha384(bytes(CASE_1_IKM), "", "",
                                               MAXIMUM_SHA384_OUTPUT)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(okm.value().size(), MAXIMUM_SHA384_OUTPUT);
}

TEST(hkdf_sha384_output_length_past_the_maximum) {
  EXPECT_FALSE(sourcemeta::core::hkdf_sha384(bytes(CASE_1_IKM), "", "",
                                             MAXIMUM_SHA384_OUTPUT + 1)
                   .has_value());
}

TEST(hkdf_sha512_extract_produces_a_digest_sized_key) {
  const auto prk{sourcemeta::core::hkdf_sha512_extract(bytes(CASE_1_SALT),
                                                       bytes(CASE_1_IKM))};
  EXPECT_EQ(prk.size(), static_cast<std::size_t>(64));
}

TEST(hkdf_sha512_maximum_output_length) {
  const auto okm{sourcemeta::core::hkdf_sha512(bytes(CASE_1_IKM), "", "",
                                               MAXIMUM_SHA512_OUTPUT)};
  EXPECT_TRUE(okm.has_value());
  EXPECT_EQ(okm.value().size(), MAXIMUM_SHA512_OUTPUT);
}

TEST(hkdf_sha512_output_length_past_the_maximum) {
  EXPECT_FALSE(sourcemeta::core::hkdf_sha512(bytes(CASE_1_IKM), "", "",
                                             MAXIMUM_SHA512_OUTPUT + 1)
                   .has_value());
}

// Each width is a distinct derivation from the same inputs
TEST(hkdf_widths_do_not_agree) {
  const auto sha256{
      sourcemeta::core::hkdf_sha256(bytes(CASE_1_IKM), "", "", 32)};
  const auto sha384{
      sourcemeta::core::hkdf_sha384(bytes(CASE_1_IKM), "", "", 32)};
  const auto sha512{
      sourcemeta::core::hkdf_sha512(bytes(CASE_1_IKM), "", "", 32)};
  EXPECT_NE(sha256.value(), sha384.value());
  EXPECT_NE(sha384.value(), sha512.value());
}
