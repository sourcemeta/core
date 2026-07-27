#ifndef SOURCEMETA_CORE_OIDC_TIME_H_
#define SOURCEMETA_CORE_OIDC_TIME_H_

#include <algorithm> // std::clamp
#include <chrono> // std::chrono::system_clock, std::chrono::seconds, std::chrono::duration_cast

namespace sourcemeta::core {

// The widest span the clock's tick type can express, truncated toward zero so
// converting it back to ticks can never exceed the representable range
inline auto oidc_bounded_ticks(const std::chrono::seconds span)
    -> std::chrono::system_clock::duration {
  using Clock = std::chrono::system_clock;
  constexpr auto limit{
      std::chrono::duration_cast<std::chrono::seconds>(Clock::duration::max())};
  return std::chrono::duration_cast<Clock::duration>(
      std::clamp(span, std::chrono::seconds::zero(), limit));
}

// Move the server clock back by a span, saturating at the oldest representable
// instant. Every freshness comparison shifts this reading rather than adding to
// or subtracting from the claim under test, because a NumericDate near the
// representable bound would otherwise overflow the arithmetic, which is the
// discipline the base JSON Web Token verification already follows. A negative
// span is treated as no span at all, so a nonsensical window fails closed
inline auto oidc_shift_backward(const std::chrono::system_clock::time_point now,
                                const std::chrono::seconds span)
    -> std::chrono::system_clock::time_point {
  using Clock = std::chrono::system_clock;
  const auto ticks{oidc_bounded_ticks(span)};
  if (now.time_since_epoch() < Clock::duration::min() + ticks) {
    return Clock::time_point{Clock::duration::min()};
  }

  return now - ticks;
}

// Move the server clock forward by a span, saturating at the newest
// representable instant
inline auto oidc_shift_forward(const std::chrono::system_clock::time_point now,
                               const std::chrono::seconds span)
    -> std::chrono::system_clock::time_point {
  using Clock = std::chrono::system_clock;
  const auto ticks{oidc_bounded_ticks(span)};
  if (now.time_since_epoch() > Clock::duration::max() - ticks) {
    return Clock::time_point{Clock::duration::max()};
  }

  return now + ticks;
}

} // namespace sourcemeta::core

#endif
