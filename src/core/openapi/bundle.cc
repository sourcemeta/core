#include <sourcemeta/core/openapi.h>

#include "discriminator.h"
#include "document.h"
#include "helpers.h"

#include <cassert>  // assert
#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint64_t
#include <map>      // std::map
#include <optional> // std::optional
#include <set>      // std::set
#include <string>   // std::to_string
#include <utility>  // std::move
#include <vector>   // std::vector

namespace {

// Every walk and every frame that bundling builds spends from the same
// allowance, as how much of it bundling ends up needing is a function of what
// the resolvers hand back rather than of the document the caller passed in. A
// walk reads a document in full before anything charges for it, so what this
// bounds is how many oversized documents are read rather than whether one is
auto charge(std::uint64_t &remaining, const std::size_t locations) -> void {
  assert(locations <= remaining);
  remaining -= locations;
}

// A reference that bundling has to make whole: the member that spells it, what
// it names, and what the position it sits in expects to find there
struct OpenAPIPending {
  sourcemeta::core::Pointer origin;
  sourcemeta::core::JSON::String destination;
  sourcemeta::core::OpenAPIObjectKind expected;
  // Whether a Discriminator Object mapping is what names it. A `$ref` of a
  // Schema Object is one that whatever reads inside a Schema Object follows
  // for itself, and Section 4.8.25 makes a mapping an annotation that nothing
  // there has any account of, so the two part company wherever the shell
  // cannot reach what is named
  bool mapping{false};
  // Whether a Security Requirement Object is what names it. OpenAPI
  // Specification 3.2.1, Section 4.30 lets one name a Security Scheme Object
  // "by URI", and that URI is the member the scopes sit under rather than a
  // value of its own, so making it whole renames a member rather than writing
  // to one. Nothing else this brings in is spelled that way
  bool requirement{false};
  // What the reference resolves against, which for everything but a Schema
  // Object is the base of the document that makes it. OpenAPI Specification
  // 3.1.1, Section 4.3 has a relative reference inside a Schema Object use
  // "the nearest parent `$id` as a Base URI" instead
  sourcemeta::core::JSON::String scope;
};

// What the description reaches for and does not hold. A reference that names
// the document being read and lands nowhere is left alone, as OpenAPI
// Specification 3.1.1, Section 4.8.23 holds a `$ref` to the form of a URI and
// says nothing about it having to resolve, so there is nothing to fetch here
// and nothing this specification lets us report
auto pending(const sourcemeta::core::OpenAPIWalk &walk)
    -> std::vector<OpenAPIPending> {
  std::vector<OpenAPIPending> result;
  for (const auto &entry : walk.references) {
    if (walk.locations.contains(entry.second.destination) ||
        sourcemeta::core::openapi_document_uri(entry.second.destination) ==
            walk.base) {
      continue;
    }

    result.push_back({.origin = entry.second.origin,
                      .destination = entry.second.destination,
                      .expected = entry.second.expected,
                      .scope = walk.base});
  }

  // And so does what a Security Requirement Object names by URI, which OpenAPI
  // Specification 3.2.1, Section 4.30 admits alongside the name of a component:
  // "The name used for each property MUST either correspond to a security
  // scheme declared in the Security Schemes under the Components Object, or be
  // the URI of a Security Scheme Object". The frame keeps these apart from the
  // references above only because a single one of those Objects may name
  // several schemes, which is more than one entry keyed by where it sits
  for (const auto &entry : walk.security_references) {
    if (walk.locations.contains(entry.second.destination) ||
        sourcemeta::core::openapi_document_uri(entry.second.destination) ==
            walk.base) {
      continue;
    }

    result.push_back({.origin = entry.second.origin,
                      .destination = entry.second.destination,
                      .expected = entry.second.expected,
                      .requirement = true,
                      .scope = walk.base});
  }

  return result;
}

// Where the Object that bundling embeds begins. A target that sits within a
// component of its own document is embedded as that whole component, so that a
// reference to it and a reference deeper into it land on one copy rather than
// on two. OpenAPI Specification 3.1.1, Section 4.8.7 puts every component at
// the same depth, which is what makes the first three tokens the whole test
constexpr std::size_t COMPONENT_DEPTH{3};

// Whether a pointer names a component of the document it belongs to, which is
// what the Components Object holds at a fixed depth of its own
auto is_component(const sourcemeta::core::Pointer &pointer) -> bool {
  return pointer.size() == COMPONENT_DEPTH && pointer.at(0).is_property() &&
         pointer.at(0).to_property() == "components" &&
         pointer.at(1).is_property();
}

auto promote(const sourcemeta::core::Pointer &target)
    -> sourcemeta::core::Pointer {
  if (target.size() < COMPONENT_DEPTH ||
      !target.starts_with(sourcemeta::core::EMPTY_POINTER, "components")) {
    return target;
  }

  return target.slice(0, COMPONENT_DEPTH);
}

// Where the Path Item Object holding an Operation Object sits. OpenAPI
// Specification 3.1.1, Section 4.8.9 puts an Operation Object directly under
// the Path Item Object that holds it, and 3.2.1, Section 4.9 adds
// `additionalOperations`, "a map of additional operations keyed by HTTP
// method", which puts a map of its own between the two. So which place holds
// it is what the walk recorded rather than a fixed number of steps up
auto path_item_of(const sourcemeta::core::OpenAPIWalk &remote,
                  const sourcemeta::core::Pointer &operation)
    -> sourcemeta::core::Pointer {
  auto prefix{operation};
  while (!prefix.empty()) {
    prefix = prefix.initial();
    const auto location{remote.locations.find(
        sourcemeta::core::openapi_location_uri(remote.base, prefix))};
    if (location != remote.locations.cend() &&
        location->second.type ==
            sourcemeta::core::OpenAPIObjectKind::PathItem) {
      return prefix;
    }
  }

  return operation.initial();
}

// Which Components Object member an embedded Object goes under. A component
// keeps the member its own document filed it under, which is what that
// document decided the Object is, rather than the member that the kind of the
// reference reaching it would take. The two differ whenever a reference names
// a place within a component rather than the component itself
auto container_of(const sourcemeta::core::Pointer &origin,
                  const sourcemeta::core::OpenAPIObjectKind expected)
    -> sourcemeta::core::JSON::StringView {
  if (is_component(origin)) {
    return origin.at(1).to_property();
  }

  return sourcemeta::core::openapi_component_container(expected);
}

// A reference that leads back to the document being bundled into names a place
// that document already holds, so it is written as a fragment of it rather
// than as the URI that document answers to
auto rebase(const sourcemeta::core::JSON::String &destination,
            const sourcemeta::core::JSON::String &base)
    -> sourcemeta::core::JSON::String {
  if (sourcemeta::core::openapi_document_uri(destination) != base) {
    return destination;
  }

  return destination.substr(base.size());
}

// A Schema Object carries the base of the document it was written in, and
// nothing below the root of a document may declare one of its own. So every
// reference inside a schema that moves is written back as whatever it resolved
// to where it came from, which a schema frame of that document is what settles
auto absolutize_schemas(sourcemeta::core::JSON &value,
                        const sourcemeta::core::JSON &remote_document,
                        const sourcemeta::core::OpenAPIWalk &remote,
                        const sourcemeta::core::Pointer &origin,
                        const sourcemeta::core::Pointer &landing,
                        const sourcemeta::core::JSON::String &dialect,
                        const sourcemeta::core::SchemaWalker &walker,
                        const sourcemeta::core::SchemaResolver &resolver,
                        std::uint64_t &remaining) -> void {
  sourcemeta::core::SchemaFrame::Paths paths;
  for (const auto &entry : remote.locations) {
    if (entry.second.type != sourcemeta::core::OpenAPIObjectKind::Schema ||
        !entry.second.pointer.starts_with(origin)) {
      continue;
    }

    paths.push_back(sourcemeta::core::to_weak_pointer(entry.second.pointer));

    // OpenAPI Specification 3.1.1, Section 4.8.24.1 scopes `jsonSchemaDialect`
    // to "all Schema Objects contained within an OAS document", so a schema
    // that says nothing about the dialect it is written against is read under
    // whichever one the document holding it declares. Moving it to a document
    // that settles on another dialect is what makes it say so itself
    if (remote.dialect == dialect) {
      continue;
    }

    auto &schema{sourcemeta::core::get(
        value, (entry.second.pointer).resolve_from(origin))};
    // Section 4.8.24 lets a Schema Object be a boolean, which declares nothing
    if (schema.is_object() && !schema.defines("$schema")) {
      schema.assign("$schema", sourcemeta::core::JSON{remote.dialect});
    }
  }

  if (paths.empty()) {
    return;
  }

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References,
      remote_document,
      walker,
      resolver,
      remote.dialect,
      "",
      sourcemeta::core::SchemaFrame::IdentifierMode::Additional,
      paths,
      remote.base,
      remaining};
  charge(remaining, frame.location_count());

