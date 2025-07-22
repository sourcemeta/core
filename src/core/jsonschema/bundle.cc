#include <sourcemeta/core/jsonschema.h>

#include <cassert>       // assert
#include <sstream>       // std::ostringstream
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

#include <iostream> // TODO DEBUG

namespace {

auto find_externals(std::unordered_set<std::string> &output,
                    sourcemeta::core::SchemaFrame &frame,
                    const sourcemeta::core::JSON &subschema,
                    const sourcemeta::core::SchemaWalker &walker,
                    const sourcemeta::core::SchemaResolver &resolver,
                    const std::optional<std::string> &default_dialect,
                    const std::optional<std::string> &default_id,
                    const sourcemeta::core::SchemaFrame::Paths &paths,
                    const std::size_t depth = 0) -> void {
  // Keep in mind that the resulting frame does miss some information. For
  // example, when we recurse to framing embedded schemas, we will frame them
  // without keeping their new relationship to their parent (after embedding if
  // to the container location). However, that's fine for the purpose of this
  // function, given we don't pass the frame back to the caller
  if (depth == 0) {
    frame.analyse(
        subschema, walker, resolver, default_dialect, default_id,
        // We only want to frame in "wrapper" mode for the top level object
        paths);
  } else {
    frame.analyse(subschema, walker, resolver, default_dialect, default_id);
  }

  // Otherwise, given recursion, we would be modifying the
  // references list *while* looping on it
  // TODO: This is very slow!
  const auto references_copy = frame.references();
  for (const auto &[key, reference] : references_copy) {
    assert(!key.second.empty());
    assert(key.second.back().is_property());
    if (frame.traverse(reference.destination).has_value() ||

        // We don't want to bundle official schemas, as we can expect
        // virtually all implementations to understand them out of the box
        (key.second.back().to_property() == "$schema" &&
         sourcemeta::core::schema_official_resolver(reference.destination)
             .has_value())) {
      continue;
    }

    // If we can't find the destination but there is a base and we can
    // find base, then we are facing an unresolved fragment
    if (reference.base.has_value() &&
        frame.traverse(reference.base.value()).has_value()) {
      throw sourcemeta::core::SchemaReferenceError(
          reference.destination, key.second,
          "Could not resolve schema reference");
    }

    if (!reference.base.has_value()) {
      throw sourcemeta::core::SchemaReferenceError(
          reference.destination, key.second,
          "Could not resolve schema reference");
    }

    assert(reference.base.has_value());
    const auto identifier{reference.base.value()};
    const auto remote{resolver(identifier)};
    if (!remote.has_value()) {
      if (frame.traverse(identifier).has_value()) {
        throw sourcemeta::core::SchemaReferenceError(
            reference.destination, key.second,
            "Could not resolve schema reference");
      }

      throw sourcemeta::core::SchemaResolutionError(
          identifier, "Could not resolve the reference to an external schema");
    }

    if (!sourcemeta::core::is_schema(remote.value())) {
      throw sourcemeta::core::SchemaReferenceError(
          identifier, key.second,
          "The JSON document is not a valid JSON Schema");
    }

    const auto dialect{
        sourcemeta::core::dialect(remote.value(), default_dialect)};
    if (!dialect.has_value()) {
      throw sourcemeta::core::SchemaReferenceError(
          identifier, key.second,
          "The JSON document is not a valid JSON Schema");
    }

    output.emplace(identifier);

    find_externals(output, frame, remote.value(), walker, resolver,
                   default_dialect, identifier, paths, depth + 1);
  }
}

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

auto bundle_externals(sourcemeta::core::JSON &root,
                      const std::unordered_set<std::string> &externals,
                      const sourcemeta::core::Pointer &container,
                      const sourcemeta::core::SchemaResolver &resolver,
                      const std::optional<std::string> &default_dialect)
    -> void {
  for (const auto &external : externals) {
    auto subschema{resolver(external)};
    assert(subschema.has_value());
    if (subschema.value().is_object()) {
      // TODO: If externals report base_dialect information, we can
      // probably speed up the re-identification quite a bit
      // Always insert an identifier, as a schema might refer to another
      // schema using another URI (i.e. due to relying on HTTP re-directions,
      // etc)
      sourcemeta::core::reidentify(subschema.value(), external, resolver,
                                   default_dialect);
    }

    embed_schema(root, container, external, std::move(subschema).value());
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
  SchemaFrame frame{SchemaFrame::Mode::References};
  std::unordered_set<std::string> externals;
  find_externals(externals, frame, schema, walker, resolver, default_dialect,
                 default_id, paths);

  if (externals.empty()) {
    return;
  } else if (default_container.has_value()) {
    // This is undefined behavior
    assert(!default_container.value().empty());
    bundle_externals(schema, externals, default_container.value(), resolver,
                     default_dialect);
    return;
  }

  const auto vocabularies{
      sourcemeta::core::vocabularies(schema, resolver, default_dialect)};
  if (vocabularies.contains(
          "https://json-schema.org/draft/2020-12/vocab/core") ||
      vocabularies.contains(
          "https://json-schema.org/draft/2019-09/vocab/core")) {
    bundle_externals(schema, externals, {"$defs"}, resolver, default_dialect);
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
    bundle_externals(schema, externals, {"definitions"}, resolver,
                     default_dialect);
    return;
  }

  // We don't attempt to bundle on dialects where we
  // don't know where to put the embedded schemas
  throw SchemaError(
      "Could not determine how to perform bundling in this dialect");
}

auto bundle(const JSON &schema, const SchemaWalker &walker,
            const SchemaResolver &resolver,
            const std::optional<std::string> &default_dialect,
            const std::optional<std::string> &default_id,
            const std::optional<Pointer> &default_container,
            const SchemaFrame::Paths &paths) -> JSON {
  JSON copy = schema;
  bundle(copy, walker, resolver, default_dialect, default_id, default_container,
         paths);
  return copy;
}

} // namespace sourcemeta::core
