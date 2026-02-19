#ifndef SOURCEMETA_CORE_NUMERIC_BIG_COEFFICIENT_H_
#define SOURCEMETA_CORE_NUMERIC_BIG_COEFFICIENT_H_

#include <algorithm> // std::max, std::copy, std::fill
#include <array>     // std::array
#include <cstdint>   // std::int32_t, std::int64_t, std::uint32_t,
                     // std::uint64_t, std::uintptr_t, std::uint8_t
#include <cstring>   // std::memcpy
#include <string>    // std::string, std::to_string

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
using uint128_t = unsigned __int128;
#pragma GCC diagnostic pop
// TODO: Expose a cross-platform 128-bit unsigned integer in the numeric module
#elif defined(_MSC_VER)
#include <intrin.h> // _umul128, _udiv128
struct uint128_t {
  std::uint64_t lo;
  std::uint64_t hi;

  uint128_t() : lo{0}, hi{0} {}
  uint128_t(int value) : lo{static_cast<std::uint64_t>(value)}, hi{0} {}
  uint128_t(unsigned int value)
      : lo{static_cast<std::uint64_t>(value)}, hi{0} {}
  uint128_t(std::uint64_t value) : lo{value}, hi{0} {}
  uint128_t(std::int64_t value)
      : lo{static_cast<std::uint64_t>(value)}, hi{0} {}
  uint128_t(std::uint64_t high, std::uint64_t low) : lo{low}, hi{high} {}

  explicit operator std::uint64_t() const { return lo; }
  explicit operator std::int64_t() const {
    return static_cast<std::int64_t>(lo);
  }
  explicit operator bool() const { return lo != 0 || hi != 0; }

  auto operator+=(const uint128_t &other) -> uint128_t & {
    auto old_lo = lo;
    lo += other.lo;
    hi += other.hi + (lo < old_lo ? 1 : 0);
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
    std::uint64_t result_hi;
    auto result_lo = _umul128(left.lo, right.lo, &result_hi);
    result_hi += left.lo * right.hi + left.hi * right.lo;
    return {result_hi, result_lo};
  }

  friend auto operator/(const uint128_t &dividend, std::uint64_t divisor)
      -> uint128_t {
    if (dividend.hi == 0) {
      return {0, dividend.lo / divisor};
    }
    auto quotient_hi = dividend.hi / divisor;
    auto remainder_hi = dividend.hi % divisor;
    std::uint64_t remainder;
    auto quotient_lo = _udiv128(remainder_hi, dividend.lo, divisor, &remainder);
    return {quotient_hi, quotient_lo};
  }

  friend auto operator/(const uint128_t &dividend, const uint128_t &divisor)
      -> uint128_t {
    return dividend / divisor.lo;
  }

  friend auto operator%(const uint128_t &dividend, std::uint64_t divisor)
      -> uint128_t {
    if (dividend.hi == 0) {
      return {0, dividend.lo % divisor};
    }
    auto remainder_hi = dividend.hi % divisor;
    std::uint64_t remainder;
    _udiv128(remainder_hi, dividend.lo, divisor, &remainder);
    return {0, remainder};
  }

  friend auto operator%(const uint128_t &dividend, const uint128_t &divisor)
      -> uint128_t {
    return dividend % divisor.lo;
  }

  friend auto operator<(const uint128_t &left, const uint128_t &right) -> bool {
    return left.hi < right.hi || (left.hi == right.hi && left.lo < right.lo);
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
    return left.hi == right.hi && left.lo == right.lo;
  }

  friend auto operator!=(const uint128_t &left, const uint128_t &right)
      -> bool {
    return !(left == right);
  }
};
#endif

