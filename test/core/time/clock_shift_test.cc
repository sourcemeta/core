#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

#include <chrono> // std::chrono::system_clock, std::chrono::seconds, std::chrono::duration_cast

using Clock = std::chrono::system_clock;

static const auto REFERENCE{Clock::from_time_t(1700000000)};

// The widest span the clock can express, which the shift saturates at
static auto representable_span() -> std::chrono::seconds {
  return std::chrono::duration_cast<std::chrono::seconds>(
      Clock::duration::max());
}

TEST(clock_shift_backward_moves_by_the_span) {
  EXPECT_EQ(sourcemeta::core::clock_shift_backward(REFERENCE,
                                                   std::chrono::seconds{40}),
            Clock::from_time_t(1699999960));
}

TEST(clock_shift_forward_moves_by_the_span) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(REFERENCE,
                                                  std::chrono::seconds{40}),
            Clock::from_time_t(1700000040));
}

TEST(clock_shift_backward_of_zero_is_the_same_point) {
  EXPECT_EQ(sourcemeta::core::clock_shift_backward(
                REFERENCE, std::chrono::seconds::zero()),
            REFERENCE);
}

TEST(clock_shift_forward_of_zero_is_the_same_point) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(REFERENCE,
                                                  std::chrono::seconds::zero()),
            REFERENCE);
}

TEST(clock_shift_backward_treats_a_negative_span_as_none) {
  EXPECT_EQ(sourcemeta::core::clock_shift_backward(REFERENCE,
                                                   std::chrono::seconds{-40}),
            REFERENCE);
}

TEST(clock_shift_forward_treats_a_negative_span_as_none) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(REFERENCE,
                                                  std::chrono::seconds{-40}),
            REFERENCE);
}

TEST(clock_shift_backward_of_an_enormous_span_does_not_underflow) {
  // The span is bounded to what the clock can express before it is applied, so
  // from a present-day reading the result lands short of the oldest instant
  // rather than wrapping past it
  const auto result{sourcemeta::core::clock_shift_backward(
      REFERENCE, std::chrono::seconds::max())};
  EXPECT_TRUE(result < REFERENCE);
  EXPECT_TRUE(result >= Clock::time_point{Clock::duration::min()});
}

TEST(clock_shift_forward_saturates_at_the_newest_instant) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(REFERENCE,
                                                  std::chrono::seconds::max()),
            Clock::time_point{Clock::duration::max()});
}

TEST(clock_shift_backward_of_the_representable_span_does_not_underflow) {
  const auto result{
      sourcemeta::core::clock_shift_backward(REFERENCE, representable_span())};
  EXPECT_TRUE(result < REFERENCE);
  EXPECT_TRUE(result >= Clock::time_point{Clock::duration::min()});
  EXPECT_EQ(result, REFERENCE - std::chrono::duration_cast<Clock::duration>(
                                    representable_span()));
}

TEST(clock_shift_backward_saturates_from_an_early_instant) {
  // Below the epoch there is no headroom left, so the shift clamps instead
  const Clock::time_point early{
      Clock::duration::min() +
      std::chrono::duration_cast<Clock::duration>(std::chrono::seconds{10})};
  EXPECT_EQ(
      sourcemeta::core::clock_shift_backward(early, std::chrono::seconds{40}),
      Clock::time_point{Clock::duration::min()});
}

TEST(clock_shift_forward_saturates_at_the_representable_span) {
  EXPECT_EQ(
      sourcemeta::core::clock_shift_forward(REFERENCE, representable_span()),
      Clock::time_point{Clock::duration::max()});
}

TEST(clock_shift_backward_from_the_oldest_instant_stays_there) {
  const Clock::time_point oldest{Clock::duration::min()};
  EXPECT_EQ(
      sourcemeta::core::clock_shift_backward(oldest, std::chrono::seconds{40}),
      oldest);
}

TEST(clock_shift_forward_from_the_newest_instant_stays_there) {
  const Clock::time_point newest{Clock::duration::max()};
  EXPECT_EQ(
      sourcemeta::core::clock_shift_forward(newest, std::chrono::seconds{40}),
      newest);
}

TEST(clock_shift_backward_from_the_newest_instant_moves_normally) {
  const Clock::time_point newest{Clock::duration::max()};
  EXPECT_EQ(
      sourcemeta::core::clock_shift_backward(newest, std::chrono::seconds{40}),
      newest - std::chrono::seconds{40});
}

TEST(clock_shift_forward_from_the_oldest_instant_moves_normally) {
  const Clock::time_point oldest{Clock::duration::min()};
  EXPECT_EQ(
      sourcemeta::core::clock_shift_forward(oldest, std::chrono::seconds{40}),
      oldest + std::chrono::seconds{40});
}

TEST(clock_shift_backward_at_the_epoch) {
  // Expressed without `from_time_t(-40)`, as `time_t` is not guaranteed to be
  // signed
  EXPECT_EQ(sourcemeta::core::clock_shift_backward(Clock::from_time_t(0),
                                                   std::chrono::seconds{40}),
            Clock::from_time_t(0) - std::chrono::seconds{40});
}

TEST(clock_shift_forward_at_the_epoch) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(Clock::from_time_t(0),
                                                  std::chrono::seconds{40}),
            Clock::from_time_t(40));
}

TEST(clock_shift_backward_of_the_minimum_span_saturates) {
  EXPECT_EQ(sourcemeta::core::clock_shift_backward(REFERENCE,
                                                   std::chrono::seconds::min()),
            REFERENCE);
}

TEST(clock_shift_forward_of_the_minimum_span_saturates) {
  EXPECT_EQ(sourcemeta::core::clock_shift_forward(REFERENCE,
                                                  std::chrono::seconds::min()),
            REFERENCE);
}
