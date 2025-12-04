#include <sourcemeta/core/jsonschema.h>

#include <initializer_list> // std::initializer_list
#include <unordered_set>    // std::unordered_set

template <typename T>
auto make_set(std::initializer_list<T> types) -> std::unordered_set<T> {
  return {types};
}

using Known = sourcemeta::core::Vocabularies::Known;

auto sourcemeta::core::schema_official_walker(
    std::string_view keyword,
    const sourcemeta::core::Vocabularies &vocabularies)
    -> sourcemeta::core::SchemaWalkerResult {
#define TYPES(...) make_set({__VA_ARGS__})
#define WALK(vocabulary, _keyword, _types, strategy, ...)                      \
  if (vocabularies.contains(vocabulary) && keyword == _keyword)                \
    return {sourcemeta::core::SchemaKeywordType::strategy,                     \
            vocabulary,                                                        \
            {__VA_ARGS__},                                                     \
            _types};

#define WALK_ANY(vocabulary_1, vocabulary_2, _keyword, types, strategy, ...)   \
  WALK(vocabulary_1, _keyword, types, strategy, __VA_ARGS__)                   \
  WALK(vocabulary_2, _keyword, types, strategy, __VA_ARGS__)

#define WALK_MAYBE_DEPENDENT(vocabulary, _keyword, types, strategy,            \
                             dependent_vocabulary, ...)                        \
  if (vocabularies.contains(dependent_vocabulary)) {                           \
    WALK(vocabulary, _keyword, types, strategy, __VA_ARGS__)                   \
  } else {                                                                     \
    WALK(vocabulary, _keyword, types, strategy)                                \
  }
  // 2020-12
  WALK(Known::JSON_Schema_2020_12_Core, "$id", {}, Other)
  WALK(Known::JSON_Schema_2020_12_Core, "$schema", {}, Other)
  WALK(Known::JSON_Schema_2020_12_Core, "$ref", {}, Reference)
  WALK(Known::JSON_Schema_2020_12_Core, "$defs", {}, LocationMembers)
  // JSON Schema still defines this for backwards-compatibility
  // See https://json-schema.org/draft/2020-12/schema
  WALK(Known::JSON_Schema_2020_12_Core, "definitions", {}, LocationMembers)
  WALK(Known::JSON_Schema_2020_12_Core, "$comment", {}, Comment)
  WALK(Known::JSON_Schema_2020_12_Core, "$anchor", {}, Other)
  WALK(Known::JSON_Schema_2020_12_Core, "$vocabulary", {}, Other)
  WALK(Known::JSON_Schema_2020_12_Core, "$dynamicRef", {}, Reference)
  WALK(Known::JSON_Schema_2020_12_Core, "$dynamicAnchor", {}, Other)
  WALK(Known::JSON_Schema_2020_12_Applicator, "oneOf", {},
       ApplicatorElementsInPlaceSome)
  WALK(Known::JSON_Schema_2020_12_Applicator, "anyOf", {},
       ApplicatorElementsInPlaceSome)
  WALK(Known::JSON_Schema_2020_12_Applicator, "allOf", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_2020_12_Applicator, "if", {},
       ApplicatorValueInPlaceMaybe)
  WALK(Known::JSON_Schema_2020_12_Applicator, "then", {},
       ApplicatorValueInPlaceMaybe, "if")
  WALK(Known::JSON_Schema_2020_12_Applicator, "else", {},
       ApplicatorValueInPlaceMaybe, "if")
  WALK(Known::JSON_Schema_2020_12_Applicator, "not", {},
       ApplicatorValueInPlaceNegate)
  // For the purpose of compiler optimizations
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2020_12_Applicator, "properties",
                       TYPES(JSON::Type::Object),
                       ApplicatorMembersTraversePropertyStatic,
                       Known::JSON_Schema_2020_12_Validation, "required")
  WALK(Known::JSON_Schema_2020_12_Applicator, "additionalProperties",
       TYPES(JSON::Type::Object), ApplicatorValueTraverseSomeProperty,
       "properties", "patternProperties")
  WALK(Known::JSON_Schema_2020_12_Applicator, "patternProperties",
       TYPES(JSON::Type::Object), ApplicatorMembersTraversePropertyRegex)
  WALK(Known::JSON_Schema_2020_12_Applicator, "propertyNames",
       TYPES(JSON::Type::Object), ApplicatorValueTraverseAnyPropertyKey)
  WALK(Known::JSON_Schema_2020_12_Applicator, "dependentSchemas",
       TYPES(JSON::Type::Object), ApplicatorMembersInPlaceSome)
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2020_12_Applicator, "contains",
                       TYPES(JSON::Type::Array), ApplicatorValueTraverseAnyItem,
                       Known::JSON_Schema_2020_12_Validation, "minContains",
                       "maxContains")
  WALK(Known::JSON_Schema_2020_12_Applicator, "items", TYPES(JSON::Type::Array),
       ApplicatorValueTraverseSomeItem, "prefixItems")
  WALK(Known::JSON_Schema_2020_12_Applicator, "prefixItems",
       TYPES(JSON::Type::Array), ApplicatorElementsTraverseItem)
  // For the purpose of compiler optimizations
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2020_12_Validation, "type", {},
                       Assertion, Known::JSON_Schema_2020_12_Applicator,
                       "properties")
  WALK(Known::JSON_Schema_2020_12_Validation, "enum", {}, Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "const", {}, Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "maxLength",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "minLength",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "pattern",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "exclusiveMinimum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "multipleOf",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "maximum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion, "type")
  WALK(Known::JSON_Schema_2020_12_Validation, "exclusiveMaximum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "minimum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion, "type")
  WALK(Known::JSON_Schema_2020_12_Validation, "dependentRequired",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "minProperties",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "maxProperties",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "required",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "maxItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "minItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "uniqueItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "minContains",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2020_12_Validation, "maxContains",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "title", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "description", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "writeOnly", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "readOnly", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "examples", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "default", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Meta_Data, "deprecated", {}, Annotation)
  WALK(Known::JSON_Schema_2020_12_Format_Annotation, "format",
       TYPES(JSON::Type::String), Annotation)
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2020_12_Unevaluated,
                       "unevaluatedProperties", TYPES(JSON::Type::Object),
                       ApplicatorValueTraverseSomeProperty,
                       Known::JSON_Schema_2020_12_Applicator, "properties",
                       "patternProperties", "additionalProperties")
  WALK_MAYBE_DEPENDENT(
      Known::JSON_Schema_2020_12_Unevaluated, "unevaluatedItems",
      TYPES(JSON::Type::Array), ApplicatorValueTraverseSomeItem,
      Known::JSON_Schema_2020_12_Applicator, "prefixItems", "items", "contains")
  WALK(Known::JSON_Schema_2020_12_Content, "contentSchema",
       TYPES(JSON::Type::String), ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2020_12_Content, "contentMediaType",
       TYPES(JSON::Type::String), Annotation)
  WALK(Known::JSON_Schema_2020_12_Content, "contentEncoding",
       TYPES(JSON::Type::String), Annotation)
  WALK(Known::JSON_Schema_2020_12_Format_Assertion, "format",
       TYPES(JSON::Type::String), Assertion)

  // 2019-09
  WALK(Known::JSON_Schema_2019_09_Core, "$id", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Core, "$schema", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Core, "$ref", {}, Reference)
  WALK(Known::JSON_Schema_2019_09_Core, "$defs", {}, LocationMembers)
  // JSON Schema still defines this for backwards-compatibility
  // See https://json-schema.org/draft/2019-09/schema
  WALK(Known::JSON_Schema_2019_09_Core, "definitions", {}, LocationMembers)
  WALK(Known::JSON_Schema_2019_09_Core, "$comment", {}, Comment)
  WALK(Known::JSON_Schema_2019_09_Core, "$anchor", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Core, "$vocabulary", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Core, "$recursiveRef", {}, Reference)
  WALK(Known::JSON_Schema_2019_09_Core, "$recursiveAnchor", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Applicator, "allOf", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_2019_09_Applicator, "anyOf", {},
       ApplicatorElementsInPlaceSome)
  WALK(Known::JSON_Schema_2019_09_Applicator, "oneOf", {},
       ApplicatorElementsInPlaceSome)
  WALK(Known::JSON_Schema_2019_09_Applicator, "if", {},
       ApplicatorValueInPlaceMaybe)
  WALK(Known::JSON_Schema_2019_09_Applicator, "then", {},
       ApplicatorValueInPlaceMaybe, "if")
  WALK(Known::JSON_Schema_2019_09_Applicator, "else", {},
       ApplicatorValueInPlaceMaybe, "if")
  WALK(Known::JSON_Schema_2019_09_Applicator, "not", {},
       ApplicatorValueInPlaceNegate)
  // For the purpose of compiler optimizations
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2019_09_Applicator, "properties",
                       TYPES(JSON::Type::Object),
                       ApplicatorMembersTraversePropertyStatic,
                       Known::JSON_Schema_2019_09_Validation, "required")
  WALK(Known::JSON_Schema_2019_09_Applicator, "patternProperties",
       TYPES(JSON::Type::Object), ApplicatorMembersTraversePropertyRegex)
  WALK(Known::JSON_Schema_2019_09_Applicator, "additionalProperties",
       TYPES(JSON::Type::Object), ApplicatorValueTraverseSomeProperty,
       "properties", "patternProperties")
  WALK(Known::JSON_Schema_2019_09_Applicator, "propertyNames",
       TYPES(JSON::Type::Object), ApplicatorValueTraverseAnyPropertyKey)
  WALK(Known::JSON_Schema_2019_09_Applicator, "dependentSchemas",
       TYPES(JSON::Type::Object), ApplicatorMembersInPlaceSome)
  WALK(Known::JSON_Schema_2019_09_Applicator, "unevaluatedProperties",
       TYPES(JSON::Type::Object), ApplicatorValueTraverseSomeProperty,
       "properties", "patternProperties", "additionalProperties")
  WALK(Known::JSON_Schema_2019_09_Applicator, "unevaluatedItems",
       TYPES(JSON::Type::Array), ApplicatorValueTraverseSomeItem, "items",
       "additionalItems")
  WALK(Known::JSON_Schema_2019_09_Applicator, "items", TYPES(JSON::Type::Array),
       ApplicatorValueOrElementsTraverseAnyItemOrItem)
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2019_09_Applicator, "contains",
                       TYPES(JSON::Type::Array), ApplicatorValueTraverseAnyItem,
                       Known::JSON_Schema_2019_09_Validation, "minContains",
                       "maxContains")
  WALK(Known::JSON_Schema_2019_09_Applicator, "additionalItems",
       TYPES(JSON::Type::Array), ApplicatorValueTraverseSomeItem, "items")
  // For the purpose of compiler optimizations
  WALK_MAYBE_DEPENDENT(Known::JSON_Schema_2019_09_Validation, "type", {},
                       Assertion, Known::JSON_Schema_2019_09_Applicator,
                       "properties")
  WALK(Known::JSON_Schema_2019_09_Validation, "enum", {}, Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "const", {}, Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "maxLength",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "minLength",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "pattern",
       TYPES(JSON::Type::String), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "exclusiveMaximum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "multipleOf",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "minimum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion, "type")
  WALK(Known::JSON_Schema_2019_09_Validation, "exclusiveMinimum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "maximum",
       TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion, "type")
  WALK(Known::JSON_Schema_2019_09_Validation, "required",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "minProperties",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "maxProperties",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "dependentRequired",
       TYPES(JSON::Type::Object), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "minItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "maxItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "maxContains",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "minContains",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2019_09_Validation, "uniqueItems",
       TYPES(JSON::Type::Array), Assertion)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "title", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "description", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "writeOnly", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "readOnly", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "examples", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "deprecated", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Meta_Data, "default", {}, Annotation)
  WALK(Known::JSON_Schema_2019_09_Format, "format", TYPES(JSON::Type::String),
       Annotation)
  WALK(Known::JSON_Schema_2019_09_Content, "contentSchema",
       TYPES(JSON::Type::String), ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2019_09_Content, "contentMediaType",
       TYPES(JSON::Type::String), Annotation)
  WALK(Known::JSON_Schema_2019_09_Content, "contentEncoding",
       TYPES(JSON::Type::String), Annotation)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "base", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "links", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "hrefSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "targetSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "headerSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "submissionSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "anchor", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "anchorPointer", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "href", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "rel", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "submissionMediaType", {},
       Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "targetHints", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "targetMediaType", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "templatePointers", {}, Other)
  WALK(Known::JSON_Schema_2019_09_Hyper_Schema, "templateRequired", {}, Other)
  // Draft7
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "$schema", {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "$id",
           {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "$comment", {}, Comment, "$ref")
  // For the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "type",
           {}, Assertion, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "enum",
           {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "const", {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "multipleOf", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "exclusiveMaximum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "exclusiveMinimum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "additionalItems", TYPES(JSON::Type::Array),
           ApplicatorValueTraverseSomeItem, "items")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "uniqueItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "contains", TYPES(JSON::Type::Array), ApplicatorValueTraverseAnyItem,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "maxProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "minProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "required", TYPES(JSON::Type::Object), Assertion, "$ref")
  // For the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic, "$ref", "required")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "patternProperties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyRegex, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties",
           "patternProperties")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "dependencies", TYPES(JSON::Type::Object),
           ApplicatorMembersInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "propertyNames", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseAnyPropertyKey, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "if",
           {}, ApplicatorValueInPlaceMaybe, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "then",
           {}, ApplicatorValueInPlaceMaybe, "if")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "else",
           {}, ApplicatorValueInPlaceMaybe, "if")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "allOf", {}, ApplicatorElementsInPlace, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "anyOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "oneOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper, "not",
           {}, ApplicatorValueInPlaceNegate, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "format", TYPES(JSON::Type::String), Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "contentEncoding", TYPES(JSON::Type::String), Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "contentMediaType", TYPES(JSON::Type::String), Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "definitions", {}, LocationMembers, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "title", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "description", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "default", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "readOnly", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "writeOnly", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
           "examples", {}, Comment, "$ref")
  WALK(Known::JSON_Schema_Draft_7_Hyper, "base", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_7_Hyper, "links", {}, ApplicatorElementsInPlace,
       "$ref")
  // Keywords from the Link Description Object are not affected by `$ref`, as
  // `$ref` is not permitted there
  WALK(Known::JSON_Schema_Draft_7_Hyper, "hrefSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "targetSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "headerSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "submissionSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "anchor", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "anchorPointer", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "submissionMediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "targetHints", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "targetMediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "templatePointers", {}, Other)
  WALK(Known::JSON_Schema_Draft_7_Hyper, "templateRequired", {}, Other)

  // $ref also takes precedence over any unknown keyword
  if ((vocabularies.contains(Known::JSON_Schema_Draft_7) ||
       vocabularies.contains(Known::JSON_Schema_Draft_7_Hyper)) &&
      keyword != "$ref") {
    return {.type = sourcemeta::core::SchemaKeywordType::Unknown,
            .vocabulary = std::nullopt,
            .dependencies = {"$ref"},
            .instances = {}};
  }

  // Draft6
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "$schema", {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper, "$id",
           {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "$comment", {}, Comment, "$ref")
  // For the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper, "type",
           {}, Assertion, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper, "enum",
           {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "const", {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "multipleOf", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "exclusiveMaximum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "exclusiveMinimum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "additionalItems", TYPES(JSON::Type::Array),
           ApplicatorValueTraverseSomeItem, "items")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "uniqueItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "contains", TYPES(JSON::Type::Array), ApplicatorValueTraverseAnyItem,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "maxProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "minProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "required", TYPES(JSON::Type::Object), Assertion, "$ref")
  // For the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic, "$ref", "required")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "patternProperties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyRegex, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties",
           "patternProperties")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "dependencies", TYPES(JSON::Type::Object),
           ApplicatorMembersInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "propertyNames", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseAnyPropertyKey, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "allOf", {}, ApplicatorElementsInPlace, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "anyOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "oneOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper, "not",
           {}, ApplicatorValueInPlaceNegate, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "format", TYPES(JSON::Type::String), Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "contentEncoding", TYPES(JSON::Type::String), Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "contentMediaType", TYPES(JSON::Type::String), Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "definitions", {}, LocationMembers, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "title", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "description", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "default", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "readOnly", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "writeOnly", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
           "examples", {}, Comment, "$ref")
  WALK(Known::JSON_Schema_Draft_6_Hyper, "base", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_6_Hyper, "links", {}, ApplicatorElementsInPlace,
       "$ref")
  WALK(Known::JSON_Schema_Draft_6_Hyper, "media", {}, Other, "$ref")
  // Keywords from the Link Description Object are not affected by `$ref`, as
  // `$ref` is not permitted there
  WALK(Known::JSON_Schema_Draft_6_Hyper, "hrefSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "targetSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "submissionSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "submissionEncType", {}, Other)
  WALK(Known::JSON_Schema_Draft_6_Hyper, "mediaType", {}, Other)

  // $ref also takes precedence over any unknown keyword
  if ((vocabularies.contains(Known::JSON_Schema_Draft_6) ||
       vocabularies.contains(Known::JSON_Schema_Draft_6_Hyper)) &&
      keyword != "$ref") {
    return {.type = sourcemeta::core::SchemaKeywordType::Unknown,
            .vocabulary = std::nullopt,
            .dependencies = {"$ref"},
            .instances = {}};
  }

  // Draft4
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "$schema", {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper, "id",
           {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper, "$ref",
           {}, Reference)
  // These dependencies are only for the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper, "type",
           {}, Assertion, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper, "enum",
           {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "multipleOf", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "exclusiveMaximum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "exclusiveMinimum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "additionalItems", TYPES(JSON::Type::Array),
           ApplicatorValueTraverseSomeItem, "items")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "uniqueItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "maxProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "minProperties", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "required", TYPES(JSON::Type::Object), Assertion, "$ref")
  // These dependencies are only for the purpose of compiler optimizations
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic, "$ref", "required")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "patternProperties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyRegex, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties",
           "patternProperties")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "dependencies", TYPES(JSON::Type::Object),
           ApplicatorMembersInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "allOf", {}, ApplicatorElementsInPlace, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "anyOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "oneOf", {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper, "not",
           {}, ApplicatorValueInPlaceNegate, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "format", TYPES(JSON::Type::String), Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "definitions", {}, LocationMembers, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "title", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "description", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
           "default", {}, Comment, "$ref")
  WALK(Known::JSON_Schema_Draft_4_Hyper, "fragmentResolution", {}, Other,
       "$ref")
  WALK(Known::JSON_Schema_Draft_4_Hyper, "links", {}, ApplicatorElementsInPlace,
       "$ref")
  WALK(Known::JSON_Schema_Draft_4_Hyper, "media", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_4_Hyper, "pathStart", {}, Other, "$ref")
  // Keywords from the Link Description Object are not affected by `$ref`, as
  // `$ref` is not permitted there
  WALK(Known::JSON_Schema_Draft_4_Hyper, "encType", {}, Other)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "mediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "method", {}, Other)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "schema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_4_Hyper, "targetSchema", {},
       ApplicatorValueInPlaceOther)

  // $ref also takes precedence over any unknown keyword
  if ((vocabularies.contains(Known::JSON_Schema_Draft_4) ||
       vocabularies.contains(Known::JSON_Schema_Draft_4_Hyper)) &&
      keyword != "$ref") {
    return {.type = sourcemeta::core::SchemaKeywordType::Unknown,
            .vocabulary = std::nullopt,
            .dependencies = {"$ref"},
            .instances = {}};
  }

  // Draft3
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper, "id",
           {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "$schema", {}, Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "extends", {}, ApplicatorValueOrElementsInPlace, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper, "type",
           {}, ApplicatorElementsInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "disallow", {}, ApplicatorElementsInPlaceSomeNegate, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "patternProperties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyRegex, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties",
           "patternProperties")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "additionalItems", TYPES(JSON::Type::Array),
           ApplicatorValueTraverseSomeItem, "items")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "uniqueItems", TYPES(JSON::Type::Array), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "required", TYPES(JSON::Type::Object), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "dependencies", TYPES(JSON::Type::Object),
           ApplicatorMembersInPlaceSome, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper, "enum",
           {}, Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "divisibleBy", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "exclusiveMinimum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion,
           "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "exclusiveMaximum", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "format", TYPES(JSON::Type::String), Other, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "description", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "title", {}, Comment, "$ref")
  WALK_ANY(Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
           "default", {}, Comment, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "fragmentResolution", {}, Other,
       "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "root", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "readonly", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "contentEncoding", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "pathStart", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "mediaType", {}, Other, "$ref")
  WALK(Known::JSON_Schema_Draft_3_Hyper, "links", {}, ApplicatorElementsInPlace,
       "$ref")
  // Keywords from the Link Description Object are not affected by `$ref`, as
  // `$ref` is not permitted there
  WALK(Known::JSON_Schema_Draft_3_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_3_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_3_Hyper, "targetSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_3_Hyper, "method", {}, Other)
  WALK(Known::JSON_Schema_Draft_3_Hyper, "enctype", {}, Other)

  // $ref also takes precedence over any unknown keyword
  if (vocabularies.contains(Known::JSON_Schema_Draft_3) && keyword != "$ref") {
    return {.type = sourcemeta::core::SchemaKeywordType::Unknown,
            .vocabulary = std::nullopt,
            .dependencies = {"$ref"},
            .instances = {}};
  }

  // Draft2
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "$schema", {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper, "id",
           {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper, "type",
           {}, ApplicatorElementsInPlaceSome)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper, "enum",
           {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "maximumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "minimumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "uniqueItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "requires", TYPES(JSON::Type::Object), ApplicatorValueTraverseParent)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "format", TYPES(JSON::Type::String), Other)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "title", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "description", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "default", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "divisibleBy", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "disallow", {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "extends", {}, ApplicatorValueOrElementsInPlace)
  WALK_ANY(Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
           "contentEncoding", TYPES(JSON::Type::String), Comment)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "fragmentResolution", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "root", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "readonly", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "pathStart", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "mediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "alternate", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "links", {}, ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "targetSchema", {},
       ApplicatorValueInPlaceOther)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "method", {}, Other)
  WALK(Known::JSON_Schema_Draft_2_Hyper, "enctype", {}, Other)

  // Draft1
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "$schema", {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper, "id",
           {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper, "type",
           {}, ApplicatorElementsInPlaceSome)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper, "enum",
           {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "maximumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "minimumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "requires", TYPES(JSON::Type::Object), ApplicatorValueTraverseParent)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "format", TYPES(JSON::Type::String), Other)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "title", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "description", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "default", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "disallow", {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "extends", {}, ApplicatorValueOrElementsInPlace)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "contentEncoding", TYPES(JSON::Type::String), Comment)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "optional", TYPES(JSON::Type::Object), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
           "maxDecimal", TYPES(JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "fragmentResolution", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "root", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "readonly", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "pathStart", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "mediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "alternate", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "links", {}, ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "method", {}, Other)
  WALK(Known::JSON_Schema_Draft_1_Hyper, "enctype", {}, Other)

  // Draft0
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "$schema", {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper, "id",
           {}, Other)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper, "$ref",
           {}, Reference)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "items", TYPES(JSON::Type::Array),
           ApplicatorValueOrElementsTraverseAnyItemOrItem)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "properties", TYPES(JSON::Type::Object),
           ApplicatorMembersTraversePropertyStatic)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "additionalProperties", TYPES(JSON::Type::Object),
           ApplicatorValueTraverseSomeProperty, "properties")
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper, "type",
           {}, ApplicatorElementsInPlaceSome)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper, "enum",
           {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "maximum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "minimum", TYPES(JSON::Type::Integer, JSON::Type::Real), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "maximumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "minimumCanEqual", TYPES(JSON::Type::Integer, JSON::Type::Real),
           Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "maxLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "minLength", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "pattern", TYPES(JSON::Type::String), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "maxItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "minItems", TYPES(JSON::Type::Array), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "requires", TYPES(JSON::Type::Object), ApplicatorValueTraverseParent)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "format", TYPES(JSON::Type::String), Other)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "title", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "description", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "default", {}, Comment)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "disallow", {}, Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "extends", {}, ApplicatorValueOrElementsInPlace)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "contentEncoding", TYPES(JSON::Type::String), Comment)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "optional", TYPES(JSON::Type::Object), Assertion)
  WALK_ANY(Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
           "maxDecimal", TYPES(JSON::Type::Real), Assertion)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "fragmentResolution", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "root", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "readonly", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "pathStart", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "mediaType", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "alternate", {},
       ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "links", {}, ApplicatorElementsInPlace)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "href", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "rel", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "method", {}, Other)
  WALK(Known::JSON_Schema_Draft_0_Hyper, "enctype", {}, Other)
#undef WALK
#undef WALK_ANY
#undef WALK_MAYBE_DEPENDENT
  return {.type = sourcemeta::core::SchemaKeywordType::Unknown,
          .vocabulary = std::nullopt,
          .dependencies = {},
          .instances = {}};
}