  // What a moved schema names by a mapping is resolved where it came from,
  // just as what it names by a reference is
  for (const auto &discriminator : sourcemeta::core::openapi_discriminators(
           remote_document, frame, remote.base, walker, resolver)) {
    if (!discriminator.origin.starts_with(origin)) {
      continue;
    }

    const auto target{frame.traverse(discriminator.destination)};
    if (target.has_value() && target.value().get().pointer.starts_with(
                                  sourcemeta::core::to_weak_pointer(origin))) {
      continue;
    }

    const auto held{(discriminator.origin).resolve_from(origin)};
    const auto *written{sourcemeta::core::try_get(value, held)};
    if (written == nullptr || !written->is_string()) {
      continue;
    }

    // Section 4.3.3 resolves a name against the Components Object of the entry
    // document wherever the Discriminator Object naming it sits, so moving the
    // schema leaves it naming exactly what it named
    if (sourcemeta::core::openapi_is_component_key(written->to_string())) {
      continue;
    }

    if (written->to_string() != discriminator.destination) {
      sourcemeta::core::set(value, held,
                            sourcemeta::core::JSON{discriminator.destination});
    }
  }

  const auto moved{sourcemeta::core::to_weak_pointer(origin)};

  // OpenAPI Specification 3.1.1, Section 4.6: "Relative references in Schema
  // Objects, including any that appear as `$id` values, use the nearest parent
  // `$id` as a Base URI". So a schema that names itself relatively names
  // something else once it sits in another document, and writing back what it
  // resolved to where it came from is what keeps its identity its own
  frame.for_each_resource([&value, &origin,
                           &moved](const auto &identifier,
                                   const auto &location) -> void {
    if (!location.pointer.starts_with(moved)) {
      return;
    }

    auto &schema{sourcemeta::core::get(
        value,
        (sourcemeta::core::to_pointer(location.pointer)).resolve_from(origin))};
    // Which keyword carries that identity is the dialect's to say rather than
    // whichever one the latest of them happens to use, and a schema that
    // declares none is left without one rather than given one it never had
    if (schema.is_object() &&
        schema.defines(sourcemeta::core::schema_identifier_keyword(
            location.base_dialect))) {
      sourcemeta::core::schema_reidentify(schema, identifier,
                                          location.base_dialect);
    }
  });

  // Section 4.8.24 puts an External Documentation Object on a Schema Object,
  // and Section 4.8.11 makes its `url` "The URI for the target documentation".
  // The frame of the shell never reaches inside a Schema Object, so the one
  // URI a schema carries that is neither a reference nor an identifier is
  // written back here
  frame.for_each_location([&value, &origin, &moved, &frame, &walker,
                           &resolver](const auto, const auto &,
                                      const auto &location) -> void {
    if (!location.pointer.starts_with(moved)) {
      return;
    }

    // Section 4.8.24 lists `externalDocs` among the keywords that the dialect
    // this specification publishes is made of, and Section 4.8.24.5 has a
    // Schema Object read under whichever dialect it declares. So a schema
    // written against one that leaves the keyword out holds no External
    // Documentation Object at all, and what it spells there names nothing that
    // moving could break
    const auto &vocabularies{frame.vocabularies(location, resolver)};
    if (walker("externalDocs", vocabularies).type ==
        sourcemeta::core::SchemaKeywordType::Unknown) {
      return;
    }

    const auto held{
        (sourcemeta::core::to_pointer(location.pointer))
            .resolve_from(origin)
            .concat(sourcemeta::core::Pointer{"externalDocs", "url"})};
    const auto *written{sourcemeta::core::try_get(value, held)};
    if (written == nullptr || !written->is_string()) {
      return;
    }

    const auto absolute{sourcemeta::core::openapi_resolve_uri(
        written->to_string(), sourcemeta::core::JSON::String{location.base})};
    if (absolute.has_value()) {
      sourcemeta::core::set(
          value, held, sourcemeta::core::JSON{absolute.value().recompose()});
    }
  });

