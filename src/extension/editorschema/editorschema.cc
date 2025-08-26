#include <sourcemeta/core/editorschema.h>

#include <cassert> // assert

namespace sourcemeta::core {

auto for_editor(JSON &schema, const SchemaWalker &walker,
                const SchemaResolver &resolver,
                const std::optional<std::string> &default_dialect) -> void {
  // (1) Bring in all of the references
  bundle(schema, walker, resolver, default_dialect);

  // (2) Re-frame before changing anything
  SchemaFrame frame{SchemaFrame::Mode::References};
  frame.analyse(schema, walker, resolver, default_dialect);

  // (3) Remove all identifiers and anchors
  for (const auto &entry : frame.locations()) {
    if (entry.second.type != SchemaFrame::LocationType::Resource &&
        entry.second.type != SchemaFrame::LocationType::Subschema) {
      continue;
    }

    auto &subschema{get(schema, entry.second.pointer)};
    if (subschema.is_boolean()) {
      continue;
    }

    anonymize(subschema, entry.second.base_dialect);

    const auto vocabularies{frame.vocabularies(entry.second, resolver)};

    if (vocabularies.contains(
            "https://json-schema.org/draft/2020-12/vocab/core")) {
      subschema.erase("$vocabulary");
      subschema.erase("$anchor");
      subschema.erase("$dynamicAnchor");
    }

    if (vocabularies.contains(
            "https://json-schema.org/draft/2019-09/vocab/core")) {
      subschema.erase("$vocabulary");
      subschema.erase("$anchor");
      subschema.erase("$recursiveAnchor");
    }
  }

  // (4) Fix-up reference based on pointers from the root
  for (const auto &[key, reference] : frame.references()) {
    const auto result{frame.traverse(reference.destination)};
    if (result.has_value()) {
      set(schema, key.second,
          JSON{to_uri(result.value().get().pointer).recompose()});
    } else if (!key.second.empty() && key.second.back().is_property() &&
               key.second.back().to_property() != "$schema") {
      set(schema, key.second, JSON{reference.destination});
    }
  }
}

} // namespace sourcemeta::core
