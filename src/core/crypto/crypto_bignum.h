#ifndef SOURCEMETA_CORE_CRYPTO_BIGNUM_H_
#define SOURCEMETA_CORE_CRYPTO_BIGNUM_H_

// Fixed-capacity unsigned big integer arithmetic for the reference signature
// backend, with one capacity sized for RSA and a smaller one for the curves.
// The verification paths consume only public inputs and stay variable time; the
// signing paths use the constant-time layer below (fixed-width multiply,
// Barrett reduction, masked select and inverse) on their secret operands. The
// Barrett context precompute stays variable time for a public modulus, and has
// a constant-time form for the secret prime factors of an RSA key

#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/text.h>

#include <algorithm>   // std::max, std::min
#include <array>       // std::array
#include <bit>         // std::bit_width
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

using BignumDoubleWord = uint128_t;

template <std::size_t Capacity> struct BasicBignum {
  static constexpr std::size_t CAPACITY{Capacity};
  std::array<std::uint64_t, Capacity> words{};
  std::size_t size{0};
};

// Enough words for an 8192-bit product plus shifting headroom
inline constexpr std::size_t BIGNUM_CAPACITY{130};
using Bignum = BasicBignum<BIGNUM_CAPACITY>;

// Enough words for the double-width products of the largest curve field, P-521
// at nine words, for the Barrett estimates built on them, and for the 114-octet
// Ed448 digest, so the curve routines do not zero and copy the RSA capacity on
// every temporary
inline constexpr std::size_t CURVE_BIGNUM_CAPACITY{32};
using CurveBignum = BasicBignum<CURVE_BIGNUM_CAPACITY>;

// Overwrite the whole word buffer of a big integer that held secret material,
// including the words past the current size that intermediate operations wrote,
// so it does not linger in freed memory. The volatile access stops the compiler
// from eliding the write as a dead store
template <std::size_t Capacity>
inline auto secure_zero(BasicBignum<Capacity> &value) noexcept -> void {
  auto *pointer{reinterpret_cast<volatile unsigned char *>(value.words.data())};
  for (std::size_t index{0}; index < sizeof(value.words); index += 1) {
    pointer[index] = 0;
  }

  value.size = 0;
}

// Overwrite the referenced big integer when leaving the current scope, so a
// secret value a local holds is wiped across every return path without
// threading a manual call through each one
template <std::size_t Capacity> struct SecureBignumScope {
  explicit SecureBignumScope(BasicBignum<Capacity> &value) noexcept
      : target{value} {}
  SecureBignumScope(const SecureBignumScope &) = delete;
  auto operator=(const SecureBignumScope &) -> SecureBignumScope & = delete;
  SecureBignumScope(SecureBignumScope &&) = delete;
  auto operator=(SecureBignumScope &&) -> SecureBignumScope & = delete;
  ~SecureBignumScope() { secure_zero(this->target); }
  BasicBignum<Capacity> &target;
};

template <std::size_t Capacity>
inline auto bignum_normalize(BasicBignum<Capacity> &value) noexcept -> void {
  const auto *value_data{value.words.data()};
  while (value.size > 0 && value_data[value.size - 1] == 0) {
    value.size -= 1;
  }
}

template <std::size_t Capacity = BIGNUM_CAPACITY>
inline auto bignum_from_bytes(const std::string_view input) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  std::size_t bytes_consumed{0};
  for (std::size_t index = input.size(); index > 0; --index) {
    const auto byte{static_cast<std::uint8_t>(input[index - 1])};
    const auto word_index{bytes_consumed / 8};
    if (word_index >= Capacity) {
      break;
    }

    result.words[word_index] |= static_cast<std::uint64_t>(byte)
                                << (8 * (bytes_consumed % 8));
    bytes_consumed += 1;
  }

  result.size = (bytes_consumed + 7) / 8;
  bignum_normalize(result);
  return result;
}

template <std::size_t Capacity = BIGNUM_CAPACITY>
inline auto bignum_from_u64(const std::uint64_t value) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  if (value > 0) {
    result.words[0] = value;
    result.size = 1;
  }

  return result;
}

template <std::size_t Capacity = BIGNUM_CAPACITY>
inline auto bignum_from_hex(const std::string_view hex)
    -> BasicBignum<Capacity> {
  // An odd length is decoded as if a zero nibble had been prepended
  const auto bytes{hex_to_bytes(hex, true)};
  if (!bytes.has_value()) {
    return BasicBignum<Capacity>{};
  }

  return bignum_from_bytes<Capacity>(bytes.value());
}

template <std::size_t Capacity>
inline auto bignum_is_zero(const BasicBignum<Capacity> &value) noexcept
    -> bool {
  return value.size == 0;
}

