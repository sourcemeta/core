#include <gtest/gtest.h>

#include <sourcemeta/core/idna.h>

TEST(IDNA_is_valid_a_label, munich_german) {
  // xn--mnchen-3ya decodes to "M\u00FCnchen"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--mnchen-3ya"));
}

TEST(IDNA_is_valid_a_label, volos_greek) {
  // xn--nxasmq6b decodes to "\u03B2\u03CC\u03BB\u03BF\u03C2"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--nxasmq6b"));
}

TEST(IDNA_is_valid_a_label, deja_french) {
  // xn--dj-kia8a decodes to "d\u00E9j\u00E0"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--dj-kia8a"));
}

TEST(IDNA_is_valid_a_label, missing_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("abc"));
}

TEST(IDNA_is_valid_a_label, partial_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn-abc"));
}

TEST(IDNA_is_valid_a_label, empty_input) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label(""));
}

TEST(IDNA_is_valid_a_label, only_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--"));
}

TEST(IDNA_is_valid_a_label, decodes_to_pure_ascii) {
  // The body "abc-" is a Punycode encoding whose result has no non-ASCII
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--abc-"));
}

TEST(IDNA_is_valid_a_label, malformed_punycode_body) {
  // A trailing hyphen with garbage extension chars is invalid Punycode
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--abc-zzzzzzzz!"));
}

TEST(IDNA_is_valid_a_label, non_ascii_byte_in_input) {
  // U+00E4 ä is non-ASCII, so the whole input fails the ASCII requirement
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--\u00E4"));
}

TEST(IDNA_is_valid_a_label, uppercase_in_body) {
  // Punycode is case-insensitive but A-labels are conventionally lowercase.
  // An uppercase letter in the Punycode body is not the canonical
  // representation, so the round-trip check rejects it.
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("xn--MNCHEN-3ya"));
}