  frame.for_each_reference_from(
      moved,
      [&value, &origin, &moved, &landing, &frame, &remote](
          const auto type, const auto &pointer, const auto &reference) -> void {
        // What a dynamic reference names is settled where the schema is
        // evaluated rather than where it sits, so a bare name is left exactly
        // as the description wrote it. Which document that name is looked for
        // in is settled on load though, which JSON Schema Section 8.2.3.2
        // calls out: "Resolved against the current URI base, it produces the
        // URI used as the starting point for runtime resolution. This initial
        // resolution is safe to perform on schema load". So one that names a
        // document of its own is written back like any other reference, and
        // only the name it carries goes on being settled later
        if (type != sourcemeta::core::SchemaReferenceType::Static &&
            reference.original.starts_with('#')) {
          return;
        }

        // Anything already spelled out as what it resolves to is left exactly
        // as the description wrote it
        if (reference.original == reference.destination) {
          return;
        }

        // And so is anything that names a place inside what moves, which is
        // what an anchor that a schema declares and names itself by comes to.
        // Both of them moving together is what keeps the one naming the other,
        // and resolving it against the document it came from is what would
        // break that.
        //
        // A pointer is not like a name that way. RFC 6901 reads one from the
        // root of a document, so one that leads into what moves leads there
        // by a route that moving is exactly what changes. Such a reference is
        // written out as the route to wherever this is headed instead
        const auto target{frame.traverse(reference.destination)};
        if (target.has_value() &&
            target.value().get().pointer.starts_with(moved)) {
          // Which root a pointer counts from is what the reference resolved
          // against rather than how it is spelled. One that resolved against
          // the document is the one moving takes somewhere else. One that
          // resolved against an identifier a schema declares for itself counts
          // from that schema, which moves along whole, and a name is not a
          // pointer at all
          if (sourcemeta::core::openapi_document_uri(reference.destination) !=
                  remote.base ||
              !reference.fragment.has_value() ||
              !reference.fragment.value().starts_with('/')) {
            return;
          }

          const auto tail{
              sourcemeta::core::to_pointer(target.value().get().pointer)
                  .resolve_from(origin)};
          sourcemeta::core::set(
              value,
              (sourcemeta::core::to_pointer(pointer)).resolve_from(origin),
              sourcemeta::core::JSON{
                  sourcemeta::core::to_uri(landing.concat(tail)).recompose()});
          return;
        }

        sourcemeta::core::set(
            value, (sourcemeta::core::to_pointer(pointer)).resolve_from(origin),
            sourcemeta::core::JSON{reference.destination});
      });
}

// What to call an embedded Object. The name it went by in the document it came
// from is the one a reader would look for, and Section 4.8.7 constrains every
// component key, so a name that does not hold up there is replaced rather than
// carried over. Taking a name the entry document already uses would change
// what an implicit connection resolves to, which is why this only takes a free
// one
// Whatever the Components Object already holds under a member, which is what a
// name has to be free of before anything is written there
auto openapi_component_container_of(
    const sourcemeta::core::JSON &document,
    const sourcemeta::core::JSON::StringView container)
    -> const sourcemeta::core::JSON * {
  const auto *components{document.try_at("components")};
  if (components == nullptr || !components->is_object()) {
    return nullptr;
  }

  const auto *entries{
      components->try_at(sourcemeta::core::JSON::String{container})};
  return entries == nullptr || !entries->is_object() ? nullptr : entries;
}

// Section 4.8.7 admits nothing but letters, digits, dots, hyphens and
// underscores into a component key, so what does not hold up to that is
// dropped rather than carried over, and a name left with nothing is replaced
auto sanitise(const sourcemeta::core::JSON::StringView candidate)
    -> sourcemeta::core::JSON::String {
  sourcemeta::core::JSON::String result;
  for (const auto character : candidate) {
    if (sourcemeta::core::openapi_is_component_key(
            sourcemeta::core::JSON::StringView{&character, 1})) {
      result.push_back(character);
    }
  }

  return result.empty() ? sourcemeta::core::JSON::String{"Bundled"} : result;
}

// Taking a name that the description already gives a meaning to would change
// what an implicit connection resolves to, which is why this only ever takes
// a free one
auto vacant(const sourcemeta::core::JSON &entries,
            sourcemeta::core::JSON::String candidate)
    -> sourcemeta::core::JSON::String {
  while (entries.defines(candidate)) {
    candidate.append("_");
  }

  return candidate;
}

auto component_name(const sourcemeta::core::JSON &document,
                    const sourcemeta::core::JSON::StringView container,
                    const sourcemeta::core::JSON::StringView source,
                    const sourcemeta::core::Pointer &target,
                    const sourcemeta::core::OpenAPIBundleOptions::Namer &namer)
    -> sourcemeta::core::JSON::String {
  sourcemeta::core::JSON::String candidate{"Bundled"};
  if (namer) {
    candidate = namer(source, container);
  } else if (!target.empty()) {
    // RFC 6901 Section 3 makes every reference token a string, and one of
    // digits reads back as an array index, which spells out to a name the
    // Components Object takes as it stands
    const auto &token{target.back()};
    if (token.is_property()) {
      candidate = token.to_property();
    } else if (token.is_index()) {
      candidate =
          sourcemeta::core::JSON::String{std::to_string(token.to_index())};
    }
  }

  const auto *entries{openapi_component_container_of(document, container)};
  candidate = sanitise(candidate);
  return entries == nullptr ? candidate
                            : vacant(*entries, std::move(candidate));
}

auto embed(sourcemeta::core::JSON &document,
           const sourcemeta::core::JSON::StringView container,
           const sourcemeta::core::JSON::String &name,
           sourcemeta::core::JSON &&value) -> sourcemeta::core::Pointer {
  const sourcemeta::core::JSON::String container_key{container};
  document.assign_if_missing("components",
                             sourcemeta::core::JSON::make_object());
  auto &components{document.at("components")};
  components.assign_if_missing(container_key,
                               sourcemeta::core::JSON::make_object());
  components.at(container_key).assign(name, std::move(value));

  sourcemeta::core::Pointer result;
  result.push_back(sourcemeta::core::JSON::String{"components"});
  result.push_back(container_key);
  result.push_back(name);
  return result;
}

// A Schema Object is addressed by the identifier it declares rather than by
// where it sits, so the name it goes under is a label rather than something a
// reference resolves through, and the last segment of that identifier is the
// part of it a reader looks for
auto schema_name(const sourcemeta::core::JSON &schemas,
                 const sourcemeta::core::JSON::StringView identifier,
                 const sourcemeta::core::OpenAPIBundleOptions::Namer &namer)
    -> sourcemeta::core::JSON::String {
  return vacant(
      schemas,
      sanitise(namer ? sourcemeta::core::JSON::StringView{namer(identifier,
                                                                "schemas")}
                     : identifier.substr(identifier.find_last_of('/') + 1)));
}

