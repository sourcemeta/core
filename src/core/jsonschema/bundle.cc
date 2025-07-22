#include <sourcemeta/core/jsonschema.h>

#include <cassert>       // assert
#include <sstream>       // std::ostringstream
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

namespace {

auto embed_schema(sourcemeta::core::JSON &root,
                  const sourcemeta::core::Pointer &container,
                  const std::string &identifier,
                  sourcemeta::core::JSON &&target) -> void {
  auto *current{&root};
  for (const auto &token : container) {
    if (token.is_property()) {
      current->assign_if_missing(token.to_property(),
                                 sourcemeta::core::JSON::make_object());
      current = &current->at(token.to_property());
    } else {
      assert(current->is_array() && current->size() >= token.to_index());
      current = &current->at(token.to_index());
    }
  }

  if (!current->is_object()) {
    throw sourcemeta::core::SchemaError(
        "Could not bundle to a container path that is not an object");
  }

  std::ostringstream key;
  key << identifier;
  // Ensure we get a definitions entry that does not exist
  while (current->defines(key.str())) {
    key << "/x";
  }

  current->assign(key.str(), std::move(target));
}

auto bundle_externals(
    sourcemeta::core::JSON &schema, const sourcemeta::core::Pointer &container,
    const sourcemeta::core::SchemaResolver &resolver,
    const std::unordered_set<sourcemeta::core::JSON::String> &externals,
    const std::optional<std::string> &default_dialect) -> void {
  for (const auto &identifier : externals) {
    auto subschema{resolver(identifier)};
    // If the dependency scanning was successful, then all external references
    // were found
    assert(subschema.has_value());

    if (!sourcemeta::core::is_schema(subschema.value())) {
      throw sourcemeta::core::SchemaResolutionError(
          identifier, "The JSON document is not a valid JSON Schema");
    }

    if (subschema.value().is_object()) {
      // Always insert an identifier, as a schema might refer to another schema
      // using another URI (i.e. due to relying on HTTP re-directions, etc)
      sourcemeta::core::reidentify(subschema.value(), identifier, resolver,
                                   default_dialect);
    }

    embed_schema(schema, container, identifier, std::move(subschema).value());
  }
}

} // namespace

namespace sourcemeta::core {

auto bundle(sourcemeta::core::JSON &schema, const SchemaWalker &walker,
            const SchemaResolver &resolver,
            const std::optional<std::string> &default_dialect,
            const std::optional<std::string> &default_id,
            const std::optional<Pointer> &default_container,
            const SchemaFrame::Paths &paths) -> void {
  // TODO: Add a bundle overload that just blindly takes the external references
  // to embed
  std::unordered_set<JSON::String> externals;
  dependencies(
      schema, walker, resolver,
      [&externals](const auto &, const auto &, const auto &dependency) {
        // TODO: This means we do bundle `$ref` to official meta-schemas
        // which apparently we still need because of Blaze?
        externals.emplace(dependency);
      },
      default_dialect, default_id, paths);

  if (externals.empty()) {
    return;
  } else if (default_container.has_value()) {
    // This is undefined behavior
    assert(!default_container.value().empty());
    bundle_externals(schema, default_container.value(), resolver, externals,
                     default_dialect);
    return;
  }

  const auto vocabularies{
      sourcemeta::core::vocabularies(schema, resolver, default_dialect)};
  if (vocabularies.contains(
          "https://json-schema.org/draft/2020-12/vocab/core") ||
      vocabularies.contains(
          "https://json-schema.org/draft/2019-09/vocab/core")) {
    bundle_externals(schema, {"$defs"}, resolver, externals, default_dialect);
    return;
  } else if (vocabularies.contains("http://json-schema.org/draft-07/schema#") ||
             vocabularies.contains(
                 "http://json-schema.org/draft-07/hyper-schema#") ||
             vocabularies.contains("http://json-schema.org/draft-06/schema#") ||
             vocabularies.contains(
                 "http://json-schema.org/draft-06/hyper-schema#") ||
             vocabularies.contains("http://json-schema.org/draft-04/schema#") ||
             vocabularies.contains(
                 "http://json-schema.org/draft-04/hyper-schema#")) {
    bundle_externals(schema, {"definitions"}, resolver, externals,
                     default_dialect);
    return;
  } else {
    // We don't attempt to bundle on dialects where we
    // don't know where to put the embedded schemas
    throw sourcemeta::core::SchemaError(
        "Could not determine how to perform bundling in this dialect");
  }
}

auto bundle(const sourcemeta::core::JSON &schema, const SchemaWalker &walker,
            const SchemaResolver &resolver,
            const std::optional<std::string> &default_dialect,
            const std::optional<std::string> &default_id,
            const std::optional<Pointer> &default_container,
            const SchemaFrame::Paths &paths) -> sourcemeta::core::JSON {
  sourcemeta::core::JSON copy = schema;
  bundle(copy, walker, resolver, default_dialect, default_id, default_container,
         paths);
  return copy;
}

} // namespace sourcemeta::core