template <std::size_t Capacity>
inline auto bignum_compare(const BasicBignum<Capacity> &left,
                           const BasicBignum<Capacity> &right) noexcept -> int {
  if (left.size != right.size) {
    return left.size < right.size ? -1 : 1;
  }

  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  for (std::size_t index = left.size; index > 0; --index) {
    if (left_data[index - 1] != right_data[index - 1]) {
      return left_data[index - 1] < right_data[index - 1] ? -1 : 1;
    }
  }

  return 0;
}

template <std::size_t Capacity>
inline auto bignum_bit_length(const BasicBignum<Capacity> &value) noexcept
    -> std::size_t {
  if (value.size == 0) {
    return 0;
  }

  return ((value.size - 1) * 64) +
         static_cast<std::size_t>(std::bit_width(value.words[value.size - 1]));
}

template <std::size_t Capacity>
inline auto bignum_get_bit(const BasicBignum<Capacity> &value,
                           const std::size_t bit) noexcept -> bool {
  const auto word{bit / 64};
  if (word >= value.size) {
    return false;
  }

  return ((value.words[word] >> (bit % 64)) & 1U) != 0;
}

// The same read without the size-dependent early return, so the signing ladders
// do not reveal the secret scalar's length through the branch. The bit index is
// bounded by the public curve size, so the guard is on public data
template <std::size_t Capacity>
inline auto bignum_get_bit_fixed(const BasicBignum<Capacity> &value,
                                 const std::size_t bit) noexcept -> bool {
  const auto word{bit / 64};
  return word < Capacity && ((value.words[word] >> (bit % 64)) & 1U) != 0;
}

// Assumes the result fits in the capacity
template <std::size_t Capacity>
inline auto bignum_shift_left(const BasicBignum<Capacity> &value,
                              const std::size_t bits) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto word_shift{bits / 64};
  const auto bit_shift{bits % 64};
  result.size = value.size + word_shift + 1;
  result.size = std::min(result.size, Capacity);

  const auto *value_data{value.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < value.size; ++index) {
    const auto destination{index + word_shift};
    if (destination >= Capacity) {
      break;
    }

    result_data[destination] |= value_data[index] << bit_shift;
    if (bit_shift > 0 && destination + 1 < Capacity) {
      result_data[destination + 1] |= value_data[index] >> (64U - bit_shift);
    }
  }

  bignum_normalize(result);
  return result;
}

// Assumes the left operand is greater than or equal to the right one
template <std::size_t Capacity>
inline auto
bignum_subtract_in_place(BasicBignum<Capacity> &left,
                         const BasicBignum<Capacity> &right) noexcept -> void {
  auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  std::uint64_t borrow{0};
  for (std::size_t index = 0; index < left.size; ++index) {
    const auto subtrahend{index < right.size ? right_data[index] : 0};
    const auto previous{left_data[index]};
    left_data[index] = previous - subtrahend - borrow;
    borrow = (previous < subtrahend || (borrow == 1 && previous == subtrahend))
                 ? 1
                 : 0;
  }

  bignum_normalize(left);
}

template <std::size_t Capacity>
inline auto bignum_shift_right(const BasicBignum<Capacity> &value,
                               const std::size_t bits) noexcept
    -> BasicBignum<Capacity>;