// Bundling what sits inside a Schema Object is JSON Schema's to do, and the
// Components Object holds the one member this specification reserves for
// schemas. Section 4.8.7 constrains the keys of that member, which the
// identifiers that bundling names an embedded schema by do not hold up to, so
// each of them is renamed once it has landed. Nothing resolves through those
// keys, so renaming reaches nothing else
auto bundle_schemas(sourcemeta::core::JSON &document,
                    const sourcemeta::core::OpenAPIWalk &walk,
                    const sourcemeta::core::SchemaWalker &walker,
                    const sourcemeta::core::SchemaResolver &resolver,
                    const sourcemeta::core::JSON::String &base,
                    const std::uint64_t remaining,
                    const sourcemeta::core::OpenAPIBundleOptions &options)
    -> void {
  const auto &callback{options.callback};
  const auto &namer{options.namer};
  sourcemeta::core::SchemaFrame::Paths paths;
  for (const auto &entry : walk.locations) {
    if (entry.second.type == sourcemeta::core::OpenAPIObjectKind::Schema) {
      paths.push_back(sourcemeta::core::to_weak_pointer(entry.second.pointer));
    }
  }

  if (paths.empty()) {
    return;
  }

  sourcemeta::core::Pointer container;
  container.push_back(sourcemeta::core::JSON::String{"components"});
  container.push_back(sourcemeta::core::JSON::String{"schemas"});

  // What each schema was resolved by, beside where it landed. Bundling picks a
  // key that is free rather than one that matches, so reading the identifier
  // back off that pointer is only right until two of them collide
  std::vector<
      std::pair<sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      landed;
  sourcemeta::core::SchemaBundleOptions schemas_options;
  // Section 4.8.7 holds the `schemas` member of the Components Object to
  // "reusable Schema Objects", and the dialect a schema is written against is
  // not one of those. So what a `$schema` names is left where it is rather
  // than carried into the description, whether or not this specification's own
  // dialect is one that JSON Schema counts as its own
  schemas_options.mode =
      sourcemeta::core::SchemaBundleOptions::Mode::References;
  schemas_options.default_container = container;
  schemas_options.paths = paths;
  schemas_options.default_base = base;
  schemas_options.max_locations = remaining;
  schemas_options.callback =
      [&landed](const std::string_view identifier,
                const sourcemeta::core::WeakPointer &location) -> void {
    landed.emplace_back(sourcemeta::core::JSON::String{identifier},
                        sourcemeta::core::to_pointer(location));
  };

  // Section 4.8.24 scopes what the OpenAPI Object sets to the Schema Objects
  // "contained within an OAS document", and says of the rest: "For standalone
  // JSON Schema documents that do not set `$schema` [...] the dialect SHOULD
  // be assumed to be the OAS dialect". A document a resolver hands back is one
  // of those, so it says so itself before anything reads it under the dialect
  // this description happens to have settled on
  const sourcemeta::core::JSON::String standalone{
      sourcemeta::core::openapi_dialect(walk.version)};
  const auto standalone_resolver{
      [&resolver, &standalone](const std::string_view identifier)
          -> sourcemeta::core::SchemaResolverResult {
        auto result{resolver(identifier)};
        if (!result.has_value() || !result.value().is_object() ||
            result.value().defines("$schema")) {
          return result;
        }

        auto owned{std::move(result).to_owned()};
        owned.assign("$schema", sourcemeta::core::JSON{standalone});
        return owned;
      }};

  sourcemeta::core::schema_bundle(document, walker, standalone_resolver,
                                  walk.dialect, "", schemas_options);
  if (landed.empty()) {
    return;
  }

  auto &schemas{sourcemeta::core::get(document, container)};
  for (const auto &entry : landed) {
    const auto &identifier{entry.first};
    const auto &key{entry.second.back().to_property()};
    const auto name{schema_name(schemas, identifier, namer)};
    schemas.rename(key, sourcemeta::core::JSON::String{name});
    if (callback) {
      callback(identifier, container.concat(name));
    }
  }
}

// What the Schema Objects of a description reach for and it does not hold.
// Section 4.3.1 lists a Schema Object `$ref` among the fields that connect the
// documents of a description, so one of these may name a Schema Object that
// another of those documents declares, which only the shell can reach
auto schema_pending(const sourcemeta::core::JSON &document,
                    const sourcemeta::core::OpenAPIWalk &walk,
                    const sourcemeta::core::SchemaWalker &walker,
                    const sourcemeta::core::SchemaResolver &resolver,
                    std::vector<OpenAPIPending> &result,
                    std::uint64_t &remaining) -> void {
  sourcemeta::core::SchemaFrame::Paths paths;
  for (const auto &entry : walk.locations) {
    if (entry.second.type == sourcemeta::core::OpenAPIObjectKind::Schema) {
      paths.push_back(sourcemeta::core::to_weak_pointer(entry.second.pointer));
    }
  }

  if (paths.empty()) {
    return;
  }

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References,
      document,
      walker,
      resolver,
      walk.dialect,
      "",
      sourcemeta::core::SchemaFrame::IdentifierMode::Additional,
      paths,
      walk.base,
      remaining};
  charge(remaining, frame.location_count());

  frame.for_each_reference([&frame, &walk,
                            &result](const auto type, const auto &pointer,
                                     const auto &reference) -> void {
    // A dynamic reference names an anchor to be settled where it is
    // evaluated, and a `$schema` names the dialect a schema is written
    // against rather than a part of the description
    if (type != sourcemeta::core::SchemaReferenceType::Static ||
        (!pointer.empty() && pointer.back().is_property() &&
         pointer.back().to_property() == "$schema")) {
      return;
    }

    if (frame.traverse(reference.destination).has_value() ||
        sourcemeta::core::openapi_document_uri(reference.destination) ==
            walk.base) {
      return;
    }

    // What a reference resolves against is the base of the schema that holds
    // it, which is the nearest identifier an enclosing one declares rather
    // than anything the reference says about itself
    const auto enclosing{frame.traverse(pointer.initial())};
    result.push_back(
        {.origin = sourcemeta::core::to_pointer(pointer),
         .destination = reference.destination,
         .expected = sourcemeta::core::OpenAPIObjectKind::Schema,
         .scope =
             enclosing.has_value()
                 ? sourcemeta::core::JSON::String{enclosing.value().get().base}
                 : walk.base});
  });

  // And so does what a Discriminator Object names by URI, which the frame
  // above does not read because the keyword it sits under belongs to the
  // OpenAPI dialect rather than to JSON Schema
  for (auto &discriminator : sourcemeta::core::openapi_discriminators(
           document, frame, walk.base, walker, resolver)) {
    if (sourcemeta::core::openapi_discriminator_lands(frame, discriminator) ||
        sourcemeta::core::openapi_document_uri(discriminator.destination) ==
            walk.base) {
      continue;
    }

    result.push_back({.origin = std::move(discriminator.origin),
                      .destination = std::move(discriminator.destination),
                      .expected = sourcemeta::core::OpenAPIObjectKind::Schema,
                      .mapping = true,
                      .scope = std::move(discriminator.scope)});
  }
}

// Whether what a reference names is the kind that the position it sits in
// expects. A Schema Object reference is the one that may name a place inside
// an Object rather than an Object this specification types, as Section 4.6
// reads the fragment as a JSON Pointer and a Schema Object may sit within a
// component of any kind. So what answers for it is the nearest enclosing
// place that framing did record, which has to be a Schema Object itself
auto lands(const sourcemeta::core::OpenAPIWalk &remote,
           const sourcemeta::core::JSON::String &identifier,
           const sourcemeta::core::Pointer &target,
           const sourcemeta::core::OpenAPIObjectKind expected) -> bool {
  auto prefix{target};
  while (true) {
    const auto location{remote.locations.find(
        sourcemeta::core::openapi_location_uri(identifier, prefix))};
    if (location != remote.locations.cend()) {
      // A Reference Object stands in for whatever the position expects, so it
      // answers for every kind rather than for one of them
      return location->second.type == expected ||
             location->second.type ==
                 sourcemeta::core::OpenAPIObjectKind::Reference;
    }

    if (expected != sourcemeta::core::OpenAPIObjectKind::Schema ||
        prefix.empty()) {
      return false;
    }

    prefix = prefix.initial();
  }
}

