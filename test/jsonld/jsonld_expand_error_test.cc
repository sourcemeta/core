#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <utility>  // std::move

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
    if (identifier == "https://example.com/import-invalid") {
      return sourcemeta::core::parse_json(R"({ "@context": { "a": "bad" } })");
    }
    if (identifier == "https://example.com/import-language") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@language": 42 } })");
    }
    if (identifier == "https://example.com/import-vocab") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@vocab": 42 } })");
    }
    if (identifier == "https://example.com/import-protected") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "@protected": true,
          "a": "urn:a",
          "I": { "@id": "urn:I", "@context": { "a": "urn:imported" } }
        }
      })");
    }
    if (identifier == "https://example.com/chain-a") {
      return sourcemeta::core::parse_json(
          R"({ "@context": [ "https://example.com/chain-b" ] })");
    }
    if (identifier == "https://example.com/chain-b") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "@protected": true,
          "a": "urn:a",
          "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
        }
      })");
    }
    if (identifier == "https://example.com/scoped-missing") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": "https://example.com/missing"
          }
        }
      })");
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
                             "Invalid language map value", "/label/en/1");
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
      "List of lists", "/http:~1~1example.com~1p/@list/0");
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
                             "/@context/a/@container/1");
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

TEST(reverse_map_with_ambiguous_aliases_names_the_offender) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "http://e/p", "b": "http://e/p" },
    "@reverse": { "a": { "@id": "http://e/s" }, "b": { "@value": "v" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/b");
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

TEST(invalid_base_precedes_the_direction_entry_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": true, "@direction": "ltr" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid base IRI", "/@context/@base");
}

TEST(invalid_vocabulary_precedes_the_propagate_entry_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@vocab": true, "@propagate": false } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid vocab mapping", "/@context/@vocab");
}

TEST(the_import_entry_precedes_the_base_entry_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": true, "@import": "ctx.jsonld" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@import");
}

TEST(keyword_reverse_term_still_validates_its_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@id", "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(keyword_reverse_term_still_validates_its_protected_flag) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@id", "@protected": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/p/@protected");
}

TEST(invalid_type_mapping_precedes_an_invalid_identifier) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": true, "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(nest_in_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@nest": {} }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/@nest");
}

TEST(aliased_nest_in_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "nest": "@nest" },
    "@reverse": { "nest": {} }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/nest");
}

TEST(deferred_external_context_error_at_root) {
  const auto context = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@context": { "@propagate": "not-a-boolean" }
      }
    }
  })");

  const auto input = sourcemeta::core::parse_json(R"({
    "p": { "https://example.com/q": "value" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid scoped context", "");
}

TEST(empty_type_keyword_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": {} },
    "@type": "urn:T"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(null_nest_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@nest": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest");
}

TEST(null_aliased_nest_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "https://example.com/p": "v",
    "n": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/n");
}

TEST(empty_type_keyword_definition_after_a_protected_one) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@type": { "@protected": true } },
      { "@type": {} }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/1/@type");
}

TEST(type_keyword_definition_inherits_context_protected) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "@type": { "@container": "@set" } },
      null
    ],
    "https://example.com/p": "value"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid context nullification", "/@context/1");
}

TEST(cyclic_iri_mapping_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "b:x", "b": "a:x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Cyclic IRI mapping", "/@context/a");
}

TEST(invalid_iri_mapping_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "bad" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a");
}

TEST(invalid_keyword_alias_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "@context" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/a");
}

TEST(malformed_base_with_space) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "bad base" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(decimal_version_in_1_0) {
  auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v"
  })");
  input.at("@context")
      .assign("@version",
              sourcemeta::core::JSON{sourcemeta::core::Decimal{"1.1"}});

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(invalid_context_entry_import) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/ctx" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@import");
}

TEST(invalid_context_entry_propagate) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@propagate": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@propagate");
}

TEST(error_inside_expansion_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "http://example.com/a", "@bogus": true }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid term definition", "");
}

TEST(error_inside_wrapped_expansion_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid term definition", "");
}

TEST(relative_context_without_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/p" } })");
  };

  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": "context.jsonld" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  // The resolver contract only admits absolute IRIs, so even a permissive
  // resolver must never see the unresolved relative reference
  EXPECT_EQ(invocations, 0);
}

TEST(relative_import_without_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/p" } })");
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "context.jsonld" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(imported_invalid_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-invalid" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/@import");
}

TEST(imported_invalid_mapping_in_context_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@import": "https://example.com/import-invalid" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/0/@import");
}