// Reduce a value modulo the modulus with Knuth's Algorithm D (TAOCP Volume 2,
// Section 4.3.1), the schoolbook long division that estimates one quotient
// word per step rather than one bit, so the cost is quadratic in the number of
// words rather than the number of bits
template <std::size_t Capacity>
inline auto bignum_reduce(BasicBignum<Capacity> &value,
                          const BasicBignum<Capacity> &modulus) noexcept
    -> void {
  if (bignum_compare(value, modulus) < 0) {
    return;
  }

  const auto divisor_words{modulus.size};

  // A zero modulus defines no residue to reduce into, and the division below
  // reads the top two divisor words
  if (divisor_words == 0) {
    return;
  }

  // A single-word divisor folds the value down word by word
  if (divisor_words == 1) {
    const auto divisor{modulus.words[0]};
    const auto *value_data{value.words.data()};
    BignumDoubleWord remainder{0};
    for (std::size_t index = value.size; index > 0; --index) {
      remainder = (remainder << 64U) | value_data[index - 1];
      remainder %= divisor;
    }

    value = bignum_from_u64<Capacity>(static_cast<std::uint64_t>(remainder));
    return;
  }

  // Normalize so the divisor's top word has its high bit set, which bounds the
  // error of each quotient word estimate to at most two
  const auto shift{static_cast<std::size_t>(
      (64U - (bignum_bit_length(modulus) % 64U)) % 64U)};
  const auto divisor{shift > 0 ? bignum_shift_left(modulus, shift) : modulus};
  auto dividend{shift > 0 ? bignum_shift_left(value, shift) : value};
  const auto *divisor_data{divisor.words.data()};
  auto *dividend_data{dividend.words.data()};
  const auto dividend_words{dividend.size};
  const auto quotient_words{dividend_words - divisor_words};
  const auto top{divisor_data[divisor_words - 1]};
  const auto next{divisor_data[divisor_words - 2]};
  const BignumDoubleWord base{static_cast<BignumDoubleWord>(1) << 64U};

  for (std::size_t step = quotient_words + 1; step > 0; --step) {
    const auto offset{step - 1};

    // Estimate the quotient word from the top two words of the running value
    const auto numerator{
        (static_cast<BignumDoubleWord>(dividend_data[offset + divisor_words])
         << 64U) |
        dividend_data[offset + divisor_words - 1]};
    auto estimate{numerator / top};
    auto estimate_remainder{numerator - (estimate * top)};
    while (estimate >= base ||
           estimate * next > (estimate_remainder << 64U) +
                                 dividend_data[offset + divisor_words - 2]) {
      estimate -= 1;
      estimate_remainder += top;
      if (estimate_remainder >= base) {
        break;
      }
    }

    // Multiply the divisor by the estimate and subtract from the running value
    const auto quotient_word{static_cast<std::uint64_t>(estimate)};
    BignumDoubleWord carry{0};
    std::uint64_t borrow{0};
    for (std::size_t index = 0; index < divisor_words; ++index) {
      const auto product{
          (static_cast<BignumDoubleWord>(quotient_word) * divisor_data[index]) +
          carry};
      carry = product >> 64U;
      const auto subtrahend{static_cast<std::uint64_t>(product)};
      const auto current{dividend_data[offset + index]};
      const auto without_subtrahend{current - subtrahend};
      auto next_borrow{current < subtrahend ? 1U : 0U};
      const auto result_word{without_subtrahend - borrow};
      if (without_subtrahend < borrow) {
        next_borrow += 1U;
      }

      dividend_data[offset + index] = result_word;
      borrow = next_borrow;
    }

    const auto current{dividend_data[offset + divisor_words]};
    const auto subtrahend{static_cast<std::uint64_t>(carry)};
    const auto without_subtrahend{current - subtrahend};
    auto next_borrow{current < subtrahend ? 1U : 0U};
    dividend_data[offset + divisor_words] = without_subtrahend - borrow;
    if (without_subtrahend < borrow) {
      next_borrow += 1U;
    }

    // The estimate was at most one too large, so add the divisor back when the
    // subtraction borrowed past the top
    if (next_borrow != 0) {
      BignumDoubleWord add_carry{0};
      for (std::size_t index = 0; index < divisor_words; ++index) {
        const auto sum{
            static_cast<BignumDoubleWord>(dividend_data[offset + index]) +
            divisor_data[index] + add_carry};
        dividend_data[offset + index] = static_cast<std::uint64_t>(sum);
        add_carry = sum >> 64U;
      }

      dividend_data[offset + divisor_words] +=
          static_cast<std::uint64_t>(add_carry);
    }
  }

  // The remainder occupies the low words, still scaled by the normalization
  dividend.size = divisor_words;
  bignum_normalize(dividend);
  value = shift > 0 ? bignum_shift_right(dividend, shift) : dividend;
}

// Assumes both operands fit in half the capacity
template <std::size_t Capacity>
inline auto bignum_multiply(const BasicBignum<Capacity> &left,
                            const BasicBignum<Capacity> &right) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  result.size = left.size + right.size;
  result.size = std::min(result.size, Capacity);

  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t left_index = 0; left_index < left.size; ++left_index) {
    std::uint64_t carry{0};
    for (std::size_t right_index = 0; right_index < right.size; ++right_index) {
      const auto destination{left_index + right_index};
      if (destination >= Capacity) {
        break;
      }

      const auto product{(static_cast<BignumDoubleWord>(left_data[left_index]) *
                          right_data[right_index]) +
                         result_data[destination] + carry};
      result_data[destination] = static_cast<std::uint64_t>(product);
      carry = static_cast<std::uint64_t>(product >> 64U);
    }

    const auto carry_destination{left_index + right.size};
    if (carry_destination < Capacity) {
      result_data[carry_destination] += carry;
    }
  }

  bignum_normalize(result);
  return result;
}

