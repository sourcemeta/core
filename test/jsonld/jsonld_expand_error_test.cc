#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

#include <optional> // std::optional, std::nullopt
#include <string>   // std::string

#define EXPECT_JSONLD_EXPAND_ERROR(expression, expected_code,                  \
                                   expected_pointer)                           \
  try {                                                                        \
    [[maybe_unused]] const auto result{expression};                            \
    FAIL() << "Expected JSON-LD error: " << (expected_code);                   \
  } catch (const sourcemeta::core::JSONLDError &error) {                       \
    EXPECT_STREQ(error.what(), (expected_code));                               \
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()),                    \
              (expected_pointer));                                             \
  } catch (...) {                                                              \
    FAIL() << "Expected a JSONLDError: " << (expected_code);                   \
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
    return std::nullopt;
  };
}

} // namespace

TEST(JSONLD_expand_error, cyclic_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "term": { "@id": "term:term" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Cyclic IRI mapping", "/@context/term");
}

TEST(JSONLD_expand_error, invalid_term_definition_empty) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context");
}

TEST(JSONLD_expand_error, keyword_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(JSONLD_expand_error, protected_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "a": "http://example.com/a" },
      { "a": "http://example.com/b" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/a");
}

TEST(JSONLD_expand_error, invalid_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/a/@protected");
}

TEST(JSONLD_expand_error, invalid_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a/@id");
}

TEST(JSONLD_expand_error, invalid_keyword_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "@context" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/a/@id");
}

TEST(JSONLD_expand_error, invalid_reverse_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@reverse": "http://example.com/a", "@id": "http://example.com/b" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/a/@reverse");
}

TEST(JSONLD_expand_error, invalid_type_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/a/@type");
}

TEST(JSONLD_expand_error, invalid_container_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@container": "@unknown" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, invalid_language_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@language": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language mapping",
                             "/@context/a/@language");
}

TEST(JSONLD_expand_error, invalid_prefix_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@prefix": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @prefix value", "/@context/a/@prefix");
}

TEST(JSONLD_expand_error, invalid_nest_value_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@context/a/@nest");
}

TEST(JSONLD_expand_error, invalid_scoped_context) {
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

TEST(JSONLD_expand_error, invalid_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({ "@context": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid local context", "/@context");
}

TEST(JSONLD_expand_error, invalid_version_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 2.0 } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, invalid_propagate_value) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@propagate": "yes" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @propagate value",
                             "/@context/@propagate");
}

TEST(JSONLD_expand_error, invalid_import_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@import": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @import value", "/@context/@import");
}

TEST(JSONLD_expand_error, invalid_base_iri) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@base": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(JSONLD_expand_error, invalid_vocab_mapping) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@vocab": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid vocab mapping", "/@context/@vocab");
}

TEST(JSONLD_expand_error, invalid_default_language) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@language": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid default language", "/@context/@language");
}

TEST(JSONLD_expand_error, invalid_base_direction) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@direction": "sideways" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/@context/@direction");
}

TEST(JSONLD_expand_error, processing_mode_conflict) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 1.1 } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(JSONLD_expand_error, invalid_context_entry) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@protected": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context");
}

TEST(JSONLD_expand_error, loading_remote_context_failed) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/missing" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context");
}

TEST(JSONLD_expand_error, invalid_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/no-context" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid remote context", "/@context");
}

TEST(JSONLD_expand_error, recursive_context_inclusion) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/recursive" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Recursive context inclusion", "/@context");
}

TEST(JSONLD_expand_error, colliding_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "id": "@id" },
    "@id": "http://example.com/a",
    "id": "http://example.com/b"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Colliding keywords", "/id");
}

TEST(JSONLD_expand_error, invalid_id_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@id": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/@id");
}

TEST(JSONLD_expand_error, invalid_type_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@type": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/@type");
}

TEST(JSONLD_expand_error, invalid_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": {
      "@value": "x", "@type": "http://example.com/t", "@language": "en"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_language_tagged_string) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@language": true }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string",
                             "/http:~1~1example.com~1p/@language");
}

TEST(JSONLD_expand_error, invalid_language_tagged_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": 1, "@language": "en" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged value",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_typed_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "_:b" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_value_object_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": { "a": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object value",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_set_or_list_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@id": "http://example.com/x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_index_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@index": true, "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @index value",
                             "/http:~1~1example.com~1p/@index");
}

TEST(JSONLD_expand_error, invalid_reverse_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@reverse": "foo" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @reverse value", "/@reverse");
}

TEST(JSONLD_expand_error, invalid_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "http://example.com/p": { "@value": "x" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse");
}

TEST(JSONLD_expand_error, invalid_included_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@included": { "@value": "x" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included");
}

TEST(JSONLD_expand_error, invalid_nest_value_expansion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "nest": "@nest" },
    "nest": { "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/nest");
}

TEST(JSONLD_expand_error, list_of_lists) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ { "@list": [ "a" ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/http:~1~1example.com~1p/@list");
}

TEST(JSONLD_expand_error, invalid_base_direction_in_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "up" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/p/@direction");
}

TEST(JSONLD_expand_error, keyword_alias_dropped_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "none": "@none" },
    "@reverse": { "none": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/none");
}

TEST(JSONLD_expand_error, colliding_type_in_json_ld_1_0) {
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

TEST(JSONLD_expand_error, invalid_language_tagged_string_without_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@language": 42 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string", "/p/@language");
}

TEST(JSONLD_expand_error, protected_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@protected");
}

TEST(JSONLD_expand_error, nest_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@nest" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@nest");
}

TEST(JSONLD_expand_error, prefix_on_compact_iri_term) {
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

TEST(JSONLD_expand_error, invalid_base_direction_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@direction": "up" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction",
                             "/@context/a/@direction");
}

TEST(JSONLD_expand_error, invalid_container_array_combination) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@id", "@language" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, unknown_entry_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/a");
}

TEST(JSONLD_expand_error, type_keyword_container_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(JSONLD_expand_error, invalid_version_precedes_mode_conflict) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.5 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, relative_base_without_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@base": null }, { "@base": "relative/path" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/1/@base");
}

TEST(JSONLD_expand_error, protected_null_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "term": { "@id": null, "@protected": true } },
      { "term": "http://example.com/x" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/term");
}

TEST(JSONLD_expand_error, free_floating_invalid_set_or_list_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@list": [ "foo" ], "@id": "http://example.com/bar"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "");
}

TEST(JSONLD_expand_error, import_loading_failed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/unknown" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(JSONLD_expand_error, duplicate_container_keyword) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@set", "@set" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, error_code_value_is_owned) {
  std::string code{"A custom error code longer than small string optimization"};
  const sourcemeta::core::JSONLDError error{code.c_str(),
                                            sourcemeta::core::Pointer{}};
  code = std::string{};
  EXPECT_STREQ(error.what(),
               "A custom error code longer than small string optimization");
}