namespace {

constexpr std::uint8_t FLAG_SIGN = 0x01;
constexpr std::uint8_t FLAG_NAN = 0x02;
constexpr std::uint8_t FLAG_SNAN = 0x04;
constexpr std::uint8_t FLAG_INFINITE = 0x08;
constexpr std::uint8_t FLAG_BIG = 0x10;
constexpr std::uint8_t FLAG_HEAP = 0x20;

constexpr std::uint8_t SPECIAL_MASK =
    FLAG_NAN | FLAG_SNAN | FLAG_INFINITE | FLAG_BIG;

constexpr std::array<std::uint64_t, 20> POWERS_OF_10 = {{
    1ULL,                    // 10^0
    10ULL,                   // 10^1
    100ULL,                  // 10^2
    1000ULL,                 // 10^3
    10000ULL,                // 10^4
    100000ULL,               // 10^5
    1000000ULL,              // 10^6
    10000000ULL,             // 10^7
    100000000ULL,            // 10^8
    1000000000ULL,           // 10^9
    10000000000ULL,          // 10^10
    100000000000ULL,         // 10^11
    1000000000000ULL,        // 10^12
    10000000000000ULL,       // 10^13
    100000000000000ULL,      // 10^14
    1000000000000000ULL,     // 10^15
    10000000000000000ULL,    // 10^16
    100000000000000000ULL,   // 10^17
    1000000000000000000ULL,  // 10^18
    10000000000000000000ULL, // 10^19
}};

constexpr std::uint64_t BASE = 1000000000000000000ULL; // 10^18
constexpr std::int32_t BASE_DIGITS = 18;
constexpr std::int64_t COMPACT_MAX =
    static_cast<std::int64_t>(BASE - 1); // 999999999999999999

struct BigCoefficient {
  std::uint64_t *words;
  std::uint32_t length;
  std::uint32_t capacity;
};

auto big_allocate(std::uint32_t capacity) -> BigCoefficient * {
  auto *big = new BigCoefficient{};
  big->words = new std::uint64_t[capacity]();
  big->length = 0;
  big->capacity = capacity;
  return big;
}

auto big_clone(const BigCoefficient *source) -> BigCoefficient * {
  auto *big = big_allocate(source->capacity);
  big->length = source->length;
  std::copy(source->words, source->words + source->length, big->words);
  return big;
}

void big_free(BigCoefficient *big) {
  if (big) {
    delete[] big->words;
    delete big;
  }
}

void big_trim(BigCoefficient *big) {
  while (big->length > 1 && big->words[big->length - 1] == 0) {
    big->length--;
  }
}

auto big_is_zero(const BigCoefficient *big) -> bool {
  return big->length == 0 || (big->length == 1 && big->words[0] == 0);
}

auto big_compare(const BigCoefficient *left, const BigCoefficient *right)
    -> int {
  if (left->length != right->length) {
    return left->length < right->length ? -1 : 1;
  }
  for (auto index = left->length; index > 0; index--) {
    if (left->words[index - 1] != right->words[index - 1]) {
      return left->words[index - 1] < right->words[index - 1] ? -1 : 1;
    }
  }
  return 0;
}

auto big_multiply_pow10(const BigCoefficient *source, std::uint32_t power)
    -> BigCoefficient * {
  if (power == 0) {
    return big_clone(source);
  }
  auto full_words = power / BASE_DIGITS;
  auto residual = power % BASE_DIGITS;
  auto *result = big_allocate(source->length + full_words + 1);

  std::fill(result->words, result->words + full_words, 0ULL);
  std::copy(source->words, source->words + source->length,
            result->words + full_words);
  result->length = source->length + full_words;

  if (residual > 0) {
    auto multiplier = POWERS_OF_10[static_cast<std::uint32_t>(residual)];
    uint128_t carry = 0;
    for (std::uint32_t index = full_words; index < result->length; index++) {
      auto product =
          static_cast<uint128_t>(result->words[index]) * multiplier + carry;
      result->words[index] = static_cast<std::uint64_t>(product % BASE);
      carry = product / BASE;
    }
    if (carry > 0) {
      result->words[result->length] = static_cast<std::uint64_t>(carry);
      result->length++;
    }
  }

  big_trim(result);
  return result;
}

auto big_add(const BigCoefficient *left, const BigCoefficient *right)
    -> BigCoefficient * {
  auto max_length = std::max(left->length, right->length);
  auto *result = big_allocate(max_length + 1);
  std::uint64_t carry = 0;
  for (std::uint32_t index = 0; index < max_length; index++) {
    uint128_t sum = carry;
    if (index < left->length) {
      sum += left->words[index];
    }
    if (index < right->length) {
      sum += right->words[index];
    }
    result->words[index] = static_cast<std::uint64_t>(sum % BASE);
    carry = static_cast<std::uint64_t>(sum / BASE);
  }
  result->length = max_length;
  if (carry > 0) {
    result->words[result->length] = carry;
    result->length++;
  }
  big_trim(result);
  return result;
}

auto big_subtract(const BigCoefficient *left, const BigCoefficient *right)
    -> BigCoefficient * {
  auto *result = big_allocate(left->length);
  std::int64_t borrow = 0;
  for (std::uint32_t index = 0; index < left->length; index++) {
    auto left_word = static_cast<std::int64_t>(left->words[index]);
    std::int64_t right_word = 0;
    if (index < right->length) {
      right_word = static_cast<std::int64_t>(right->words[index]);
    }
    auto difference = left_word - right_word - borrow;
    if (difference < 0) {
      difference += static_cast<std::int64_t>(BASE);
      borrow = 1;
    } else {
      borrow = 0;
    }
    result->words[index] = static_cast<std::uint64_t>(difference);
  }
  result->length = left->length;
  big_trim(result);
  return result;
}

auto big_multiply(const BigCoefficient *left, const BigCoefficient *right)
    -> BigCoefficient * {
  auto result_length = left->length + right->length;
  auto *result = big_allocate(result_length);
  result->length = result_length;
  std::fill(result->words, result->words + result_length, 0ULL);

  for (std::uint32_t index_left = 0; index_left < left->length; index_left++) {
    uint128_t carry = 0;
    for (std::uint32_t index_right = 0; index_right < right->length;
         index_right++) {
      auto position = index_left + index_right;
      auto product = static_cast<uint128_t>(left->words[index_left]) *
                         right->words[index_right] +
                     result->words[position] + carry;
      result->words[position] = static_cast<std::uint64_t>(product % BASE);
      carry = product / BASE;
    }
    if (carry > 0) {
      result->words[index_left + right->length] +=
          static_cast<std::uint64_t>(carry);
    }
  }

  big_trim(result);
  return result;
}

void big_divide_modulo(const BigCoefficient *dividend,
                       const BigCoefficient *divisor,
                       BigCoefficient **quotient_out,
                       BigCoefficient **remainder_out) {
  auto compare = big_compare(dividend, divisor);
  if (compare < 0) {
    *quotient_out = big_allocate(1);
    (*quotient_out)->words[0] = 0;
    (*quotient_out)->length = 1;
    *remainder_out = big_clone(dividend);
    return;
  }
  if (compare == 0) {
    *quotient_out = big_allocate(1);
    (*quotient_out)->words[0] = 1;
    (*quotient_out)->length = 1;
    *remainder_out = big_allocate(1);
    (*remainder_out)->words[0] = 0;
    (*remainder_out)->length = 1;
    return;
  }

  if (divisor->length == 1) {
    auto *quotient = big_allocate(dividend->length);
    quotient->length = dividend->length;
    uint128_t remainder = 0;
    for (auto index = dividend->length; index > 0; index--) {
      auto current = remainder * BASE + dividend->words[index - 1];
      quotient->words[index - 1] =
          static_cast<std::uint64_t>(current / divisor->words[0]);
      remainder = current % divisor->words[0];
    }
    big_trim(quotient);
    *quotient_out = quotient;
    *remainder_out = big_allocate(1);
    (*remainder_out)->words[0] = static_cast<std::uint64_t>(remainder);
    (*remainder_out)->length = 1;
    return;
  }

  auto *remainder = big_clone(dividend);
  auto *quotient = big_allocate(dividend->length);
  quotient->length = dividend->length;
  std::fill(quotient->words, quotient->words + quotient->length, 0ULL);

  while (big_compare(remainder, divisor) >= 0) {
    auto remainder_top =
        static_cast<uint128_t>(remainder->words[remainder->length - 1]);
    if (remainder->length > divisor->length) {
      auto shift = remainder->length - divisor->length;
      auto divisor_top = divisor->words[divisor->length - 1];
      auto estimate =
          static_cast<std::uint64_t>(remainder_top / (divisor_top + 1));
      if (estimate == 0) {
        estimate = 1;
      }

      auto *estimate_big = big_allocate(1);
      estimate_big->words[0] = estimate;
      estimate_big->length = 1;

      auto *scaled = big_multiply_pow10(estimate_big, shift * BASE_DIGITS);
      auto *product = big_multiply(scaled, divisor);
      big_free(estimate_big);
      big_free(scaled);

      if (big_compare(product, remainder) > 0) {
        big_free(product);
        auto *new_remainder = big_subtract(remainder, divisor);
        big_free(remainder);
        remainder = new_remainder;
        quotient->words[0]++;
      } else {
        auto *new_remainder = big_subtract(remainder, product);
        big_free(remainder);
        remainder = new_remainder;
        big_free(product);
        auto *estimated_quotient = big_allocate(shift + 1);
        std::fill(estimated_quotient->words, estimated_quotient->words + shift,
                  0ULL);
        estimated_quotient->words[shift] = estimate;
        estimated_quotient->length = shift + 1;
        auto *new_quotient = big_add(quotient, estimated_quotient);
        big_free(quotient);
        big_free(estimated_quotient);
        quotient = new_quotient;
      }
    } else {
      auto *new_remainder = big_subtract(remainder, divisor);
      big_free(remainder);
      remainder = new_remainder;
      quotient->words[0]++;
    }
  }

  big_trim(quotient);
  big_trim(remainder);
  *quotient_out = quotient;
  *remainder_out = remainder;
}

auto big_digit_count(const BigCoefficient *big) -> std::uint64_t {
  if (big_is_zero(big)) {
    return 1;
  }
  auto top_word = big->words[big->length - 1];
  std::uint64_t top_digits = 0;
  while (top_word > 0) {
    top_word /= 10;
    top_digits++;
  }
  return top_digits + static_cast<std::uint64_t>(big->length - 1) * BASE_DIGITS;
}

auto big_to_string(const BigCoefficient *big) -> std::string {
  if (big_is_zero(big)) {
    return "0";
  }

  std::string result;
  auto top = big->words[big->length - 1];
  result += std::to_string(top);

  for (auto index = big->length - 1; index > 0; index--) {
    auto word_string = std::to_string(big->words[index - 1]);
    result.append(static_cast<std::size_t>(BASE_DIGITS) - word_string.size(),
                  '0');
    result += word_string;
  }

  return result;
}

auto big_strip_trailing_zeros(BigCoefficient *big) -> std::int32_t {
  if (big_is_zero(big)) {
    return 0;
  }

  std::int32_t total_stripped = 0;

  std::uint32_t zero_words = 0;
  while (zero_words < big->length && big->words[zero_words] == 0) {
    zero_words++;
  }
  if (zero_words > 0 && zero_words < big->length) {
    std::copy(big->words + zero_words, big->words + big->length, big->words);
    big->length -= zero_words;
    total_stripped += static_cast<std::int32_t>(zero_words) * BASE_DIGITS;
  }

  if (big->words[0] != 0) {
    while (big->words[0] % 10 == 0) {
      big->words[0] /= 10;
      total_stripped++;
    }
  }

  return total_stripped;
}

auto big_from_uint64(std::uint64_t value) -> BigCoefficient * {
  if (value < BASE) {
    auto *result = big_allocate(1);
    result->words[0] = value;
    result->length = 1;
    return result;
  }
  auto *result = big_allocate(2);
  result->words[0] = value % BASE;
  result->words[1] = value / BASE;
  result->length = 2;
  return result;
}

auto big_from_digits(const char *digits, std::uint32_t count)
    -> BigCoefficient * {
  auto word_count = (count + BASE_DIGITS - 1) / BASE_DIGITS;
  auto *result = big_allocate(word_count);
  result->length = word_count;

  for (std::uint32_t word_index = 0; word_index < word_count; word_index++) {
    std::uint64_t word = 0;
    auto end_position = count - word_index * BASE_DIGITS;
    auto start_position =
        end_position > static_cast<std::uint32_t>(BASE_DIGITS)
            ? end_position - static_cast<std::uint32_t>(BASE_DIGITS)
            : 0U;
    for (auto position = start_position; position < end_position; position++) {
      word = word * 10 + static_cast<std::uint64_t>(digits[position] - '0');
    }
    result->words[word_index] = word;
  }

  big_trim(result);
  return result;
}

// Lemire, "Computing the number of digits of an integer even faster" (2021)
auto digit_count(std::uint64_t value) -> std::uint32_t {
  if (value == 0) {
    return 1;
  }
#if defined(_MSC_VER)
  unsigned long bit_index;
  _BitScanReverse64(&bit_index, value);
  auto const bits = static_cast<std::uint32_t>(bit_index);
#else
  auto const bits = static_cast<std::uint32_t>(63 - __builtin_clzll(value));
#endif
  auto approximation = 1 + ((bits * 77) >> 8);
  if (approximation <= 19 && value >= POWERS_OF_10[approximation]) {
    approximation++;
  }
  return approximation;
}

auto store_big_pointer(std::int64_t &coefficient, BigCoefficient *big) -> void {
  // NOLINTNEXTLINE(performance-no-int-to-ptr)
  auto as_integer = reinterpret_cast<std::uintptr_t>(big);
  std::memcpy(&coefficient, &as_integer, sizeof(coefficient));
}

auto load_big_pointer(std::int64_t coefficient) -> BigCoefficient * {
  std::uintptr_t as_integer;
  std::memcpy(&as_integer, &coefficient, sizeof(as_integer));
  // NOLINTNEXTLINE(performance-no-int-to-ptr)
  return reinterpret_cast<BigCoefficient *>(as_integer);
}

auto coefficient_to_digit_string(std::int64_t coefficient,
                                 std::uint64_t coefficient_hi,
                                 std::uint8_t flags) -> std::string {
  if (flags & FLAG_HEAP) {
    return big_to_string(load_big_pointer(coefficient));
  }
  if (flags & FLAG_BIG) {
    auto high_string = std::to_string(coefficient_hi);
    auto low_string = std::to_string(static_cast<std::uint64_t>(coefficient));
    std::string result = high_string;
    result.append(static_cast<std::size_t>(BASE_DIGITS) - low_string.size(),
                  '0');
    result += low_string;
    return result;
  }
  return std::to_string(coefficient);
}

auto modular_pow10(std::uint32_t exponent, std::uint64_t modulus)
    -> std::uint64_t {
  if (modulus == 1) {
    return 0;
  }
  if (exponent == 0) {
    return 1;
  }
  if (exponent <= 19) {
    return POWERS_OF_10[exponent] % modulus;
  }
  std::uint64_t result = 1;
  std::uint64_t base = 10 % modulus;
  auto remaining = exponent;
  while (remaining > 0) {
    if (remaining & 1) {
      result = static_cast<std::uint64_t>(static_cast<uint128_t>(result) *
                                          base % modulus);
    }
    base = static_cast<std::uint64_t>(static_cast<uint128_t>(base) * base %
                                      modulus);
    remaining >>= 1;
  }
  return result;
}

// Round-half-even (banker's rounding) to WORKING_PRECISION significant digits
constexpr std::int32_t WORKING_PRECISION = 16;

auto round_to_precision(std::int64_t &coefficient,
                        std::uint64_t &coefficient_hi, std::int32_t &exponent,
                        std::uint8_t &flags) -> void {
  if (flags & (FLAG_NAN | FLAG_SNAN | FLAG_INFINITE)) {
    return;
  }

  if (flags & FLAG_BIG) {
    auto digit_string =
        coefficient_to_digit_string(coefficient, coefficient_hi, flags);
    auto total_digits = static_cast<std::int32_t>(digit_string.size());
    if (total_digits <= WORKING_PRECISION) {
      return;
    }

    auto excess = total_digits - WORKING_PRECISION;

    auto kept =
        digit_string.substr(0, static_cast<std::size_t>(WORKING_PRECISION));
    auto dropped =
        digit_string.substr(static_cast<std::size_t>(WORKING_PRECISION));

    bool round_up = false;
    if (!dropped.empty()) {
      if (dropped[0] > '5') {
        round_up = true;
      } else if (dropped[0] == '5') {
        bool has_trailing = false;
        for (std::size_t index = 1; index < dropped.size(); index++) {
          if (dropped[index] != '0') {
            has_trailing = true;
            break;
          }
        }
        if (has_trailing) {
          round_up = true;
        } else {
          round_up = (kept.back() - '0') % 2 != 0;
        }
      }
    }

    if (flags & FLAG_HEAP) {
      big_free(load_big_pointer(coefficient));
    }

    std::int64_t new_coefficient = 0;
    for (auto character : kept) {
      new_coefficient = new_coefficient * 10 + (character - '0');
    }
    if (round_up) {
      new_coefficient++;
    }

    coefficient = new_coefficient;
    coefficient_hi = 0;
    exponent += excess;
    flags = static_cast<std::uint8_t>(flags & ~(FLAG_BIG | FLAG_HEAP));
    return;
  }

  auto digits = digit_count(static_cast<std::uint64_t>(coefficient));
  if (static_cast<std::int32_t>(digits) <= WORKING_PRECISION) {
    return;
  }

  auto excess = static_cast<std::int32_t>(digits) - WORKING_PRECISION;
  std::int64_t divisor = 1;
  for (std::int32_t index = 0; index < excess; index++) {
    divisor *= 10;
  }

  auto quotient = coefficient / divisor;
  auto remainder = coefficient % divisor;
  auto half = divisor / 2;

  if (remainder > half || (remainder == half && quotient % 2 != 0)) {
    quotient++;
  }

  coefficient = quotient;
  coefficient_hi = 0;
  exponent += excess;
}

void free_big_coefficient(std::int64_t coefficient, std::uint8_t flags) {
  if (flags & FLAG_HEAP) {
    big_free(load_big_pointer(coefficient));
  }
}

auto big_to_uint128(const BigCoefficient *big, std::int32_t exponent)
    -> uint128_t {
  uint128_t value = 0;
  for (auto index = big->length; index > 0; index--) {
    value = value * BASE + big->words[index - 1];
  }
  while (exponent > 0) {
    value *= 10;
    exponent--;
  }
  return value;
}

auto coefficient_as_big(std::int64_t coefficient, std::uint64_t coefficient_hi,
                        std::uint8_t flags) -> BigCoefficient * {
  if (flags & FLAG_HEAP) {
    return big_clone(load_big_pointer(coefficient));
  }
  if (flags & FLAG_BIG) {
    auto *result = big_allocate(2);
    result->words[0] = static_cast<std::uint64_t>(coefficient);
    result->words[1] = coefficient_hi;
    result->length = coefficient_hi > 0 ? 2 : 1;
    return result;
  }
  return big_from_uint64(static_cast<std::uint64_t>(coefficient));
}

void align_big_exponents(BigCoefficient *&left, BigCoefficient *&right,
                         std::int32_t left_exponent,
                         std::int32_t right_exponent) {
  if (left_exponent > right_exponent) {
    auto *aligned = big_multiply_pow10(
        left, static_cast<std::uint32_t>(left_exponent - right_exponent));
    big_free(left);
    left = aligned;
  } else if (right_exponent > left_exponent) {
    auto *aligned = big_multiply_pow10(
        right, static_cast<std::uint32_t>(right_exponent - left_exponent));
    big_free(right);
    right = aligned;
  }
}

struct BigAddSignedResult {
  BigCoefficient *coefficient;
  bool is_negative;
};

auto big_add_signed(BigCoefficient *left, BigCoefficient *right,
                    bool left_negative, bool right_negative)
    -> BigAddSignedResult {
  if (left_negative == right_negative) {
    return {.coefficient = big_add(left, right), .is_negative = left_negative};
  }
  auto comparison = big_compare(left, right);
  if (comparison >= 0) {
    return {.coefficient = big_subtract(left, right),
            .is_negative = left_negative};
  }
  return {.coefficient = big_subtract(right, left),
          .is_negative = right_negative};
}

void store_big_result(std::int64_t &coefficient, std::uint64_t &coefficient_hi,
                      std::uint8_t &flags, BigCoefficient *result_big,
                      bool result_negative) {
  if (result_big->length <= 1 &&
      result_big->words[0] <= static_cast<std::uint64_t>(COMPACT_MAX)) {
    coefficient = static_cast<std::int64_t>(result_big->words[0]);
    coefficient_hi = 0;
    flags = result_negative ? FLAG_SIGN : 0;
    big_free(result_big);
  } else if (result_big->length <= 2) {
    coefficient = static_cast<std::int64_t>(result_big->words[0]);
    coefficient_hi = result_big->length > 1 ? result_big->words[1] : 0;
    flags =
        static_cast<std::uint8_t>(FLAG_BIG | (result_negative ? FLAG_SIGN : 0));
    big_free(result_big);
  } else {
    store_big_pointer(coefficient, result_big);
    coefficient_hi = 0;
    flags = static_cast<std::uint8_t>(FLAG_BIG | FLAG_HEAP |
                                      (result_negative ? FLAG_SIGN : 0));
  }
}

} // namespace

#endif
