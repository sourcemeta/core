#include <sourcemeta/core/jsonschema.h>

namespace sourcemeta::core {

using Known = Vocabularies::Known;

#define MATCHES(_keyword) (keyword == (_keyword))

// Creates a static SchemaWalkerResult and returns a const reference to it.
// The variadic args are the dependency keywords (strings).
#define RETURN(_vocabulary, _types, _strategy, ...)                            \
  {                                                                            \
    static const SchemaWalkerResult result{                                    \
        SchemaKeywordType::_strategy, _vocabulary, {__VA_ARGS__}, _types};     \
    return result;                                                             \
  }

#define RETURN_IF_MATCHES(_keyword, _vocabulary, _types, _strategy, ...)       \
  if (MATCHES((_keyword))) {                                                   \
    RETURN(_vocabulary, _types, _strategy __VA_OPT__(, ) __VA_ARGS__)          \
  }

// For drafts with paired base/hyper vocabularies, this macro generates
// separate static results for each vocabulary to avoid runtime vocabulary
// variable initialization issues.
#define RETURN_IF_MATCHES_EITHER(_keyword, _base_vocab, _hyper_vocab, _types,  \
                                 _strategy, ...)                               \
  if (MATCHES((_keyword))) {                                                   \
    if (vocabularies.contains(_base_vocab)) {                                  \
      RETURN(_base_vocab, _types, _strategy __VA_OPT__(, ) __VA_ARGS__);       \
    } else {                                                                   \
      RETURN(_hyper_vocab, _types, _strategy __VA_OPT__(, ) __VA_ARGS__);      \
    }                                                                          \
  }