TEST(local_override_of_imported_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-invalid",
      "a": { "@id": true }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/a/@id");
}

TEST(scoped_context_error_from_remote_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/scoped-missing",
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context");
}

TEST(scoped_context_error_from_local_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/missing"
      }
    },
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(scoped_context_error_from_unused_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/missing"
      }
    },
    "http://example.com/q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(scoped_context_without_context_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/no-context"
      }
    },
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(aliased_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "@reverse": { "p": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/p");
}

TEST(merged_aliased_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": "http://example.com/p",
      "b": "http://example.com/p"
    },
    "@reverse": {
      "a": { "@id": "http://example.com/ok" },
      "b": "x"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/b");
}

TEST(aliased_reverse_keyword_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "rev": "@reverse" },
    "rev": { "http://example.com/p": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/rev/http:~1~1example.com~1p");
}

TEST(keyword_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@id": "http://example.com/x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/@id");
}

TEST(aliased_value_inside_nest) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "v": "@value" },
    "@nest": { "v": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest");
}

TEST(aliased_value_inside_aliased_nest) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "v": "@value", "data": "@nest" },
    "data": { "v": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/data");
}

TEST(invalid_id_inside_nest_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@nest": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/@nest/1/@id");
}

TEST(invalid_id_inside_aliased_nest_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "data": "@nest" },
    "data": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/data/1/@id");
}

TEST(explicit_json_literal_scalar_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": 1, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(explicit_json_literal_aliased_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "val": "@value", "type": "@type" },
    "http://example.com/p": { "val": { "x": 1 }, "type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/val");
}

TEST(resolver_jsonld_error_translated_in_context_array) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": [ {}, "https://example.com/context" ] })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/1");
}

TEST(imported_invalid_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-language" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid default language", "/@context/@import");
}

TEST(imported_invalid_vocab) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-vocab" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid vocab mapping", "/@context/@import");
}

TEST(imported_invalid_vocab_in_context_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@import": "https://example.com/import-vocab" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid vocab mapping", "/@context/0/@import");
}

TEST(local_override_of_imported_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-language",
      "@language": 42
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid default language", "/@context/@language");
}

TEST(invalid_local_context_under_scoped_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "@propagate": false, "a": "http://example.com/a" }
      }
    },
    "p": { "@context": false, "a": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid local context", "/p/@context");
}

TEST(reverse_term_list_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": "@list" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/p/@container");
}

TEST(local_scoped_context_base_is_validated) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@context": { "@base": 42 } }
    },
    "http://example.com/q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/p/@context");
}

TEST(reverse_map_error_uses_its_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": {
      "@context": { "p": "https://example.com/p" },
      "p": { "@value": 42 }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/p");
}

TEST(reverse_map_error_without_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "https://example.com/p": { "@value": 42 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/https:~1~1example.com~1p");
}

TEST(protected_nest_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "p": {
          "@id": "https://example.com/p",
          "@protected": true,
          "@nest": "meta"
        }
      },
      { "p": { "@id": "https://example.com/p", "@nest": "other" } }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/p");
}

TEST(nearby_version_number) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1000000005 },
    "https://example.com/p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(nearby_version_number_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1000000005 },
    "https://example.com/p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid @version value", "/@context/@version");
}

TEST(invalid_nest_member_with_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "n": [ {}, "bad" ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/n/1");
}

TEST(invalid_nest_member_literal) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@nest": [ {}, "bad" ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest/1");
}

TEST(nest_member_error_keeps_item_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "n": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/n/1/@id");
}

TEST(malformed_expansion_context_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({ "@base": 42 })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid base IRI", "");
}

TEST(malformed_context_reference) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/%zz" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(malformed_context_reference_with_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/%zz" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(malformed_context_reference_in_array) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": [ {}, "https://example.com/%zz" ] })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context/1");
  EXPECT_EQ(invocations, 0);
}

TEST(malformed_import_reference) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/%zz" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(context_direction_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@direction": "rtl", "p": "https://example.com/p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@direction");
}

TEST(graph_container_with_both_map_kinds) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@id", "@index" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(graph_container_with_both_map_kinds_and_set) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@id", "@index", "@set" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(protected_context_nullification_mid_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@protected": true, "a": "urn:a" }, null ],
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid context nullification", "/@context/1");
}

TEST(invalid_member_in_array_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:foo": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/urn:foo/1/@id");
}

TEST(invalid_context_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@protected": "yes", "p": "urn:p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/@protected");
}

TEST(invalid_context_protected_value_in_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ {}, { "@protected": 1, "p": "urn:p" } ],
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/1/@protected");
}

