#ifndef SOURCEMETA_CORE_NUMERIC_UINT128_H_
#define SOURCEMETA_CORE_NUMERIC_UINT128_H_

#ifndef SOURCEMETA_CORE_NUMERIC_EXPORT
#include <sourcemeta/core/numeric_export.h>
#endif

#include <cstdint> // std::uint64_t, std::int64_t

#if defined(_MSC_VER)
#include <cassert>  // assert
#include <intrin.h> // _umul128, _udiv128
#endif

namespace sourcemeta::core {

/// @ingroup numeric
/// Cross-platform 128-bit unsigned integer type.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
using uint128_t = unsigned __int128;
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
// We keep the full implementation header-only to allow the compiler to better
// inline
struct uint128_t {
  std::uint64_t low;
  std::uint64_t high;

  uint128_t() : low{0}, high{0} {}
  uint128_t(int value)
      : low{static_cast<std::uint64_t>(value)},
        high{value < 0 ? UINT64_MAX : 0} {}
  uint128_t(unsigned int value)
      : low{static_cast<std::uint64_t>(value)}, high{0} {}
  uint128_t(std::uint64_t value) : low{value}, high{0} {}
  uint128_t(std::int64_t value)
      : low{static_cast<std::uint64_t>(value)},
        high{value < 0 ? UINT64_MAX : 0} {}
  uint128_t(std::uint64_t high_part, std::uint64_t low_part)
      : low{low_part}, high{high_part} {}

  explicit operator std::uint64_t() const { return this->low; }
  explicit operator std::int64_t() const {
    return static_cast<std::int64_t>(this->low);
  }

  explicit operator bool() const { return this->low != 0 || this->high != 0; }

  auto operator+=(const uint128_t &other) -> uint128_t & {
    auto old_low = this->low;
    this->low += other.low;
    this->high += other.high + (this->low < old_low ? 1 : 0);
    return *this;
  }

  auto operator*=(const uint128_t &other) -> uint128_t & {
    *this = *this * other;
    return *this;
  }

  friend auto operator+(uint128_t left, const uint128_t &right) -> uint128_t {
    left += right;
    return left;
  }

  friend auto operator*(const uint128_t &left, const uint128_t &right)
      -> uint128_t {
    std::uint64_t result_high;
    auto result_low = _umul128(left.low, right.low, &result_high);
    result_high += left.low * right.high + left.high * right.low;
    return {result_high, result_low};
  }

  friend auto operator/(const uint128_t &dividend, std::uint64_t divisor)
      -> uint128_t {
    if (dividend.high == 0) {
      return {0, dividend.low / divisor};
    }

    auto quotient_high = dividend.high / divisor;
    auto remainder_high = dividend.high % divisor;
    std::uint64_t remainder;
    auto quotient_low =
        _udiv128(remainder_high, dividend.low, divisor, &remainder);
    return {quotient_high, quotient_low};
  }

  friend auto operator/(const uint128_t &dividend, const uint128_t &divisor)
      -> uint128_t {
    assert(divisor.high == 0);
    return dividend / divisor.low;
  }

  friend auto operator%(const uint128_t &dividend, std::uint64_t divisor)
      -> uint128_t {
    if (dividend.high == 0) {
      return {0, dividend.low % divisor};
    }

    auto remainder_high = dividend.high % divisor;
    std::uint64_t remainder;
    _udiv128(remainder_high, dividend.low, divisor, &remainder);
    return {0, remainder};
  }

  friend auto operator%(const uint128_t &dividend, const uint128_t &divisor)
      -> uint128_t {
    assert(divisor.high == 0);
    return dividend % divisor.low;
  }

  friend auto operator<(const uint128_t &left, const uint128_t &right) -> bool {
    return left.high < right.high ||
           (left.high == right.high && left.low < right.low);
  }

  friend auto operator>(const uint128_t &left, const uint128_t &right) -> bool {
    return right < left;
  }

  friend auto operator<=(const uint128_t &left, const uint128_t &right)
      -> bool {
    return !(right < left);
  }

  friend auto operator>=(const uint128_t &left, const uint128_t &right)
      -> bool {
    return !(left < right);
  }

  friend auto operator==(const uint128_t &left, const uint128_t &right)
      -> bool {
    return left.high == right.high && left.low == right.low;
  }

  friend auto operator!=(const uint128_t &left, const uint128_t &right)
      -> bool {
    return !(left == right);
  }
};
#endif

} // namespace sourcemeta::core

#endif
