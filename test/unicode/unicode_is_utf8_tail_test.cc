#include <sourcemeta/core/test.h>
#include <sourcemeta/core/unicode.h>

#include <cstddef> // std::size_t

// The predicate is constexpr, so arguments the compiler can read fold the call
// away at compile time and leave the comparisons behind it unexecuted. Every
// case goes through values it cannot fold so that each range is decided where
// it can be observed
static auto tail(const unsigned char lead, const std::size_t position,
                 const unsigned char byte) -> bool {
  volatile unsigned char observed_lead{lead};
  volatile std::size_t observed_position{position};
  volatile unsigned char observed_byte{byte};
  return sourcemeta::core::is_utf8_tail(
      static_cast<unsigned char>(observed_lead),
      static_cast<std::size_t>(observed_position),
      static_cast<unsigned char>(observed_byte));
}

// RFC 3629 Section 4 writes the three byte form as `%xE0 %xA0-BF UTF8-tail`,
// so this lead admits nothing below %xA0, which is what rejects the overlong
// encodings of the two byte range
TEST(an_overlong_three_byte_lead_rejects_below_the_range) {
  EXPECT_FALSE(tail(0xE0, 1, 0x9F));
}

TEST(an_overlong_three_byte_lead_accepts_the_range) {
  EXPECT_TRUE(tail(0xE0, 1, 0xA0));
  EXPECT_TRUE(tail(0xE0, 1, 0xBF));
}

TEST(an_overlong_three_byte_lead_rejects_above_the_range) {
  EXPECT_FALSE(tail(0xE0, 1, 0xC0));
}

// Section 4 writes `%xED %x80-9F UTF8-tail`, so this lead admits nothing above
// %x9F, which is what rejects the surrogate range
TEST(a_surrogate_lead_rejects_above_the_range) {
  EXPECT_FALSE(tail(0xED, 1, 0xA0));
}

TEST(a_surrogate_lead_accepts_the_range) {
  EXPECT_TRUE(tail(0xED, 1, 0x80));
  EXPECT_TRUE(tail(0xED, 1, 0x9F));
}

TEST(a_surrogate_lead_rejects_below_the_range) {
  EXPECT_FALSE(tail(0xED, 1, 0x7F));
}

// Section 4 writes the four byte form as `%xF0 %x90-BF 2( UTF8-tail )`, so this
// lead admits nothing below %x90, which is what rejects the overlong
// encodings of the three byte range
TEST(an_overlong_four_byte_lead_rejects_below_the_range) {
  EXPECT_FALSE(tail(0xF0, 1, 0x8F));
}

TEST(an_overlong_four_byte_lead_accepts_the_range) {
  EXPECT_TRUE(tail(0xF0, 1, 0x90));
  EXPECT_TRUE(tail(0xF0, 1, 0xBF));
}

// Section 4 writes `%xF4 %x80-8F 2( UTF8-tail )`, so this lead admits nothing
// above %x8F, which is what holds the encoded value at or below U+10FFFF
TEST(the_last_four_byte_lead_rejects_above_the_range) {
  EXPECT_FALSE(tail(0xF4, 1, 0x90));
}

TEST(the_last_four_byte_lead_accepts_the_range) {
  EXPECT_TRUE(tail(0xF4, 1, 0x80));
  EXPECT_TRUE(tail(0xF4, 1, 0x8F));
}

// Section 4 gives `UTF8-tail = %x80-BF`, which is what a lead carrying no
// restriction of its own admits
TEST(an_unrestricted_lead_takes_the_whole_tail_range) {
  EXPECT_FALSE(tail(0xE2, 1, 0x7F));
  EXPECT_TRUE(tail(0xE2, 1, 0x80));
  EXPECT_TRUE(tail(0xE2, 1, 0xBF));
  EXPECT_FALSE(tail(0xE2, 1, 0xC0));
}

// Only the first tail carries a restriction from its lead, so every later one
// takes the whole range however the sequence began
TEST(a_later_tail_takes_the_whole_range_whatever_the_lead) {
  EXPECT_FALSE(tail(0xE0, 2, 0x7F));
  EXPECT_TRUE(tail(0xE0, 2, 0x80));
  EXPECT_TRUE(tail(0xE0, 2, 0xBF));
  EXPECT_FALSE(tail(0xE0, 2, 0xC0));
}

TEST(a_later_tail_is_unrestricted_after_a_surrogate_lead) {
  EXPECT_TRUE(tail(0xED, 2, 0xA0));
  EXPECT_TRUE(tail(0xED, 2, 0xBF));
  EXPECT_FALSE(tail(0xED, 2, 0xC0));
}

TEST(a_later_tail_is_unrestricted_after_the_last_four_byte_lead) {
  EXPECT_TRUE(tail(0xF4, 3, 0x90));
  EXPECT_TRUE(tail(0xF4, 3, 0xBF));
  EXPECT_FALSE(tail(0xF4, 3, 0xC0));
}
