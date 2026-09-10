#ifndef SOURCEMETA_CORE_OPENAPI_HELPERS_H_
#define SOURCEMETA_CORE_OPENAPI_HELPERS_H_

#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/uri.h>

#include <algorithm>        // std::ranges::find
#include <array>            // std::array
#include <cstddef>          // std::size_t
#include <cstdint>          // std::uint8_t
#include <deque>            // std::deque
#include <initializer_list> // std::initializer_list
#include <map>              // std::map
#include <optional>         // std::optional
#include <set>              // std::set
#include <string_view>      // std::string_view
#include <utility>          // std::pair, std::unreachable
#include <vector>           // std::vector

namespace sourcemeta::core {

using namespace std::string_view_literals;

// OpenAPI Specification 3.1.1, Section 4.9: "The field name MUST begin with
// `x-`, for example, `x-internal-id`"
constexpr auto OPENAPI_EXTENSION_PREFIX{"x-"sv};

constexpr auto OPENAPI_HASH_DESCRIPTION{JSON::Object::hash("description"sv)};
constexpr auto OPENAPI_HASH_SUMMARY{JSON::Object::hash("summary"sv)};
constexpr auto OPENAPI_HASH_URL{JSON::Object::hash("url"sv)};
constexpr auto OPENAPI_HASH_NAME{JSON::Object::hash("name"sv)};
constexpr auto OPENAPI_HASH_IN{JSON::Object::hash("in"sv)};
constexpr auto OPENAPI_HASH_TAGS{JSON::Object::hash("tags"sv)};
constexpr auto OPENAPI_HASH_SERVERS{JSON::Object::hash("servers"sv)};

// What a reference expects to find at the far end of itself, which is fixed by
// where the reference sits rather than by anything the target says about
// itself. OpenAPI Specification 3.1.1, Section 3 lists "Detecting a document
// containing a referenceable Object at its root based on the expected type of
// the reference" among the ways to tell what a referenced document is, and
// this is that expected type
enum class OpenAPIObjectKind : std::uint8_t {
  /// A whole OpenAPI Description, which is what a document declaring a root
  /// `openapi` field is read as no matter where the reference sat
  Document,
  // The ten a reference may expect to find
  PathItem,
  Parameter,
  RequestBody,
  Response,
  Example,
  Header,
  Link,
  Callbacks,
  SecurityScheme,
  // The rest are never referenced, but every Object gets a location
  Info,
  Contact,
  License,
  Server,
  ServerVariable,
  Components,
  Paths,
  Operation,
  ExternalDocumentation,
  MediaType,
  Encoding,
  Responses,
  Tag,
  Reference,
  Schema,
  OAuthFlows,
  OAuthFlow,
  SecurityRequirement
};

inline auto openapi_kind_name(const OpenAPIObjectKind kind) noexcept
    -> JSON::StringView {
  switch (kind) {
    case OpenAPIObjectKind::Document:
      return "openapi"sv;
    case OpenAPIObjectKind::PathItem:
      return "path-item"sv;
    case OpenAPIObjectKind::Parameter:
      return "parameter"sv;
    case OpenAPIObjectKind::RequestBody:
      return "request-body"sv;
    case OpenAPIObjectKind::Response:
      return "response"sv;
    case OpenAPIObjectKind::Example:
      return "example"sv;
    case OpenAPIObjectKind::Header:
      return "header"sv;
    case OpenAPIObjectKind::Link:
      return "link"sv;
    case OpenAPIObjectKind::Callbacks:
      return "callback"sv;
    case OpenAPIObjectKind::SecurityScheme:
      return "security-scheme"sv;
    case OpenAPIObjectKind::Info:
      return "info"sv;
    case OpenAPIObjectKind::Contact:
      return "contact"sv;
    case OpenAPIObjectKind::License:
      return "license"sv;
    case OpenAPIObjectKind::Server:
      return "server"sv;
    case OpenAPIObjectKind::ServerVariable:
      return "server-variable"sv;
    case OpenAPIObjectKind::Components:
      return "components"sv;
    case OpenAPIObjectKind::Paths:
      return "paths"sv;
    case OpenAPIObjectKind::Operation:
      return "operation"sv;
    case OpenAPIObjectKind::ExternalDocumentation:
      return "external-documentation"sv;
    case OpenAPIObjectKind::MediaType:
      return "media-type"sv;
    case OpenAPIObjectKind::Encoding:
      return "encoding"sv;
    case OpenAPIObjectKind::Responses:
      return "responses"sv;
    case OpenAPIObjectKind::Tag:
      return "tag"sv;
    case OpenAPIObjectKind::Reference:
      return "reference"sv;
    case OpenAPIObjectKind::Schema:
      return "schema"sv;
    case OpenAPIObjectKind::OAuthFlows:
      return "oauth-flows"sv;
    case OpenAPIObjectKind::OAuthFlow:
      return "oauth-flow"sv;
    case OpenAPIObjectKind::SecurityRequirement:
      return "security-requirement"sv;
  }

  std::unreachable();
}

/// Where an Object that stands in for another leads. OpenAPI Specification
/// 3.1.1 has a Reference Object and a Path Item Object each declare at most
/// one `$ref`, and a Schema Object's `$ref` never reaches here, so this is a
/// field of the Object that makes it rather than a table of its own
struct OpenAPIReference {
  /// The value as the document wrote it
  JSON::String original;
  /// Where it points, resolved against the base and canonicalised. The
  /// document it names and the fragment it carries are that string either side
  /// of its `#`, so neither is repeated here
  JSON::String destination;
};

/// How an Operation Object is reached from the entry document. OpenAPI
/// Specification 3.1.1, Section 3: "only the entry document's Paths Object
/// contributes URLs to the described API", so what an operation is reached
/// through is a property of that document rather than of the one it sits in
enum class OpenAPIOperationKind : std::uint8_t { Path, Webhook, Callback };

inline auto
openapi_operation_kind_name(const OpenAPIOperationKind kind) noexcept
    -> JSON::StringView {
  switch (kind) {
    case OpenAPIOperationKind::Path:
      return "path"sv;
    case OpenAPIOperationKind::Webhook:
      return "webhook"sv;
    case OpenAPIOperationKind::Callback:
      return "callback"sv;
  }

  std::unreachable();
}

/// What a Path Item Object declares that the endpoints reaching it need. Two
/// endpoints may reach one Path Item, and following a reference reads its
/// target once, so this is kept rather than read again
struct OpenAPIPathItemRecord {
  /// The methods it declares, in the order this specification lists them, each
  /// against the location of the Operation Object it holds
  std::vector<std::pair<JSON::StringView, JSON::String>> operations;
  /// Where the Server Objects it declares sit
  std::vector<JSON::String> servers;
  /// Where the Parameter Objects it declares sit, in the order it writes them.
  /// A position may hold a Reference Object rather than a Parameter Object
  std::vector<JSON::String> parameters;
};

/// What an Operation Object declares that the endpoint reaching it needs
struct OpenAPIOperationRecord {
  /// Where the Server Objects it declares sit
  std::vector<JSON::String> servers;
  /// Where the Security Requirement Objects it declares sit. Declaring none
  /// and declaring an empty array differ, as Section 4.8.10 makes the latter
  /// the way "to remove a top-level security declaration"
  std::optional<std::vector<JSON::String>> security;
  /// Where each Callback Object it declares sits, which is a Reference Object
  /// rather than a Callback Object when the description wrote one there
  std::vector<JSON::String> callbacks;
  /// Where the Parameter Objects it declares sit, in the order it writes them
  std::vector<JSON::String> parameters;
  /// The tags it names, in the order it writes them
  std::vector<JSON::String> tags;
};

/// A Path Item Object that the entry document exposes, and what it is exposed
/// as: a path template, a webhook name, or a runtime expression
struct OpenAPIEndpoint {
  OpenAPIOperationKind kind;
  JSON::String path;
  JSON::String path_item;
};

/// One operation of the described API, which is what an endpoint and the Path
/// Item it reaches come to between them
struct OpenAPIOperation {
  OpenAPIOperationKind kind;
  JSON::String path;
  JSON::StringView method;
  /// Where the Operation Object sits
  JSON::String origin;
  /// Where the Path Item Object that exposes it sits, which is the position
  /// that gives it a URL rather than the one that defines it. The two differ
  /// whenever a reference stands between them
  JSON::String endpoint;
  /// Where the Server Objects in force sit, empty when nothing declares any,
  /// in which case Section 4.8.1 puts a single Server Object with a `url` of
  /// `/` in their place
  std::vector<JSON::String> servers;
  /// Where the Security Requirement Objects in force sit
  std::vector<JSON::String> security;
  /// Where the Parameter Objects in force sit, which is what the Path Item
  /// Object declares once anything the Operation Object overrides is taken
  /// out, followed by what the Operation Object declares itself
  std::vector<JSON::String> parameters;
  /// Where the Tag Object each of its tags names sits, in the order the
  /// operation wrote them, with no value where the entry document declares no
  /// tag by that name. Section 4.8.1 permits exactly that: "Not all tags that
  /// are used by the Operation Object must be declared"
  std::vector<std::optional<JSON::String>> tags;
};

struct OpenAPILocation {
  OpenAPIObjectKind type;
  Pointer pointer;
  /// Set on the root of a document alone: the default `$schema` in force for
  /// the Schema Objects it holds, resolved against its base. A Schema Object
  /// that declares its own overrides it, which is a matter for whatever reads
  /// inside one
  JSON::String dialect;
};

// What every check needs to reach beyond the Object in front of it: the
// document it is reading, so an error can name it, and the means to follow a
// reference out of it. OpenAPI Specification 3.1.1, Section 4.8.10 makes
// operation identifiers unique across the whole description rather than one
// document, so the set that tracks them spans every document too
struct OpenAPIWalk {
  const OpenAPIResolver &resolver;
  JSON::String base;
  /// Every document a reference brought in. A resolver may hand back a
  /// document it owns rather than one the caller keeps alive, so reading one
  /// must not be the last thing that happens to it. This is a deque rather
  /// than a vector because the walk points into these while it reads them
  std::deque<OpenAPIResolverResult> documents;
  // The document the checks are reading, which a reference that stays inside
  // it resolves its fragment against
  const JSON *document{nullptr};
  // Kept against where each identifier was read, so that reaching one Operation
  // Object twice, which following a reference into the document being read
  // does, is told apart from two Operation Objects claiming one identifier.
  // These own their strings, as a resolver that hands back a document it owns
  // has that document destroyed once it has been read
  std::map<JSON::String, JSON::String> operation_ids;
  // A description may reference the same document twice, or reference its way
  // back to one already read. This is keyed by the type expected of a document
  // as well as by the document itself, because the same file referenced from
  // two positions is two different Objects, and reading it once as whichever
  // reference happened to be walked first would let read order decide what it
  // is. Section 3.2 of OAS 3.2 names this hazard and says the behaviour "MAY
  // be treated as an error if detected", so checking it as each type in turn
  // surfaces a genuine conflict rather than hiding it
  std::set<std::pair<JSON::String, OpenAPIObjectKind>> visited;

