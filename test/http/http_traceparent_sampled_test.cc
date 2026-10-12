#include <sourcemeta/core/http_tracecontext.h>
#include <sourcemeta/core/test.h>

// "As this is a bit field, you cannot interpret flags by decoding the hex
// value and looking at the resulting number", so the one defined bit is masked
// out rather than compared against the byte
// https://www.w3.org/TR/trace-context/#trace-flags
TEST(the_sampled_bit_alone_is_set) {
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(0x01));
}

TEST(no_bit_is_set) {
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(0x00));
}

TEST(the_sampled_bit_is_set_alongside_a_reserved_one) {
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(0x05));
}

TEST(every_bit_is_set) {
  EXPECT_TRUE(sourcemeta::core::http_traceparent_sampled(0xff));
}

// "Other Flags" are reserved, so a byte carrying one of those and not the
// sampled bit is not sampled, which is what a comparison would get wrong
// https://www.w3.org/TR/trace-context/#other-flags
TEST(only_a_reserved_bit_is_set) {
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(0x04));
}

TEST(every_bit_but_the_sampled_one_is_set) {
  EXPECT_FALSE(sourcemeta::core::http_traceparent_sampled(0xfe));
}