template <std::size_t Capacity>
inline auto bignum_mod_exp(const BasicBignum<Capacity> &base,
                           const BasicBignum<Capacity> &exponent,
                           const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  result.words[0] = 1;
  result.size = 1;

  auto reduced_base{base};
  bignum_reduce(reduced_base, modulus);

  const auto exponent_bits{bignum_bit_length(exponent)};
  for (std::size_t index = exponent_bits; index > 0; --index) {
    result = bignum_multiply(result, result);
    bignum_reduce(result, modulus);
    if (bignum_get_bit(exponent, index - 1)) {
      result = bignum_multiply(result, reduced_base);
      bignum_reduce(result, modulus);
    }
  }

  return result;
}

template <std::size_t Capacity>
inline auto bignum_add(const BasicBignum<Capacity> &left,
                       const BasicBignum<Capacity> &right) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto larger{left.size > right.size ? left.size : right.size};
  std::uint64_t carry{0};
  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < larger; ++index) {
    const auto first{index < left.size ? left_data[index] : 0};
    const auto second{index < right.size ? right_data[index] : 0};
    const auto sum{static_cast<BignumDoubleWord>(first) + second + carry};
    result_data[index] = static_cast<std::uint64_t>(sum);
    carry = static_cast<std::uint64_t>(sum >> 64U);
  }

  result.size = larger;
  if (carry > 0 && larger < Capacity) {
    result_data[larger] = carry;
    result.size = larger + 1;
  }

  bignum_normalize(result);
  return result;
}

template <std::size_t Capacity>
inline auto bignum_shift_right(const BasicBignum<Capacity> &value,
                               const std::size_t bits) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto word_shift{bits / 64};
  const auto bit_shift{bits % 64};
  if (word_shift >= value.size) {
    return result;
  }

  result.size = value.size - word_shift;
  const auto *value_data{value.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < result.size; ++index) {
    auto word{value_data[index + word_shift] >> bit_shift};
    if (bit_shift > 0 && index + word_shift + 1 < value.size) {
      word |= value_data[index + word_shift + 1] << (64U - bit_shift);
    }

    result_data[index] = word;
  }

  bignum_normalize(result);
  return result;
}

// All modular helpers below assume their operands are already reduced to
// less than the modulus, as the elliptic curve routines guarantee

template <std::size_t Capacity>
inline auto bignum_mod_add(const BasicBignum<Capacity> &left,
                           const BasicBignum<Capacity> &right,
                           const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  auto result{bignum_add(left, right)};
  if (bignum_compare(result, modulus) >= 0) {
    bignum_subtract_in_place(result, modulus);
  }

  return result;
}

template <std::size_t Capacity>
inline auto bignum_mod_subtract(const BasicBignum<Capacity> &left,
                                const BasicBignum<Capacity> &right,
                                const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  if (bignum_compare(left, right) >= 0) {
    auto result{left};
    bignum_subtract_in_place(result, right);
    return result;
  }

  auto result{bignum_add(left, modulus)};
  bignum_subtract_in_place(result, right);
  return result;
}

template <std::size_t Capacity>
inline auto bignum_mod_multiply(const BasicBignum<Capacity> &left,
                                const BasicBignum<Capacity> &right,
                                const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  auto result{bignum_multiply(left, right)};
  bignum_reduce(result, modulus);
  return result;
}

// Halve a value modulo an odd modulus: an even value shifts down, an odd one
// becomes even by adding the modulus first, so the result stays an integer
template <std::size_t Capacity>
inline auto bignum_mod_halve(const BasicBignum<Capacity> &value,
                             const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  if ((value.words[0] & 1U) == 0) {
    return bignum_shift_right(value, 1);
  }

  return bignum_shift_right(bignum_add(value, modulus), 1);
}

