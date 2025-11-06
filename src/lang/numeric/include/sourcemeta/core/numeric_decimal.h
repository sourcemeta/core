#ifndef SOURCEMETA_CORE_NUMERIC_DECIMAL_H_
#define SOURCEMETA_CORE_NUMERIC_DECIMAL_H_

#ifndef SOURCEMETA_CORE_NUMERIC_EXPORT
#include <sourcemeta/core/numeric_export.h>
#endif

#include <concepts> // std::integral
#include <cstdint>  // std::int32_t, std::int64_t, std::uint32_t, std::uint64_t
#include <string>   // std::string
#include <string_view> // std::string_view

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <decimal.hh> // decimal::Decimal
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace sourcemeta::core {

/// @ingroup numeric
/// Represents an arbitrary-precision decimal number.
class SOURCEMETA_CORE_NUMERIC_EXPORT Decimal {
public:
  /// Construct a decimal number initialized to zero
  Decimal() noexcept : data{0} {}

  /// Copy constructor
  Decimal(const Decimal &other) = default;

  /// Move constructor
  Decimal(Decimal &&other) noexcept = default;

  /// Construct a decimal number from an integral type
  template <typename T>
    requires std::integral<T>
  Decimal(const T value) : data{value} {}

  /// Construct a decimal number from a C-string
  explicit Decimal(const char *const value);

  /// Construct a decimal number from a C++ string
  explicit Decimal(const std::string &value);

  /// Construct a decimal number from a string view
  explicit Decimal(const std::string_view value);

  /// Copy assignment operator
  auto operator=(const Decimal &other) -> Decimal & = default;

  /// Move assignment operator
  auto operator=(Decimal &&other) noexcept -> Decimal & = default;

  /// Convert the decimal number to scientific notation string
  [[nodiscard]] auto to_scientific_string() const -> std::string;

  /// Convert the decimal number to a plain string representation
  [[nodiscard]] auto to_string() const -> std::string;

  /// Convert the decimal number to a 64-bit signed integer
  [[nodiscard]] auto to_int64() const -> std::int64_t;

  /// Convert the decimal number to a 32-bit signed integer
  [[nodiscard]] auto to_int32() const -> std::int32_t;

  /// Convert the decimal number to a 64-bit unsigned integer
  [[nodiscard]] auto to_uint64() const -> std::uint64_t;

  /// Convert the decimal number to a 32-bit unsigned integer
  [[nodiscard]] auto to_uint32() const -> std::uint32_t;

  /// Check if the decimal number is zero
  [[nodiscard]] auto is_zero() const -> bool;

  /// Check if the decimal number represents an integer value
  [[nodiscard]] auto is_integer() const -> bool;

  /// Check if the decimal number is finite
  [[nodiscard]] auto is_finite() const -> bool;

  /// Check if the decimal number fits in a 32-bit signed integer
  [[nodiscard]] auto is_int32() const -> bool;

  /// Check if the decimal number fits in a 64-bit signed integer
  [[nodiscard]] auto is_int64() const -> bool;

  /// Check if the decimal number fits in a 32-bit unsigned integer
  [[nodiscard]] auto is_uint32() const -> bool;

  /// Check if the decimal number fits in a 64-bit unsigned integer
  [[nodiscard]] auto is_uint64() const -> bool;

  /// Add another decimal number to this one
  auto operator+=(const Decimal &other) -> Decimal &;

  /// Subtract another decimal number from this one
  auto operator-=(const Decimal &other) -> Decimal &;

  /// Multiply this decimal number by another
  auto operator*=(const Decimal &other) -> Decimal &;

  /// Divide this decimal number by another
  auto operator/=(const Decimal &other) -> Decimal &;

  /// Compute the modulo of this decimal number with another
  auto operator%=(const Decimal &other) -> Decimal &;

  /// Add two decimal numbers
  [[nodiscard]] auto operator+(const Decimal &other) const -> Decimal;

  /// Subtract two decimal numbers
  [[nodiscard]] auto operator-(const Decimal &other) const -> Decimal;

  /// Multiply two decimal numbers
  [[nodiscard]] auto operator*(const Decimal &other) const -> Decimal;

  /// Divide two decimal numbers
  [[nodiscard]] auto operator/(const Decimal &other) const -> Decimal;

  /// Compute the modulo of two decimal numbers
  [[nodiscard]] auto operator%(const Decimal &other) const -> Decimal;

  /// Unary negation operator
  [[nodiscard]] auto operator-() const -> Decimal;

  /// Unary plus operator
  [[nodiscard]] auto operator+() const -> Decimal;

  /// Prefix increment operator
  auto operator++() -> Decimal &;

  /// Postfix increment operator
  auto operator++(int) -> Decimal;

  /// Prefix decrement operator
  auto operator--() -> Decimal &;

  /// Postfix decrement operator
  auto operator--(int) -> Decimal;

  /// Check if two decimal numbers are equal
  [[nodiscard]] auto operator==(const Decimal &other) const -> bool;

  /// Check if two decimal numbers are not equal
  [[nodiscard]] auto operator!=(const Decimal &other) const -> bool;

  /// Check if this decimal number is less than another
  [[nodiscard]] auto operator<(const Decimal &other) const -> bool;

  /// Check if this decimal number is less than or equal to another
  [[nodiscard]] auto operator<=(const Decimal &other) const -> bool;

  /// Check if this decimal number is greater than another
  [[nodiscard]] auto operator>(const Decimal &other) const -> bool;

  /// Check if this decimal number is greater than or equal to another
  [[nodiscard]] auto operator>=(const Decimal &other) const -> bool;

private:
  decimal::Decimal data;
};

template <typename T>
  requires std::integral<T>
inline auto operator+(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} + right;
}

template <typename T>
  requires std::integral<T>
inline auto operator-(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} - right;
}

template <typename T>
  requires std::integral<T>
inline auto operator*(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} * right;
}

template <typename T>
  requires std::integral<T>
inline auto operator/(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} / right;
}

template <typename T>
  requires std::integral<T>
inline auto operator%(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} % right;
}

template <typename T>
  requires std::integral<T>
inline auto operator==(const T left, const Decimal &right) -> bool {
  return Decimal{left} == right;
}

template <typename T>
  requires std::integral<T>
inline auto operator!=(const T left, const Decimal &right) -> bool {
  return Decimal{left} != right;
}

template <typename T>
  requires std::integral<T>
inline auto operator<(const T left, const Decimal &right) -> bool {
  return Decimal{left} < right;
}

template <typename T>
  requires std::integral<T>
inline auto operator<=(const T left, const Decimal &right) -> bool {
  return Decimal{left} <= right;
}

template <typename T>
  requires std::integral<T>
inline auto operator>(const T left, const Decimal &right) -> bool {
  return Decimal{left} > right;
}

template <typename T>
  requires std::integral<T>
inline auto operator>=(const T left, const Decimal &right) -> bool {
  return Decimal{left} >= right;
}

} // namespace sourcemeta::core

#endif