TEST(null_context_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@protected": null, "p": "urn:p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/@protected");
}

TEST(deferred_local_scoped_error_keeps_term_pointer) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@protected": true,
      "a": "http://example.com/a",
      "Type": {
        "@id": "http://example.com/Type",
        "@context": { "a": "http://example.com/other" }
      }
    },
    "@type": "Type",
    "a": "foo"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition",
                             "/@context/Type/@context/a");
}

TEST(imported_dependency_error_points_to_import) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-dependency") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "z": { "@id": 12 } } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-dependency",
      "a": "z"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid IRI mapping", "/@context/@import");
}

TEST(index_map_member_keeps_original_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i"
      }
    },
    "p": { "x": [ null, { "@value": "bad" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1");
}

TEST(index_map_single_member_keeps_entry_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i"
      }
    },
    "p": { "x": { "@value": "bad" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x");
}

TEST(included_array_member_keeps_original_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [ { "@id": "urn:n" }, { "@value": "x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1");
}

TEST(aliased_included_array_member_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "inc": "@included" },
    "https://example.com/p": "v",
    "inc": [ { "@id": "urn:n" }, { "@value": "x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/inc/1");
}

TEST(nested_external_context_error_at_root) {
  const auto input = sourcemeta::core::parse_json(R"({
    "p": { "@type": "T" }
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "urn:a", "@protected": true },
    "p": {
      "@id": "urn:p",
      "@context": {
        "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(nested_external_protected_scope_error_at_root) {
  const auto input = sourcemeta::core::parse_json(R"({
    "p": { "@type": "T" }
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "urn:p",
      "@context": {
        "@protected": true,
        "a": "urn:a",
        "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(invalid_type_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/@type/1");
}

TEST(invalid_aliased_type_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": "@type" },
    "t": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/t/1");
}

TEST(invalid_type_member_keeps_index_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid type value", "/@type/1");
}

TEST(invalid_reverse_term_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": [ { "@id": "urn:a" }, { "@value": 1 } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/1");
}

TEST(invalid_reverse_map_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "urn:p": [ { "@id": "urn:a" }, { "@value": 1 } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/urn:p/1");
}

TEST(included_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [ { "@id": "urn:ok" }, [ { "@id": "urn:n" }, { "@value": "x" } ] ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1/1");
}

TEST(included_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [
      { "@id": "urn:ok" },
      { "@set": [ { "@id": "urn:n" }, { "@value": "x" } ] }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1/@set/1");
}

TEST(equivalent_protected_redefinition_retains_origin) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "urn:a", "@protected": true },
      "T": {
        "@id": "urn:T",
        "@protected": true,
        "@context": { "a": "urn:other" }
      }
    },
    "@type": "T",
    "a": "x"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "urn:a", "@protected": true },
    "T": {
      "@id": "urn:T",
      "@protected": true,
      "@context": { "a": "urn:other" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(reverse_definition_rejects_unknown_entries) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "urn:p", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/@bogus");
}

TEST(index_map_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": [ null, [ null, { "@value": "bad" } ] ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1/1");
}

TEST(index_map_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": [ null, { "@set": [ null, { "@value": "bad" } ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1/@set/1");
}

TEST(index_map_direct_set_value_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": { "@set": [ null, { "@value": "bad" } ] } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/@set/1");
}

TEST(reverse_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": [ null, [ { "@value": 1 } ] ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/1/0");
}

TEST(reverse_set_value_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": { "@set": [ null, { "@value": "x" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/@set/1");
}

TEST(reverse_map_nested_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "urn:p": [ null, [ { "@value": 1 } ] ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/urn:p/1/0");
}

TEST(legacy_list_container_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": "@list" } },
    "p": [ null, { "@list": [ "x" ] } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/p/1");
}

TEST(protected_type_empty_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@type": { "@protected": true } }, { "@type": {} } ],
    "@type": "urn:T"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/1/@type");
}

TEST(imported_term_local_dependency_keeps_location) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-local-dependency") {
      return sourcemeta::core::parse_json(R"({ "@context": { "a": "z" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-local-dependency",
      "z": { "@id": 42 }
    },
    "a": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid IRI mapping", "/@context/z/@id");
}

TEST(blank_node_remote_context_rejected) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(R"({ "@context": {} })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "_:ctx"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(blank_node_import_rejected) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(R"({ "@context": {} })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "_:ctx" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(included_set_alias_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": {
      "@context": { "s": "@set" },
      "s": [ null, { "@value": "x" } ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/s/1");
}

TEST(included_set_alias_member_after_valid_node) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": {
      "@context": { "s": "@set" },
      "s": [ { "@id": "urn:n" }, { "@value": "x" } ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/s/1");
}

TEST(invalid_container_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": [ "@set", false ] } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container/1");
}

TEST(unresolved_property_valued_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "relative" }
    },
    "p": { "x": {} }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/@index");
}

TEST(ignored_type_alias_preserves_provenance) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "urn:", "a": "@type", "z": "@type" },
    "urn:p": { "@value": 1, "a": "_:blank", "z": "@ignore" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/a");
}

