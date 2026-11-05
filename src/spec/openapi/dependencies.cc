#include <sourcemeta/core/openapi.h>

#include "dialect.h"
#include "discriminator.h"
#include "document.h"

#include <cassert> // assert
#include <cstddef> // std::size_t
#include <cstdint> // std::uint64_t
#include <deque>   // std::deque
#include <exception> // std::current_exception, std::exception_ptr, std::rethrow_exception
#include <functional>    // std::equal_to, std::hash
#include <memory>        // std::make_unique, std::unique_ptr
#include <optional>      // std::optional
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
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

// A caller's resolver may run an allowance of its own, and running out of
// that one is its business rather than this walk's, so such an error travels
// wrapped to pass through the conversions below untouched
struct ForeignLimitError {
  std::exception_ptr error;
};

// A document that the walk has read and not yet examined, which owns whatever
// the resolver handed over for as long as the edges that name it are reported
struct Fetched {
  JSON::String identifier;
  JSON document;
  // The revision of the description that named it, which OpenAPI 3.1.1 Section
  // 4.8.24.5 is what settles the dialect of a schema declaring none: "For
  // standalone JSON Schema documents that do not set `$schema` [...] the
  // dialect SHOULD be assumed to be the OAS dialect", which is the one the
  // description making the reference is written against
  OpenAPIVersion version;
  // Whether the schema walk below this document has already reported what it
  // reaches, which is so of a document that walk came upon rather than one a
  // description named. Such a document is read here for the Objects that
  // belong to this specification alone
  bool closure_read;
};

// What the walk knows a URI by, which is a different question from whether it
// has seen the string before. OpenAPI 3.1.1 Section 4.3.2 has a document that
// two references require to be read as different kinds of Object be one whose
// handling "is implementation defined, and MAY be treated as an error if
// detected", and telling what the walk holds apart from the bare name of it is
// what detecting that takes
struct Held {
  Reached reached;
  // The dialect it was read under, and whether that reading is beyond question.
  // It is not for a standalone schema that declares no dialect, as the OAS
  // dialect the section above falls back on is one per revision, so two
  // descriptions of different revisions naming such a document mean two
  // readings of it
  JSON::StringView dialect;
  bool dialect_settled;
  // Whether what is held names a dialect that is not the one the earliest
  // revision publishes, which a description of that revision may not reach
  // however the walk came to hold it. Recording it is what keeps the answer
  // from turning on which namer arrived first
  bool foreign_to_earliest;
};

// Every URI the walk has either reported or holds of its own, so that a
// reference naming one of them is no edge and no second fetch. The schema
// walk answers from this one and reports back into it, so what either half
// comes to know is known to the other
struct AccountedHash {
  using is_transparent = void;

  [[nodiscard]] auto operator()(const JSON::StringView value) const noexcept
      -> std::size_t {
    return std::hash<JSON::StringView>{}(value);
  }
};

using Accounted =
    std::unordered_map<JSON::String, Held, AccountedHash, std::equal_to<>>;

// A reference that nothing answers for yet, which owns what reporting it takes
// for as long as the walk may still settle it
struct Deferred {
  JSON::String origin;
  Pointer pointer;
  JSON::String destination;
  // The dialect the position naming it would have read it under, as a document
  // that answers later is answering to this reference as much as to any other
  JSON::StringView dialect;
  // What the position spelling it expects the far end to be, as a document
  // answering to that name settles the reference only where it is that
  Reached reached;
};

struct Walk {
  const SchemaWalker &walker;
  const SchemaResolver &schema_resolver;
  const OpenAPIResolver &resolver;
  const OpenAPIDependencyCallback &callback;
  Accounted accounted;
  // Reading a description can reach both kinds, while what a Schema Object
  // names reaches only schemas, so every description of the span is read
  // before the first schema. That leaves the whole of what this walk holds
  // known to the schema walk rather than only the part read so far
  std::deque<Fetched> descriptions;
  std::deque<Fetched> schemas;
  std::vector<Deferred> deferred;
  std::uint64_t remaining;
  const std::uint64_t limit;
};

