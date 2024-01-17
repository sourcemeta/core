#ifndef SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_REFERENCE_H_
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_REFERENCE_H_

#if defined(__EMSCRIPTEN__) || defined(__Unikraft__)
#define SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
#else
#include "jsonschema_export.h"
#endif

#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema_reference_frame.h>
#include <sourcemeta/jsontoolkit/jsonschema_reference_type.h>
#include <sourcemeta/jsontoolkit/jsonschema_resolver.h>
#include <sourcemeta/jsontoolkit/jsonschema_walker.h>

#include <future>   // std::future
#include <map>      // std::map
#include <optional> // std::optional
#include <string>   // std::string
#include <tuple>    // std::tuple
#include <utility>  // std::pair

namespace sourcemeta::jsontoolkit {

// TODO: Encapsulate this behind a class to make sure we
// can keep a stable API even if changing the underlying map.
/// @ingroup jsonschema
/// A JSON Schema reference map is a mapping of a JSON Pointer
/// of a subschema to a destination static reference URI.
/// For convenient, the value consists of the URI on its entirety,
/// but also broken down by its potential fragment component.
/// The reference type is part of the key as it is possible to
/// have a static and a dynamic reference to the same location
/// on the same schema object.
using ReferenceMap =
    std::map<std::pair<Pointer, ReferenceType>,
             std::tuple<std::string, std::optional<std::string>,
                        std::optional<std::string>>>;

// TODO: Support $recursiveAnchor
// TODO: Support $recursiveRef

/// @ingroup jsonschema
///
/// This function computes the static reference frame of a schema. Each entry
/// consists in the URI of the resulting schema, the JSON Pointer within that
/// resulting schema, and the dialect URI of the resulting subschema. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/jsontoolkit/json.h>
/// #include <sourcemeta/jsontoolkit/jsonschema.h>
/// #include <cassert>
///
/// const sourcemeta::jsontoolkit::JSON document =
///     sourcemeta::jsontoolkit::parse(R"JSON({
///   "$id": "https://www.example.com/schema",
///   "$schema": "https://json-schema.org/draft/2020-12/schema",
///   "items": { "$id": "foo", "type": "string" },
///   "properties": {
///     "foo": { "$anchor": "test", "type": "number" },
///     "bar": { "$ref": "#/properties/foo" }
///   }
/// })JSON");
///
/// sourcemeta::jsontoolkit::ReferenceFrame frame;
/// sourcemeta::jsontoolkit::ReferenceMap references;
/// sourcemeta::jsontoolkit::frame(document, frame,
/// references,
///                                sourcemeta::jsontoolkit::default_schema_walker,
///                                sourcemeta::jsontoolkit::official_resolver)
///     .wait();
///
/// // IDs
/// assert(frame.defines("https://www.example.com/schema"));
/// assert(frame.defines("https://www.example.com/foo"));
///
/// // Anchors
/// assert(frame.defines("https://www.example.com/schema#test"));
///
/// // Root Pointers
/// assert(frame.defines("https://www.example.com/schema#/$id"));
/// assert(frame.defines("https://www.example.com/schema#/$schema"));
/// assert(frame.defines("https://www.example.com/schema#/items"));
/// assert(frame.defines("https://www.example.com/schema#/items/$id"));
/// assert(frame.defines("https://www.example.com/schema#/items/type"));
/// assert(frame.defines("https://www.example.com/schema#/properties"));
/// assert(frame.defines("https://www.example.com/schema#/properties/foo"));
/// assert(frame.defines("https://www.example.com/schema#/properties/foo/$anchor"));
/// assert(frame.defines("https://www.example.com/schema#/properties/foo/type"));
/// assert(frame.defines("https://www.example.com/schema#/properties/bar"));
/// assert(frame.defines("https://www.example.com/schema#/properties/bar/$ref"));
///
/// // Subpointers
/// assert(frame.defines("https://www.example.com/foo#/$id"));
/// assert(frame.defines("https://www.example.com/foo#/type"));
///
/// // References
/// assert(references.contains({{ "properties", "bar", "$ref" },
///   sourcemeta::jsontoolkit::ReferenceType::Static}));
/// assert(references.at({{ "properties", "bar", "$ref" },
///   sourcemeta::jsontoolkit::ReferenceType::Static}) ==
///     "https://www.example.com/schema#/properties/foo");
/// ```
SOURCEMETA_JSONTOOLKIT_JSONSCHEMA_EXPORT
auto frame(const JSON &schema, ReferenceFrame &frame, ReferenceMap &references,
           const SchemaWalker &walker, const SchemaResolver &resolver,
           const std::optional<std::string> &default_dialect = std::nullopt,
           const std::optional<std::string> &default_id = std::nullopt)
    -> std::future<void>;

} // namespace sourcemeta::jsontoolkit

#endif