// Modular inverse by the binary extended Euclidean algorithm, which needs only
// halving, subtraction, and comparison rather than the modular exponentiation
// a Fermat inverse over a prime modulus would spend. The modulus must be odd.
// Returns zero when the value has no inverse, which is when it shares a factor
// with the modulus or reduces to zero
template <std::size_t Capacity>
inline auto bignum_mod_inverse(const BasicBignum<Capacity> &value,
                               const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBignum<Capacity> {
  const auto one{bignum_from_u64<Capacity>(1)};
  auto first{value};
  bignum_reduce(first, modulus);
  auto second{modulus};
  auto first_coefficient{one};
  BasicBignum<Capacity> second_coefficient;

  while (bignum_compare(first, one) != 0 && bignum_compare(second, one) != 0) {
    // A side reaching zero means the greatest common divisor exceeds one, so no
    // inverse exists. Stopping here also keeps the halving below from spinning
    // forever on a zero value
    if (bignum_is_zero(first) || bignum_is_zero(second)) {
      return {};
    }

    while ((first.words[0] & 1U) == 0) {
      first = bignum_shift_right(first, 1);
      first_coefficient = bignum_mod_halve(first_coefficient, modulus);
    }

    while ((second.words[0] & 1U) == 0) {
      second = bignum_shift_right(second, 1);
      second_coefficient = bignum_mod_halve(second_coefficient, modulus);
    }

    if (bignum_compare(first, second) >= 0) {
      bignum_subtract_in_place(first, second);
      first_coefficient =
          bignum_mod_subtract(first_coefficient, second_coefficient, modulus);
    } else {
      bignum_subtract_in_place(second, first);
      second_coefficient =
          bignum_mod_subtract(second_coefficient, first_coefficient, modulus);
    }
  }

  return bignum_compare(first, one) == 0 ? first_coefficient
                                         : second_coefficient;
}

// A branch-free select, so a secret condition does not steer control flow. It
// blends the given number of words, a count fixed by the public operand width,
// so neither the condition nor the running size steers the loop
template <std::size_t Capacity>
inline auto bignum_conditional_select(const bool condition,
                                      const BasicBignum<Capacity> &when_true,
                                      const BasicBignum<Capacity> &when_false,
                                      const std::size_t words) noexcept
    -> BasicBignum<Capacity> {
  const std::uint64_t mask{std::uint64_t{0} -
                           static_cast<std::uint64_t>(condition)};
  BasicBignum<Capacity> result;
  const auto *true_data{when_true.words.data()};
  const auto *false_data{when_false.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < words; ++index) {
    result_data[index] =
        (true_data[index] & mask) | (false_data[index] & ~mask);
  }

  const std::size_t size_mask{std::size_t{0} -
                              static_cast<std::size_t>(condition)};
  result.size = (when_true.size & size_mask) | (when_false.size & ~size_mask);
  return result;
}

// Fixed-width subtraction over the given number of words, returning the final
// borrow. It always visits every word, so its timing does not depend on where
// the operands' significant words fall the way the size-driven routines above
// do
template <std::size_t Capacity>
inline auto bignum_subtract_fixed(const BasicBignum<Capacity> &left,
                                  const BasicBignum<Capacity> &right,
                                  const std::size_t words,
                                  BasicBignum<Capacity> &out) noexcept
    -> std::uint64_t {
  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  auto *out_data{out.words.data()};
  std::uint64_t borrow{0};
  for (std::size_t index = 0; index < words; ++index) {
    const auto left_word{left_data[index]};
    const auto right_word{right_data[index]};
    const auto without_right{left_word - right_word};
    const std::uint64_t borrow_from_right{left_word < right_word ? 1U : 0U};
    const auto result_word{without_right - borrow};
    const std::uint64_t borrow_from_previous{without_right < borrow ? 1U : 0U};
    out_data[index] = result_word;
    borrow = borrow_from_right | borrow_from_previous;
  }

  out.size = words;
  return borrow;
}

// Multiply visiting exactly the given word counts, so timing does not reveal
// where either operand's significant words fall
template <std::size_t Capacity>
inline auto bignum_multiply_fixed(const BasicBignum<Capacity> &left,
                                  const BasicBignum<Capacity> &right,
                                  const std::size_t left_words,
                                  const std::size_t right_words) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t left_index = 0; left_index < left_words; ++left_index) {
    std::uint64_t carry{0};
    for (std::size_t right_index = 0; right_index < right_words;
         ++right_index) {
      const auto destination{left_index + right_index};
      const auto product{(static_cast<BignumDoubleWord>(left_data[left_index]) *
                          right_data[right_index]) +
                         result_data[destination] + carry};
      result_data[destination] = static_cast<std::uint64_t>(product);
      carry = static_cast<std::uint64_t>(product >> 64U);
    }

    result_data[left_index + right_words] = carry;
  }

  result.size = left_words + right_words;
  return result;
}

// The same product truncated to its given number of low words, visiting only
// the columns below that width, for a reduction that discards the high words
template <std::size_t Capacity>
inline auto bignum_multiply_low_fixed(const BasicBignum<Capacity> &left,
                                      const BasicBignum<Capacity> &right,
                                      const std::size_t left_words,
                                      const std::size_t right_words,
                                      const std::size_t width) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  auto *result_data{result.words.data()};
  const auto rows{std::min(left_words, width)};
  for (std::size_t left_index = 0; left_index < rows; ++left_index) {
    std::uint64_t carry{0};
    const auto columns{std::min(right_words, width - left_index)};
    for (std::size_t right_index = 0; right_index < columns; ++right_index) {
      const auto destination{left_index + right_index};
      const auto product{(static_cast<BignumDoubleWord>(left_data[left_index]) *
                          right_data[right_index]) +
                         result_data[destination] + carry};
      result_data[destination] = static_cast<std::uint64_t>(product);
      carry = static_cast<std::uint64_t>(product >> 64U);
    }

    if (left_index + columns < width) {
      result_data[left_index + columns] = carry;
    }
  }

  result.size = width;
  return result;
}

