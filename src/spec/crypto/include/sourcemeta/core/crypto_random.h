#ifndef SOURCEMETA_CORE_CRYPTO_RANDOM_H_
#define SOURCEMETA_CORE_CRYPTO_RANDOM_H_

#ifndef SOURCEMETA_CORE_CRYPTO_EXPORT
#include <sourcemeta/core/crypto_export.h>
#endif

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint8_t
#include <functional> // std::function
#include <span>       // std::span
#include <string>     // std::string

namespace sourcemeta::core {

/// @ingroup crypto
///
/// Every function here that needs random bytes is handed the generator to draw
/// them from, rather than reaching for one of its own. Key generation and
/// RSA-OAEP encryption are the exception on a build with a system provider, as
/// the provider draws those bytes itself and cannot be pointed at another
/// source, which is what the fallback naming on those parameters marks. A
/// generator fills the whole of the buffer it is given, and reports a refusal
/// by throwing, as there is no shorter answer it could give.
///
/// For convenience, we provide the following default generator:
///
/// - sourcemeta::core::system_random_generator
///
/// Supplying your own lets a caller draw from somewhere else, and lets a test
/// watch what happens where the drawing refuses.
using RandomGenerator = std::function<void(std::span<std::uint8_t>)>;

/// @ingroup crypto
/// Draw random bytes from the operating system's cryptographic provider, which
/// is the generator to pass where a caller has no reason to supply its own.
/// Every build draws from the kernel provider, which fails closed by throwing
/// rather than ever handing back bytes of lower quality. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <cassert>
///
/// const auto bytes{sourcemeta::core::random_bytes(
///     32, sourcemeta::core::system_random_generator)};
/// assert(bytes.size() == 32);
/// ```
auto SOURCEMETA_CORE_CRYPTO_EXPORT
system_random_generator(std::span<std::uint8_t> buffer) -> void;

/// @ingroup crypto
/// Fill a buffer with random bytes from the given generator. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <array>
/// #include <cstdint>
///
/// std::array<std::uint8_t, 16> buffer{};
/// sourcemeta::core::random_bytes(buffer,
///                                sourcemeta::core::system_random_generator);
/// ```
auto SOURCEMETA_CORE_CRYPTO_EXPORT random_bytes(
    std::span<std::uint8_t> buffer, const RandomGenerator &generator) -> void;

/// @ingroup crypto
/// Return the given number of random bytes as a string, drawn from the given
/// generator. For example:
///
/// ```cpp
/// #include <sourcemeta/core/crypto.h>
/// #include <cassert>
///
/// const auto bytes{sourcemeta::core::random_bytes(
///     32, sourcemeta::core::system_random_generator)};
/// assert(bytes.size() == 32);
/// ```
auto SOURCEMETA_CORE_CRYPTO_EXPORT random_bytes(
    const std::size_t length, const RandomGenerator &generator) -> std::string;

} // namespace sourcemeta::core

#endif
