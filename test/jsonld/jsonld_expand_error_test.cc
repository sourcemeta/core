#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <cstddef> // std::size_t
#include <iostream>
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <typeinfo>
#include <utility> // std::move

#define EXPECT_JSONLD_EXPAND_ERROR(expression, expected_code,                  \
                                   expected_pointer)                           \
  try {                                                                        \
    [[maybe_unused]] const auto result{expression};                            \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::JSONLDError &error) {                       \
    EXPECT_STREQ(error.what(), (expected_code));                               \
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()),                    \
              (expected_pointer));                                             \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

namespace {

auto remote_resolver() -> sourcemeta::core::JSONLDResolver {
  return [](const sourcemeta::core::JSON::StringView identifier)
             -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/recursive") {
      return sourcemeta::core::parse_json(
          R"({ "@context": "https://example.com/recursive" })");
    }
    if (identifier == "https://example.com/no-context") {
      return sourcemeta::core::parse_json(R"({ "foo": "bar" })");
    }
    if (identifier == "https://example.com/invalid-term") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } } })");
    }
    if (identifier == "https://example.com/valid-term") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "a": "http://example.com/a" } })");
    }
    return std::nullopt;
  };
}

} // namespace

TEST(cyclic_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "term": { "@id": "term:term" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Cyclic IRI mapping", "/@context/term");
}

TEST(invalid_term_definition_empty) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/");
}

TEST(keyword_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(protected_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "a": "http://example.com/a" },
      { "a": "http://example.com/b" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/a");
}

TEST(invalid_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/a/@protected");
}

TEST(invalid_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a/@id");
}

TEST(invalid_keyword_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "@context" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/a/@id");
}

TEST(invalid_reverse_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@reverse": "http://example.com/a", "@id": "http://example.com/b" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/a/@reverse");
}

TEST(null_id_term_with_an_invalid_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null, "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/a/@type");
}

TEST(null_id_term_with_a_reverse_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null, "@reverse": "http://example.com/r" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/a/@reverse");
}

TEST(null_id_term_with_an_unknown_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null, "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/a/@bogus");
}

TEST(invalid_type_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/a/@type");
}

TEST(invalid_container_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@container": "@unknown" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(invalid_language_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@language": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language mapping",
                             "/@context/a/@language");
}

TEST(invalid_prefix_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@prefix": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @prefix value", "/@context/a/@prefix");
}

TEST(invalid_nest_value_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@context/a/@nest");
}

TEST(invalid_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": {
        "@id": "http://example.com/a",
        "@context": { "b": { "@id": true } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/a/@context");
}

TEST(invalid_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({ "@context": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid local context", "/@context");
}

TEST(invalid_version_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 2.0 } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(invalid_propagate_value) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@propagate": "yes" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @propagate value",
                             "/@context/@propagate");
}

TEST(invalid_import_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@import": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @import value", "/@context/@import");
}

TEST(invalid_base_iri) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@base": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(invalid_base_iri_malformed_string) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "https://example.com/%zz" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(invalid_base_iri_malformed_string_against_a_base) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "bad base" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/"),
      "Invalid base IRI", "/@context/@base");
}

TEST(invalid_vocab_mapping) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@vocab": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid vocab mapping", "/@context/@vocab");
}

TEST(invalid_default_language) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@language": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid default language", "/@context/@language");
}

TEST(invalid_base_direction) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@direction": "sideways" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/@context/@direction");
}

TEST(processing_mode_conflict) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 1.1 } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(invalid_context_entry) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@protected": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@protected");
}

TEST(loading_remote_context_failed) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/missing" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context");
}

TEST(invalid_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/no-context" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid remote context", "/@context");
}

TEST(relative_context_without_a_base_fails_to_load) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "a": "http://example.com/a" } })");
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "relative-context.jsonld" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(relative_import_without_a_base_fails_to_load) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "relative-context.jsonld" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(relative_context_resolves_against_a_base) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": "invalid-term" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/",
                                      remote_resolver()),
      "Invalid term definition", "/@context");
}

TEST(shorthand_term_definition_invalid_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "term": "notaniri" }, "term": "v" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/term");
}

TEST(shorthand_term_definition_aliasing_context) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "term": "@context" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/term");
}

TEST(shorthand_term_definition_with_vocabulary_is_valid) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/", "term": "notaniri" },
    "term": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/notaniri": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(context_overflow_on_repeated_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/recursive" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Context overflow", "/@context");
}

TEST(context_overflow_on_distinct_remote_contexts) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    auto next{sourcemeta::core::JSON::String{identifier}};
    next += "x";
    auto document{sourcemeta::core::JSON::make_object()};
    document.assign("@context", sourcemeta::core::JSON{std::move(next)});
    return document;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/chain" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver), "Context overflow",
      "/@context");
}

TEST(recursive_context_inclusion_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/recursive" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver(),
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Recursive context inclusion", "/@context");
}

TEST(error_inside_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/invalid-term" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid term definition", "/@context");
}

