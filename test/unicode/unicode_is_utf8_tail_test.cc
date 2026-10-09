#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

// RFC 3629 Section 4 writes the three byte form as `%xE0 %xA0-BF
// UTF8-tail`, so this lead admits nothing below %xA0, which is what
// rejects the overlong encodings of the two byte range
TEST(an_overlong_three_byte_lead_rejects_below_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xE0};
  unsigned char byte_0{0x9F};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

TEST(an_overlong_three_byte_lead_accepts_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xE0};
  unsigned char byte_0{0xA0};
  unsigned char lead_1{0xE0};
  unsigned char byte_1{0xBF};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 1, byte_1));
}

TEST(an_overlong_three_byte_lead_rejects_above_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xE0};
  unsigned char byte_0{0xC0};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

// Section 4 writes `%xED %x80-9F UTF8-tail`, so this lead admits nothing
// above %x9F, which is what rejects the surrogate range
TEST(a_surrogate_lead_rejects_above_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xED};
  unsigned char byte_0{0xA0};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

TEST(a_surrogate_lead_accepts_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xED};
  unsigned char byte_0{0x80};
  unsigned char lead_1{0xED};
  unsigned char byte_1{0x9F};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 1, byte_1));
}

TEST(a_surrogate_lead_rejects_below_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xED};
  unsigned char byte_0{0x7F};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

// Section 4 writes the four byte form as `%xF0 %x90-BF 2( UTF8-tail )`,
// so this lead admits nothing below %x90, which is what rejects the
// overlong encodings of the three byte range
TEST(an_overlong_four_byte_lead_rejects_below_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xF0};
  unsigned char byte_0{0x8F};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

TEST(an_overlong_four_byte_lead_accepts_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xF0};
  unsigned char byte_0{0x90};
  unsigned char lead_1{0xF0};
  unsigned char byte_1{0xBF};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 1, byte_1));
}

// Section 4 writes `%xF4 %x80-8F 2( UTF8-tail )`, so this lead admits
// nothing above %x8F, which is what holds the encoded value at or below
// U+10FFFF
TEST(the_last_four_byte_lead_rejects_above_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xF4};
  unsigned char byte_0{0x90};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
}

TEST(the_last_four_byte_lead_accepts_the_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xF4};
  unsigned char byte_0{0x80};
  unsigned char lead_1{0xF4};
  unsigned char byte_1{0x8F};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 1, byte_1));
}

// Section 4 gives `UTF8-tail = %x80-BF`, which is what a lead carrying no
// restriction of its own admits
TEST(an_unrestricted_lead_takes_the_whole_tail_range) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xE2};
  unsigned char byte_0{0x7F};
  unsigned char lead_1{0xE2};
  unsigned char byte_1{0x80};
  unsigned char lead_2{0xE2};
  unsigned char byte_2{0xBF};
  unsigned char lead_3{0xE2};
  unsigned char byte_3{0xC0};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 1, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 1, byte_1));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_2, 1, byte_2));
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_3, 1, byte_3));
}

// Only the first tail carries a restriction from its lead, so every later
// one takes the whole range however the sequence began
TEST(a_later_tail_takes_the_whole_range_whatever_the_lead) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xE0};
  unsigned char byte_0{0x7F};
  unsigned char lead_1{0xE0};
  unsigned char byte_1{0x80};
  unsigned char lead_2{0xE0};
  unsigned char byte_2{0xBF};
  unsigned char lead_3{0xE0};
  unsigned char byte_3{0xC0};
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_0, 2, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 2, byte_1));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_2, 2, byte_2));
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_3, 2, byte_3));
}

TEST(a_later_tail_is_unrestricted_after_a_surrogate_lead) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xED};
  unsigned char byte_0{0xA0};
  unsigned char lead_1{0xED};
  unsigned char byte_1{0xBF};
  unsigned char lead_2{0xED};
  unsigned char byte_2{0xC0};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 2, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 2, byte_1));
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_2, 2, byte_2));
}

TEST(a_later_tail_is_unrestricted_after_the_last_four_byte_lead) {
  // The predicate is constexpr, so an argument the compiler can read folds
  // the call away and leaves the comparisons behind it unexecuted
  unsigned char lead_0{0xF4};
  unsigned char byte_0{0x90};
  unsigned char lead_1{0xF4};
  unsigned char byte_1{0xBF};
  unsigned char lead_2{0xF4};
  unsigned char byte_2{0xC0};
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_0, 3, byte_0));
  EXPECT_TRUE(sourcemeta::core::is_utf8_tail(lead_1, 3, byte_1));
  EXPECT_FALSE(sourcemeta::core::is_utf8_tail(lead_2, 3, byte_2));
}
