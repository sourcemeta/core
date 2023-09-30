#include <sourcemeta/jsontoolkit/jsonschema_walker.h>

// Because standard "contains()" is introduced in C++20
namespace {
auto contains(const std::map<std::string, bool> &map, const std::string &key)
    -> bool {
  return map.find(key) != map.end();
}
} // namespace

// A stub walker that doesn't walk
auto sourcemeta::jsontoolkit::schema_walker_none(
    const std::string &, const std::map<std::string, bool> &)
    -> sourcemeta::jsontoolkit::SchemaWalkerStrategy {
  return sourcemeta::jsontoolkit::SchemaWalkerStrategy::None;
}

// TODO: Extend this default walker to recognize as many official
// JSON Schema vocabularies as possible.
auto sourcemeta::jsontoolkit::default_schema_walker(
    const std::string &keyword, const std::map<std::string, bool> &vocabularies)
    -> sourcemeta::jsontoolkit::SchemaWalkerStrategy {
  if (::contains(vocabularies,
                 "https://json-schema.org/draft/2020-12/vocab/core") &&
      keyword == "$defs") {
    return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Members;
  }

  if (::contains(vocabularies,
                 "https://json-schema.org/draft/2020-12/vocab/content") &&
      keyword == "contentSchema") {
    return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Value;
  }

  if (::contains(vocabularies,
                 "https://json-schema.org/draft/2020-12/vocab/unevaluated")) {
    if (keyword == "unevaluatedProperties" || keyword == "unevaluatedItems") {
      return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Value;
    }
  }

  if (::contains(vocabularies,
                 "https://json-schema.org/draft/2020-12/vocab/applicator")) {
    if (keyword == "dependentSchemas" || keyword == "properties" ||
        keyword == "patternProperties") {
      return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Members;
    }

    if (keyword == "allOf" || keyword == "anyOf" || keyword == "oneOf" ||
        keyword == "prefixItems") {
      return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Elements;
    }

    if (keyword == "not" || keyword == "if" || keyword == "then" ||
        keyword == "else" || keyword == "items" || keyword == "contains" ||
        keyword == "additionalProperties" || keyword == "propertyNames") {
      return sourcemeta::jsontoolkit::SchemaWalkerStrategy::Value;
    }
  }

  return sourcemeta::jsontoolkit::SchemaWalkerStrategy::None;
}