// The square of a value over the given number of words, a count fixed by the
// public width. Each cross product appears twice in a square, so it is computed
// once and doubled before the diagonal squares are added
template <std::size_t Capacity>
inline auto bignum_square_fixed(const BasicBignum<Capacity> &value,
                                const std::size_t words) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto *value_data{value.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t left_index = 0; left_index < words; ++left_index) {
    std::uint64_t carry{0};
    for (std::size_t right_index = left_index + 1; right_index < words;
         ++right_index) {
      const auto destination{left_index + right_index};
      const auto product{
          (static_cast<BignumDoubleWord>(value_data[left_index]) *
           value_data[right_index]) +
          result_data[destination] + carry};
      result_data[destination] = static_cast<std::uint64_t>(product);
      carry = static_cast<std::uint64_t>(product >> 64U);
    }

    result_data[left_index + words] = carry;
  }

  std::uint64_t shifted_out{0};
  for (std::size_t index = 0; index < 2 * words; ++index) {
    const auto word{result_data[index]};
    result_data[index] = (word << 1U) | shifted_out;
    shifted_out = word >> 63U;
  }

  std::uint64_t carry{0};
  for (std::size_t index = 0; index < words; ++index) {
    const auto square{static_cast<BignumDoubleWord>(value_data[index]) *
                      value_data[index]};
    const auto low{static_cast<BignumDoubleWord>(result_data[2 * index]) +
                   static_cast<std::uint64_t>(square) + carry};
    result_data[2 * index] = static_cast<std::uint64_t>(low);
    const auto high{
        static_cast<BignumDoubleWord>(result_data[(2 * index) + 1]) +
        static_cast<std::uint64_t>(square >> 64U) +
        static_cast<std::uint64_t>(low >> 64U)};
    result_data[(2 * index) + 1] = static_cast<std::uint64_t>(high);
    carry = static_cast<std::uint64_t>(high >> 64U);
  }

  result.size = 2 * words;
  return result;
}

// A word-granular right shift dropping the low words, and its counterpart that
// keeps them, for the word-aligned truncations the Barrett reduction needs. The
// shift keeps the given number of words above the dropped ones, a count fixed
// by the public modulus width rather than by the operand size
template <std::size_t Capacity>
inline auto bignum_drop_low_words(const BasicBignum<Capacity> &value,
                                  const std::size_t words,
                                  const std::size_t width) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto *value_data{value.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < width && index + words < Capacity;
       ++index) {
    result_data[index] = value_data[index + words];
  }

  result.size = value.size > words ? std::min(value.size - words, width) : 0;
  return result;
}

template <std::size_t Capacity>
inline auto bignum_keep_low_words(const BasicBignum<Capacity> &value,
                                  const std::size_t words) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> result;
  const auto *value_data{value.words.data()};
  auto *result_data{result.words.data()};
  for (std::size_t index = 0; index < words; ++index) {
    result_data[index] = value_data[index];
  }

  result.size = words;
  return result;
}

// Quotient of a division, for building a public reduction constant only, so a
// plain bit-by-bit long division is enough
template <std::size_t Capacity>
inline auto bignum_divide(const BasicBignum<Capacity> &numerator,
                          const BasicBignum<Capacity> &denominator) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> quotient;
  BasicBignum<Capacity> remainder;
  const auto bits{bignum_bit_length(numerator)};
  for (std::size_t index = bits; index > 0; --index) {
    remainder = bignum_shift_left(remainder, 1);
    if (bignum_get_bit(numerator, index - 1)) {
      remainder.words[0] |= 1U;
      if (remainder.size == 0) {
        remainder.size = 1;
      }
    }

    if (bignum_compare(remainder, denominator) >= 0) {
      bignum_subtract_in_place(remainder, denominator);
      quotient.words[(index - 1) / 64] |= std::uint64_t{1}
                                          << ((index - 1) % 64);
      quotient.size = std::max(quotient.size, ((index - 1) / 64) + 1);
    }
  }

  return quotient;
}

// Precomputed constants for Barrett reduction modulo a fixed modulus. The plain
// setup below reads a public modulus, so it need not be constant time
template <std::size_t Capacity> struct BasicBarrettContext {
  BasicBignum<Capacity> modulus;
  std::size_t words;
  BasicBignum<Capacity> factor;
};

using BarrettContext = BasicBarrettContext<BIGNUM_CAPACITY>;
using CurveBarrettContext = BasicBarrettContext<CURVE_BIGNUM_CAPACITY>;

