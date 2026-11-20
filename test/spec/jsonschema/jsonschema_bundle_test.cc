#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <optional>
#include <string_view>

// A dialect names the keyword that introduces an identifier, and a walker is
// what says whether the dialect in force gives that keyword a meaning, so one
// that leaves it out is how a caller describes a dialect built on another one
// with that keyword dropped. Bundling then has no name to embed a remote
// under, and carries the dialect it was read under by marking every subschema
// of the copy instead
static constexpr sourcemeta::core::SchemaWalkerResult UNKNOWN_KEYWORD{
    sourcemeta::core::SchemaKeywordType::Unknown, std::nullopt, {}, {}, {}};

static auto walker_without_an_identifier(
    const std::string_view keyword,
    const sourcemeta::core::SchemaVocabularies &vocabularies)
    -> const sourcemeta::core::SchemaWalkerResult & {
  if (keyword == "$id") {
    return UNKNOWN_KEYWORD;
  }

  return sourcemeta::core::schema_walker(keyword, vocabularies);
}

static auto remote_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://example.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://example.com/remote",
      "$defs": {
        "one": { "$anchor": "inner", "type": "string" }
      }
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_remote_carrying_an_anchor_where_no_keyword_names_a_resource) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com/remote"
  })JSON");

  sourcemeta::core::schema_bundle(document, walker_without_an_identifier,
                                  remote_resolver);

  // The anchor the remote carries names a place of it rather than a schema of
  // its own, so the marking passes over it and lands on the two subschemas
  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "#/$defs/https:~1~1example.com~1remote",
    "$defs": {
      "https://example.com/remote": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://example.com/remote",
        "$defs": {
          "one": {
            "$anchor": "inner",
            "type": "string",
            "x-sourcemeta-dialect-override-subschema":
              "https://json-schema.org/draft/2020-12/schema"
          }
        },
        "x-sourcemeta-dialect-override-subschema":
          "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

// A document read against the address it came from answers to a name while
// nothing in it declares one, so a remote that answers to none cannot be
// named on its own from within it and needs a copy of its own. The copy reads
// under the dialect the document holding it reads under, which leaves nothing
// for the copy to say about which dialect that is
TEST(a_copy_that_reads_by_the_dialect_of_the_place_it_goes_to) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "x": { "$ref": "https://example.com/remote" }
    }
  })JSON");

  sourcemeta::core::SchemaBundleOptions options;
  options.default_base = "https://example.com/retrieved";
  sourcemeta::core::schema_bundle(document, walker_without_an_identifier,
                                  remote_resolver, "", "", options);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "x": { "$ref": "#/$defs/https:~1~1example.com~1remote" }
    },
    "$defs": {
      "https://example.com/remote": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://example.com/remote",
        "$defs": {
          "one": { "$anchor": "inner", "type": "string" }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

// Besides whatever a document says of itself, it answers to what the caller
// says it is, and the fixture suite has no way to hand one a name of its own

// A document that is nothing but a boolean says nothing, and there is nowhere
// on it to write a name down, so the name the caller handed over stands for it
// without being written into it
TEST(a_boolean_document_handed_a_name_of_its_own) {
  auto document = sourcemeta::core::parse_json("true");

  const auto spent{sourcemeta::core::schema_bundle(
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/handed")};

  EXPECT_EQ(document, sourcemeta::core::parse_json("true"));
  EXPECT_TRUE(spent > 0);
}