// Which Object of the remote document a reference names, as a pointer. OpenAPI
// Specification 3.1.1, Section 4.6: "If the representation of the referenced
// document is JSON or YAML, then the fragment identifier SHOULD be interpreted
// as a JSON-Pointer as per RFC6901", and a fragment shaped like anything else
// names nothing this can embed
auto target_of(const sourcemeta::core::JSON::String &destination)
    -> std::optional<sourcemeta::core::Pointer> {
  const auto pointer{sourcemeta::core::fragment_to_pointer(
      sourcemeta::core::URI{destination})};
  // An empty fragment names the root of the document, which is the document
  // itself just as naming no fragment at all is
  if (pointer.has_value() && pointer.value().empty()) {
    return std::nullopt;
  }

  return pointer;
}

} // namespace

namespace sourcemeta::core {

namespace {

// Nothing below the root of a document may declare a base of its own, so an
// Object taken out of one goes on reading whatever it carries against
// whichever document it ends up in. Writing back what each of those resolved
// to where it came from is what keeps them naming the same places once that
// Object sits somewhere else
auto absolutize(JSON &value, const OpenAPIWalk &remote, const Pointer &origin,
                const JSON::String &base) -> void {
  for (const auto &entry : remote.references) {
    if (entry.second.origin.starts_with(origin)) {
      set(value, (entry.second.origin).resolve_from(origin),
          JSON{rebase(entry.second.destination, base)});
    }
  }

  // OpenAPI Specification 3.2.1, Section 4.30 has a Security Requirement
  // Object name a Security Scheme Object by the URI of one, which is the one
  // connection of a description spelled as the member that holds the scopes
  // rather than as a value, so this renames rather than writes
  for (const auto &entry : remote.security_references) {
    if (!entry.second.origin.starts_with(origin)) {
      continue;
    }

    auto rewritten{rebase(entry.second.destination, base)};
    if (rewritten == entry.second.original) {
      continue;
    }

    auto &requirement{
        get(value, (entry.second.origin).resolve_from(origin).initial())};
    // Section 4.30 says nothing against two names of one Object leading to one
    // scheme, and reading such an Object is no trouble. Writing one back out
    // is what cannot keep both, as the single name they come to is a key that
    // holds one list of scopes rather than two
    const auto *taken{requirement.try_at(rewritten)};
    if (taken != nullptr && *taken != requirement.at(entry.second.original)) {
      throw OpenAPIError{remote.base, entry.second.origin,
                         "A Security Requirement Object that names one "
                         "security scheme twice over cannot keep a list of "
                         "scopes for each of them"};
    }

    requirement.rename(entry.second.original, std::move(rewritten));
  }

  // And so is every other URI it carries, which the frame does not record as a
  // reference because nothing about the description hangs off where it leads
  for (const auto &entry : remote.locations) {
    if (!entry.second.pointer.starts_with(origin)) {
      continue;
    }

    const auto relative{(entry.second.pointer).resolve_from(origin)};

    // Section 4.8.5 makes a Server Object URL a template rather than a URI
    // reference, and Section 4.3 has a relative one name a place "relative to
    // the location where the document containing the Server Object is being
    // served", so it resolves as a template against the document it was
    // written in
    if (entry.second.type == OpenAPIObjectKind::Server) {
      const auto held{relative.concat(JSON::String{"url"})};
      const auto *written{try_get(value, held)};
      if (written != nullptr && written->is_string()) {
        // Section 4.5.2.1 works this very case through: a document retrieved
        // from one place and naming itself another resolves what it says of
        // the API against where it was found rather than against the name it
        // gave itself
        auto address{
            openapi_resolve_server_url(written->to_string(), remote.retrieval)};
        if (!address.has_value()) {
          throw OpenAPIError{
              remote.base, entry.second.pointer.concat(JSON::String{"url"}),
              "A Server Object URL template that leaves what it is "
              "relative to for its variables to decide cannot be read "
              "from another document"};
        }

        set(value, held, JSON{std::move(address.value())});
      }

      continue;
    }

    for (const auto &field : openapi_embedded_uri_fields(entry.second.type)) {
      const auto held{relative.concat(JSON::String{field})};
      const auto *written{try_get(value, held)};
      if (written == nullptr || !written->is_string()) {
        continue;
      }

      const auto absolute{
          openapi_reference_target(written->to_string(), remote)};
      if (absolute.has_value()) {
        set(value, held, JSON{absolute.value().recompose()});
      }
    }
  }
}

// Lift one Object out of the document that declares it and into the entry
// document, writing back everything it carries that would otherwise go on
// resolving against a base that is no longer its own. Where it lands is what
// every reference that reaches it is then written to name
auto adopt(sourcemeta::core::JSON &document,
           const sourcemeta::core::JSON &remote_document,
           const sourcemeta::core::OpenAPIWalk &remote,
           const sourcemeta::core::Pointer &origin,
           const sourcemeta::core::JSON::StringView container,
           const sourcemeta::core::JSON::String &key,
           const sourcemeta::core::JSON::String &base,
           const sourcemeta::core::JSON::String &dialect,
           const sourcemeta::core::SchemaWalker &walker,
           const sourcemeta::core::SchemaResolver &schema_resolver,
           const sourcemeta::core::OpenAPIBundleOptions &options,
           std::uint64_t &remaining) -> sourcemeta::core::Pointer {
  auto value{*try_get(remote_document, origin)};
  absolutize(value, remote, origin, base);

  // Where this is headed is settled before anything it carries is written
  // back, as a reference of its own that names a place within it by a pointer
  // from the root of a document names that pointer rather than the place, and
  // the pointer is what moving changes
  const auto name{
      component_name(document, container, key, origin, options.namer)};
  sourcemeta::core::Pointer landing;
  landing.push_back(sourcemeta::core::JSON::String{"components"});
  landing.push_back(sourcemeta::core::JSON::String{container});
  landing.push_back(name);

  absolutize_schemas(value, remote_document, remote, origin, landing, dialect,
                     walker, schema_resolver, remaining);

  const auto landed{embed(document, container, name, std::move(value))};
  if (options.callback) {
    options.callback(key, landed);
  }

  return landed;
}

// OpenAPI Specification 3.1.1, Section 4.3.3: "It is RECOMMENDED to consider
// all Operation Objects from all parsed documents when resolving any Link
// Object `operationId`. This requires parsing all referenced documents prior
// to determining an `operationId` to be unresolvable". Every document the
// description spans is one bundling has read, so an identifier that named an
// operation of one of them named something before bundling and has to go on
// naming it afterwards. Nothing points at that operation for its own sake, so
// the Path Item holding it is brought along the way one named by an
// `operationRef` is
auto adopt_operations(JSON &document, const OpenAPIWalk &walk,
                      const std::map<JSON::String, JSON> &documents,
                      const std::map<JSON::String, OpenAPIWalk> &walks,
                      std::map<JSON::String, Pointer> &bundled,
                      const JSON::String &base, const SchemaWalker &walker,
                      const SchemaResolver &schema_resolver,
                      const OpenAPIBundleOptions &options,
                      std::uint64_t &remaining) -> bool {
  bool changed{false};
  for (const auto &entry : walk.operation_id_links) {
    if (walk.operation_ids.contains(entry.second)) {
      continue;
    }

    for (const auto &other : walks) {
      const auto named{other.second.operation_ids.find(entry.second)};
      if (named == other.second.operation_ids.cend()) {
        continue;
      }

      const auto operation{other.second.locations.find(named->second)};
      if (operation == other.second.locations.cend()) {
        continue;
      }

      // A Path Item Object is what holds an Operation Object, and the Object
      // the Components Object has a home for
      const auto origin{
          promote(path_item_of(other.second, operation->second.pointer))};
      const auto key{openapi_location_uri(other.first, origin)};
      if (bundled.contains(key)) {
        break;
      }

      bundled.emplace(
          key,
          adopt(document, documents.at(other.first), other.second, origin,
                container_of(origin, OpenAPIObjectKind::PathItem), key, base,
                walk.dialect, walker, schema_resolver, options, remaining));
      changed = true;
      break;
    }
  }

  return changed;
}

// Where a document declares the Tag Object of a given name, which the walk
// records as a place of its own rather than by the name it goes under
auto tag_of(const JSON &document, const OpenAPIWalk &walk,
            const JSON::String &name) -> std::optional<Pointer> {
  for (const auto &entry : walk.locations) {
    if (entry.second.type != OpenAPIObjectKind::Tag) {
      continue;
    }

    const auto *value{try_get(document, entry.second.pointer)};
    if (value == nullptr || !value->is_object()) {
      continue;
    }

    const auto *declared{value->try_at("name")};
    if (declared != nullptr && declared->is_string() &&
        declared->to_string() == name) {
      return entry.second.pointer;
    }
  }

  return std::nullopt;
}

// OpenAPI Specification 3.2.1, Section 4.22, of a Tag Object's `parent`: "The
// `name` of a tag that this tag is nested under. The named tag MUST exist in
// the API description". A description is every document it spans rather than
// the entry one alone, so that tag may be one another document declares, and
// Section 4.1 only ever puts a Tag Object at the root of a document. Bundling
// moves what the Components Object holds and leaves every root where it is, so
// a name that the description satisfied has to travel along to go on being
// satisfied by what bundling produces. Every document that holds one is one
// bundling has read, just as for a Link Object `operationId`, as a name is not
// something there is anywhere to go and fetch
auto adopt_tags(JSON &document, const OpenAPIWalk &walk,
                const std::map<JSON::String, JSON> &documents,
                const std::map<JSON::String, OpenAPIWalk> &walks,
                const JSON::String &base, const OpenAPIBundleOptions &options)
    -> bool {
  bool changed{false};
  // What this pass has already brought in. The names the walk holds are the
  // ones it read before any of them travelled, and Section 4.1 has "Each tag
  // name in the list MUST be unique", so two tags nested under one that only
  // another document declares bring it along once between them
  std::set<JSON::String> adopted;
  for (const auto &entry : walk.tag_parents) {
    if (walk.tag_names.contains(entry.second.second) ||
        adopted.contains(entry.second.second)) {
      continue;
    }

    for (const auto &other : walks) {
      const auto &remote_document{documents.at(other.first)};
      const auto declared{
          tag_of(remote_document, other.second, entry.second.second)};
      if (!declared.has_value()) {
        continue;
      }

      auto value{*try_get(remote_document, declared.value())};
      absolutize(value, other.second, declared.value(), base);
      document.assign_if_missing("tags", JSON::make_array());
      auto &tags{document.at("tags")};
      if (options.callback) {
        options.callback(
            openapi_location_uri(other.second.base, declared.value()),
            Pointer{JSON::String{"tags"}, tags.size()});
      }

      tags.push_back(std::move(value));
      adopted.insert(entry.second.second);
      changed = true;
      break;
    }
  }

  return changed;
}

// Section 4.3 counts "the URI form of the Discriminator Object `mapping`
// field" among the fields that identify the referenced elements of a
// description, so one that names a schema of another document names a part of
// that description. The shell reaches such a schema wherever an OpenAPI
// document holds it. One that stands on its own is left here instead, as a
// mapping is an annotation to whatever reads inside a Schema Object and
// nothing there follows it. It goes under the member Section 4.8.7 reserves
// for schemas, keeping the identifier it answers to, so the mapping goes on
// naming what it always named
auto adopt_mappings(
    JSON &document,
    const std::map<JSON::String, std::vector<OpenAPIPending>> &deferred,
    std::set<JSON::String> &adopted, const JSON::String &base,
    const SchemaResolver &schema_resolver, const JSON::StringView dialect,
    const OpenAPIBundleOptions &options) -> bool {
  bool changed{false};
  for (const auto &entry : deferred) {
    const auto identifier{openapi_document_uri(entry.first)};
    // One schema answers for a mapping once. Bringing it in again would not
    // make a mapping that still does not land any likelier to, and settling is
    // what tells bundling it has nothing left to do
    if (adopted.contains(identifier)) {
      continue;
    }

    auto resolved{schema_resolver(identifier)};
    if (!resolved.has_value()) {
      continue;
    }

    auto schema{std::move(resolved).to_owned()};
    // Section 4.8.24: "The empty schema [...] MAY be represented by the
    // boolean value `true` and a schema which allows no instance to validate
    // MAY be represented by the boolean value `false`". Neither carries a
    // keyword, so neither can be made to answer to the identifier it was found
    // under, and what names it has to name where it lands instead
    const auto identifies{schema.is_object()};
    if (identifies) {
      // Section 4.8.24 has a standalone document that says nothing of the
      // dialect it is written against read under the one this specification
      // publishes, and an identifier of its own is what keeps a mapping naming
      // it once it sits somewhere else
      if (!schema.defines("$schema")) {
        schema.assign("$schema", JSON{dialect});
      }

      // Which keyword carries that identity is what the dialect says rather
      // than what 2020-12 happens to call it, so this is left to whatever
      // knows the dialect a schema declares
      schema_reidentify(schema, identifier, schema_resolver,
                        JSON::String{dialect});
    }

    adopted.insert(identifier);

    const auto *schemas{openapi_component_container_of(document, "schemas"sv)};
    const auto name{
        schema_name(schemas == nullptr ? JSON::make_object() : *schemas,
                    identifier, options.namer)};
    const auto landed{embed(document, "schemas"sv, name, std::move(schema))};
    if (options.callback) {
      options.callback(identifier, landed);
    }

    // One that says nothing of itself is only findable by where it went, so
    // every mapping that named it is written out to name that instead. Which
    // root that is counted from is what the mapping resolved against, as
    // Section 4.6 has one inside a Schema Object that declares an identifier
    // count from there rather than from the document
    if (!identifies) {
      for (const auto &pending : entry.second) {
        set(document, pending.origin,
            JSON{pending.scope == base ? to_uri(landed).recompose()
                                       : openapi_location_uri(base, landed)});
      }
    }

    changed = true;
  }

  return changed;
}

auto bundle_internal(JSON &document, const SchemaWalker &walker,
                     const SchemaResolver &schema_resolver,
                     const OpenAPIResolver &resolver,
                     const OpenAPIBundleOptions &options,
                     std::uint64_t &remaining) -> void {
  // Where the document was retrieved from, which is only what it answers to
  // until it says otherwise. OpenAPI Specification 3.2.1, Section 4.1 lets one
  // name itself with `$self`, "which also serves as its base URI", so what
  // every place of this document is named by is what its own analysis settled
  // on rather than what the caller handed over
  const auto retrieval{openapi_canonical_base(options.default_base)};
  // Where each component that bundling embedded ended up, keyed by the place
  // it came from. A description that reaches for one place twice embeds it
  // once, which is also what keeps a cycle of documents from going round
  std::map<JSON::String, Pointer> bundled;
  // A document that more than one reference reaches is read once. What it
  // holds cannot change between one reference and the next, and reading it
  // again would charge the allowance twice for the same thing
  std::map<JSON::String, JSON> documents;
  std::map<JSON::String, OpenAPIWalk> walks;
  // The documents that the shell cannot reach, which only a Schema Object may
  // name and which it is left free to go on naming
  std::set<JSON::String> unavailable;
  // And of those, the schemas that a Discriminator Object mapping is what
  // names, which nothing but this brings in, along with the ones it already did
  std::map<JSON::String, std::vector<OpenAPIPending>> deferred;
  std::set<JSON::String> adopted;
  // The names the description gave before bundling moved anything. Section
  // 4.1.2.3 resolves the names a referenced document uses from the entry
  // document, and bundling embedding a Security Scheme Object puts a name
  // there that the description never had. Letting that answer for a document
  // read afterwards would decide, by nothing but how many references away it
  // sits, that an operation requires a credential its own document never named
  OpenAPIWalk names;
  bool named{false};

  while (true) {
    const auto walk{openapi_analyse(document, retrieval, remaining)};
    const auto &base{walk.base};
    if (!named) {
      names.security_schemes = walk.security_schemes;
      names.tags = walk.tags;
      names.tag_names = walk.tag_names;
      named = true;
    }

    charge(remaining, walk.locations.size());
    auto unresolved{pending(walk)};
    // A Schema Object may name one that another document of the description
    // declares, which the shell is what reaches rather than anything that
    // reads inside a Schema Object
    schema_pending(document, walk, walker, schema_resolver, unresolved,
                   remaining);

    bool changed{false};
    for (const auto &reference : unresolved) {
      // Only the shell knows which documents an OpenAPI Description spans. A
      // Schema Object naming something this does not reach is left exactly as
      // it was written, for whatever reads inside one to resolve
      const auto names_a_schema{reference.expected ==
                                OpenAPIObjectKind::Schema};

      // A document that holds an OpenAPI Description is never what a reference
      // from the shell expects to find, so one naming a whole document has
      // landed on the wrong kind of thing
      const auto target{target_of(reference.destination)};
      if (!target.has_value()) {
        if (names_a_schema) {
          if (reference.mapping) {
            deferred[reference.destination].push_back(reference);
          }

          continue;
        }

        throw OpenAPIReferenceError{base, reference.origin,
                                    reference.destination,
                                    "This reference must name a place within "
                                    "the document it points at"};
      }

      const auto identifier{openapi_document_uri(reference.destination)};

      if (names_a_schema && unavailable.contains(identifier)) {
        continue;
      }

      if (!documents.contains(identifier)) {
        auto resolved{resolver(identifier)};
        if (!resolved.has_value()) {
          if (names_a_schema) {
            unavailable.insert(identifier);
            if (reference.mapping) {
              deferred[reference.destination].push_back(reference);
            }

            continue;
          }

          throw OpenAPIResolutionError{
              base, reference.origin, identifier,
              "Could not resolve the reference to an external document"};
        }

        auto candidate{std::move(resolved).to_owned()};
        // OpenAPI Specification 3.2.1, Section 4.1.2: "all documents in an
        // OAD MUST have either an OpenAPI Object or a Schema Object at the
        // root, and MUST be parsed as complete documents". A Schema Object
        // at the root is what a Schema Object reference may name, and that
        // document is one a JSON Schema implementation reads rather than
        // this. No revision of 3.1 carries that sentence, and what it carries
        // instead is the choice Section 4.3.1 leaves open, so declining to
        // read a document of some other shape is one rule for both revisions
        if (!openapi_is_document(candidate)) {
          if (names_a_schema) {
            unavailable.insert(identifier);
            if (reference.mapping) {
              deferred[reference.destination].push_back(reference);
            }

            continue;
          }

          throw OpenAPIReferenceError{base, reference.origin, identifier,
                                      "This reference must name a document "
                                      "that holds an OpenAPI Description"};
        }

        // Bundling ends in one document, and Section 4.1 has that document
        // declare one revision, so everything it holds has to be what that
        // revision can express. Section 1.1 leaves no room to assume an older
        // one always can: "Occasionally, non-backwards compatible changes may
        // be made in `minor` versions of the OAS where impact is believed to
        // be low relative to the benefit provided". Neither direction is safe
        // to take on faith, as 3.2 both adds fields that 3.1 has no way of
        // spelling and holds what 3.1 already spells to rules 3.1 never had,
        // so a description that spans revisions is turned down rather than
        // merged. What the patch component says is no part of this, as
        // Section 1.1 makes a revision the `major`.`minor` pair alone
        const auto revision{openapi_version(candidate)};
        if (revision.has_value() && revision.value() != walk.version) {
          throw OpenAPIReferenceError{
              base, reference.origin, identifier,
              "This reference must name a document of the same OpenAPI "
              "Specification revision"};
        }

        // The walk keeps a view into the document it read, so the document
        // takes its place before anything walks it
        const auto &held{
            documents.emplace(identifier, std::move(candidate)).first->second};
        auto analysis{openapi_analyse(held, identifier, remaining, &names)};
        charge(remaining, analysis.locations.size());
        walks.emplace(identifier, std::move(analysis));
      }

      const auto &remote_document{documents.at(identifier)};
      const auto &remote{walks.at(identifier)};
      // What a reference names is held to the kind the position it sits in
      // expects, however many references reach the Object that holds it. A
      // second one that named nothing would otherwise be written out as a
      // place the result does not hold
      if (!lands(remote, remote.base, target.value(), reference.expected)) {
        throw OpenAPIReferenceError{
            base, reference.origin, reference.destination,
            "This reference must name an Object of the kind that the "
            "position it sits in expects"};
      }

      // An Operation Object is the one kind the Components Object has no home
      // for, so what gets embedded is the Path Item Object holding it and the
      // reference goes on reaching its operation through that
      const auto names_an_operation{reference.expected ==
                                    OpenAPIObjectKind::Operation};
      const auto origin{promote(names_an_operation
                                    ? path_item_of(remote, target.value())
                                    : target.value())};
      const auto container{
          container_of(origin, names_an_operation ? OpenAPIObjectKind::PathItem
                                                  : reference.expected)};
      // Every kind that a reference position expects has a home of its own
      // once an Operation Object is reached through the Path Item holding it
      assert(!container.empty());

      // Where a document was retrieved from is how a resolver is asked for it,
      // and what it answers to is its own to say. OpenAPI Specification 3.2.1,
      // Section 4.1.1 lets a document declare the latter and requires the two
      // to be told apart: "references MUST use the target document's `$self`
      // URI if the `$self` field is present". So every place of it is named by
      // the base its own analysis settled on rather than by where it was found
      const auto key{openapi_location_uri(remote.base, origin)};

      if (!bundled.contains(key)) {
        bundled.emplace(key, adopt(document, remote_document, remote, origin,
                                   container, key, base, walk.dialect, walker,
                                   schema_resolver, options, remaining));
      }

      // The reference is rewritten as a fragment of the document it now sits
      // in, rather than as the URI that document answers to, so that bundling
      // leaves behind an output that keeps working wherever it is moved to
      const auto landed{target.value().rebase(origin, bundled.at(key))};

      // What a Security Requirement Object names is the member the scopes sit
      // under, so what makes it whole is renaming that member to whatever the
      // scheme is called once it sits here, which carries the scopes across
      // untouched. Section 4.30 then reads the result as a component name
      // rather than as a URI: "Property names that are identical to a
      // component name under the Components Object MUST be treated as a
      // component name", and the name was taken free of that Object for it
      if (reference.requirement) {
        auto &requirement{get(document, reference.origin.initial())};
        const auto &spelled{reference.origin.back().to_property()};
        JSON::String name{landed.back().to_property()};
        const auto *taken{requirement.try_at(name)};
        if (taken != nullptr && *taken != requirement.at(spelled)) {
          throw OpenAPIError{base, reference.origin,
                             "A Security Requirement Object that names one "
                             "security scheme twice over cannot keep a list of "
                             "scopes for each of them"};
        }

        requirement.rename(spelled, std::move(name));
        changed = true;
        continue;
      }

      // A reference that resolves against the document it sits in is written
      // as a fragment of it, which is what keeps what bundling produces
      // working wherever it is moved to. One that resolves against something
      // else, which is a Schema Object that declares an identifier of its own,
      // would name a place within that identifier instead, so it cannot be
      // written that way
      if (reference.scope == base) {
        set(document, reference.origin, JSON{to_uri(landed).recompose()});
        changed = true;
        continue;
      }

      // What such a reference names instead is what the schema it leads to
      // says of itself, wherever that schema says anything. JSON Schema
      // Section 9.3.1 has a reference to an embedded resource "resolve to a
      // schema using the `$id` of an embedded Schema Resource", and an
      // identifier holds wherever the description ends up while a place in a
      // document does not. Only a schema that declares none leaves the
      // document itself as the sole way to name where it leads
      const auto *reached{try_get(document, landed)};
      const auto *identity{reached == nullptr || !reached->is_object()
                               ? nullptr
                               : reached->try_at("$id")};
      set(document, reference.origin,
          identity != nullptr && identity->is_string()
              ? JSON{identity->to_string()}
              : JSON{openapi_location_uri(base, landed)});
      changed = true;
    }

    // A pass that leaves the document as it found it is one that has nothing
    // left to bring in, which counts a reference left for a JSON Schema
    // implementation as nothing
    if (!changed) {
      // What the shell reaches is settled before this, as an operation is
      // brought in for the sake of a name rather than of a reference and only
      // the documents a reference reached are ones to look through
      if (adopt_operations(document, walk, documents, walks, bundled, base,
                           walker, schema_resolver, options, remaining)) {
        continue;
      }

      // And so is the tag a Tag Object is nested under, which is a name the
      // description settles rather than a reference to go and follow
      if (adopt_tags(document, walk, documents, walks, base, options)) {
        continue;
      }

      // And so is what a mapping names, which is settled after the references
      // are, as bringing one schema in may be what lets the next be read
      if (adopt_mappings(document, deferred, adopted, base, schema_resolver,
                         openapi_dialect(walk.version), options)) {
        deferred.clear();
        continue;
      }

      // What is left is one document that holds every OpenAPI document the
      // description spanned, as the only ones bundling leaves out are the ones
      // a Schema Object may name, which hold no Tag Object and no Operation
      // Object to name. So the names that Section 4.3.3 has resolve across the
      // whole of a description are ones there is now an answer for, and
      // leaving a description that has none to be turned down by whoever
      // frames it next would be to hand back a bundle that does not describe
      // anything
      openapi_check_operation_id_links(walk, walk.locations);
      openapi_check_tag_parents(walk, walk.locations, true);
      // And so are the path parameters a templated path corresponds to, which
      // the projection is what settles. What it works out is of no use here,
      // as bundling moves what a description holds rather than reporting what
      // it exposes, but a description that cannot be projected is one this
      // would otherwise hand back for the next reader to turn down
      [[maybe_unused]] const auto operations{openapi_project(walk)};

      bundle_schemas(document, walk, walker, schema_resolver, base, remaining,
                     options);
      return;
    }
  }
}

} // namespace

auto openapi_bundle(JSON &document, const SchemaWalker &walker,
                    const SchemaResolver &schema_resolver,
                    const OpenAPIResolver &resolver,
                    const OpenAPIBundleOptions &options) -> void {
  auto remaining{options.max_locations};
  try {
    bundle_internal(document, walker, schema_resolver, resolver, options,
                    remaining);
  } catch (const OpenAPIFrameLimitError &) {
    throw OpenAPIBundleLimitError{options.max_locations};
  } catch (const SchemaFrameLimitError &) {
    // Every frame spends from what is left rather than from the whole, so the
    // one that ran out reports what it was handed. The caller set the
    // allowance for the operation, so that is what the operation reports back
    throw OpenAPIBundleLimitError{options.max_locations};
  }
}

auto openapi_bundle(const JSON &document, const SchemaWalker &walker,
                    const SchemaResolver &schema_resolver,
                    const OpenAPIResolver &resolver,
                    const OpenAPIBundleOptions &options) -> JSON {
  JSON copy{document};
  openapi_bundle(copy, walker, schema_resolver, resolver, options);
  return copy;
}

} // namespace sourcemeta::core