TEST(colliding_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "id": "@id" },
    "@id": "http://example.com/a",
    "id": "http://example.com/b"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Colliding keywords", "/id");
}

TEST(invalid_id_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@id": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/@id");
}

TEST(invalid_type_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@type": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/@type");
}

TEST(invalid_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": {
      "@value": "x", "@type": "http://example.com/t", "@language": "en"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object",
                             "/http:~1~1example.com~1p/@language");
}

TEST(invalid_language_tagged_string) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@language": true }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string",
                             "/http:~1~1example.com~1p/@language");
}

TEST(invalid_language_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "label": { "@id": "http://example.com/label", "@container": "@language" }
    },
    "label": { "en": [ "ok", 5 ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language map value", "/label/en");
}

TEST(invalid_language_tagged_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": 1, "@language": "en" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged value",
                             "/http:~1~1example.com~1p/@value");
}

TEST(invalid_typed_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "_:b" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value",
                             "/http:~1~1example.com~1p/@type");
}

TEST(invalid_typed_value_relative_datatype) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "relative" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value",
                             "/http:~1~1example.com~1p/@type");
}

TEST(json_datatype_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": { "x": 1 }, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(json_datatype_with_a_null_value_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": null, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(json_datatype_alias_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "value": "@value", "type": "@type" },
    "http://example.com/p": { "value": "x", "type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/value");
}

TEST(invalid_value_object_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": { "a": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object value",
                             "/http:~1~1example.com~1p/@value");
}

TEST(invalid_value_object_value_keyword_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "v": "@value" },
    "http://example.com/p": { "v": { "a": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object value",
                             "/http:~1~1example.com~1p/v");
}

TEST(invalid_value_object_value_with_ignored_key) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "0": 1, "@value": { "a": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object value",
                             "/http:~1~1example.com~1p/@value");
}

TEST(invalid_value_object_nested_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "labels": "@nest" },
    "http://example.com/p": {
      "@value": "x", "@type": "http://example.com/t",
      "labels": { "@language": "en" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object",
                             "/http:~1~1example.com~1p");
}

TEST(invalid_set_or_list_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@id": "http://example.com/x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p/@id");
}

TEST(invalid_set_or_list_object_with_both_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@set": [ "b" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(identifier_map_array_item_error_names_the_element) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@id" }
    },
    "p": { "urn:n": [ {}, { "@id": false } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/p/urn:n/1/@id");
}

TEST(identifier_map_object_value_error_names_the_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@id" }
    },
    "p": { "urn:n": { "@id": false } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/p/urn:n/@id");
}

TEST(resolver_jsonld_error_is_a_loading_failure) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/context" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context");
}

TEST(resolver_jsonld_error_on_an_import_is_a_loading_failure) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/context" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
}

TEST(invalid_index_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@index": true, "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @index value",
                             "/http:~1~1example.com~1p/@index");
}

TEST(invalid_reverse_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@reverse": "foo" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @reverse value", "/@reverse");
}

TEST(invalid_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "http://example.com/p": { "@value": "x" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/http:~1~1example.com~1p");
}

TEST(invalid_included_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@included": { "@value": "x" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included");
}

TEST(invalid_nest_value_expansion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "nest": "@nest" },
    "nest": { "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/nest");
}

TEST(invalid_nest_value_aliased_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "value": "@value" },
    "@nest": { "value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest");
}

TEST(invalid_nest_value_aliased_value_from_a_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "nest": { "@id": "@nest", "@context": { "value": "@value" } }
    },
    "nest": { "value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/nest");
}

TEST(invalid_nest_value_aliased_value_in_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "value": "@value", "nest": "@nest" },
    "nest": [ {}, { "value": "x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/nest/1");
}

TEST(list_of_lists) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ { "@list": [ "a" ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/http:~1~1example.com~1p/@list");
}

TEST(invalid_base_direction_in_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "up" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/p/@direction");
}

TEST(keyword_alias_dropped_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "none": "@none" },
    "@reverse": { "none": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/none");
}

TEST(colliding_type_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "type": "@type" },
    "@type": "http://example.com/A",
    "type": "http://example.com/B"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Colliding keywords", "/type");
}

TEST(invalid_language_tagged_string_without_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@language": 42 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string", "/p/@language");
}

TEST(protected_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@protected");
}

TEST(nest_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@nest" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@nest");
}

TEST(prefix_on_compact_iri_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex": "http://example.com/",
      "ex:foo": { "@id": "http://example.com/foo", "@prefix": true }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition",
                             "/@context/ex:foo/@prefix");
}

TEST(invalid_base_direction_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@direction": "up" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction",
                             "/@context/a/@direction");
}

