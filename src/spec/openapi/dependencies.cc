#include <sourcemeta/core/openapi.h>

#include <cassert>       // assert
#include <cstdint>       // std::uint64_t
#include <deque>         // std::deque
#include <memory>        // std::make_unique, std::unique_ptr
#include <optional>      // std::optional
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move
#include <vector>        // std::vector

namespace sourcemeta::core {

namespace {

// How a document was reached, which decides which walk reads it. An OpenAPI
// reference leads to another description, while what a Schema Object names is
// a plain schema that JSON Schema rather than this specification walks
enum class Reached : std::uint8_t { Description, Schema };

// OpenAPI 3.1.1 Section 4.3.1 offers detecting the root `openapi` field and
// detecting a schema by its keywords as equal ways of telling a description
// from a schema, which is what this walk needs to refuse one where the other
// belongs. Neither is given precedence there, so a document that says which
// dialect it is written against or what it answers to is taken at its word,
// leaving the field to tell of a document that says neither. A boolean is a
// schema in full per JSON Schema Section 4.3, and carries no field at all
auto is_description(const JSON &document) -> bool {
  if (!document.is_object()) {
    return false;
  }

  if (document.defines("$schema") || document.defines("$id")) {
    return false;
  }

  const auto *version{document.try_at("openapi")};
  return version != nullptr && version->is_string();
}

// A document that the walk has read and not yet examined, which owns whatever
// the resolver handed over for as long as the edges that name it are reported
struct Fetched {
  JSON::String identifier;
  JSON document;
  Reached reached;
};

// Every URI the walk has either reported or holds of its own, so that a
// reference naming one of them is no edge and no second fetch. The schema
// walk answers from this one and reports back into it, so what either half
// comes to know is known to the other
using Accounted = std::unordered_set<JSON::String>;

// A reference that nothing answers for yet, which owns what reporting it takes
// for as long as the walk may still settle it
struct Deferred {
  JSON::String origin;
  Pointer pointer;
  JSON::String destination;
};

struct Walk {
  const SchemaWalker &walker;
  const SchemaResolver &schema_resolver;
  const OpenAPIResolver &resolver;
  const OpenAPIDependencyCallback &callback;
  Accounted accounted;
  std::deque<Fetched> pending;
  std::vector<Deferred> deferred;
  std::uint64_t remaining;
  const std::uint64_t limit;
};

// The document a destination names, which is whatever precedes the fragment it
// carries. OpenAPIFrame resolves and canonicalises a destination already, so
// this cuts rather than parses
auto document_of(const JSON::StringView destination) -> JSON::StringView {
  const auto fragment{destination.find('#')};
  return fragment == JSON::StringView::npos ? destination
                                            : destination.substr(0, fragment);
}

// An official dialect is reported when a reference names it and never read, as
// one can only ever reach other official schemas
auto is_traversable(const JSON::StringView identifier) -> bool {
  return !schema_is_official(identifier);
}

// Naming an official dialect to declare one is no dependency, as every reader
// of this specification is expected to know them already, while naming one by
// reference asks for the document itself
auto is_skippable_metaschema_reference(const WeakPointer &pointer,
                                       const JSON::StringView destination)
    -> bool {
  assert(!pointer.empty());
  assert(pointer.back().is_property());
  if (pointer.back().to_property() != "$schema") {
    return false;
  }

  return schema_is_official(destination);
}

// What one frame registered, which is both the Objects of the description and
// the schemas it holds, as the allowance it was given covers the two together
auto registered(const OpenAPIFrame &frame) -> std::uint64_t {
  return frame.locations().size() + frame.schemas().location_count();
}

// Reading a document takes an allowance that every frame of the walk spends
// from, and the frame that runs out reports the allowance the caller set
// rather than whatever was left of it
auto charge(Walk &walk, const std::uint64_t locations) -> void {
  assert(locations <= walk.remaining);
  walk.remaining -= locations;
}

// Framing reports whatever was left of the allowance when it ran out, so this
// restates it as the one the caller set
auto frame_within(const Walk &walk, const JSON &document,
                  const JSON::StringView default_base)
    -> std::unique_ptr<OpenAPIFrame> {
  try {
    return std::make_unique<OpenAPIFrame>(document, walk.walker,
                                          walk.schema_resolver, default_base,
                                          walk.remaining);
  } catch (const OpenAPIFrameLimitError &) {
    throw OpenAPIFrameLimitError{walk.limit};
  }
}

auto report(Walk &walk, const JSON::StringView origin, const Pointer &pointer,
            const JSON::StringView destination,
            const OpenAPIObjectKind expected, const Reached reached) -> void {
  const JSON::String key{destination};
  if (walk.accounted.contains(key)) {
    return;
  }

  if (!sourcemeta::core::URI{key}.scheme().has_value()) {
    throw OpenAPIReferenceError{
        "The reference does not resolve to an absolute URI",
        JSON::String{origin}, pointer, key};
  }

  // What answers for a document depends on what it is. A description comes
  // from the resolver of descriptions, and what a Schema Object names is a
  // schema, which is the other resolver's to answer for
  auto remote{reached == Reached::Description
                  ? walk.resolver(destination)
                  : walk.schema_resolver(destination)};
  if (!remote.has_value()) {
    // A document the walk has not read yet may answer to this name of its
    // own, whether it declares it or carries the Object within itself.
    // OpenAPI 3.1.1 Section 4.3.1 requires complete documents to be parsed
    // before a Schema Object reference is deemed unresolvable, and what holds
    // for those holds for the rest, so whether this leads anywhere is settled
    // once there is nothing left to read rather than here
    walk.deferred.emplace_back(Deferred{.origin = JSON::String{origin},
                                        .pointer = pointer,
                                        .destination = key});
    return;
  }

  // Reading a part of a description without the rest of it would miss what
  // bears on the reference, which OpenAPI 3.1.1 Section 4.3.1 warns resolves
  // references to unintended URIs, so what a Schema Object names has to be a
  // schema document rather than a description holding one
  if (reached == Reached::Schema && is_description(remote.value())) {
    throw OpenAPIReferenceError{
        "A Schema Object may only reference a JSON Schema document",
        JSON::String{origin}, pointer, key};
  }

  walk.callback({.origin = origin,
                 .pointer = pointer,
                 .destination = destination,
                 .document = remote.value(),
                 .expected = expected});
  walk.accounted.emplace(key);

  if (is_traversable(destination) && !remote.value().is_boolean()) {
    walk.pending.emplace_back(Fetched{.identifier = key,
                                      .document = std::move(remote).to_owned(),
                                      .reached = reached});
  }
}

// What a Schema Object names has to be a schema document at every hop, not
// only the first, so this holds the whole closure below one to that
auto schema_resolver_within(const Walk &walk, const JSON::StringView origin)
    -> SchemaResolver {
  return [&walk,
          origin](const JSON::StringView identifier) -> SchemaResolverResult {
    auto result{walk.schema_resolver(identifier)};
    if (result.has_value() && is_description(result.value())) {
      throw OpenAPIReferenceError{
          "A Schema Object may only reference a JSON Schema document",
          JSON::String{origin}, EMPTY_POINTER, JSON::String{identifier}};
    }

    return result;
  };
}

// Everything one document of the description reaches, which is the references
// this specification gives meaning to and the ones its schemas make
auto examine(Walk &walk, const JSON &document, const OpenAPIFrame &frame,
             const JSON::StringView origin) -> void {
  // Every URI this document answers to is one the caller never has to fetch,
  // whether the document declares it or carries the Object within itself
  for (const auto &location : frame.locations()) {
    walk.accounted.emplace(location.first);
  }

  // A Schema Object of this document answers to whatever it declares, which
  // the Objects above never hold, and OpenAPI 3.2.1 Section 4.3.1 counts it
  // among the fields a reference target may be defined by
  frame.schemas().for_each_resource_uri(
      [&walk](const auto &uri) -> void { walk.accounted.emplace(uri); });

  // Every Schema Object of a description that declares no dialect of its own
  // is read against the one the description names, so reading any of them
  // takes that document
  const auto *dialect{document.try_at("jsonSchemaDialect")};
  if (dialect != nullptr && dialect->is_string()) {
    // OpenAPI 3.1.1 Section 4.4 has every field that is a URI take a relative
    // reference, which resolves against the base of the document naming it
    auto settled{sourcemeta::core::URI{dialect->to_string()}};
    if (!origin.empty()) {
      settled.resolve_from(sourcemeta::core::URI{JSON::String{origin}});
    }

    settled.canonicalize();
    const auto identifier{settled.recompose()};
    if (!schema_is_official(identifier)) {
      static const Pointer POINTER{"jsonSchemaDialect"};
      report(walk, origin, POINTER, identifier, OpenAPIObjectKind::Schema,
             Reached::Schema);
    }
  }

  for (const auto &entry : frame.references()) {
    if (entry.second.dangling) {
      report(walk, origin, entry.second.origin,
             document_of(entry.second.destination), entry.second.expected,
             Reached::Description);
    }
  }

  for (const auto &entry : frame.security_references()) {
    if (entry.second.dangling) {
      report(walk, origin, entry.second.origin,
             document_of(entry.second.destination), entry.second.expected,
             Reached::Description);
    }
  }

  for (const auto &discriminator : frame.discriminators()) {
    if (discriminator.dangling) {
      report(walk, origin, discriminator.origin,
             document_of(discriminator.destination), OpenAPIObjectKind::Schema,
             Reached::Schema);
    }
  }

  // What a Schema Object reaches is JSON Schema's to give meaning to, so the
  // kind a position expects has nothing to say about it
  frame.schemas().for_each_unresolved_reference(
      [&walk, &origin](const auto &pointer, const auto &reference) -> void {
        if (!reference.base.empty() &&
            !is_skippable_metaschema_reference(pointer, reference.base)) {
          report(walk, origin, to_pointer(pointer), reference.base,
                 OpenAPIObjectKind::Schema, Reached::Schema);
        }
      });
}

} // namespace

auto openapi_dependencies(const JSON &document, const SchemaWalker &walker,
                          const SchemaResolver &schema_resolver,
                          const OpenAPIResolver &resolver,
                          const OpenAPIDependencyCallback &callback,
                          const OpenAPIDependencyOptions &options)
    -> std::uint64_t {
  assert(callback);

  Walk walk{.walker = walker,
            .schema_resolver = schema_resolver,
            .resolver = resolver,
            .callback = callback,
            .accounted = {},
            .pending = {},
            .deferred = {},
            .remaining = options.max_locations,
            .limit = options.max_locations};

  {
    const auto frame{frame_within(walk, document, options.default_base)};
    charge(walk, registered(*frame));
    if (!options.default_base.empty()) {
      walk.accounted.emplace(
          sourcemeta::core::URI::canonicalize(options.default_base));
    }

    examine(walk, document, *frame, options.default_base);
  }

  // Reading a document can only ever add to what the walk accounts for, so a
  // document reached from one already read is reported from where it was
  // reached rather than from the top
  while (!walk.pending.empty()) {
    const auto entry{std::move(walk.pending.front())};
    walk.pending.pop_front();

    if (entry.reached == Reached::Description) {
      const auto frame{frame_within(walk, entry.document, entry.identifier)};
      charge(walk, registered(*frame));
      examine(walk, entry.document, *frame, entry.identifier);
      continue;
    }

    // What a Schema Object names is a schema rather than a description, so
    // what it reaches in turn is for the schema walk to follow. That walk
    // reports the whole closure below this document, so what comes back is
    // passed straight on, and the one allowance covers both
    SchemaDependenciesOptions schemas;
    schemas.default_id = entry.identifier;
    schemas.default_base = entry.identifier;
    schemas.max_locations = walk.remaining;
    schemas.accounted = [&walk](const std::string_view identifier) -> bool {
      return walk.accounted.contains(JSON::String{identifier});
    };
    schemas.accounted_callback =
        [&walk](const std::string_view identifier) -> void {
      walk.accounted.emplace(identifier);
    };
    // The schema walk spends from the same allowance and reports running out
    // in its own terms, which the caller was promised in these
    try {
      charge(walk,
             schema_dependencies(
                 entry.document, walk.walker,
                 schema_resolver_within(walk, entry.identifier),
                 [&walk](const SchemaDependency &dependency) -> void {
                   walk.callback({.origin = dependency.origin,
                                  .pointer = to_pointer(dependency.pointer),
                                  .destination = dependency.destination,
                                  .document = dependency.document,
                                  .expected = OpenAPIObjectKind::Schema});
                 },
                 schemas));
    } catch (const SchemaFrameLimitError &) {
      throw OpenAPIFrameLimitError{walk.limit};
    } catch (const SchemaResolutionError &error) {
      // The schema walk settles what it could not resolve against the
      // documents it read of its own, which are not every document of the
      // span, so this one is settled against the rest once there is nothing
      // left to read. Its own walk has run to the end by now, so nothing it
      // reaches goes unreported for this
      walk.deferred.emplace_back(
          Deferred{.origin = entry.identifier,
                   .pointer = EMPTY_POINTER,
                   .destination = JSON::String{error.identifier()}});
    }
  }

  for (const auto &entry : walk.deferred) {
    if (!walk.accounted.contains(entry.destination)) {
      throw OpenAPIResolutionError{
          "Could not resolve the reference to an external document",
          entry.origin, entry.pointer, entry.destination};
    }
  }

  return options.max_locations - walk.remaining;
}

} // namespace sourcemeta::core
