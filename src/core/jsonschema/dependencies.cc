#include <sourcemeta/core/jsonschema.h>

#include <unordered_set> // std::unordered_set

static auto
dependencies_impl(std::unordered_set<sourcemeta::core::JSON::String> &traversed,
                  const sourcemeta::core::JSON &schema,
                  const sourcemeta::core::SchemaWalker &walker,
                  const sourcemeta::core::SchemaResolver &resolver,
                  const std::function<void(
                      const std::optional<sourcemeta::core::JSON::String> &,
                      const sourcemeta::core::Pointer &,
                      const sourcemeta::core::JSON::String &)> &callback,
                  const std::optional<std::string> &default_dialect,
                  const std::optional<std::string> &identifier,
                  const sourcemeta::core::SchemaFrame::Paths &paths) -> void {
  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(schema, walker, resolver, default_dialect, identifier, paths);

  if (identifier.has_value()) {
    traversed.emplace(identifier.value());
  }

  for (const auto &[key, reference] : frame.references()) {
    // We don't care about official meta-schemas
    if (key.second.back().to_property() == "$schema" &&
        sourcemeta::core::schema_official_resolver(reference.destination)
            .has_value()) {
      continue;
    }

    if (!frame.traverse(reference.destination).has_value() &&
        reference.base.has_value()) {
      // TODO: Avoid even considering official schemas? Otherwise the consumer
      // will get a ton of them that they have to discard anyway?
      callback(identifier, key.second, reference.base.value());
      if (!traversed.contains(reference.destination)) {
        // TODO: For Draft 7 and older we should also check directly resolving
        // `reference.destination`, as in those versions of JSON Schema you
        // _can_ have fragments in identifiers?
        const auto subschema{resolver(reference.base.value())};
        if (!subschema.has_value()) {
          throw sourcemeta::core::SchemaResolutionError(
              reference.base.value(),
              "Could not resolve the reference to an external schema");
        }

        // TODO: Re-implement without recursion
        dependencies_impl(traversed, subschema.value(), walker, resolver,
                          callback, default_dialect, reference.base.value(),
                          // We only want to apply custom paths at the top
                          {sourcemeta::core::empty_pointer});
      }
    }
  }
}

namespace sourcemeta::core {

auto dependencies(
    const JSON &schema, const SchemaWalker &walker,
    const SchemaResolver &resolver,
    const std::function<void(const std::optional<JSON::String> &,
                             const Pointer &, const JSON::String &)> &callback,
    const std::optional<std::string> &default_dialect,
    const std::optional<std::string> &default_id,
    const SchemaFrame::Paths &paths) -> void {
  std::unordered_set<JSON::String> traversed;
  const auto identifier{identify(schema, resolver,
                                 SchemaIdentificationStrategy::Strict,
                                 default_dialect, default_id)};
  return dependencies_impl(traversed, schema, walker, resolver, callback,
                           default_dialect, identifier, paths);
}

} // namespace sourcemeta::core
