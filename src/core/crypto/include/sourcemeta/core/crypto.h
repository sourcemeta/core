#ifndef SOURCEMETA_CORE_CRYPTO_H_
#define SOURCEMETA_CORE_CRYPTO_H_

#ifndef SOURCEMETA_CORE_CRYPTO_EXPORT
#include <sourcemeta/core/crypto_export.h>
#endif

/// @defgroup crypto Crypto
/// @brief Cryptographic hash functions, UUID generation, and Base64 codecs.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// ```

#include <sourcemeta/core/crypto_base64.h>
#include <sourcemeta/core/crypto_crc32.h>
#include <sourcemeta/core/crypto_fnv128.h>
#include <sourcemeta/core/crypto_hmac_sha256.h>
#include <sourcemeta/core/crypto_sha1.h>
#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/crypto_sha384.h>
#include <sourcemeta/core/crypto_sha512.h>
#include <sourcemeta/core/crypto_uuid.h>
#include <sourcemeta/core/crypto_verify.h>

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