TEST(relative_base_without_caller_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@base": "relative/path" },
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(relative_base_in_second_context_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ null, { "@base": "relative/path" } ],
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/1/@base");
}

TEST(relative_base_in_expand_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@base": "relative/path"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid base IRI", "");
}

TEST(explicit_list_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@list": [ { "@set": [ null, { "@list": [ "x" ] } ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/urn:p/@list/0/@set/1");
}

TEST(unknown_term_entries_lexicographic_reversed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "a": false, "z": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/a");
}

TEST(imported_name_keeps_nested_scope_validation) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-collision") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "urn:imported" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-collision",
      "T": {
        "@id": "urn:T",
        "@context": {
          "p": { "@id": "urn:local", "@context": { "@base": 42 } }
        }
      }
    },
    "urn:q": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid scoped context", "/@context/T/@context");
}

TEST(nested_scope_validation_without_import) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "T": {
        "@id": "urn:T",
        "@context": {
          "p": { "@id": "urn:local", "@context": { "@base": 42 } }
        }
      }
    },
    "urn:q": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/T/@context");
}

TEST(nested_shape_pointer) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "urn:p": { "n": { "@list": [ "x" ], "@set": [ "y" ] } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/n");
}

TEST(nested_shape_pointer_array_member) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "urn:p": {
      "n": [
        { "urn:q": "v" },
        { "@list": [ "x" ], "@set": [ "y" ] }
      ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/n/1");
}

TEST(invalid_array_datatype_on_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@value": "x", "@type": [ "@foo" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/@type");
}

TEST(invalid_array_datatype_on_aliased_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": "@type" },
    "urn:p": { "@value": "x", "t": [ "@foo" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/t");
}

TEST(empty_container_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": [] } },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(invalid_type_with_keyword_form_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "@foo", "@type": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(invalid_type_with_keyword_form_reverse) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@future", "@type": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(null_set_inside_nest_keeps_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@nest": { "@set": null, "@type": "urn:T" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/@nest");
}

TEST(null_set_inside_nest_array_keeps_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@nest": [ { "@set": null, "@type": "urn:T" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/@nest/0");
}

TEST(reverse_alias_errors_report_lexicographically) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "urn:p", "z": "urn:p" },
    "@reverse": { "z": 1, "a": 2 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/a");
}

TEST(reverse_alias_lexicographic_reversed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "urn:p", "z": "urn:p" },
    "@reverse": { "a": 2, "z": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/a");
}

TEST(vocab_mapping_that_cannot_be_resolved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "bad%zz" },
    "name": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/", {}),
      "Invalid vocab mapping", "/@context/@vocab");
}

TEST(index_map_member_scope_redefines_a_protected_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@protected": true,
      "x": "urn:x",
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i",
        "@context": { "x": "urn:scoped" }
      }
    },
    "p": { "k": { "@context": { "x": "urn:local" }, "@value": "v" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/k");
}

TEST(index_map_scoped_set_alias_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i",
        "@context": { "s": "@set" }
      }
    },
    "p": { "k": { "s": [ { "@id": "urn:a" }, { "@value": "v" } ] } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/k/s/1");
}

TEST(deferred_scope_from_a_nested_remote_context_reports_at_the_reference) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/chain-a",
    "@type": "T",
    "a": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Protected term redefinition", "/@context");
}

TEST(deferred_scope_of_an_imported_term_reports_at_the_import) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-protected" },
    "@type": "I",
    "a": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Protected term redefinition", "/@context/@import");
}

TEST(deferred_scope_of_a_local_term_beside_an_import_keeps_its_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-protected",
      "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
    },
    "@type": "T",
    "a": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Protected term redefinition", "/@context/T/@context/a");
}

TEST(included_null_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": "v",
    "@included": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included");
}

TEST(included_aliased_null_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "inc": "@included" },
    "urn:p": "v",
    "inc": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/inc");
}

TEST(included_value_object_with_a_null_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": { "@value": null }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included");
}
