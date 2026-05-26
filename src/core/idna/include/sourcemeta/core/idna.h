#ifndef SOURCEMETA_CORE_IDNA_H_
#define SOURCEMETA_CORE_IDNA_H_

#ifndef SOURCEMETA_CORE_IDNA_EXPORT
#include <sourcemeta/core/idna_export.h>
#endif

#include <cstddef>     // std::size_t
#include <string_view> // std::u32string_view

/// @defgroup idna IDNA
/// @brief Internationalized Domain Names in Applications (IDNA2008) utilities.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/idna.h>
/// ```

namespace sourcemeta::core {

/// @ingroup idna
/// Return whether the codepoint at `position` within `label` does not
/// violate any RFC 5892 Appendix A.3-A.9 contextual rule. Returns true
/// vacuously when the codepoint has no such rule. See
/// https://www.rfc-editor.org/rfc/rfc5892#appendix-A for the rules.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/idna.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::idna_passes_contexto(U"l\u00B7l", 1));
/// assert(!sourcemeta::core::idna_passes_contexto(U"a\u00B7b", 1));
/// ```
SOURCEMETA_CORE_IDNA_EXPORT
auto idna_passes_contexto(const std::u32string_view label,
                          const std::size_t position) noexcept -> bool;

/// @ingroup idna
/// Return whether the codepoint at `position` within `label` does not
/// violate any RFC 5892 Appendix A.1 / A.2 contextual rule. Returns true
/// vacuously when the codepoint has no such rule. See
/// https://www.rfc-editor.org/rfc/rfc5892#appendix-A for the rules.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/idna.h>
/// #include <cassert>
///
/// // Devanagari KA + VIRAMA + ZWJ
/// assert(sourcemeta::core::idna_passes_contextj(U"\u0915\u094D\u200D", 2));
/// // Arabic BEH + ZWNJ + Arabic ALEF
/// assert(sourcemeta::core::idna_passes_contextj(U"\u0628\u200C\u0627", 1));
/// // ZWJ not preceded by Virama
/// assert(!sourcemeta::core::idna_passes_contextj(U"a\u200D", 1));
/// ```
SOURCEMETA_CORE_IDNA_EXPORT
auto idna_passes_contextj(const std::u32string_view label,
                          const std::size_t position) noexcept -> bool;

} // namespace sourcemeta::core

#endif
