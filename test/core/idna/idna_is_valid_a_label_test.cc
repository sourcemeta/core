#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(munich_german) {
  // xn--mnchen-3ya decodes to "M\u00FCnchen"
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--mnchen-3ya"));
}

TEST(volos_greek) {
  // xn--nxasmq6b decodes to "\u03B2\u03CC\u03BB\u03BF\u03C3"
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

// RFC 5890 §2.3.2.4: A-labels "should be produced only in lowercase, despite
// matching other (mixed-case or uppercase) potential labels in the DNS", so
// lowercase is a producer convention rather than grounds for rejection
TEST(uppercase_in_body) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--MNCHEN-3ya"));
}

// RFC 5890 §2.3.2.1: a label in A-label form is at most 63 octets. A 64-octet
// input is rejected on length alone, before any Punycode decoding
TEST(exceeds_63_octets) {
  const std::string label{"xn--" + std::string(60, 'a')};
  EXPECT_EQ(label.size(), 64);
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label(label));
}

// RFC 3492 §5: "A decoder MUST recognize the letters in both uppercase and
// lowercase forms (including mixtures of both forms)"
TEST(uppercase_body) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--NXASMQ6B"));
}

TEST(mixed_case_body) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--NxAsMq6B"));
}

// RFC 5891 §5.3: the ACE prefix is "interpreted case-insensitively"
TEST(uppercase_prefix) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("XN--nxasmq6b"));
}

TEST(mixed_case_prefix) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("Xn--nxasmq6b"));
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xN--nxasmq6b"));
}

TEST(uppercase_prefix_and_body) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("XN--NXASMQ6B"));
}

// RFC 3492 §5: basic code points are copied verbatim by the decoder, so the
// literal portion of the body is lowercased before decoding too
TEST(uppercase_basic_portion) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--Mnchen-3ya"));
}

TEST(uppercase_extended_portion) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("xn--mnchen-3YA"));
}

TEST(uppercase_whole_label_with_basic_portion) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("XN--MNCHEN-3YA"));
}

TEST(uppercase_accented_label) {
  EXPECT_TRUE(sourcemeta::core::idna_is_valid_a_label("XN--DJ-KIA8A"));
}

// Case folding must not make an otherwise invalid label pass
TEST(uppercase_empty_body) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("XN--"));
}

TEST(uppercase_pure_ascii_decode) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("XN--ABC-"));
}

TEST(uppercase_leading_spacing_combining_mark) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("XN--HELLO-TXK"));
}

TEST(uppercase_non_ascii_body) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("XN--Ä"));
}

TEST(uppercase_partial_prefix) {
  EXPECT_FALSE(sourcemeta::core::idna_is_valid_a_label("XN-ABC"));
}
