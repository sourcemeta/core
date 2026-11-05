#ifndef SOURCEMETA_CORE_OPENAPI_DEPENDENCIES_H_
#define SOURCEMETA_CORE_OPENAPI_DEPENDENCIES_H_

#ifndef SOURCEMETA_CORE_OPENAPI_EXPORT
#include <sourcemeta/core/openapi_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/openapi_types.h>

#include <cstdint>     // std::uint64_t
#include <functional>  // std::function
#include <limits>      // std::numeric_limits
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup openapi
///
/// What one reference of a description reaches. Every member of this points
/// into something that analysis owns for no longer than the call that reports
/// it, so a callback that keeps any of them past its own return has to copy
/// them out
struct OpenAPIDependency {
  /// The identifier that the document making the reference goes by, which is
  /// the URI it was asked for, or, below what a Schema Object names, the
  /// identifier that document declares where it declares one. This is empty
  /// for the description that analysis started from unless the caller named
  /// where it came from
  JSON::StringView origin;
  /// Where in that document the reference sits
  const Pointer &pointer;
  /// Where it points, resolved and canonicalised, without the fragment it
  /// carried
  JSON::StringView destination;
  /// The document that the reference leads to
  const JSON &document;
  /// What the position that spells it expects to find at the far end, which
  /// is fixed by where the reference sits rather than by anything the target
  /// says about itself. What a Schema Object or a discriminator mapping names
  /// is a schema, as nothing else may be named there
  OpenAPIObjectKind expected;
};

/// @ingroup openapi
/// A callback to report every dependency that analysis finds
using OpenAPIDependencyCallback =
    std::function<void(const OpenAPIDependency &)>;

/// @ingroup openapi
///
/// Everything dependency analysis takes beyond the description and how to read
/// it
struct OpenAPIDependencyOptions {
  /// The URI the document was retrieved from, which every relative reference
  /// it makes resolves against
  std::string_view default_base{};
  /// The maximum number of locations that analysis may register. Every frame
  /// it constructs spends from this one allowance, throwing
  /// sourcemeta::core::OpenAPIFrameLimitError once it runs out
  std::uint64_t max_locations{std::numeric_limits<std::uint64_t>::max()};
};

/// @ingroup openapi
///
/// This function reports every other document that an OpenAPI Description
/// reaches, transitively, each of them once, whether the reference that
/// reaches it is one this specification gives meaning to or one that JSON
/// Schema does. A document that the description already carries within itself
/// is not one of them. The dialect that a description or one of its Schema
/// Objects names counts as one, as reading them takes it, unless it is a
/// dialect of JSON Schema itself. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <sourcemeta/core/openapi.h>
///
/// const auto document = sourcemeta::core::parse_json(R"JSON({
///   "openapi": "3.1.1",
///   "info": { "title": "Example", "version": "1.0.0" },
///   "paths": {
///     "/thing": {
///       "$ref": "https://www.example.com/shared.json#/paths/~1thing"
///     }
///   }
/// })JSON");
///
/// sourcemeta::core::openapi_dependencies(document,
///   sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver,
///   test_resolver,
///   [](const auto &dependency) {
///     // Do something with the information
///   });
/// ```
///
/// Returns how many frame locations the analysis registered, which a caller
/// that spends from the same allowance elsewhere takes off its own count.
///
/// What a Schema Object names has to be a document that is a schema and
/// nothing else. A description holding the schema it names is refused with
/// sourcemeta::core::OpenAPIReferenceError rather than followed, as reading
/// part of a description without the rest of it would miss whatever bears on
/// the reference.
///
/// Analysis throws where it cannot go on. A reference that nothing answers for
/// ends it, though only once every document reached has been read, since one
/// of them may still answer to the name an earlier reference made. A document
/// that cannot be read as a description at all ends it too. Whether a
/// destination holds the kind of Object that the position naming it expects is
/// not checked, and neither is whether the documents of a span agree on a
/// revision, as reporting leaves both judgements to the caller.
SOURCEMETA_CORE_OPENAPI_EXPORT
auto openapi_dependencies(const JSON &document, const SchemaWalker &walker,
                          const SchemaResolver &schema_resolver,
                          const OpenAPIResolver &resolver,
                          const OpenAPIDependencyCallback &callback,
                          const OpenAPIDependencyOptions &options = {})
    -> std::uint64_t;

} // namespace sourcemeta::core

#endif
