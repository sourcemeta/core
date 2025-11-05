#include <sourcemeta/core/numeric_decimal.h>

#include <cassert> // assert
#include <limits>  // std::numeric_limits
#include <utility> // std::move

#include <decimal.hh>  // decimal::Decimal
#include <mpdecimal.h> // MPD_*

namespace sourcemeta::core {

Decimal::Decimal(const char *const value) : data{value} {}

Decimal::Decimal(const std::string &value) : data{value} {}

Decimal::Decimal(const std::string_view value) : data{std::string{value}} {}

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

auto Decimal::is_zero() const -> bool { return this->data.iszero(); }

auto Decimal::is_integer() const -> bool { return this->data.isinteger(); }

auto Decimal::is_finite() const -> bool { return this->data.isfinite(); }

auto Decimal::is_int32() const -> bool {
  assert(this->data.isinteger());
  return *this >= std::numeric_limits<std::int32_t>::min() &&
         *this <= std::numeric_limits<std::int32_t>::max();
}

auto Decimal::is_int64() const -> bool {
  assert(this->data.isinteger());
  return *this >= std::numeric_limits<std::int64_t>::min() &&
         *this <= std::numeric_limits<std::int64_t>::max();
}

auto Decimal::is_uint32() const -> bool {
  assert(this->data.isinteger());
  return *this >= 0 && *this <= std::numeric_limits<std::uint32_t>::max();
}

auto Decimal::is_uint64() const -> bool {
  assert(this->data.isinteger());
  return *this >= 0 && *this <= std::numeric_limits<std::uint64_t>::max();
}

auto Decimal::operator+=(const Decimal &other) -> Decimal & {
  this->data += other.data;
  return *this;
}

auto Decimal::operator-=(const Decimal &other) -> Decimal & {
  this->data -= other.data;
  return *this;
}

auto Decimal::operator*=(const Decimal &other) -> Decimal & {
  this->data *= other.data;
  return *this;
}

auto Decimal::operator/=(const Decimal &other) -> Decimal & {
  this->data /= other.data;
  return *this;
}

auto Decimal::operator%=(const Decimal &other) -> Decimal & {
  this->data %= other.data;
  return *this;
}

auto Decimal::operator+(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  result.data = this->data + other.data;
  return result;
}

auto Decimal::operator-(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  result.data = this->data - other.data;
  return result;
}

auto Decimal::operator*(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  result.data = this->data * other.data;
  return result;
}

auto Decimal::operator/(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  result.data = this->data / other.data;
  return result;
}

auto Decimal::operator%(const Decimal &other) const -> Decimal {
  Decimal result{*this};
  result.data = this->data % other.data;
  return result;
}

auto Decimal::operator-() const -> Decimal {
  Decimal result{*this};
  result.data = -this->data;
  return result;
}

auto Decimal::operator+() const -> Decimal {
  Decimal result{*this};
  result.data = +this->data;
  return result;
}

auto Decimal::operator++() -> Decimal & {
  this->data += decimal::Decimal{1};
  return *this;
}

auto Decimal::operator++(int) -> Decimal {
  Decimal result{*this};
  this->data += decimal::Decimal{1};
  return result;
}

auto Decimal::operator--() -> Decimal & {
  this->data -= decimal::Decimal{1};
  return *this;
}

auto Decimal::operator--(int) -> Decimal {
  Decimal result{*this};
  this->data -= decimal::Decimal{1};
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