  /// Keyed the way a schema frame keys its own, by the base with the pointer
  /// as a fragment, or by the pointer alone when no base was established. The
  /// document an Object sits in is that key up to its fragment, so nothing
  /// records it a second time
  std::map<JSON::String, OpenAPILocation> locations;
  /// Every reference the description makes, whether or not it was followed,
  /// keyed by the location of the Object that makes it. Kept apart from the
  /// locations themselves only because reading one Object twice records it
  /// twice, and what it stands in for must survive that
  std::map<JSON::String, OpenAPIReference> references;
  /// Every Path Item Object and Operation Object read, along with the Callback
  /// Objects that hold more of them, all keyed by where they sit. The
  /// projection that turns these into operations runs once the walk is over,
  /// as a Path Item may be reached before the endpoint that exposes it
  /// What every Parameter Object read is called and where it goes, keyed by
  /// where it sits. Section 4.8.9 identifies a parameter "by a combination of a
  /// name and location", which is what tells an override from an addition. It
  /// owns its strings, as the document it was read from may be gone by the
  /// time an operation is projected
  std::map<JSON::String, std::pair<JSON::String, JSON::String>> parameters;
  std::map<JSON::String, OpenAPIPathItemRecord> path_items;
  std::map<JSON::String, OpenAPIOperationRecord> operation_records;
  std::map<JSON::String, std::vector<std::pair<JSON::String, JSON::String>>>
      callbacks;
  /// What the entry document exposes, in the order it writes it
  std::vector<OpenAPIEndpoint> endpoints;
  /// Where the entry document's own Server Objects and Security Requirement
  /// Objects sit, which is what an operation declaring neither falls back on
  std::vector<JSON::String> servers;
  std::optional<std::vector<JSON::String>> security;
  /// The names the entry document declares as security schemes, which is what
  /// a Security Requirement Object anywhere in the description may name
  std::set<JSON::String> security_schemes;
  /// Where the entry document declares each Tag Object, by the name it gave
  /// it, which is the name an Operation Object's tags resolve against
  std::map<JSON::String, JSON::String> tags;
  /// The operation each Link Object names, keyed by where that Link Object
  /// sits. Section 3 has resolving one of these require "parsing all
  /// referenced documents prior to determining an `operationId` to be
  /// unresolvable", so nothing is decided about them until the walk is over
  std::map<JSON::String, JSON::String> operation_id_links;
  /// Whether the document being read is the entry document, which is the only
  /// one whose Paths Object describes the API
  bool entry{true};
  /// The default `$schema` in force for the document being read, which every
  /// Schema Object position in it hands on
  JSON::String dialect;
  /// What the entry document says about the API, kept so that reading it once
  /// serves both the walk and the caller
  OpenAPIInfo info;
};

// Follow a reference that leaves the document being read. Defined alongside
// the document-level checks, as those are what a referenced document goes
// through, and declared here because a Reference Object is the thing that
// triggers it
inline auto openapi_follow_reference(JSON::StringView reference,
                                     const Pointer &origin,
                                     OpenAPIObjectKind expected,
                                     OpenAPIWalk &walk) -> void;

inline auto openapi_child(const Pointer &base, const JSON::StringView field)
    -> Pointer {
  return base.concat(JSON::String{field});
}

inline auto openapi_child(const Pointer &base, const std::size_t index)
    -> Pointer {
  return base.concat(index);
}

// OpenAPI Specification 3.1.1, Section 4.2: "The schema exposes two types of
// fields: fixed fields, which have a declared name, and patterned fields,
// which have a declared pattern for the field name". These objects declare
// `^x-` as their only pattern, so a member that is neither is not a field that
// this specification defines
template <std::size_t Size>
auto openapi_reject_unknown_fields(
    const JSON &object, const std::array<JSON::StringView, Size> &fields,
    const Pointer &base, const char *message) -> void {
  for (const auto &entry : object.as_object()) {
    if (entry.first.starts_with(OPENAPI_EXTENSION_PREFIX) ||
        std::ranges::find(fields, entry.first) != fields.cend()) {
      continue;
    }

    throw OpenAPIError{openapi_child(base, entry.first), message};
  }
}

// A location is keyed the way a schema frame keys its own: the document base
// with the pointer hung off it as a fragment, or the pointer alone when no
// base was established, and the base by itself for the root of a document.
//
// RFC 6901 Section 6: "A JSON Pointer can be represented in a URI fragment
// identifier by encoding it into octets using UTF-8, while percent-encoding
// those characters not allowed by the fragment rule in [RFC3986]". A path
// template holds braces and a callback expression holds a `#`, neither of
// which a fragment admits, so writing the pointer out as it stands would give
// a key that is no URI and that a reference to the same place could never
// match
inline auto openapi_location_uri(const JSON::String &base,
                                 const Pointer &pointer) -> JSON::String {
  if (pointer.empty()) {
    return base;
  }

  // RFC 3986 Section 5.2.2 resolves a fragment-only reference by keeping every
  // other component of the base as it stands, and a base here carries no
  // fragment of its own, so appending is that resolution without parsing the
  // base again for every Object recorded
  JSON::String result{base};
  result.append(to_uri(pointer).recompose());
  return result;
}

// Every Object gets one of these. The nearest recorded ancestor is the parent,
// which holds because an Object is always recorded before anything inside it
inline auto openapi_record(OpenAPIWalk &walk, const Pointer &pointer,
                           const OpenAPIObjectKind kind,
                           JSON::String dialect = {}) -> void {
  walk.locations.insert_or_assign(
      openapi_location_uri(walk.base, pointer),
      OpenAPILocation{
          .type = kind, .pointer = pointer, .dialect = std::move(dialect)});
}

inline auto openapi_expect_object(const JSON &value, const Pointer &location,
                                  const char *message) -> void {
  if (!value.is_object()) {
    throw OpenAPIError{location, message};
  }
}

inline auto openapi_expect_array(const JSON &value, const Pointer &location,
                                 const char *message) -> void {
  if (!value.is_array()) {
    throw OpenAPIError{location, message};
  }
}

inline auto openapi_expect_boolean(const JSON &value, const Pointer &base,
                                   const JSON::StringView field,
                                   const char *message) -> void {
  if (!value.is_boolean()) {
    throw OpenAPIError{openapi_child(base, field), message};
  }
}

// A Schema Object is where framing stops, and Section 4.8.24 permits it to be
// "the boolean value `true`" or "the boolean value `false`" as well as an
// object, which is all the meta-schema asserts of one it leaves unvalidated.
// Its location is the handoff: a pointer, the base its key carries, and the
// dialect in force, which is everything a JSON Schema implementation needs to
// take it from here
inline auto openapi_expect_schema(const JSON &value, const Pointer &location,
                                  const char *message, OpenAPIWalk &walk)
    -> void {
  if (!value.is_object() && !value.is_boolean()) {
    throw OpenAPIError{location, message};
  }

  openapi_record(walk, location, OpenAPIObjectKind::Schema, walk.dialect);
}

inline auto openapi_check_map_of_strings(const JSON &value,
                                         const Pointer &location,
                                         const char *type_message,
                                         const char *entry_message) -> void {
  openapi_expect_object(value, location, type_message);
  for (const auto &entry : value.as_object()) {
    if (!entry.second.is_string()) {
      throw OpenAPIError{openapi_child(location, entry.first), entry_message};
    }
  }
}

inline auto openapi_check_array_of_strings(const JSON &value,
                                           const Pointer &location,
                                           const char *type_message,
                                           const char *entry_message) -> void {
  openapi_expect_array(value, location, type_message);
  std::size_t index{0};
  for (const auto &entry : value.as_array()) {
    if (!entry.is_string()) {
      throw OpenAPIError{openapi_child(location, index), entry_message};
    }

    index += 1;
  }
}

inline auto openapi_expect_string(const JSON &value, const Pointer &base,
                                  const JSON::StringView field,
                                  const char *message) -> JSON::StringView {
  if (!value.is_string()) {
    throw OpenAPIError{openapi_child(base, field), message};
  }

  return value.to_string();
}

// OpenAPI Specification 3.1.1, Section 4.6: "Unless specified otherwise, all
// fields that are URIs MAY be relative references as defined by RFC3986", so
// what these fields hold is a URI reference rather than an absolute URI
inline auto openapi_expect_uri_reference(const JSON &value, const Pointer &base,
                                         const JSON::StringView field,
                                         const char *type_message,
                                         const char *syntax_message)
    -> JSON::StringView {
  const auto result{openapi_expect_string(value, base, field, type_message)};
  if (!URI::is_uri_reference(result)) {
    throw OpenAPIError{openapi_child(base, field), syntax_message};
  }

  return result;
}

inline auto openapi_expect_enumeration(
    const JSON &value, const Pointer &base, const JSON::StringView field,
    const std::initializer_list<JSON::StringView> options,
    const char *type_message, const char *value_message) -> JSON::StringView {
  const auto result{openapi_expect_string(value, base, field, type_message)};
  if (std::ranges::find(options, result) == options.end()) {
    throw OpenAPIError{openapi_child(base, field), value_message};
  }

  return result;
}

} // namespace sourcemeta::core

#endif
