#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <string> // std::string

TEST(empty_input_returns_zero) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("", 0), 0u);
}

TEST(position_at_size_returns_zero) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("A", 1), 0u);
}

TEST(position_past_size_returns_zero) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("A", 5), 0u);
}

TEST(ascii_letter_returns_one) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("A", 0), 1u);
}

TEST(ascii_null_returns_one) {
  const std::string null_byte(1, '\0');
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length(null_byte, 0), 1u);
}

TEST(ascii_low_boundary_returns_one) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\x01", 0), 1u);
}

TEST(ascii_high_boundary_returns_one) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\x7f", 0), 1u);
}

TEST(ascii_digit_returns_one) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("0", 0), 1u);
}

TEST(ascii_space_returns_one) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length(" ", 0), 1u);
}

TEST(two_byte_lead_low_boundary) {
  // U+0080: \xC2\x80 (smallest 2-byte codepoint)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xc2\x80", 0), 2u);
}

TEST(two_byte_lead_high_boundary) {
  // U+07FF: \xDF\xBF (largest 2-byte codepoint)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xdf\xbf", 0), 2u);
}

TEST(two_byte_greek_alpha) {
  // U+03B1 GREEK SMALL ALPHA: \xCE\xB1
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xce\xb1", 0), 2u);
}

TEST(two_byte_latin_e_acute) {
  // U+00E9 LATIN SMALL E ACUTE: \xC3\xA9
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xc3\xa9", 0), 2u);
}

TEST(two_byte_overlong_c0_zero) {
  // %xC0 is forbidden (overlong encoding of U+0000)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xc0\x80", 0), 0u);
}

TEST(two_byte_overlong_c1_max) {
  // %xC1 is forbidden (overlong encoding of U+007F)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xc1\xbf", 0), 0u);
}

TEST(two_byte_invalid_tail_below_range) {
  // UTF8-tail = %x80-BF; \x7F is below range
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xce\x7f", 0), 0u);
}

TEST(two_byte_invalid_tail_above_range) {
  // UTF8-tail = %x80-BF; \xC0 is above range
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xce\xc0", 0), 0u);
}

TEST(two_byte_truncated) {
  // Lead byte with no continuation
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xce", 0), 0u);
}

TEST(three_byte_e0_low_boundary) {
  // U+0800: \xE0\xA0\x80 (smallest 3-byte codepoint)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe0\xa0\x80", 0), 3u);
}

TEST(three_byte_e0_high_boundary) {
  // U+0FFF: \xE0\xBF\xBF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe0\xbf\xbf", 0), 3u);
}

TEST(three_byte_e0_overlong_low) {
  // %xE0 %x80-9F is overlong (codepoints < U+0800)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe0\x80\x80", 0), 0u);
}

TEST(three_byte_e0_overlong_boundary) {
  // %xE0 %x9F is just below the valid %xA0
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe0\x9f\xbf", 0), 0u);
}

TEST(three_byte_e1_letter) {
  // U+1000 MYANMAR: \xE1\x80\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe1\x80\x80", 0), 3u);
}

TEST(three_byte_e4_cjk) {
  // U+4E2D CJK 中: \xE4\xB8\xAD
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe4\xb8\xad", 0), 3u);
}

TEST(three_byte_ec_high_boundary) {
  // %xEC range is fully open: \xEC\xBF\xBF (U+CFFF)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xec\xbf\xbf", 0), 3u);
}

TEST(three_byte_ec_korean_si) {
  // U+C2E4 실 (Hangul SI): \xEC\x8B\xA4
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xec\x8b\xa4", 0), 3u);
}

TEST(three_byte_ed_low_boundary) {
  // U+D000: \xED\x80\x80 (just below surrogate range)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xed\x80\x80", 0), 3u);
}

TEST(three_byte_ed_high_boundary) {
  // U+D7FF: \xED\x9F\xBF (the last codepoint before the surrogate range)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xed\x9f\xbf", 0), 3u);
}

TEST(three_byte_surrogate_low) {
  // U+D800: \xED\xA0\x80 (forbidden surrogate range)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xed\xa0\x80", 0), 0u);
}

TEST(three_byte_surrogate_high) {
  // U+DFFF: \xED\xBF\xBF (forbidden surrogate range)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xed\xbf\xbf", 0), 0u);
}

TEST(three_byte_ee_low_boundary) {
  // U+E000 (first codepoint after the surrogate range): \xEE\x80\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xee\x80\x80", 0), 3u);
}

