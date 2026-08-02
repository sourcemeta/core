#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#define EXPECT_JSONLD_FRAGMENT_ERROR(expression, expected_code,                \
                                     expected_pointer, expected_key)           \
  try {                                                                        \
    [[maybe_unused]] const auto result{expression};                            \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::JSONLDFragmentError &error) {               \
    EXPECT_STREQ(error.what(), (expected_code));                               \
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()),                    \
              (expected_pointer));                                             \
    EXPECT_EQ(error.key(), (expected_key));                                    \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

TEST(canonicalize_empty_fragment) {
  const auto fragment{sourcemeta::core::parse_json(R"({})")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({})")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_non_object_fragment_array) {
  const auto fragment{sourcemeta::core::parse_json(R"([])")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment must be an object", "", "");
}

TEST(canonicalize_non_object_fragment_string) {
  const auto fragment{sourcemeta::core::parse_json(R"("foo")")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment must be an object", "", "");
}

TEST(canonicalize_non_object_fragment_null) {
  const auto fragment{sourcemeta::core::parse_json(R"(null)")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment must be an object", "", "");
}

TEST(canonicalize_bare_string_literal) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/status": "REC"
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/status": [ { "@value": "REC" } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_bare_number_literal) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/decimals": 2
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/decimals": [ { "@value": 2 } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_bare_boolean_literal) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/active": true
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/active": [ { "@value": true } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_node_reference) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": { "@id": "https://example.com/metre" }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": [ { "@id": "https://example.com/metre" } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_value_object_passthrough) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/label": { "@value": "metre", "@language": "en" }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/label": [ { "@value": "metre", "@language": "en" } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_typed_string_literal_passthrough) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/factor": {
      "@value": "0.1",
      "@type": "http://www.w3.org/2001/XMLSchema#decimal"
    }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/factor": [
      {
        "@value": "0.1",
        "@type": "http://www.w3.org/2001/XMLSchema#decimal"
      }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_typed_fractional_native_rewritten) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/factor": {
      "@value": 1.85,
      "@type": "http://www.w3.org/2001/XMLSchema#decimal"
    }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/factor": [
      {
        "@value": "1.85",
        "@type": "http://www.w3.org/2001/XMLSchema#decimal"
      }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_typed_integer_native_rewritten) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/count": {
      "@value": 2,
      "@type": "http://www.w3.org/2001/XMLSchema#byte"
    }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/count": [
      { "@value": "2", "@type": "http://www.w3.org/2001/XMLSchema#byte" }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_typed_boolean_native_rewritten) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/flag": {
      "@value": true,
      "@type": "http://www.w3.org/2001/XMLSchema#boolean"
    }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/flag": [
      {
        "@value": "true",
        "@type": "http://www.w3.org/2001/XMLSchema#boolean"
      }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_untyped_native_number_kept) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/decimals": { "@value": 2 }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/decimals": [ { "@value": 2 } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_array_of_terms) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [
      { "@id": "https://example.com/a" },
      { "@id": "https://example.com/b" }
    ]
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [
      { "@id": "https://example.com/a" },
      { "@id": "https://example.com/b" }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_type_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "@type": "https://example.com/Quantity"
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment cannot declare node types", "/@type", "@type");
}

TEST(canonicalize_rdf_type_predicate_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": {
      "@id": "https://example.com/Quantity"
    }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment cannot declare node types",
      "/http:~1~1www.w3.org~11999~102~122-rdf-syntax-ns#type",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
}

TEST(canonicalize_id_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "@id": "https://example.com/thing"
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment cannot declare a node identifier", "/@id", "@id");
}

TEST(canonicalize_keyword_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/context"
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment key cannot be a keyword", "/@context", "@context");
}

