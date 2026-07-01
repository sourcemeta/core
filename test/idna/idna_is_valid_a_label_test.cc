#include <sourcemeta/core/test.h>

#include <sourcemeta/core/idna.h>

#include <string> // std::string

TEST(munich_german) {
  // xn--mnchen-3ya decodes to "M\u00FCnchen"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--mnchen-3ya"));
}

TEST(volos_greek) {
  // xn--nxasmq6b decodes to "\u03B2\u03CC\u03BB\u03BF\u03C2"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--nxasmq6b"));
}

TEST(deja_french) {
  // xn--dj-kia8a decodes to "d\u00E9j\u00E0"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--dj-kia8a"));
}

TEST(missing_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("abc"));
}

TEST(partial_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn-abc"));
}

TEST(empty_input) { EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("")); }

TEST(only_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--"));
}

TEST(decodes_to_pure_ascii) {
  // The body "abc-" is a Punycode encoding whose result has no non-ASCII
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--abc-"));
}

TEST(malformed_punycode_body) {
  // A trailing hyphen with garbage extension chars is invalid Punycode
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--abc-zzzzzzzz!"));
}

TEST(non_ascii_byte_in_input) {
  // U+00E4 ä is non-ASCII, so the whole input fails the ASCII requirement
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--\u00E4"));
}

TEST(uppercase_in_body) {
  // Punycode is case-insensitive but A-labels are conventionally lowercase.
  // An uppercase letter in the Punycode body is not the canonical
  // representation, so the round-trip check rejects it.
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--MNCHEN-3ya"));
}

// RFC 5890 §2.3.2.1: a label in A-label form is at most 63 octets. A 64-octet
// input is rejected on length alone, before any Punycode decoding
TEST(exceeds_63_octets) {
  const std::string label{"xn--" + std::string(60, 'a')};
  EXPECT_EQ(label.size(), 64);
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label(label));
}
