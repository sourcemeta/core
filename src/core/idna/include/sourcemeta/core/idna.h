#ifndef SOURCEMETA_CORE_IDNA_H_
#define SOURCEMETA_CORE_IDNA_H_

#ifndef SOURCEMETA_CORE_IDNA_EXPORT
#include <sourcemeta/core/idna_export.h>
#endif

#include <sourcemeta/core/idna_ucd.h>

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
/// Return the RFC 5892 derived property of a Unicode codepoint. See
/// https://www.rfc-editor.org/rfc/rfc5892 for the property's definition.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/idna.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::idna_property(U'a') ==
///        sourcemeta::core::IDNAProperty::PValid);
/// assert(sourcemeta::core::idna_property(U'\u200D') ==
///        sourcemeta::core::IDNAProperty::ContextJ);
/// ```
SOURCEMETA_CORE_IDNA_EXPORT
auto idna_property(const char32_t codepoint) noexcept -> IDNAProperty;

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

/// @ingroup idna
/// Return whether the given label satisfies the RFC 5893 Bidi rule. The
/// rule applies to every label of a Bidi domain name (a domain name that
/// contains at least one right-to-left codepoint). The caller is
/// responsible for invoking this only when the domain is a Bidi domain.
/// See https://www.rfc-editor.org/rfc/rfc5893#section-2 for the rule.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/idna.h>
/// #include <cassert>
///
/// // Hebrew letter Alef
/// assert(sourcemeta::core::idna_passes_bidi_rule(U"\u05D0"));
/// // Label starting with a digit (Bidi rule condition 1 violated)
/// assert(!sourcemeta::core::idna_passes_bidi_rule(U"0abc"));
/// ```
SOURCEMETA_CORE_IDNA_EXPORT
auto idna_passes_bidi_rule(const std::u32string_view label) noexcept -> bool;

} // namespace sourcemeta::core

#endif