TEST(canonicalize_relative_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "unit": { "@id": "https://example.com/metre" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment key must be an absolute IRI", "/unit", "unit");
}

TEST(canonicalize_blank_node_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "_:b0": { "@id": "https://example.com/metre" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment key must be an absolute IRI", "/_:b0", "_:b0");
}

TEST(canonicalize_empty_key_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "": "foo"
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment key must be an absolute IRI", "/", "");
}

TEST(canonicalize_array_of_bare_scalars) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/tags": [ "alpha", "beta" ]
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/tags": [
      { "@value": "alpha" },
      { "@value": "beta" }
    ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_null_term_in_array_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [ null ]
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment term must be a scalar, a node reference, or a "
      "value object",
      "/https:~1~1example.com~1see/0", "https://example.com/see");
}

TEST(canonicalize_nested_array_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [ [ "nested" ] ]
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment term must be a scalar, a node reference, or a "
      "value object",
      "/https:~1~1example.com~1see/0", "https://example.com/see");
}

TEST(canonicalize_node_object_with_properties_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": {
      "@id": "https://example.com/a",
      "https://example.com/comment": "x"
    }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A node reference can only carry an identifier",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_blank_node_reference_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@id": "_:b0" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A node reference identifier must be an absolute IRI",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_relative_reference_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@id": "../metre" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A node reference identifier must be an absolute IRI",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_list_term_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@list": [ 1, 2 ] }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment term must be a scalar, a node reference, or a "
      "value object",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_set_term_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@set": [ 1 ] }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A constants fragment term must be a scalar, a node reference, or a "
      "value object",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_null_value_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": null }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object value must be a non-null scalar",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_array_value_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": [ 1 ] }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object value must be a non-null scalar",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_type_and_language_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": {
      "@value": "x",
      "@type": "https://example.com/t",
      "@language": "en"
    }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object cannot combine a type and a language",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_extra_member_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": "x", "@index": "i" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object can only carry a value, a type, and a language",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_language_on_number_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": 1, "@language": "en" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object language requires a string value",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_non_canonical_language_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": "x", "@language": "en-us" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object language must be a canonical BCP 47 language tag",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_json_type_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": "x", "@type": "@json" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object type cannot be the JSON literal type",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_value_object_relative_type_rejected) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": { "@value": "x", "@type": "decimal" }
  })")};
  EXPECT_JSONLD_FRAGMENT_ERROR(
      sourcemeta::core::jsonld_canonicalize_fragment(fragment),
      "A value object type must be an absolute IRI",
      "/https:~1~1example.com~1see", "https://example.com/see");
}

TEST(canonicalize_sorts_keys) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/b": "second",
    "https://example.com/a": "first"
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  EXPECT_EQ(result.as_object().cbegin()->first, "https://example.com/a");
}

TEST(canonicalize_dedupes_identical_terms) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [
      { "@id": "https://example.com/a" },
      { "@id": "https://example.com/a" }
    ]
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [ { "@id": "https://example.com/a" } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_dedupes_bare_scalar_against_value_object) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [ 1, { "@value": 1 } ]
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [ { "@value": 1 } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_null_entry_passes_through) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": null
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": null
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_empty_array_entry_dropped) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/see": [],
    "https://example.com/unit": { "@id": "https://example.com/metre" }
  })")};
  const auto result{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto expected{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": [ { "@id": "https://example.com/metre" } ]
  })")};
  EXPECT_EQ(result, expected);
}

TEST(canonicalize_is_idempotent) {
  const auto fragment{sourcemeta::core::parse_json(R"({
    "https://example.com/unit": { "@id": "https://example.com/metre" },
    "https://example.com/factor": {
      "@value": 1.85,
      "@type": "http://www.w3.org/2001/XMLSchema#decimal"
    },
    "https://example.com/tags": [ "alpha", "alpha", "beta" ]
  })")};
  const auto once{sourcemeta::core::jsonld_canonicalize_fragment(fragment)};
  const auto twice{sourcemeta::core::jsonld_canonicalize_fragment(once)};
  EXPECT_EQ(once, twice);
}
