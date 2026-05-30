#ifndef SOURCEMETA_CORE_JSON_HASH_H_
#define SOURCEMETA_CORE_JSON_HASH_H_

#include <sourcemeta/core/numeric.h>

#include <cassert>    // assert
#include <cstring>    // std::memcpy
#include <functional> // std::reference_wrapper

// -----------------------------------------------------------------------------
// SIMD backend auto-detection for `PropertyHashJSON::perfect`.
//
// We use SIMD *only* for the size >= 16 case, where the caller has guaranteed
// at least 16 contiguous readable bytes at `data`. For size < 16 we fall back
// to scalar `std::memcpy`, which the compiler already inlines to optimal
// fixed-size moves for these small sizes and is, crucially, free of any
// out-of-bounds read.
// -----------------------------------------------------------------------------
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
// NOLINTBEGIN(clang-diagnostic-error)
// Some older clang-tidy versions choke when parsing newer Xcode/LLVM
// `arm_neon.h` (e.g. unrecognized `__builtin_neon_*` symbols for bf16 and
// complex-vector intrinsics). The header is correct, the diagnostic is a
// clang-tidy bug; suppress it locally.
#include <arm_neon.h>
// NOLINTEND(clang-diagnostic-error)
#define SOURCEMETA_HASH_SIMD_NEON 1
#elif defined(__SSE2__) || defined(_M_X64) || defined(_M_AMD64)
#include <emmintrin.h>
#define SOURCEMETA_HASH_SIMD_SSE2 1
#endif

namespace sourcemeta::core {

/// @ingroup json
template <typename T> struct HashJSON {
  using hash_type = std::uint64_t;

  inline auto operator()(const T &value) const noexcept -> hash_type {
    if constexpr (requires { value.get().fast_hash(); }) {
      return value.get().fast_hash();
    } else {
      return value.fast_hash();
    }
  }

  [[nodiscard]]
  inline auto is_perfect(const hash_type) const noexcept -> bool {
    return false;
  }
};

/// @ingroup json
template <typename T> struct PropertyHashJSON {
  struct hash_type {
    using type = sourcemeta::core::uint128_t;
    type a{0};
    type b{0};

    auto operator==(const hash_type &) const noexcept -> bool = default;
  };

  [[nodiscard]]
  inline auto perfect(const char *data, const std::size_t size) const noexcept
      -> hash_type {
    hash_type result;
    assert(size > 0);
    assert(size <= 31);

#if defined(SOURCEMETA_HASH_SIMD_NEON)
    if (size >= 16) {
      // Safe: caller guarantees at least 16 readable bytes at `data`.
      auto *const dst = reinterpret_cast<std::uint8_t *>(&result) + 1;
      const auto *const src = reinterpret_cast<const std::uint8_t *>(data);
      const uint8x16_t v0 = vld1q_u8(src);
      vst1q_u8(dst, v0);
      if (size > 16) {
        // Overlapping tail load aligned to the end of the string. The second
        // store partially overwrites the first and produces the same byte
        // coverage as `memcpy(dst, src, size)`, with no mask required.
        const std::size_t tail_off = size - 16;
        const uint8x16_t v1 = vld1q_u8(src + tail_off);
        vst1q_u8(dst + tail_off, v1);
      }
      return result;
    }
#elif defined(SOURCEMETA_HASH_SIMD_SSE2)
    if (size >= 16) {
      auto *const dst = reinterpret_cast<std::uint8_t *>(&result) + 1;
      const auto *const src = reinterpret_cast<const std::uint8_t *>(data);
      const __m128i v0 =
          _mm_loadu_si128(reinterpret_cast<const __m128i *>(src));
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dst), v0);
      if (size > 16) {
        const std::size_t tail_off = size - 16;
        const __m128i v1 =
            _mm_loadu_si128(reinterpret_cast<const __m128i *>(src + tail_off));
        _mm_storeu_si128(reinterpret_cast<__m128i *>(dst + tail_off), v1);
      }
      return result;
    }
#endif

    // Scalar path: also handles non-SIMD builds and the size < 16 case where
    // a 16-byte SIMD load would over-read past the live string buffer.
    std::memcpy(reinterpret_cast<char *>(&result) + 1, data, size);
    return result;
  }

  // GCC does not optimise well across implicit type conversions such as
  // std::string to std::string_view, so we provide separate overloads with
  // duplicated logic instead of unifying on a single parameter type

  inline auto operator()(const T &value) const noexcept -> hash_type {
    const auto size{value.size()};
    if (size == 0) {
      return {};
    }
    if (size <= 31) {
      return this->perfect(value.data(), size);
    }
    auto hash = this->perfect(value.data(), 31);
    hash.a |= 1 + (size + static_cast<typename hash_type::type>(value.front()) +
                   static_cast<typename hash_type::type>(value.back())) %
                      // Make sure the property hash can never exceed 8 bits
                      255;
    return hash;
  }

  inline auto operator()(const char *data,
                         const std::size_t size) const noexcept -> hash_type {
    if (size == 0) {
      return {};
    }
    if (size <= 31) {
      return this->perfect(data, size);
    }
    auto hash = this->perfect(data, 31);
    hash.a |= 1 + (size + static_cast<typename hash_type::type>(data[0]) +
                   static_cast<typename hash_type::type>(data[size - 1])) %
                      // Make sure the property hash can never exceed 8 bits
                      255;
    return hash;
  }

  [[nodiscard]]
  inline auto is_perfect(const hash_type &hash) const noexcept -> bool {
    // If there is anything written past the first byte,
    // then it is a perfect hash
    return (hash.a & 255) == 0;
  }
};

/// @ingroup json
/// Until C++26, `std::reference_wrapper` does not overload `operator==`,
/// so we need custom comparisons for use in i.e. `unordered_set`
/// See
/// https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper/operator_cmp.html
template <typename T> struct EqualJSON {
  inline auto operator()(const T &left, const T &right) const -> bool {
    if constexpr (requires { left.get() == right.get(); }) {
      return left.get() == right.get();
    } else {
      return left == right;
    }
  }
};

} // namespace sourcemeta::core

#endif