// Whatever the caller's resolver makes of its own allowance is not this
// walk's to restate, so it is marked as theirs before anything here sees it
auto resolver_within_allowance(const SchemaResolver &resolver)
    -> SchemaResolver {
  return
      [&resolver](const JSON::StringView identifier) -> SchemaResolverResult {
        try {
          return resolver(identifier);
        } catch (const SchemaFrameLimitError &) {
          throw ForeignLimitError{std::current_exception()};
        } catch (const OpenAPIFrameLimitError &) {
          throw ForeignLimitError{std::current_exception()};
        }
      };
}

// Reporting runs the caller's own code, which may keep an allowance of its own
// and report running out of it in the terms this walk uses. Marking it as
// theirs is what keeps the conversions below from restating it as this one
auto report_to_caller(const Walk &walk, const OpenAPIDependency &dependency)
    -> void {
  try {
    walk.callback(dependency);
  } catch (const SchemaFrameLimitError &) {
    throw ForeignLimitError{std::current_exception()};
  } catch (const OpenAPIFrameLimitError &) {
    throw ForeignLimitError{std::current_exception()};
  }
}

// The document a destination names, which is whatever precedes the fragment it
// carries. OpenAPIFrame resolves and canonicalises a destination already, so
// this cuts rather than parses
auto document_of(const JSON::StringView destination) -> JSON::StringView {
  return sourcemeta::core::take_until(destination, '#');
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

// Whether the dialect this document is read under is beyond question, which
// OpenAPI 3.1.1 Section 4.8.24.5 has it be wherever the document says so for
// itself: "The `$schema` keyword MAY be present in any Schema Object that is a
// schema resource root, and if present MUST be used to determine which dialect
// should be used when processing the schema". A boolean is a schema in full
// with nowhere to carry that keyword, so nothing about it is in question either
auto dialect_settled(const JSON &document) -> bool {
  return !document.is_object() || document.defines("$schema");
}

// A destination that names the document being examined, and a place within it,
// is no reference out of that document. Where such a fragment leads is the
// business of whoever reads the document rather than a dependency on another,
// and naming the document itself without one is a different matter that the
// kinds below settle
// Whether a document says of itself that it is written against a dialect other
// than the one the earliest revision publishes. OpenAPI 3.0.4 Section 4.7.24.1
// closes that revision's keyword set with "Additional keywords defined by the
// JSON Schema specification that are not mentioned here are strictly
// unsupported", which leaves a Schema Object of it no way of naming one, so
// anything a document names here is another
auto declares_foreign_dialect(const JSON &document) -> bool {
  if (!document.is_object()) {
    return false;
  }

  const auto *declared{document.try_at("$schema")};
  return declared != nullptr &&
         !(declared->is_string() &&
           declared->to_string() ==
               openapi_dialect(OpenAPIVersion::OPENAPI_3_0));
}

auto names_this_document(const JSON::StringView destination,
                         const JSON::StringView base) -> bool {
  return destination.size() > base.size() && destination.starts_with(base) &&
         destination[base.size()] == '#';
}

auto account(Walk &walk, const JSON::StringView uri, const Reached reached,
             const JSON::StringView dialect = {},
             const bool dialect_settled = true,
             const bool foreign_to_earliest = false) -> void {
  walk.accounted.emplace(JSON::String{uri},
                         Held{.reached = reached,
                              .dialect = dialect,
                              .dialect_settled = dialect_settled,
                              .foreign_to_earliest = foreign_to_earliest});
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
                  const JSON::StringView default_base,
                  const OpenAPIFrame *entry) -> std::unique_ptr<OpenAPIFrame> {
  try {
    return std::make_unique<OpenAPIFrame>(document, walk.walker,
                                          walk.schema_resolver, default_base,
                                          walk.remaining, entry);
  } catch (const OpenAPIFrameLimitError &) {
    throw OpenAPIFrameLimitError{walk.limit};
  }
}

// Registering what a schema of the span answers to, and reading the
// Discriminator Objects it holds, takes framing it as a schema, which reports
// running out in its own terms rather than in the ones this operation promised
//
// What this registers is not charged for. The schemas below are framed again by
// the walk that follows, which charges them, and this frame is released before
// that walk begins, so what the allowance bounds is unchanged either way
auto schema_frame_within(const Walk &walk, const Fetched &entry)
    -> std::unique_ptr<SchemaFrame> {
  try {
    return std::make_unique<SchemaFrame>(
        SchemaFrame::Mode::References, entry.document, walk.walker,
        walk.schema_resolver, openapi_dialect(entry.version), entry.identifier,
        SchemaFrame::IdentifierMode::Additional,
        SchemaFrame::Paths{sourcemeta::core::EMPTY_WEAK_POINTER},
        entry.identifier, walk.remaining);
  } catch (const SchemaFrameLimitError &) {
    throw OpenAPIFrameLimitError{walk.limit};
  }
}

// OpenAPI 3.1.1 Section 4.3.2 leaves a document that two references require to
// be read as different kinds of Object to the reader, which "MAY be treated as
// an error if detected". Reading part of a description as a schema would miss
// what bears on the reference, which Section 4.3.1 warns resolves references to
// unintended URIs, so this is detected wherever the walk knows the document by
// another kind rather than only where it reads one afresh
auto reject_conflicting_kind(const Held &held, const Reached reached,
                             const JSON::StringView origin,
                             const Pointer &pointer,
                             const JSON::StringView destination) -> void {
  if (held.reached == reached) {
    return;
  }

  if (reached == Reached::Schema) {
    throw OpenAPIReferenceError{
        "A Schema Object may only reference a JSON Schema document",
        JSON::String{origin}, pointer, JSON::String{destination}};
  }

  throw OpenAPIReferenceError{
      "A reference of this specification may only reach a document of it",
      JSON::String{origin}, pointer, JSON::String{destination}};
}

// The dialect a schema declaring none falls back on is the one its namer's
// revision publishes, so two namers of different revisions give one document
// two readings, and the keywords of one are not the keywords of the other.
// OpenAPI 3.1.1 Section 4.8.24.5 has the remedy rather than a winner: "for
// maximum interoperability, it is RECOMMENDED that OpenAPI description authors
// explicitly set the dialect through `$schema` in such documents"
auto reject_two_readings(const Held &held, const Reached reached,
                         const JSON::StringView dialect,
                         const JSON::StringView origin, const Pointer &pointer,
                         const JSON::StringView destination) -> void {
  if (reached != Reached::Schema || held.dialect_settled ||
      held.dialect == dialect) {
    return;
  }

  throw OpenAPIReferenceError{
      "A schema that declares no dialect cannot be read under the dialect of "
      "two revisions at once",
      JSON::String{origin}, pointer, JSON::String{destination}};
}

// What a description of the earliest revision reaches has to be written against
// the dialect that revision publishes whether this walk is reading it now or
// read it for another description earlier
auto reject_foreign_hold(const Held &held, const Reached reached,
                         const OpenAPIVersion version,
                         const JSON::StringView origin, const Pointer &pointer)
    -> void {
  if (reached == Reached::Schema && version == OpenAPIVersion::OPENAPI_3_0 &&
      held.foreign_to_earliest) {
    throw OpenAPIError{"A Schema Object of this revision must be written "
                       "against the OpenAPI 3.0 dialect",
                       JSON::String{origin}, pointer};
  }
}

auto report(Walk &walk, const JSON::StringView origin, const Pointer &pointer,
            const JSON::StringView destination,
            const OpenAPIObjectKind expected, const Reached reached,
            const OpenAPIVersion version) -> void {
  const JSON::String key{destination};
  if (!sourcemeta::core::URI{key}.scheme().has_value()) {
    throw OpenAPIReferenceError{
        "The reference does not resolve to an absolute URI",
        JSON::String{origin}, pointer, key};
  }

  if (reached == Reached::Schema && version == OpenAPIVersion::OPENAPI_3_0 &&
      schema_is_official(destination)) {
    throw OpenAPIError{"A Schema Object of this revision must be written "
                       "against the OpenAPI 3.0 dialect",
                       JSON::String{origin}, pointer};
  }

  const auto dialect{openapi_dialect(version)};
  const auto known{walk.accounted.find(destination)};
  if (known != walk.accounted.cend()) {
    reject_conflicting_kind(known->second, reached, origin, pointer,
                            destination);
    reject_two_readings(known->second, reached, dialect, origin, pointer,
                        destination);
    reject_foreign_hold(known->second, reached, version, origin, pointer);
    return;
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
    // OpenAPI 3.2.1 Section 4.1.2.1 has an implementation "MUST NOT treat a
    // reference as unresolvable before completely parsing all documents
    // provided to the implementation as possible parts of the OAD", so
    // whether this leads anywhere is settled once there is nothing left to
    // read rather than here
    walk.deferred.emplace_back(Deferred{.origin = JSON::String{origin},
                                        .pointer = pointer,
                                        .destination = key,
                                        .dialect = dialect,
                                        .reached = reached});
    return;
  }

  // Reading a part of a description without the rest of it would miss what
  // bears on the reference, which OpenAPI 3.1.1 Section 4.3.1 warns resolves
  // references to unintended URIs, so what a Schema Object names has to be a
  // schema document rather than a description holding one
  if (reached == Reached::Schema &&
      ((!remote.value().is_object() && !remote.value().is_boolean()) ||
       is_description(remote.value()))) {
    throw OpenAPIReferenceError{
        "A Schema Object may only reference a JSON Schema document",
        JSON::String{origin}, pointer, key};
  }

  report_to_caller(walk, {.origin = origin,
                          .pointer = pointer,
                          .destination = destination,
                          .document = remote.value(),
                          .expected = expected});
  const auto settled{reached == Reached::Description ||
                     dialect_settled(remote.value())};
  account(walk, key, reached, dialect, settled,
          reached == Reached::Schema &&
              declares_foreign_dialect(remote.value()));

  if (is_traversable(destination) &&
      (reached == Reached::Description || !remote.value().is_boolean())) {
    auto &queue{reached == Reached::Description ? walk.descriptions
                                                : walk.schemas};
    queue.emplace_back(Fetched{.identifier = key,
                               .document = std::move(remote).to_owned(),
                               .version = version,
                               .closure_read = false});
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

// Whether anything of this document sits under the keyword a Discriminator
// Object goes by, which is what decides whether reading it takes a frame of
// its own. Most documents hold none, and this spares them that
auto holds_discriminator(const JSON &document) -> bool {
  std::vector<std::reference_wrapper<const JSON>> pending;
  pending.emplace_back(document);
  while (!pending.empty()) {
    const auto &current{pending.back().get()};
    pending.pop_back();
    if (current.is_object()) {
      if (current.defines("discriminator")) {
        return true;
      }

      for (const auto &entry : current.as_object()) {
        pending.emplace_back(entry.second);
      }
    } else if (current.is_array()) {
      for (const auto &entry : current.as_array()) {
        pending.emplace_back(entry);
      }
    }
  }

  return false;
}

// Everything one document of the description reaches, which is the references
// this specification gives meaning to and the ones its schemas make
auto examine(Walk &walk, const JSON &document, const OpenAPIFrame &frame,
             const JSON::StringView origin) -> void {
  const auto base{frame.base().empty() ? origin : frame.base()};
  const auto version{sourcemeta::core::openapi_version(document)};
  assert(version.has_value());
  // Every URI this document answers to is one the caller never has to fetch,
  // whether the document declares it or carries the Object within itself
  for (const auto &location : frame.locations()) {
    account(walk, location.first, Reached::Description);
  }

  // A Schema Object of this document answers to whatever it declares, which
  // the Objects above never hold, and OpenAPI 3.2.1 Section 4.1.2.1 counts it
  // among the fields a reference target may be defined by
  frame.schemas().for_each_resource_uri(
      [&walk, &version](const auto &uri) -> void {
        account(walk, uri, Reached::Schema, {}, true,
                version.value() != OpenAPIVersion::OPENAPI_3_0);
      });

  // Every Schema Object of a description that declares no dialect of its own
  // is read against the one the description names, so reading any of them
  // takes that document
  const auto *dialect{document.try_at("jsonSchemaDialect")};
  if (dialect != nullptr && dialect->is_string()) {
    // OpenAPI 3.1.1 Section 4.6 has every field that is a URI take a relative
    // reference, which resolves against the base of the document naming it
    auto settled{sourcemeta::core::URI{dialect->to_string()}};
    if (!base.empty()) {
      settled.resolve_from(sourcemeta::core::URI{JSON::String{base}});
    }

    settled.canonicalize();
    const auto identifier{settled.recompose()};
    if (!schema_is_official(identifier)) {
      static const Pointer POINTER{"jsonSchemaDialect"};
      report(walk, origin, POINTER, document_of(identifier),
             OpenAPIObjectKind::Schema, Reached::Schema, version.value());
    }
  }

  for (const auto &entry : frame.references()) {
    if (entry.second.dangling) {
      report(walk, origin, entry.second.origin,
             document_of(entry.second.destination), entry.second.expected,
             Reached::Description, version.value());
    }
  }

  for (const auto &entry : frame.security_references()) {
    if (entry.second.dangling) {
      report(walk, origin, entry.second.origin,
             document_of(entry.second.destination), entry.second.expected,
             Reached::Description, version.value());
    }
  }

  // The name form of a `mapping` stands for a Schema Object of the Components
  // Object that the document a description begins at holds, so where it is
  // spelled that way it lands in a document of the span rather than reaching
  // out of it. Only the URI form names something the span may not hold, and
  // that one names a document this walk does not account for yet
  for (const auto &discriminator : frame.discriminators()) {
    if (discriminator.dangling &&
        !names_this_document(discriminator.destination, base) &&
        !walk.accounted.contains(document_of(discriminator.destination))) {
      report(walk, origin, discriminator.origin,
             document_of(discriminator.destination), OpenAPIObjectKind::Schema,
             Reached::Schema, version.value());
    }
  }

  // What a Schema Object reaches is JSON Schema's to give meaning to, so the
  // kind a position expects has nothing to say about it
  frame.schemas().for_each_unresolved_reference(
      [&walk, &origin, &base, &version](const auto &pointer,
                                        const auto &reference) -> void {
        if (!reference.base.empty() &&
            !names_this_document(reference.destination, base) &&
            !is_skippable_metaschema_reference(pointer, reference.base)) {
          report(walk, origin, to_pointer(pointer), reference.base,
                 OpenAPIObjectKind::Schema, Reached::Schema, version.value());
        }
      });
}

// Which dialect a Schema Object may be read under is the revision's to say,
// and the earliest of them gives one no way of naming any, so what a
// description of that revision reaches has to be written against the dialect
// it publishes. OpenAPI 3.0.4 Section 4.7.24.1 says of seven of the keywords it
// tables that the "Inline or referenced schema MUST be of a Schema Object and
// not a standard JSON Schema", and closes the set for every other position with
// "Additional keywords defined by the JSON Schema specification that are not
// mentioned here are strictly unsupported", which together rule out a schema of
// another dialect at the far end of a reference as much as one written within
auto reject_foreign_dialect(const Walk &walk, const Fetched &entry,
                            const SchemaFrame &frame) -> void {
  const SchemaFrame::Paths paths{sourcemeta::core::EMPTY_WEAK_POINTER};
  const auto foreign{openapi_schema_of_another_dialect(
      entry.version, frame, paths, walk.schema_resolver)};
  if (foreign.has_value()) {
    throw OpenAPIError{"A Schema Object of this revision must be written "
                       "against the OpenAPI 3.0 dialect",
                       entry.identifier, foreign.value()};
  }
}

// A schema the walk never frames can still be asked what it says of itself. The
// earliest revision publishes one dialect and gives a Schema Object no way of
// naming any, so a document of such a span that names one of its own is among
// those OpenAPI 3.0.4 Section 4.7.24.1 calls "a standard JSON Schema"
auto reject_foreign_declaration(const Fetched &entry,
                                const JSON::StringView destination,
                                const JSON &document) -> void {
  if (entry.version != OpenAPIVersion::OPENAPI_3_0 || !document.is_object()) {
    return;
  }

  if (!declares_foreign_dialect(document)) {
    return;
  }

  throw OpenAPIError{"A Schema Object of this revision must be written "
                     "against the OpenAPI 3.0 dialect",
                     JSON::String{destination}, EMPTY_POINTER};
}

// Every name a schema of the span answers to, and every Discriminator Object it
// holds, before any of them is walked. OpenAPI 3.2.1 Section 4.1.2.1 has an
// implementation "MUST NOT treat a reference as unresolvable before completely
// parsing all documents provided to the implementation as possible parts of the
// OAD", and a schema still queued is one not parsed yet, so what one of them
// answers to has to be known before the first of them asks
auto register_schema(Walk &walk, const Fetched &entry) -> void {
  const auto frame{schema_frame_within(walk, entry)};
  // A document the schema walk reached is framed here and nowhere else, so what
  // this registers of it is charged. One a description named is framed again by
  // the walk that follows, which charges it there instead
  if (entry.closure_read) {
    charge(walk, frame->location_count());
  }

  reject_foreign_dialect(walk, entry, *frame);
  const auto dialect{openapi_dialect(entry.version)};
  // Every resource of the document answers for its own reading rather than for
  // the one at the top of it, as the keyword that settles a reading may sit on
  // any resource root
  frame->for_each_location(
      [&walk, &entry, &dialect](const auto, const auto &uri,
                                const auto &location) -> void {
        if (location.type != SchemaFrame::LocationType::Resource) {
          return;
        }

        const auto *held{
            sourcemeta::core::try_get(entry.document, location.pointer)};
        account(walk, uri, Reached::Schema, dialect,
                held == nullptr || dialect_settled(*held));
      });

  // What a Discriminator Object names is as much a dependency within a schema
  // this walk fetched as within the description itself, and the schema walk
  // below reads neither, as the keyword it sits under belongs to the OpenAPI
  // dialect rather than to JSON Schema
  if (!holds_discriminator(entry.document)) {
    return;
  }

  // The identifier a document declares is the one it goes by, which is what
  // every other edge out of it is reported from
  const auto &reported{frame->root().empty() ? entry.identifier
                                             : frame->root()};
  for (const auto &discriminator : sourcemeta::core::openapi_discriminators(
           entry.document, *frame, entry.identifier, walk.walker,
           walk.schema_resolver)) {
    if (sourcemeta::core::openapi_discriminator_lands(*frame, discriminator)) {
      continue;
    }

    report(walk, reported, discriminator.origin,
           document_of(discriminator.destination), OpenAPIObjectKind::Schema,
           Reached::Schema, entry.version);
  }
}

// What a Schema Object names is a schema rather than a description, so what it
// reaches in turn is for the schema walk to follow. That walk reports the whole
// closure below this document, so what comes back is passed straight on, and
// the one allowance covers both
auto read_closure(Walk &walk, const Fetched &entry) -> void {
  SchemaDependenciesOptions schemas;
  schemas.default_dialect = openapi_dialect(entry.version);
  schemas.default_id = entry.identifier;
  schemas.default_base = entry.identifier;
  schemas.max_locations = walk.remaining;
  schemas.accounted = [&walk](const std::string_view identifier) -> bool {
    return walk.accounted.contains(identifier);
  };
  // What the schema walk reached is accounted without a claim as to which
  // dialect it goes by, as the document at the far end of such a name is not
  // one this walk holds. Where it is reported below it is, and there the claim
  // is made from that document rather than from this one
  schemas.accounted_callback =
      [&walk](const std::string_view identifier) -> void {
    account(walk, identifier, Reached::Schema);
  };

  // The schema walk spends from the same allowance and reports running out
  // in its own terms, which the caller was promised in these
  try {
    charge(
        walk,
        schema_dependencies(
            entry.document, walk.walker,
            schema_resolver_within(walk, entry.identifier),
            [&walk, &entry](const SchemaDependency &dependency) -> void {
              report_to_caller(walk, {.origin = dependency.origin,
                                      .pointer = to_pointer(dependency.pointer),
                                      .destination = dependency.destination,
                                      .document = dependency.document,
                                      .expected = OpenAPIObjectKind::Schema});
              account(walk, dependency.destination, Reached::Schema,
                      openapi_dialect(entry.version),
                      dialect_settled(dependency.document));

              reject_foreign_declaration(entry, dependency.destination,
                                         dependency.document);

              // A Discriminator Object of this document is a
              // dependency that the walk reporting it reads nothing
              // of, so it is queued for that pass alone. Its own
              // closure is what that walk already reported, which is
              // why it is not read a second time
              if (is_traversable(dependency.destination) &&
                  holds_discriminator(dependency.document)) {
                walk.schemas.emplace_back(
                    Fetched{.identifier = JSON::String{dependency.destination},
                            .document = dependency.document,
                            .version = entry.version,
                            .closure_read = true});
              }
            },
            schemas));
  } catch (const SchemaFrameLimitError &) {
    throw OpenAPIFrameLimitError{walk.limit};
  }
}

} // namespace

auto openapi_dependencies(const JSON &document, const SchemaWalker &walker,
                          const SchemaResolver &schema_resolver,
                          const OpenAPIResolver &resolver,
                          const OpenAPIDependencyCallback &callback,
                          const OpenAPIDependencyOptions &options)
    -> std::uint64_t {
  assert(callback);

  const SchemaResolver guarded{resolver_within_allowance(schema_resolver)};
  Walk walk{.walker = walker,
            .schema_resolver = guarded,
            .resolver = resolver,
            .callback = callback,
            .accounted = {},
            .descriptions = {},
            .schemas = {},
            .deferred = {},
            .remaining = options.max_locations,
            .limit = options.max_locations};

  try {
    // OpenAPI 3.1.1 Section 4.3.3 resolves the component and tag names a
    // referenced document uses from the document the description begins at,
    // so the frame holding those names outlives the pass that reads it
    const auto entry_frame{
        frame_within(walk, document, options.default_base, nullptr)};
    charge(walk, registered(*entry_frame));
    if (!options.default_base.empty()) {
      account(walk, openapi_canonical_base(options.default_base),
              Reached::Description);
    }

    examine(walk, document, *entry_frame, entry_frame->base());

    // Reading a document can only ever add to what the walk accounts for, so a
    // document reached from one already read is reported from where it was
    // reached rather than from the top
    while (!walk.descriptions.empty()) {
      const auto held{std::move(walk.descriptions.front())};
      walk.descriptions.pop_front();
      const auto frame{frame_within(walk, held.document, held.identifier,
                                    entry_frame.get())};
      charge(walk, registered(*frame));
      examine(walk, held.document, *frame, frame->base());
    }

    // Registering runs ahead of reading rather than alongside it, so that
    // whatever one schema of the span answers to is known before another of
    // them asks. Reading one may reach further schemas, which are registered
    // in their turn before the next is read
    std::size_t registered_upto{0};
    std::size_t read_upto{0};
    // What one schema of the span reaches may answer to the name another of
    // them could not, and what it reaches is only known once it has been read,
    // so a name nothing answers for yet is left for a later pass. OpenAPI 3.2.1
    // Section 4.1.2.1 has an implementation "MUST NOT treat a reference as
    // unresolvable before completely parsing all documents provided to the
    // implementation as possible parts of the OAD", and a schema whose own
    // closure is unread is one of those documents
    std::vector<std::size_t> postponed;
    while (true) {
      while (registered_upto < walk.schemas.size()) {
        register_schema(walk, walk.schemas[registered_upto]);
        registered_upto += 1;
      }

      if (read_upto < walk.schemas.size()) {
        const auto index{read_upto};
        read_upto += 1;
        if (!walk.schemas[index].closure_read) {
          try {
            read_closure(walk, walk.schemas[index]);
          } catch (const SchemaResolutionError &) {
            postponed.push_back(index);
          }
        }

        continue;
      }

      if (postponed.empty()) {
        break;
      }

      std::vector<std::size_t> remaining;
      std::exception_ptr unresolved;
      for (const auto index : postponed) {
        try {
          read_closure(walk, walk.schemas[index]);
        } catch (const SchemaResolutionError &) {
          if (!unresolved) {
            unresolved = std::current_exception();
          }

          remaining.push_back(index);
        }
      }

      // A pass that settles none of them is one that never will, and the name
      // nothing answers for is reported in the terms it was raised in rather
      // than restated in these
      if (remaining.size() == postponed.size()) {
        std::rethrow_exception(unresolved);
      }

      postponed = std::move(remaining);
    }

    for (const auto &entry : walk.deferred) {
      const auto known{walk.accounted.find(entry.destination)};
      if (known == walk.accounted.cend()) {
        throw OpenAPIResolutionError{
            "Could not resolve the reference to an external document",
            entry.origin, entry.pointer, entry.destination};
      }

      reject_conflicting_kind(known->second, entry.reached, entry.origin,
                              entry.pointer, entry.destination);
      reject_two_readings(known->second, entry.reached, entry.dialect,
                          entry.origin, entry.pointer, entry.destination);
    }
  } catch (const ForeignLimitError &foreign) {
    std::rethrow_exception(foreign.error);
  }

  return options.max_locations - walk.remaining;
}

} // namespace sourcemeta::core
