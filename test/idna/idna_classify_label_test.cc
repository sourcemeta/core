#include <sourcemeta/core/test.h>

#include <sourcemeta/core/idna.h>

#include <optional>
#include <string>

TEST(empty_label_returns_nullopt) {
  std::u32string decoded;
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"", decoded), std::nullopt);
}

TEST(pure_ascii_letters_classify_as_ascii) {
  std::u32string decoded;
  const auto kind{sourcemeta::core::idna_classify_label(U"example", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::Ascii);
  EXPECT_EQ(decoded, U"example");
}

TEST(ascii_with_hyphen_classify_as_ascii) {
  std::u32string decoded;
  const auto kind{sourcemeta::core::idna_classify_label(U"host-name", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::Ascii);
  EXPECT_EQ(decoded, U"host-name");
}

// RFC 5891 §4.2.3.1: Ascii classification does not enforce the U-label
// "no consecutive hyphens at positions 3,4" rule. That is the caller's job.
TEST(ascii_with_consecutive_hyphens_classify_as_ascii) {
  std::u32string decoded;
  const auto kind{sourcemeta::core::idna_classify_label(U"a--b", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::Ascii);
  EXPECT_EQ(decoded, U"a--b");
}

// RFC 5890 §2.3.2.1: ACE prefix "xn--" detection is case-insensitive
TEST(valid_a_label_lowercase_prefix) {
  std::u32string decoded;
  const auto kind{
      sourcemeta::core::idna_classify_label(U"xn--mnchen-3ya", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::ALabel);
  // U+00FC LATIN SMALL LETTER U WITH DIAERESIS
  EXPECT_EQ(decoded, U"m\u00FCnchen");
}

TEST(valid_a_label_uppercase_prefix) {
  std::u32string decoded;
  const auto kind{
      sourcemeta::core::idna_classify_label(U"XN--mnchen-3ya", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::ALabel);
  EXPECT_EQ(decoded, U"m\u00FCnchen");
}

TEST(valid_a_label_mixed_case_prefix) {
  std::u32string decoded;
  const auto kind{
      sourcemeta::core::idna_classify_label(U"Xn--mnchen-3ya", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::ALabel);
  EXPECT_EQ(decoded, U"m\u00FCnchen");
}

// RFC 5891 §4.2: A-label Punycode body must round-trip
TEST(invalid_a_label_decoded_violates_u_label_rules) {
  std::u32string decoded;
  // Body decodes to "aa--點看" which has "--" at positions 3 and 4
  EXPECT_EQ(
      sourcemeta::core::idna_classify_label(U"xn--aa---o47jg78q", decoded),
      std::nullopt);
}

TEST(invalid_a_label_empty_body) {
  std::u32string decoded;
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"xn--", decoded),
            std::nullopt);
}

TEST(invalid_a_label_garbage_body) {
  std::u32string decoded;
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"xn--X", decoded),
            std::nullopt);
}

// RFC 5890 §2.3.2.2: U-label has at least one non-ASCII codepoint
TEST(valid_u_label_two_byte) {
  std::u32string decoded;
  const auto kind{sourcemeta::core::idna_classify_label(U"\u03B1", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::ULabel);
  EXPECT_EQ(decoded, U"\u03B1");
}

TEST(valid_u_label_mixed_pvalid_codepoints) {
  std::u32string decoded;
  const auto kind{
      sourcemeta::core::idna_classify_label(U"m\u00FCnchen", decoded)};
  EXPECT_TRUE(kind.has_value());
  EXPECT_EQ(*kind, sourcemeta::core::IDNALabelKind::ULabel);
  EXPECT_EQ(decoded, U"m\u00FCnchen");
}

// RFC 5891 §4.2.3.1: U-label must not start with a hyphen
TEST(invalid_u_label_leading_hyphen) {
  std::u32string decoded;
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"-\u03B1", decoded),
            std::nullopt);
}

// RFC 5891 §4.2.3.2: U-label first codepoint must not be a combining mark
TEST(invalid_u_label_leading_combining_mark) {
  std::u32string decoded;
  // U+0300 COMBINING GRAVE ACCENT
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"\u0300hello", decoded),
            std::nullopt);
}

// RFC 5892: DISALLOWED codepoint rejects
TEST(invalid_u_label_disallowed_codepoint) {
  std::u32string decoded;
  // U+302E HANGUL SINGLE DOT TONE MARK is DISALLOWED
  EXPECT_EQ(sourcemeta::core::idna_classify_label(U"a\u302Eb", decoded),
            std::nullopt);
}