template <std::size_t Capacity>
inline auto barrett_context(const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBarrettContext<Capacity> {
  BasicBarrettContext<Capacity> context;
  context.modulus = modulus;
  context.words = modulus.size;
  const auto power{
      bignum_shift_left(bignum_from_u64<Capacity>(1), 128 * context.words)};
  context.factor = bignum_divide(power, modulus);
  return context;
}

// The same constants for a secret modulus, such as a prime factor of an RSA
// key, built in constant time. The quotient comes from binary long division
// over the public bit length of the power, where a fixed-width subtraction and
// a masked select stand in for each comparison
template <std::size_t Capacity>
inline auto barrett_context_ct(const BasicBignum<Capacity> &modulus) noexcept
    -> BasicBarrettContext<Capacity> {
  BasicBarrettContext<Capacity> context;
  context.modulus = modulus;
  context.words = modulus.size;
  const auto width{context.words + 1};
  const auto power_bits{(128 * context.words) + 1};
  BasicBignum<Capacity> remainder;
  remainder.size = width;
  BasicBignum<Capacity> quotient;
  for (std::size_t index = power_bits; index > 0; --index) {
    // The power is a single set bit above zeros, so only its top bit carries in
    std::uint64_t carry{index == power_bits ? 1U : 0U};
    auto *remainder_data{remainder.words.data()};
    for (std::size_t word = 0; word < width; ++word) {
      const auto current{remainder_data[word]};
      remainder_data[word] = (current << 1U) | carry;
      carry = current >> 63U;
    }

    BasicBignum<Capacity> trial;
    const auto borrow{bignum_subtract_fixed(remainder, modulus, width, trial)};
    remainder = bignum_conditional_select(borrow == 0, trial, remainder, width);
    quotient.words[(index - 1) / 64] |= static_cast<std::uint64_t>(borrow == 0)
                                        << ((index - 1) % 64);
  }

  secure_zero(remainder);
  quotient.size = width;
  context.factor = quotient;
  secure_zero(quotient);
  return context;
}

// If the value is at least the modulus over the given width, subtract it. The
// choice is a masked select rather than a branch
template <std::size_t Capacity>
inline auto bignum_conditional_subtract(const BasicBignum<Capacity> &value,
                                        const BasicBignum<Capacity> &modulus,
                                        const std::size_t words) noexcept
    -> BasicBignum<Capacity> {
  BasicBignum<Capacity> reduced;
  const auto borrow{bignum_subtract_fixed(value, modulus, words, reduced)};
  return bignum_conditional_select(borrow == 0, reduced, value, words);
}

// Reduce a value below the square of the modulus down to the modulus in
// constant time (Handbook of Applied Cryptography Algorithm 14.42). The
// estimate is at most two too small, so three masked subtractions always finish
// the reduction
template <std::size_t Capacity>
inline auto
barrett_reduce(const BasicBignum<Capacity> &value,
               const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  const auto width{context.words};
  const auto high{bignum_drop_low_words(value, width - 1, width + 1)};
  const auto estimate{
      bignum_multiply_fixed(high, context.factor, width + 1, width + 1)};
  const auto quotient{bignum_drop_low_words(estimate, width + 1, width + 1)};
  const auto value_low{bignum_keep_low_words(value, width + 1)};
  const auto product_low{bignum_multiply_low_fixed(
      quotient, context.modulus, width + 1, width, width + 1)};
  BasicBignum<Capacity> remainder;
  bignum_subtract_fixed(value_low, product_low, width + 1, remainder);
  remainder =
      bignum_conditional_subtract(remainder, context.modulus, width + 1);
  remainder =
      bignum_conditional_subtract(remainder, context.modulus, width + 1);
  remainder =
      bignum_conditional_subtract(remainder, context.modulus, width + 1);
  remainder.size = width;
  return remainder;
}

template <std::size_t Capacity>
inline auto
field_mod_multiply_ct(const BasicBignum<Capacity> &left,
                      const BasicBignum<Capacity> &right,
                      const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  return barrett_reduce(
      bignum_multiply_fixed(left, right, context.words, context.words),
      context);
}

template <std::size_t Capacity>
inline auto
field_square_ct(const BasicBignum<Capacity> &value,
                const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  return barrett_reduce(bignum_square_fixed(value, context.words), context);
}

template <std::size_t Capacity>
inline auto field_add_ct(const BasicBignum<Capacity> &left,
                         const BasicBignum<Capacity> &right,
                         const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  const auto width{context.words};
  const auto *left_data{left.words.data()};
  const auto *right_data{right.words.data()};
  BasicBignum<Capacity> sum;
  auto *sum_data{sum.words.data()};
  std::uint64_t carry{0};
  for (std::size_t index = 0; index < width; ++index) {
    const auto total{static_cast<BignumDoubleWord>(left_data[index]) +
                     right_data[index] + carry};
    sum_data[index] = static_cast<std::uint64_t>(total);
    carry = static_cast<std::uint64_t>(total >> 64U);
  }

  sum_data[width] = carry;
  sum.size = width + 1;
  auto reduced{bignum_conditional_subtract(sum, context.modulus, width + 1)};
  reduced.size = width;
  return reduced;
}

template <std::size_t Capacity>
inline auto
field_subtract_ct(const BasicBignum<Capacity> &left,
                  const BasicBignum<Capacity> &right,
                  const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  const auto width{context.words};
  BasicBignum<Capacity> difference;
  const auto borrow{bignum_subtract_fixed(left, right, width, difference)};
  const auto *difference_data{difference.words.data()};
  const auto *modulus_data{context.modulus.words.data()};
  BasicBignum<Capacity> wrapped;
  auto *wrapped_data{wrapped.words.data()};
  std::uint64_t carry{0};
  for (std::size_t index = 0; index < width; ++index) {
    const auto total{static_cast<BignumDoubleWord>(difference_data[index]) +
                     modulus_data[index] + carry};
    wrapped_data[index] = static_cast<std::uint64_t>(total);
    carry = static_cast<std::uint64_t>(total >> 64U);
  }

  wrapped.size = width;
  auto result{
      bignum_conditional_select(borrow != 0, wrapped, difference, width)};
  result.size = width;
  return result;
}

// Fermat inverse over the field in constant time. The exponent is the public
// modulus minus two, so its bit pattern reveals nothing secret, and the field
// multiplications underneath do not depend on the value being inverted. The
// modulus must be prime
template <std::size_t Capacity>
inline auto
field_inverse_ct(const BasicBignum<Capacity> &value,
                 const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  auto exponent{context.modulus};
  bignum_subtract_in_place(exponent, bignum_from_u64<Capacity>(2));
  BasicBignum<Capacity> result;
  result.words[0] = 1;
  result.size = context.words;
  const auto base{barrett_reduce(value, context)};
  const auto exponent_bits{bignum_bit_length(exponent)};
  for (std::size_t index = exponent_bits; index > 0; --index) {
    result = field_square_ct(result, context);
    if (bignum_get_bit(exponent, index - 1)) {
      result = field_mod_multiply_ct(result, base, context);
    }
  }

  return result;
}

// Modular exponentiation for a secret exponent (the RSA private key), in
// constant time. The exponent is secret, so it is consumed in fixed four-bit
// windows over a count fixed by the public modulus, and every window multiplies
// by a power of the base taken from a precomputed table through a masked scan
// over all of its entries rather than an index. The modulus need not be prime
template <std::size_t Capacity>
inline auto
bignum_mod_exp_ct(const BasicBignum<Capacity> &base,
                  const BasicBignum<Capacity> &exponent,
                  const BasicBarrettContext<Capacity> &context) noexcept
    -> BasicBignum<Capacity> {
  const auto width{context.words};
  std::array<BasicBignum<Capacity>, 16> powers{};
  powers[0].words[0] = 1;
  powers[0].size = width;
  powers[1] = barrett_reduce(base, context);
  for (std::size_t index = 2; index < powers.size(); ++index) {
    powers[index] =
        field_mod_multiply_ct(powers[index - 1], powers[1], context);
  }

  const auto windows{(bignum_bit_length(context.modulus) + 3) / 4};
  auto result{powers[0]};
  for (std::size_t window = windows; window > 0; --window) {
    for (std::size_t step = 0; step < 4; ++step) {
      result = field_square_ct(result, context);
    }

    std::size_t digit{0};
    for (std::size_t bit = 0; bit < 4; ++bit) {
      digit |= static_cast<std::size_t>(
                   bignum_get_bit_fixed(exponent, ((window - 1) * 4) + bit))
               << bit;
    }

    BasicBignum<Capacity> selected;
    for (std::size_t index = 0; index < powers.size(); ++index) {
      selected = bignum_conditional_select(digit == index, powers[index],
                                           selected, width);
    }

    result = field_mod_multiply_ct(result, selected, context);
  }

  return result;
}

template <std::size_t Capacity>
inline auto bignum_to_bytes(const BasicBignum<Capacity> &value,
                            const std::size_t length) -> std::string {
  std::string result(length, '\x00');
  for (std::size_t index = 0; index < length; ++index) {
    const auto word_index{index / 8};
    if (word_index >= value.size) {
      break;
    }

    const auto byte{static_cast<std::uint8_t>(
        (value.words[word_index] >> (8 * (index % 8))) & 0xffU)};
    result[length - 1 - index] = static_cast<char>(byte);
  }

  return result;
}

} // namespace sourcemeta::core

#endif
