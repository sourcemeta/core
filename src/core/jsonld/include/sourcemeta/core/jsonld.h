#ifndef SOURCEMETA_CORE_JSONLD_H_
#define SOURCEMETA_CORE_JSONLD_H_

#ifndef SOURCEMETA_CORE_JSONLD_EXPORT
#include <sourcemeta/core/jsonld_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld_error.h>

#include <cstdint>    // std::uint8_t
#include <functional> // std::function
#include <optional>   // std::optional

/// @defgroup jsonld JSON-LD
/// @brief A JSON-LD 1.1 processor, with support for the JSON-LD 1.0 processing
/// mode.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/jsonld.h>
/// ```

namespace sourcemeta::core {

/// @ingroup jsonld
/// A resolver callback for loading remote JSON-LD contexts referenced during
/// expansion. Given an absolute IRI, it returns the referenced document, or no
/// value if it cannot be resolved.
using JSONLDResolver = std::function<std::optional<JSON>(JSON::StringView)>;

/// @ingroup jsonld
/// The JSON-LD processing mode
enum class JSONLDVersion : std::uint8_t { V1_0, V1_1 };

/// @ingroup jsonld
///
/// Expand a JSON-LD document into its expanded form, resolving relative
/// references against the given base IRI and loading any remote context through
/// the given resolver. The result is always a JSON array. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/jsonld.h>
/// #include <iostream>
///
/// const auto document{sourcemeta::core::parse_json(R"({
///   "@context": { "name": "https://schema.org/name" },
///   "name": "Sourcemeta"
/// })")};
/// const auto expanded{sourcemeta::core::jsonld_expand(document)};
/// sourcemeta::core::prettify(expanded, std::cout);
/// std::cout << std::endl;
/// ```
SOURCEMETA_CORE_JSONLD_EXPORT
auto jsonld_expand(const JSON &input, const JSON::StringView base_iri = "",
                   const JSONLDResolver &resolver = {},
                   const JSONLDVersion version = JSONLDVersion::V1_1) -> JSON;

/// @ingroup jsonld
///
/// Expand a JSON-LD document, applying the given expansion context before the
/// document's own context, as if it had been prepended to the input. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/jsonld.h>
/// #include <iostream>
///
/// const auto document{sourcemeta::core::parse_json(
///     R"({ "name": "Sourcemeta" })")};
/// const auto context{sourcemeta::core::parse_json(
///     R"({ "name": "https://schema.org/name" })")};
/// const auto expanded{sourcemeta::core::jsonld_expand(document, context)};
/// sourcemeta::core::prettify(expanded, std::cout);
/// std::cout << std::endl;
/// ```
SOURCEMETA_CORE_JSONLD_EXPORT
auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView base_iri = "",
                   const JSONLDResolver &resolver = {},
                   const JSONLDVersion version = JSONLDVersion::V1_1) -> JSON;

} // namespace sourcemeta::core

#endif