TEST(invalid_container_array_combination) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@id", "@language" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(invalid_container_set_with_multiple_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": {
        "@id": "http://example.com/a",
        "@container": [ "@set", "@id", "@language" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(unknown_entry_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/a/@bogus");
}

TEST(type_keyword_container_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(invalid_version_precedes_mode_conflict) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.5 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid @version value", "/@context/@version");
}

TEST(version_with_an_extreme_exponent) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 12345678901234567890123456789e2147483600 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(exponent_version_conflicts_with_the_mode_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1e0 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(relative_base_without_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@base": null }, { "@base": "relative/path" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/1/@base");
}

TEST(protected_null_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "term": { "@id": null, "@protected": true } },
      { "term": "http://example.com/x" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/term");
}

TEST(free_floating_invalid_set_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@set": [ "foo" ], "@id": "http://example.com/bar"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/@id");
}

TEST(import_loading_failed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/unknown" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(error_inside_imported_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/invalid-term" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid term definition", "/@context/@import");
}

TEST(error_beside_imported_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/valid-term",
      "b": { "@id": "http://example.com/b", "@bogus": true }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid term definition", "/@context/b/@bogus");
}

TEST(duplicate_container_keyword) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@set", "@set" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(error_code_value_is_owned) {
  std::string code{"A custom error code longer than small string optimization"};
  const sourcemeta::core::JSONLDError error{code.c_str(),
                                            sourcemeta::core::Pointer{}};
  code = std::string{};
  EXPECT_STREQ(error.what(),
               "A custom error code longer than small string optimization");
}

TEST(type_redefinition_with_non_boolean_protected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@set", "@protected": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/@type/@protected");
}

TEST(type_redefinition_with_invalid_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@list" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(null_term_with_non_boolean_protected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "term": { "@id": null, "@protected": "yes" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/term/@protected");
}

TEST(literal_keyword_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@id": "http://example.com/a" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/@id");
}

TEST(nested_reverse_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@reverse": { "http://example.com/p": "x" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map",
                             "/@reverse/@reverse");
}

TEST(nest_array_element_error_names_the_element) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1, "n": "@nest" },
    "n": [ { "http://e/a": "x" }, { "@id": 1 } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/n/1/@id");
}

TEST(nest_object_error_names_the_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1, "n": "@nest" },
    "n": { "@id": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/n/@id");
}

TEST(invalid_nest_value_in_array_names_the_element) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1, "n": "@nest" },
    "n": [ { "http://e/a": "x" }, "scalar" ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/n/1");
}

TEST(reverse_map_value_object_names_the_input_key) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://e/p" },
    "@reverse": { "p": { "@value": "v" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/p");
}

TEST(malformed_context_reference_fails_to_load) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": "relative%ZZ" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Loading document failed", "/@context");
}

TEST(malformed_import_reference_fails_to_load) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "relative%ZZ" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Loading remote context failed",
                             "/@context/@import");
}

TEST(scoped_relative_context_is_invalid) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "t": { "@id": "http://e/t", "@context": "relative-ctx.jsonld" }
    },
    "t": { "http://e/x": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/t/@context");
}

TEST(scoped_unresolvable_context_is_invalid) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "t": { "@id": "http://e/t", "@context": "https://example.com/unknown" }
    },
    "t": { "http://e/x": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/t/@context");
}

TEST(scoped_unresolvable_context_of_an_unused_term_is_invalid) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "t": { "@id": "http://e/t", "@context": "https://example.com/unknown" }
    },
    "http://e/x": 1
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/t/@context");
}

TEST(scoped_context_without_a_context_entry_is_invalid) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "t": { "@id": "http://e/t", "@context": "https://example.com/no-context" }
    },
    "t": { "http://e/x": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/t/@context");
}

TEST(reverse_map_with_ambiguous_aliases_names_the_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "http://e/p", "b": "http://e/p" },
    "@reverse": { "a": { "@id": "http://e/s" }, "b": { "@value": "v" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse");
}

TEST(reverse_term_with_an_array_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "r": { "@reverse": "http://example.com/r", "@container": [ "@set" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/r/@container");
}

TEST(reverse_term_with_a_list_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "r": { "@reverse": "http://example.com/r", "@container": "@list" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/r/@container");
}

TEST(reverse_map_error_uses_the_local_context_of_the_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": {
      "@context": { "a": "http://example.com/p" },
      "a": { "@value": "v" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/a");
}

TEST(reverse_map_error_under_a_protected_type_scope) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@version": 1.1,
      "T": {
        "@id": "https://example.com/T",
        "@context": { "@protected": true, "a": "https://example.com/a" }
      }
    },
    "@type": "T",
    "@reverse": {
      "@context": null,
      "https://example.com/p": { "@value": "v" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/https:~1~1example.com~1p");
}

TEST(direction_in_a_context_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@direction": "ltr" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@direction");
}

TEST(malformed_context_reference_fails_to_load_against_a_base) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/%zz" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/"),
      "Loading document failed", "/@context");
}

TEST(malformed_import_reference_fails_to_load_against_a_base) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/%zz" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/"),
      "Loading remote context failed", "/@context/@import");
}

TEST(context_reference_against_a_malformed_base_fails_to_load) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": "context.jsonld" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, "bad base"),
                             "Loading document failed", "/@context");
}