auto schema_official_walker(std::string_view keyword,
                            const Vocabularies &vocabularies)
    -> const SchemaWalkerResult & {

  // 2020-12
  if (vocabularies.contains(Known::JSON_Schema_2020_12_Core)) {
    RETURN_IF_MATCHES("$id", Known::JSON_Schema_2020_12_Core, {}, Other);
    RETURN_IF_MATCHES("$schema", Known::JSON_Schema_2020_12_Core, {}, Other);
    RETURN_IF_MATCHES("$ref", Known::JSON_Schema_2020_12_Core, {}, Reference);
    RETURN_IF_MATCHES("$defs", Known::JSON_Schema_2020_12_Core, {},
                      LocationMembers);
    // JSON Schema still defines this for backwards-compatibility
    // See https://json-schema.org/draft/2020-12/schema
    RETURN_IF_MATCHES("definitions", Known::JSON_Schema_2020_12_Core, {},
                      LocationMembers);
    RETURN_IF_MATCHES("$comment", Known::JSON_Schema_2020_12_Core, {}, Comment);
    RETURN_IF_MATCHES("$anchor", Known::JSON_Schema_2020_12_Core, {}, Other);
    RETURN_IF_MATCHES("$vocabulary", Known::JSON_Schema_2020_12_Core, {},
                      Other);
    RETURN_IF_MATCHES("$dynamicRef", Known::JSON_Schema_2020_12_Core, {},
                      Reference);
    RETURN_IF_MATCHES("$dynamicAnchor", Known::JSON_Schema_2020_12_Core, {},
                      Other);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Applicator)) {
    const auto has_validation =
        vocabularies.contains(Known::JSON_Schema_2020_12_Validation);
    RETURN_IF_MATCHES("oneOf", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES("anyOf", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES("allOf", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorElementsInPlace);
    RETURN_IF_MATCHES("if", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorValueInPlaceMaybe);
    RETURN_IF_MATCHES("then", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES("else", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES("not", Known::JSON_Schema_2020_12_Applicator, {},
                      ApplicatorValueInPlaceNegate);
    if (MATCHES("properties")) {
      // For the purpose of compiler optimizations
      if (has_validation) {
        RETURN(Known::JSON_Schema_2020_12_Applicator,
               make_set({JSON::Type::Object}),
               ApplicatorMembersTraversePropertyStatic, "required");
      } else {
        RETURN(Known::JSON_Schema_2020_12_Applicator,
               make_set({JSON::Type::Object}),
               ApplicatorMembersTraversePropertyStatic);
      }
    }
    RETURN_IF_MATCHES(
        "additionalProperties", Known::JSON_Schema_2020_12_Applicator,
        make_set({JSON::Type::Object}), ApplicatorValueTraverseSomeProperty,
        "properties", "patternProperties");
    RETURN_IF_MATCHES(
        "patternProperties", Known::JSON_Schema_2020_12_Applicator,
        make_set({JSON::Type::Object}), ApplicatorMembersTraversePropertyRegex);
    RETURN_IF_MATCHES("propertyNames", Known::JSON_Schema_2020_12_Applicator,
                      make_set({JSON::Type::Object}),
                      ApplicatorValueTraverseAnyPropertyKey);
    RETURN_IF_MATCHES("dependentSchemas", Known::JSON_Schema_2020_12_Applicator,
                      make_set({JSON::Type::Object}),
                      ApplicatorMembersInPlaceSome);
    if (MATCHES("contains")) {
      if (has_validation) {
        RETURN(Known::JSON_Schema_2020_12_Applicator,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseAnyItem,
               "minContains", "maxContains");
      } else {
        RETURN(Known::JSON_Schema_2020_12_Applicator,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseAnyItem);
      }
    }
    RETURN_IF_MATCHES("items", Known::JSON_Schema_2020_12_Applicator,
                      make_set({JSON::Type::Array}),
                      ApplicatorValueTraverseSomeItem, "prefixItems");
    RETURN_IF_MATCHES("prefixItems", Known::JSON_Schema_2020_12_Applicator,
                      make_set({JSON::Type::Array}),
                      ApplicatorElementsTraverseItem);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Validation)) {
    const auto has_applicator =
        vocabularies.contains(Known::JSON_Schema_2020_12_Applicator);
    if (MATCHES("type")) {
      // For the purpose of compiler optimizations
      if (has_applicator) {
        RETURN(Known::JSON_Schema_2020_12_Validation, {}, Assertion,
               "properties");
      } else {
        RETURN(Known::JSON_Schema_2020_12_Validation, {}, Assertion);
      }
    }
    RETURN_IF_MATCHES("enum", Known::JSON_Schema_2020_12_Validation, {},
                      Assertion);
    RETURN_IF_MATCHES("const", Known::JSON_Schema_2020_12_Validation, {},
                      Assertion);
    RETURN_IF_MATCHES("maxLength", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("minLength", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("pattern", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("exclusiveMinimum", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("multipleOf", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("maximum", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion, "type");
    RETURN_IF_MATCHES("exclusiveMaximum", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("minimum", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion, "type");
    RETURN_IF_MATCHES("dependentRequired",
                      Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("minProperties", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("maxProperties", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("required", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("maxItems", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("minItems", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("uniqueItems", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("minContains", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("maxContains", Known::JSON_Schema_2020_12_Validation,
                      make_set({JSON::Type::Array}), Assertion);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Meta_Data)) {
    RETURN_IF_MATCHES("title", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("description", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("writeOnly", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("readOnly", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("examples", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("default", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("deprecated", Known::JSON_Schema_2020_12_Meta_Data, {},
                      Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Format_Annotation)) {
    RETURN_IF_MATCHES("format", Known::JSON_Schema_2020_12_Format_Annotation,
                      make_set({JSON::Type::String}), Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Unevaluated)) {
    const auto has_applicator =
        vocabularies.contains(Known::JSON_Schema_2020_12_Applicator);
    if (MATCHES("unevaluatedProperties")) {
      if (has_applicator) {
        RETURN(Known::JSON_Schema_2020_12_Unevaluated,
               make_set({JSON::Type::Object}),
               ApplicatorValueTraverseSomeProperty, "properties",
               "patternProperties", "additionalProperties");
      } else {
        RETURN(Known::JSON_Schema_2020_12_Unevaluated,
               make_set({JSON::Type::Object}),
               ApplicatorValueTraverseSomeProperty);
      }
    }
    if (MATCHES("unevaluatedItems")) {
      if (has_applicator) {
        RETURN(Known::JSON_Schema_2020_12_Unevaluated,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseSomeItem,
               "prefixItems", "items", "contains");
      } else {
        RETURN(Known::JSON_Schema_2020_12_Unevaluated,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseSomeItem);
      }
    }
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Content)) {
    RETURN_IF_MATCHES("contentSchema", Known::JSON_Schema_2020_12_Content,
                      make_set({JSON::Type::String}),
                      ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("contentMediaType", Known::JSON_Schema_2020_12_Content,
                      make_set({JSON::Type::String}), Annotation);
    RETURN_IF_MATCHES("contentEncoding", Known::JSON_Schema_2020_12_Content,
                      make_set({JSON::Type::String}), Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2020_12_Format_Assertion)) {
    RETURN_IF_MATCHES("format", Known::JSON_Schema_2020_12_Format_Assertion,
                      make_set({JSON::Type::String}), Assertion);
  }

  // 2019-09
  if (vocabularies.contains(Known::JSON_Schema_2019_09_Core)) {
    RETURN_IF_MATCHES("$id", Known::JSON_Schema_2019_09_Core, {}, Other);
    RETURN_IF_MATCHES("$schema", Known::JSON_Schema_2019_09_Core, {}, Other);
    RETURN_IF_MATCHES("$ref", Known::JSON_Schema_2019_09_Core, {}, Reference);
    RETURN_IF_MATCHES("$defs", Known::JSON_Schema_2019_09_Core, {},
                      LocationMembers);
    // JSON Schema still defines this for backwards-compatibility
    // See https://json-schema.org/draft/2019-09/schema
    RETURN_IF_MATCHES("definitions", Known::JSON_Schema_2019_09_Core, {},
                      LocationMembers);
    RETURN_IF_MATCHES("$comment", Known::JSON_Schema_2019_09_Core, {}, Comment);
    RETURN_IF_MATCHES("$anchor", Known::JSON_Schema_2019_09_Core, {}, Other);
    RETURN_IF_MATCHES("$vocabulary", Known::JSON_Schema_2019_09_Core, {},
                      Other);
    RETURN_IF_MATCHES("$recursiveRef", Known::JSON_Schema_2019_09_Core, {},
                      Reference);
    RETURN_IF_MATCHES("$recursiveAnchor", Known::JSON_Schema_2019_09_Core, {},
                      Other);
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Applicator)) {
    const auto has_validation =
        vocabularies.contains(Known::JSON_Schema_2019_09_Validation);
    RETURN_IF_MATCHES("allOf", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorElementsInPlace);
    RETURN_IF_MATCHES("anyOf", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES("oneOf", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES("if", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorValueInPlaceMaybe);
    RETURN_IF_MATCHES("then", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES("else", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES("not", Known::JSON_Schema_2019_09_Applicator, {},
                      ApplicatorValueInPlaceNegate);
    if (MATCHES("properties")) {
      // For the purpose of compiler optimizations
      if (has_validation) {
        RETURN(Known::JSON_Schema_2019_09_Applicator,
               make_set({JSON::Type::Object}),
               ApplicatorMembersTraversePropertyStatic, "required");
      } else {
        RETURN(Known::JSON_Schema_2019_09_Applicator,
               make_set({JSON::Type::Object}),
               ApplicatorMembersTraversePropertyStatic);
      }
    }
    RETURN_IF_MATCHES(
        "patternProperties", Known::JSON_Schema_2019_09_Applicator,
        make_set({JSON::Type::Object}), ApplicatorMembersTraversePropertyRegex);
    RETURN_IF_MATCHES(
        "additionalProperties", Known::JSON_Schema_2019_09_Applicator,
        make_set({JSON::Type::Object}), ApplicatorValueTraverseSomeProperty,
        "properties", "patternProperties");
    RETURN_IF_MATCHES("propertyNames", Known::JSON_Schema_2019_09_Applicator,
                      make_set({JSON::Type::Object}),
                      ApplicatorValueTraverseAnyPropertyKey);
    RETURN_IF_MATCHES("dependentSchemas", Known::JSON_Schema_2019_09_Applicator,
                      make_set({JSON::Type::Object}),
                      ApplicatorMembersInPlaceSome);
    RETURN_IF_MATCHES(
        "unevaluatedProperties", Known::JSON_Schema_2019_09_Applicator,
        make_set({JSON::Type::Object}), ApplicatorValueTraverseSomeProperty,
        "properties", "patternProperties", "additionalProperties");
    RETURN_IF_MATCHES("unevaluatedItems", Known::JSON_Schema_2019_09_Applicator,
                      make_set({JSON::Type::Array}),
                      ApplicatorValueTraverseSomeItem, "items",
                      "additionalItems");
    RETURN_IF_MATCHES("items", Known::JSON_Schema_2019_09_Applicator,
                      make_set({JSON::Type::Array}),
                      ApplicatorValueOrElementsTraverseAnyItemOrItem);
    if (MATCHES("contains")) {
      if (has_validation) {
        RETURN(Known::JSON_Schema_2019_09_Applicator,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseAnyItem,
               "minContains", "maxContains");
      } else {
        RETURN(Known::JSON_Schema_2019_09_Applicator,
               make_set({JSON::Type::Array}), ApplicatorValueTraverseAnyItem);
      }
    }
    RETURN_IF_MATCHES("additionalItems", Known::JSON_Schema_2019_09_Applicator,
                      make_set({JSON::Type::Array}),
                      ApplicatorValueTraverseSomeItem, "items");
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Validation)) {
    const auto has_applicator =
        vocabularies.contains(Known::JSON_Schema_2019_09_Applicator);
    if (MATCHES("type")) {
      // For the purpose of compiler optimizations
      if (has_applicator) {
        RETURN(Known::JSON_Schema_2019_09_Validation, {}, Assertion,
               "properties");
      } else {
        RETURN(Known::JSON_Schema_2019_09_Validation, {}, Assertion);
      }
    }
    RETURN_IF_MATCHES("enum", Known::JSON_Schema_2019_09_Validation, {},
                      Assertion);
    RETURN_IF_MATCHES("const", Known::JSON_Schema_2019_09_Validation, {},
                      Assertion);
    RETURN_IF_MATCHES("maxLength", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("minLength", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("pattern", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES("exclusiveMaximum", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("multipleOf", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("minimum", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion, "type");
    RETURN_IF_MATCHES("exclusiveMinimum", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion);
    RETURN_IF_MATCHES("maximum", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Integer, JSON::Type::Real}),
                      Assertion, "type");
    RETURN_IF_MATCHES("required", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("minProperties", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("maxProperties", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("dependentRequired",
                      Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES("minItems", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("maxItems", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("maxContains", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("minContains", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES("uniqueItems", Known::JSON_Schema_2019_09_Validation,
                      make_set({JSON::Type::Array}), Assertion);
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Meta_Data)) {
    RETURN_IF_MATCHES("title", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("description", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("writeOnly", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("readOnly", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("examples", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("deprecated", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
    RETURN_IF_MATCHES("default", Known::JSON_Schema_2019_09_Meta_Data, {},
                      Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Format)) {
    RETURN_IF_MATCHES("format", Known::JSON_Schema_2019_09_Format,
                      make_set({JSON::Type::String}), Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Content)) {
    RETURN_IF_MATCHES("contentSchema", Known::JSON_Schema_2019_09_Content,
                      make_set({JSON::Type::String}),
                      ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("contentMediaType", Known::JSON_Schema_2019_09_Content,
                      make_set({JSON::Type::String}), Annotation);
    RETURN_IF_MATCHES("contentEncoding", Known::JSON_Schema_2019_09_Content,
                      make_set({JSON::Type::String}), Annotation);
  }

  if (vocabularies.contains(Known::JSON_Schema_2019_09_Hyper_Schema)) {
    RETURN_IF_MATCHES("base", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      Other);
    RETURN_IF_MATCHES("links", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      ApplicatorElementsInPlace);
    RETURN_IF_MATCHES("hrefSchema", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_2019_09_Hyper_Schema,
                      {}, ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("headerSchema", Known::JSON_Schema_2019_09_Hyper_Schema,
                      {}, ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("submissionSchema",
                      Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      ApplicatorValueInPlaceOther);
    RETURN_IF_MATCHES("anchor", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      Other);
    RETURN_IF_MATCHES("anchorPointer", Known::JSON_Schema_2019_09_Hyper_Schema,
                      {}, Other);
    RETURN_IF_MATCHES("href", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      Other);
    RETURN_IF_MATCHES("rel", Known::JSON_Schema_2019_09_Hyper_Schema, {},
                      Other);
    RETURN_IF_MATCHES("submissionMediaType",
                      Known::JSON_Schema_2019_09_Hyper_Schema, {}, Other);
    RETURN_IF_MATCHES("targetHints", Known::JSON_Schema_2019_09_Hyper_Schema,
                      {}, Other);
    RETURN_IF_MATCHES("targetMediaType",
                      Known::JSON_Schema_2019_09_Hyper_Schema, {}, Other);
    RETURN_IF_MATCHES("templatePointers",
                      Known::JSON_Schema_2019_09_Hyper_Schema, {}, Other);
    RETURN_IF_MATCHES("templateRequired",
                      Known::JSON_Schema_2019_09_Hyper_Schema, {}, Other);
  }

  // Draft7
  // Note: Draft7 and earlier use a single vocabulary that includes all
  // keywords, so we only need one contains() check per dialect
  if (vocabularies.contains(Known::JSON_Schema_Draft_7) ||
      vocabularies.contains(Known::JSON_Schema_Draft_7_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$id", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("$comment", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Assertion,
                             "properties");
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("const", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("multipleOf", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMaximum", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMinimum", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "items", Known::JSON_Schema_Draft_7, Known::JSON_Schema_Draft_7_Hyper,
        make_set({JSON::Type::Array}),
        ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref");
    RETURN_IF_MATCHES_EITHER("additionalItems", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseSomeItem, "items");
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("uniqueItems", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("contains", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseAnyItem, "$ref");
    RETURN_IF_MATCHES_EITHER("maxProperties", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minProperties", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("required", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "properties", Known::JSON_Schema_Draft_7,
        Known::JSON_Schema_Draft_7_Hyper, make_set({JSON::Type::Object}),
        ApplicatorMembersTraversePropertyStatic, "$ref", "required");
    RETURN_IF_MATCHES_EITHER("patternProperties", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyRegex, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "additionalProperties", Known::JSON_Schema_Draft_7,
        Known::JSON_Schema_Draft_7_Hyper, make_set({JSON::Type::Object}),
        ApplicatorValueTraverseSomeProperty, "properties", "patternProperties");
    RETURN_IF_MATCHES_EITHER("dependencies", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("propertyNames", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseAnyPropertyKey, "$ref");
    RETURN_IF_MATCHES_EITHER("if", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorValueInPlaceMaybe, "$ref");
    RETURN_IF_MATCHES_EITHER("then", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES_EITHER("else", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorValueInPlaceMaybe, "if");
    RETURN_IF_MATCHES_EITHER("allOf", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorElementsInPlace, "$ref");
    RETURN_IF_MATCHES_EITHER("anyOf", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("oneOf", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("not", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             ApplicatorValueInPlaceNegate, "$ref");
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Other, "$ref");
    RETURN_IF_MATCHES_EITHER("contentEncoding", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Comment, "$ref");
    RETURN_IF_MATCHES_EITHER("contentMediaType", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper,
                             make_set({JSON::Type::String}), Comment, "$ref");
    RETURN_IF_MATCHES_EITHER("definitions", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {},
                             LocationMembers, "$ref");
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("readOnly", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("writeOnly", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("examples", Known::JSON_Schema_Draft_7,
                             Known::JSON_Schema_Draft_7_Hyper, {}, Comment,
                             "$ref");

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_7_Hyper)) {
      RETURN_IF_MATCHES("base", Known::JSON_Schema_Draft_7_Hyper, {}, Other,
                        "$ref");
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_7_Hyper, {},
                        ApplicatorElementsInPlace, "$ref");
      // Keywords from the Link Description Object are not affected by `$ref`,
      // as `$ref` is not permitted there
      RETURN_IF_MATCHES("hrefSchema", Known::JSON_Schema_Draft_7_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_Draft_7_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("headerSchema", Known::JSON_Schema_Draft_7_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("submissionSchema", Known::JSON_Schema_Draft_7_Hyper,
                        {}, ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("anchor", Known::JSON_Schema_Draft_7_Hyper, {}, Other);
      RETURN_IF_MATCHES("anchorPointer", Known::JSON_Schema_Draft_7_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_7_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_7_Hyper, {}, Other);
      RETURN_IF_MATCHES("submissionMediaType", Known::JSON_Schema_Draft_7_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("targetHints", Known::JSON_Schema_Draft_7_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("targetMediaType", Known::JSON_Schema_Draft_7_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("templatePointers", Known::JSON_Schema_Draft_7_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("templateRequired", Known::JSON_Schema_Draft_7_Hyper,
                        {}, Other);
    }

    // $ref takes precedence over any unknown keyword
    static const SchemaWalkerResult unknown_with_ref(
        SchemaKeywordType::Unknown, std::nullopt, {"$ref"}, {});
    return unknown_with_ref;
  }

  // Draft6
  if (vocabularies.contains(Known::JSON_Schema_Draft_6) ||
      vocabularies.contains(Known::JSON_Schema_Draft_6_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$id", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("$comment", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Assertion,
                             "properties");
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("const", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("multipleOf", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMaximum", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMinimum", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "items", Known::JSON_Schema_Draft_6, Known::JSON_Schema_Draft_6_Hyper,
        make_set({JSON::Type::Array}),
        ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref");
    RETURN_IF_MATCHES_EITHER("additionalItems", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseSomeItem, "items");
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("uniqueItems", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("contains", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseAnyItem, "$ref");
    RETURN_IF_MATCHES_EITHER("maxProperties", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minProperties", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("required", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "properties", Known::JSON_Schema_Draft_6,
        Known::JSON_Schema_Draft_6_Hyper, make_set({JSON::Type::Object}),
        ApplicatorMembersTraversePropertyStatic, "$ref", "required");
    RETURN_IF_MATCHES_EITHER("patternProperties", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyRegex, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "additionalProperties", Known::JSON_Schema_Draft_6,
        Known::JSON_Schema_Draft_6_Hyper, make_set({JSON::Type::Object}),
        ApplicatorValueTraverseSomeProperty, "properties", "patternProperties");
    RETURN_IF_MATCHES_EITHER("dependencies", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("propertyNames", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseAnyPropertyKey, "$ref");
    RETURN_IF_MATCHES_EITHER("allOf", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {},
                             ApplicatorElementsInPlace, "$ref");
    RETURN_IF_MATCHES_EITHER("anyOf", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("oneOf", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("not", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {},
                             ApplicatorValueInPlaceNegate, "$ref");
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Other, "$ref");
    RETURN_IF_MATCHES_EITHER("contentEncoding", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Comment, "$ref");
    RETURN_IF_MATCHES_EITHER("contentMediaType", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper,
                             make_set({JSON::Type::String}), Comment, "$ref");
    RETURN_IF_MATCHES_EITHER("definitions", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {},
                             LocationMembers, "$ref");
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("readOnly", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("writeOnly", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("examples", Known::JSON_Schema_Draft_6,
                             Known::JSON_Schema_Draft_6_Hyper, {}, Comment,
                             "$ref");

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_6_Hyper)) {
      RETURN_IF_MATCHES("base", Known::JSON_Schema_Draft_6_Hyper, {}, Other,
                        "$ref");
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_6_Hyper, {},
                        ApplicatorElementsInPlace, "$ref");
      RETURN_IF_MATCHES("media", Known::JSON_Schema_Draft_6_Hyper, {}, Other,
                        "$ref");
      // Keywords from the Link Description Object are not affected by `$ref`,
      // as `$ref` is not permitted there
      RETURN_IF_MATCHES("hrefSchema", Known::JSON_Schema_Draft_6_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_Draft_6_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("submissionSchema", Known::JSON_Schema_Draft_6_Hyper,
                        {}, ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_6_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_6_Hyper, {}, Other);
      RETURN_IF_MATCHES("submissionEncType", Known::JSON_Schema_Draft_6_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_6_Hyper, {},
                        Other);
    }

    // $ref takes precedence over any unknown keyword
    static const SchemaWalkerResult unknown_with_ref(
        SchemaKeywordType::Unknown, std::nullopt, {"$ref"}, {});
    return unknown_with_ref;
  }

  // Draft4
  if (vocabularies.contains(Known::JSON_Schema_Draft_4) ||
      vocabularies.contains(Known::JSON_Schema_Draft_4_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("id", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Assertion,
                             "properties");
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("multipleOf", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMaximum", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMinimum", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "items", Known::JSON_Schema_Draft_4, Known::JSON_Schema_Draft_4_Hyper,
        make_set({JSON::Type::Array}),
        ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref");
    RETURN_IF_MATCHES_EITHER("additionalItems", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseSomeItem, "items");
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("uniqueItems", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxProperties", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minProperties", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("required", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "properties", Known::JSON_Schema_Draft_4,
        Known::JSON_Schema_Draft_4_Hyper, make_set({JSON::Type::Object}),
        ApplicatorMembersTraversePropertyStatic, "$ref", "required");
    RETURN_IF_MATCHES_EITHER("patternProperties", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyRegex, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "additionalProperties", Known::JSON_Schema_Draft_4,
        Known::JSON_Schema_Draft_4_Hyper, make_set({JSON::Type::Object}),
        ApplicatorValueTraverseSomeProperty, "properties", "patternProperties");
    RETURN_IF_MATCHES_EITHER("dependencies", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("allOf", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {},
                             ApplicatorElementsInPlace, "$ref");
    RETURN_IF_MATCHES_EITHER("anyOf", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("oneOf", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("not", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {},
                             ApplicatorValueInPlaceNegate, "$ref");
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper,
                             make_set({JSON::Type::String}), Other, "$ref");
    RETURN_IF_MATCHES_EITHER("definitions", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {},
                             LocationMembers, "$ref");
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_4,
                             Known::JSON_Schema_Draft_4_Hyper, {}, Comment,
                             "$ref");

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_4_Hyper)) {
      RETURN_IF_MATCHES("fragmentResolution", Known::JSON_Schema_Draft_4_Hyper,
                        {}, Other, "$ref");
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_4_Hyper, {},
                        ApplicatorElementsInPlace, "$ref");
      RETURN_IF_MATCHES("media", Known::JSON_Schema_Draft_4_Hyper, {}, Other,
                        "$ref");
      RETURN_IF_MATCHES("pathStart", Known::JSON_Schema_Draft_4_Hyper, {},
                        Other, "$ref");
      // Keywords from the Link Description Object are not affected by `$ref`,
      // as `$ref` is not permitted there
      RETURN_IF_MATCHES("encType", Known::JSON_Schema_Draft_4_Hyper, {}, Other);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_4_Hyper, {}, Other);
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_4_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("method", Known::JSON_Schema_Draft_4_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_4_Hyper, {}, Other);
      RETURN_IF_MATCHES("schema", Known::JSON_Schema_Draft_4_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_Draft_4_Hyper, {},
                        ApplicatorValueInPlaceOther);
    }

    // $ref takes precedence over any unknown keyword
    static const SchemaWalkerResult unknown_with_ref(
        SchemaKeywordType::Unknown, std::nullopt, {"$ref"}, {});
    return unknown_with_ref;
  }

  // Draft3
  if (vocabularies.contains(Known::JSON_Schema_Draft_3) ||
      vocabularies.contains(Known::JSON_Schema_Draft_3_Hyper)) {
    RETURN_IF_MATCHES_EITHER("id", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Other,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("extends", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {},
                             ApplicatorValueOrElementsInPlace, "$ref");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {},
                             ApplicatorElementsInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("disallow", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {},
                             ApplicatorElementsInPlaceSomeNegate, "$ref");
    RETURN_IF_MATCHES_EITHER("properties", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyStatic, "$ref");
    RETURN_IF_MATCHES_EITHER("patternProperties", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyRegex, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "additionalProperties", Known::JSON_Schema_Draft_3,
        Known::JSON_Schema_Draft_3_Hyper, make_set({JSON::Type::Object}),
        ApplicatorValueTraverseSomeProperty, "properties", "patternProperties");
    RETURN_IF_MATCHES_EITHER(
        "items", Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
        make_set({JSON::Type::Array}),
        ApplicatorValueOrElementsTraverseAnyItemOrItem, "$ref");
    RETURN_IF_MATCHES_EITHER("additionalItems", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueTraverseSomeItem, "items");
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("uniqueItems", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Array}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("required", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Object}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("dependencies", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersInPlaceSome, "$ref");
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Assertion,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::String}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("divisibleBy", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMinimum", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_3, Known::JSON_Schema_Draft_3_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("exclusiveMaximum", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion, "$ref");
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper,
                             make_set({JSON::Type::String}), Other, "$ref");
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Comment,
                             "$ref");
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_3,
                             Known::JSON_Schema_Draft_3_Hyper, {}, Comment,
                             "$ref");

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_3_Hyper)) {
      RETURN_IF_MATCHES("fragmentResolution", Known::JSON_Schema_Draft_3_Hyper,
                        {}, Other, "$ref");
      RETURN_IF_MATCHES("root", Known::JSON_Schema_Draft_3_Hyper, {}, Other,
                        "$ref");
      RETURN_IF_MATCHES("readonly", Known::JSON_Schema_Draft_3_Hyper, {}, Other,
                        "$ref");
      RETURN_IF_MATCHES("contentEncoding", Known::JSON_Schema_Draft_3_Hyper, {},
                        Other, "$ref");
      RETURN_IF_MATCHES("pathStart", Known::JSON_Schema_Draft_3_Hyper, {},
                        Other, "$ref");
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_3_Hyper, {},
                        Other, "$ref");
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_3_Hyper, {},
                        ApplicatorElementsInPlace, "$ref");
      // Keywords from the Link Description Object are not affected by `$ref`,
      // as `$ref` is not permitted there
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_3_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_3_Hyper, {}, Other);
      RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_Draft_3_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("method", Known::JSON_Schema_Draft_3_Hyper, {}, Other);
      RETURN_IF_MATCHES("enctype", Known::JSON_Schema_Draft_3_Hyper, {}, Other);
    }

    // $ref takes precedence over any unknown keyword
    static const SchemaWalkerResult unknown_with_ref(
        SchemaKeywordType::Unknown, std::nullopt, {"$ref"}, {});
    return unknown_with_ref;
  }

  // Draft2
  if (vocabularies.contains(Known::JSON_Schema_Draft_2) ||
      vocabularies.contains(Known::JSON_Schema_Draft_2_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("id", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("items", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueOrElementsTraverseAnyItemOrItem);
    RETURN_IF_MATCHES_EITHER("properties", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyStatic);
    RETURN_IF_MATCHES_EITHER("additionalProperties", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseSomeProperty, "properties");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {},
                             ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_2, Known::JSON_Schema_Draft_2_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER("maximumCanEqual", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("minimumCanEqual", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("uniqueItems", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("requires", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseParent);
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::String}), Other);
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("divisibleBy", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("disallow", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER("extends", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper, {},
                             ApplicatorValueOrElementsInPlace);
    RETURN_IF_MATCHES_EITHER("contentEncoding", Known::JSON_Schema_Draft_2,
                             Known::JSON_Schema_Draft_2_Hyper,
                             make_set({JSON::Type::String}), Comment);

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_2_Hyper)) {
      RETURN_IF_MATCHES("fragmentResolution", Known::JSON_Schema_Draft_2_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("root", Known::JSON_Schema_Draft_2_Hyper, {}, Other);
      RETURN_IF_MATCHES("readonly", Known::JSON_Schema_Draft_2_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("pathStart", Known::JSON_Schema_Draft_2_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_2_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("alternate", Known::JSON_Schema_Draft_2_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_2_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_2_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_2_Hyper, {}, Other);
      RETURN_IF_MATCHES("targetSchema", Known::JSON_Schema_Draft_2_Hyper, {},
                        ApplicatorValueInPlaceOther);
      RETURN_IF_MATCHES("method", Known::JSON_Schema_Draft_2_Hyper, {}, Other);
      RETURN_IF_MATCHES("enctype", Known::JSON_Schema_Draft_2_Hyper, {}, Other);
    }
  }

  // Draft1
  if (vocabularies.contains(Known::JSON_Schema_Draft_1) ||
      vocabularies.contains(Known::JSON_Schema_Draft_1_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("id", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("items", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueOrElementsTraverseAnyItemOrItem);
    RETURN_IF_MATCHES_EITHER("properties", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyStatic);
    RETURN_IF_MATCHES_EITHER("additionalProperties", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseSomeProperty, "properties");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {},
                             ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_1, Known::JSON_Schema_Draft_1_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER("maximumCanEqual", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("minimumCanEqual", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("requires", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseParent);
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::String}), Other);
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("disallow", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER("extends", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper, {},
                             ApplicatorValueOrElementsInPlace);
    RETURN_IF_MATCHES_EITHER("contentEncoding", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::String}), Comment);
    RETURN_IF_MATCHES_EITHER("optional", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES_EITHER("maxDecimal", Known::JSON_Schema_Draft_1,
                             Known::JSON_Schema_Draft_1_Hyper,
                             make_set({JSON::Type::Real}), Assertion);

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_1_Hyper)) {
      RETURN_IF_MATCHES("fragmentResolution", Known::JSON_Schema_Draft_1_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("root", Known::JSON_Schema_Draft_1_Hyper, {}, Other);
      RETURN_IF_MATCHES("readonly", Known::JSON_Schema_Draft_1_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("pathStart", Known::JSON_Schema_Draft_1_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_1_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("alternate", Known::JSON_Schema_Draft_1_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_1_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_1_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_1_Hyper, {}, Other);
      RETURN_IF_MATCHES("method", Known::JSON_Schema_Draft_1_Hyper, {}, Other);
      RETURN_IF_MATCHES("enctype", Known::JSON_Schema_Draft_1_Hyper, {}, Other);
    }
  }

  // Draft0
  if (vocabularies.contains(Known::JSON_Schema_Draft_0) ||
      vocabularies.contains(Known::JSON_Schema_Draft_0_Hyper)) {
    RETURN_IF_MATCHES_EITHER("$schema", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("id", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Other);
    RETURN_IF_MATCHES_EITHER("$ref", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Reference);
    RETURN_IF_MATCHES_EITHER("items", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Array}),
                             ApplicatorValueOrElementsTraverseAnyItemOrItem);
    RETURN_IF_MATCHES_EITHER("properties", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorMembersTraversePropertyStatic);
    RETURN_IF_MATCHES_EITHER("additionalProperties", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseSomeProperty, "properties");
    RETURN_IF_MATCHES_EITHER("type", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {},
                             ApplicatorElementsInPlaceSome);
    RETURN_IF_MATCHES_EITHER("enum", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER(
        "maximum", Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER(
        "minimum", Known::JSON_Schema_Draft_0, Known::JSON_Schema_Draft_0_Hyper,
        make_set({JSON::Type::Integer, JSON::Type::Real}), Assertion);
    RETURN_IF_MATCHES_EITHER("maximumCanEqual", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("minimumCanEqual", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Integer, JSON::Type::Real}),
                             Assertion);
    RETURN_IF_MATCHES_EITHER("maxLength", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("minLength", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("pattern", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::String}), Assertion);
    RETURN_IF_MATCHES_EITHER("maxItems", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("minItems", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Array}), Assertion);
    RETURN_IF_MATCHES_EITHER("requires", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Object}),
                             ApplicatorValueTraverseParent);
    RETURN_IF_MATCHES_EITHER("format", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::String}), Other);
    RETURN_IF_MATCHES_EITHER("title", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("description", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("default", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Comment);
    RETURN_IF_MATCHES_EITHER("disallow", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {}, Assertion);
    RETURN_IF_MATCHES_EITHER("extends", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper, {},
                             ApplicatorValueOrElementsInPlace);
    RETURN_IF_MATCHES_EITHER("contentEncoding", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::String}), Comment);
    RETURN_IF_MATCHES_EITHER("optional", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Object}), Assertion);
    RETURN_IF_MATCHES_EITHER("maxDecimal", Known::JSON_Schema_Draft_0,
                             Known::JSON_Schema_Draft_0_Hyper,
                             make_set({JSON::Type::Real}), Assertion);

    // Hyper-schema specific keywords
    if (vocabularies.contains(Known::JSON_Schema_Draft_0_Hyper)) {
      RETURN_IF_MATCHES("fragmentResolution", Known::JSON_Schema_Draft_0_Hyper,
                        {}, Other);
      RETURN_IF_MATCHES("root", Known::JSON_Schema_Draft_0_Hyper, {}, Other);
      RETURN_IF_MATCHES("readonly", Known::JSON_Schema_Draft_0_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("pathStart", Known::JSON_Schema_Draft_0_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("mediaType", Known::JSON_Schema_Draft_0_Hyper, {},
                        Other);
      RETURN_IF_MATCHES("alternate", Known::JSON_Schema_Draft_0_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("links", Known::JSON_Schema_Draft_0_Hyper, {},
                        ApplicatorElementsInPlace);
      RETURN_IF_MATCHES("href", Known::JSON_Schema_Draft_0_Hyper, {}, Other);
      RETURN_IF_MATCHES("rel", Known::JSON_Schema_Draft_0_Hyper, {}, Other);
      RETURN_IF_MATCHES("method", Known::JSON_Schema_Draft_0_Hyper, {}, Other);
      RETURN_IF_MATCHES("enctype", Known::JSON_Schema_Draft_0_Hyper, {}, Other);
    }
  }

#undef MATCHES
#undef RETURN
#undef RETURN_IF_MATCHES
#undef RETURN_IF_MATCHES_EITHER
  static const SchemaWalkerResult unknown(SchemaKeywordType::Unknown,
                                          std::nullopt, {}, {});
  return unknown;
}

} // namespace sourcemeta::core