TEST(three_byte_ef_high_boundary) {
  // U+FFFF (largest 3-byte codepoint): \xEF\xBF\xBF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xef\xbf\xbf", 0), 3u);
}

TEST(three_byte_truncated_one_byte) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe4", 0), 0u);
}

TEST(three_byte_truncated_two_bytes) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe4\xb8", 0), 0u);
}

TEST(three_byte_invalid_third_byte) {
  // Third byte must be %x80-BF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xe4\xb8\x7f", 0), 0u);
}

TEST(four_byte_f0_low_boundary) {
  // U+10000 (smallest 4-byte codepoint): \xF0\x90\x80\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x90\x80\x80", 0), 4u);
}

TEST(four_byte_f0_high_boundary) {
  // %xF0 %xBF\xBF\xBF (U+3FFFF)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\xbf\xbf\xbf", 0), 4u);
}

TEST(four_byte_f0_overlong_low) {
  // %xF0 %x80-8F is overlong (codepoints < U+10000)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x80\x80\x80", 0), 0u);
}

TEST(four_byte_f0_overlong_high_boundary) {
  // %xF0 %x8F is just below the valid %x90
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x8f\xbf\xbf", 0), 0u);
}

TEST(four_byte_f1_low) {
  // U+40000: \xF1\x80\x80\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf1\x80\x80\x80", 0), 4u);
}

TEST(four_byte_f3_high) {
  // U+FFFFF: \xF3\xBF\xBF\xBF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf3\xbf\xbf\xbf", 0), 4u);
}

TEST(four_byte_emoji_grinning) {
  // U+1F600 😀: \xF0\x9F\x98\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x98\x80", 0), 4u);
}

TEST(four_byte_f4_low_boundary) {
  // U+100000: \xF4\x80\x80\x80
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf4\x80\x80\x80", 0), 4u);
}

TEST(four_byte_f4_high_boundary) {
  // U+10FFFF (last valid Unicode codepoint): \xF4\x8F\xBF\xBF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf4\x8f\xbf\xbf", 0), 4u);
}

TEST(four_byte_above_max_codepoint) {
  // %xF4 %x90+ would encode codepoints > U+10FFFF (forbidden)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf4\x90\x80\x80", 0), 0u);
}

TEST(four_byte_above_max_high) {
  // %xF4 %xBF\xBF\xBF would encode U+13FFFF (forbidden)
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf4\xbf\xbf\xbf", 0), 0u);
}

TEST(four_byte_truncated_one_byte) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0", 0), 0u);
}

TEST(four_byte_truncated_two_bytes) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x9f", 0), 0u);
}

TEST(four_byte_truncated_three_bytes) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x98", 0), 0u);
}

TEST(four_byte_invalid_third) {
  // Third byte must be %x80-BF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x7f\x80", 0), 0u);
}

TEST(four_byte_invalid_fourth) {
  // Fourth byte must be %x80-BF
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x98\xc0", 0), 0u);
}

TEST(four_byte_f5_forbidden_lead) {
  // %xF5-FF are not valid lead bytes per RFC 6532 §3.1
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xf5\x80\x80\x80", 0), 0u);
}

TEST(lead_byte_fe_forbidden) {
  // %xFE is not a valid lead byte
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xfe\x80\x80\x80", 0), 0u);
}

TEST(lead_byte_ff_forbidden) {
  // %xFF is not a valid lead byte
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xff", 0), 0u);
}

TEST(lone_continuation_low) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\x80", 0), 0u);
}

TEST(lone_continuation_high) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xbf", 0), 0u);
}

TEST(lone_continuation_middle) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("\xa0", 0), 0u);
}

TEST(ascii_at_offset) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("ABC", 1), 1u);
}

TEST(two_byte_at_offset) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("xy\xce\xb1z", 2), 2u);
}

TEST(three_byte_at_offset) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("xy\xe4\xb8\xad", 2), 3u);
}

TEST(four_byte_at_offset) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("xy\xf0\x9f\x98\x80", 2),
            4u);
}

TEST(two_byte_truncated_at_offset) {
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("xy\xce", 2), 0u);
}

TEST(three_byte_truncated_at_offset) {
  // Only one byte after the lead at position 2
  EXPECT_EQ(sourcemeta::core::utf8_codepoint_length("xy\xe4\xb8", 2), 0u);
}
