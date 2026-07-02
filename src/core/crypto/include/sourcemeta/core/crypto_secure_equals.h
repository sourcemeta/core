#ifndef SOURCEMETA_CORE_CRYPTO_SECURE_EQUALS_H_
#define SOURCEMETA_CORE_CRYPTO_SECURE_EQUALS_H_

#ifndef SOURCEMETA_CORE_CRYPTO_EXPORT
#include <sourcemeta/core/crypto_export.h>
#endif

#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup crypto
/// Compare two byte sequences for equality in constant time when their lengths
/// match, returning false immediately when the lengths differ. The comparison
/// does not short-circuit on the first differing byte, so it does not leak the
/// position of a mismatch through timing. The length of the inputs is not
/// treated as secret. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::secure_equals("expected", "expected"));
/// assert(!sourcemeta::core::secure_equals("expected", "actual"));
/// ```
auto SOURCEMETA_CORE_CRYPTO_EXPORT secure_equals(
    const std::string_view left, const std::string_view right) noexcept -> bool;

} // namespace sourcemeta::core

#endif
