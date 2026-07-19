#ifndef SOURCEMETA_CORE_CRYPTO_SECURE_H_
#define SOURCEMETA_CORE_CRYPTO_SECURE_H_

#ifndef SOURCEMETA_CORE_CRYPTO_EXPORT
#include <sourcemeta/core/crypto_export.h>
#endif

#include <cstddef> // std::size_t
#include <new>     // operator new, operator delete
#include <string>  // std::string, std::char_traits

namespace sourcemeta::core {

/// @ingroup crypto
/// Overwrite a buffer that held secret material with zeroes, so it does not
/// linger in memory after it is no longer needed. The write goes through a
/// volatile access, so the compiler does not elide it as a dead store. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <array>
/// #include <cstdint>
///
/// std::array<std::uint8_t, 4> buffer{{1, 2, 3, 4}};
/// sourcemeta::core::secure_zero(buffer.data(), buffer.size());
/// ```
inline auto secure_zero(void *const data, const std::size_t size) noexcept
    -> void {
  if (data == nullptr) {
    return;
  }

  auto *pointer{static_cast<volatile unsigned char *>(data)};
  for (std::size_t index{0}; index < size; index += 1) {
    pointer[index] = 0;
  }
}

/// @ingroup crypto
/// Overwrite the storage a string owns with zeroes, so a secret it held does
/// not linger in memory. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <cassert>
/// #include <string>
///
/// std::string secret{"hunter2"};
/// sourcemeta::core::secure_zero(secret);
/// assert(secret == std::string(7, '\x00'));
/// ```
inline auto secure_zero(std::string &value) noexcept -> void {
  secure_zero(value.data(), value.size());
}

/// @ingroup crypto
/// Overwrite the referenced string when leaving the current scope, so secret
/// material a local holds is wiped across every return path without threading
/// a manual call through each one. It clears the storage the string owns at
/// scope exit, so a reassignment or a growth that reallocates before then can
/// still leave an earlier copy in freed memory, a residual that a wiping
/// allocator would be needed to close. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <cassert>
/// #include <string>
///
/// std::string secret{"hunter2"};
/// {
///   const sourcemeta::core::SecureStringScope scope{secret};
/// }
/// assert(secret == std::string(7, '\x00'));
/// ```
struct SecureStringScope {
  /// Capture the string to wipe when leaving the current scope.
  explicit SecureStringScope(std::string &value) noexcept : target{value} {}
  SecureStringScope(const SecureStringScope &) = delete;
  auto operator=(const SecureStringScope &) -> SecureStringScope & = delete;
  SecureStringScope(SecureStringScope &&) = delete;
  auto operator=(SecureStringScope &&) -> SecureStringScope & = delete;
  ~SecureStringScope() { secure_zero(this->target); }
  /// The captured string, whose storage is wiped at scope exit.
  std::string &target;
};

/// @ingroup crypto
/// A standard allocator that wipes every block it owns before releasing it, so
/// a secret the storage held does not survive in freed memory. The deallocation
/// covers the whole block, so a reallocation, a reassignment, or the
/// destruction of the owner all clear the earlier bytes, closing the residue
/// that the scope-based cleanup above cannot reach on its own. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <vector>
///
/// std::vector<char, sourcemeta::core::SecureAllocator<char>> secret;
/// secret.push_back('x');
/// ```
template <typename T> struct SecureAllocator {
  /// The type of object this allocator hands out storage for.
  using value_type = T;

  /// Construct an allocator, which holds no state of its own.
  SecureAllocator() noexcept = default;

  /// Construct from an allocator for another type, as the containers require.
  template <typename Other>
  constexpr SecureAllocator(const SecureAllocator<Other> &) noexcept {}

  /// Allocate storage for the given number of objects.
  [[nodiscard]] auto allocate(const std::size_t count) -> T * {
    return static_cast<T *>(::operator new(count * sizeof(T)));
  }

  /// Wipe and release the storage of the given number of objects.
  auto deallocate(T *const pointer, const std::size_t count) noexcept -> void {
    secure_zero(pointer, count * sizeof(T));
    ::operator delete(pointer);
  }

  /// Every instance is interchangeable, since the allocator holds no state.
  template <typename Other>
  auto operator==(const SecureAllocator<Other> &) const noexcept -> bool {
    return true;
  }

  /// Every instance is interchangeable, since the allocator holds no state.
  template <typename Other>
  auto operator!=(const SecureAllocator<Other> &) const noexcept -> bool {
    return false;
  }
};

/// @ingroup crypto
/// A string that wipes its heap storage whenever it is released, so a secret it
/// held does not linger after a growth, a reassignment, or its destruction. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
///
/// sourcemeta::core::SecureString secret{"hunter2"};
/// secret.append("!");
/// ```
using SecureString =
    std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;

} // namespace sourcemeta::core

#endif
