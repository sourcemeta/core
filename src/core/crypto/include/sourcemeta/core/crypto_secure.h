#ifndef SOURCEMETA_CORE_CRYPTO_SECURE_H_
#define SOURCEMETA_CORE_CRYPTO_SECURE_H_

#ifndef SOURCEMETA_CORE_CRYPTO_EXPORT
#include <sourcemeta/core/crypto_export.h>
#endif

#include <cstddef> // std::size_t
#include <string>  // std::string

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

} // namespace sourcemeta::core

#endif
