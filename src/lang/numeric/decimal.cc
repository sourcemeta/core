#include <sourcemeta/core/numeric_decimal.h>
#include <sourcemeta/core/numeric_error.h>

#include <cassert> // assert
#include <cmath>   // std::isfinite
#include <limits>  // std::numeric_limits
#include <string>  // std::stof, std::stod
#include <utility> // std::move

#include <mpdecimal.h> // MPD_*

namespace sourcemeta::core {

Decimal::Decimal(const char *const value) {
  try {
    this->data = decimal::Decimal{value};
  } catch (const decimal::ConversionSyntax &) {
    throw DecimalParseError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }
}

Decimal::Decimal(const std::string &value) {
  try {
    this->data = decimal::Decimal{value};
  } catch (const decimal::ConversionSyntax &) {
    throw DecimalParseError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }
}

Decimal::Decimal(const std::string_view value) {
  try {
    this->data = decimal::Decimal{std::string{value}};
  } catch (const decimal::ConversionSyntax &) {
    throw DecimalParseError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }
}

auto Decimal::nan() -> Decimal { return Decimal{"NaN"}; }

auto Decimal::infinity() -> Decimal { return Decimal{"Infinity"}; }

auto Decimal::negative_infinity() -> Decimal { return Decimal{"-Infinity"}; }

auto Decimal::to_scientific_string() const -> std::string {
  return this->data.to_sci(true);
}

auto Decimal::to_string() const -> std::string {
  const std::string repr{this->data.repr()};
  return repr.substr(9, repr.size() - 11);
}

auto Decimal::to_int64() const -> std::int64_t {
  assert(this->is_int64());
  return this->data.i64();
}

auto Decimal::to_int32() const -> std::int32_t {
  assert(this->is_int32());
  return this->data.i32();
}

auto Decimal::to_uint64() const -> std::uint64_t {
  assert(this->is_uint64());
  return this->data.u64();
}

auto Decimal::to_uint32() const -> std::uint32_t {
  assert(this->is_uint32());
  return this->data.u32();
}

auto Decimal::to_float() const -> float {
  assert(this->is_float());
  const std::string str{this->to_scientific_string()};
  return std::stof(str);
}

auto Decimal::to_double() const -> double {
  assert(this->is_double());
  const std::string str{this->to_scientific_string()};
  return std::stod(str);
}

auto Decimal::is_zero() const -> bool { return this->data.iszero(); }

auto Decimal::is_integer() const -> bool { return this->data.isinteger(); }

auto Decimal::is_finite() const -> bool { return this->data.isfinite(); }

auto Decimal::is_real() const -> bool {
  return this->data.isfinite() && !this->data.isinteger();
}

auto Decimal::is_float() const -> bool {
  if (this->is_nan() || this->is_infinite()) {
    return true;
  }

  if (!this->is_finite()) {
    return false;
  }

  const std::string str{this->to_scientific_string()};
  const float value{std::stof(str)};
  if (!std::isfinite(value)) {
    return false;
  }

  const Decimal roundtrip{std::to_string(value)};
  return *this == roundtrip;
}

auto Decimal::is_double() const -> bool {
  if (this->is_nan() || this->is_infinite()) {
    return true;
  }

  if (!this->is_finite()) {
    return false;
  }

  const std::string str{this->to_scientific_string()};
  const double value{std::stod(str)};
  if (!std::isfinite(value)) {
    return false;
  }

  const Decimal roundtrip{std::to_string(value)};
  return *this == roundtrip;
}

auto Decimal::is_int32() const -> bool {
  assert(this->is_integer());
  return *this >= std::numeric_limits<std::int32_t>::min() &&
         *this <= std::numeric_limits<std::int32_t>::max();
}

auto Decimal::is_int64() const -> bool {
  assert(this->is_integer());
  return *this >= std::numeric_limits<std::int64_t>::min() &&
         *this <= std::numeric_limits<std::int64_t>::max();
}

auto Decimal::is_uint32() const -> bool {
  assert(this->is_integer());
  return *this >= 0 && *this <= std::numeric_limits<std::uint32_t>::max();
}

auto Decimal::is_uint64() const -> bool {
  assert(this->is_integer());
  return *this >= 0 && *this <= std::numeric_limits<std::uint64_t>::max();
}

auto Decimal::is_nan() const -> bool { return this->data.isnan(); }

auto Decimal::is_infinite() const -> bool { return this->data.isinfinite(); }

auto Decimal::is_signed() const -> bool { return this->data.issigned(); }

auto Decimal::to_integral() const -> Decimal {
  Decimal result;
  result.data = this->data.to_integral();
  return result;
}

auto Decimal::divisible_by(const Decimal &divisor) const -> bool {
  if (divisor.is_zero()) {
    return false;
  }

  if (this->is_integer() && divisor.is_integer()) {
    return (*this % divisor).is_zero();
  }

  const Decimal quotient{*this / divisor};
  return quotient.is_finite() && quotient.is_integer();
}

auto Decimal::operator+=(const Decimal &other) -> Decimal & {
  try {
    this->data += other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator-=(const Decimal &other) -> Decimal & {
  try {
    this->data -= other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator*=(const Decimal &other) -> Decimal & {
  try {
    this->data *= other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator/=(const Decimal &other) -> Decimal & {
  if (other.is_zero()) {
    if (this->is_zero()) {
      throw NumericInvalidOperationError{};
    }

    throw NumericDivisionByZeroError{};
  }

  try {
    this->data /= other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator%=(const Decimal &other) -> Decimal & {
  if (other.is_zero()) {
    throw NumericInvalidOperationError{};
  }

  try {
    this->data %= other.data;
  } catch (const decimal::InvalidOperation &) {
    throw NumericInvalidOperationError{};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator+(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  try {
    result.data = this->data + other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator-(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  try {
    result.data = this->data - other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator*(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  try {
    result.data = this->data * other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator/(const Decimal &other) const -> Decimal {
  if (other.is_zero()) {
    if (this->is_zero()) {
      throw NumericInvalidOperationError{};
    }

    throw NumericDivisionByZeroError{};
  }

  Decimal result{*this};
  try {
    result.data = this->data / other.data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator%(const Decimal &other) const -> Decimal {
  if (other.is_zero()) {
    throw NumericInvalidOperationError{};
  }

  Decimal result{*this};
  try {
    result.data = this->data % other.data;
  } catch (const decimal::InvalidOperation &) {
    throw NumericInvalidOperationError{};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator-() const -> Decimal {
  Decimal result{*this};
  try {
    result.data = -this->data;
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator+() const -> Decimal {
  Decimal result{*this};
  try {
    result.data = +this->data;
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator++() -> Decimal & {
  try {
    this->data += decimal::Decimal{1};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator++(int) -> Decimal {
  Decimal result{*this};
  try {
    this->data += decimal::Decimal{1};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator--() -> Decimal & {
  try {
    this->data -= decimal::Decimal{1};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return *this;
}

auto Decimal::operator--(int) -> Decimal {
  Decimal result{*this};
  try {
    this->data -= decimal::Decimal{1};
  } catch (const decimal::Overflow &) {
    throw NumericOverflowError{};
  } catch (const decimal::MallocError &) {
    throw NumericOutOfMemoryError{};
  }

  return result;
}

auto Decimal::operator==(const Decimal &other) const -> bool {
  return this->data == other.data;
}

auto Decimal::operator!=(const Decimal &other) const -> bool {
  return this->data != other.data;
}

auto Decimal::operator<(const Decimal &other) const -> bool {
  return this->data < other.data;
}

auto Decimal::operator<=(const Decimal &other) const -> bool {
  return this->data <= other.data;
}

auto Decimal::operator>(const Decimal &other) const -> bool {
  return this->data > other.data;
}

auto Decimal::operator>=(const Decimal &other) const -> bool {
  return this->data >= other.data;
}

} // namespace sourcemeta::core
