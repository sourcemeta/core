#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/test-1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-2") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-2",
      "allOf": [ { "$ref": "test-3" } ]
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-3") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-3",
      "allOf": [ { "$ref": "test-1" } ]
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-4") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-4",
      "type": "boolean"
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier ==
             "https://www.sourcemeta.com/recursive-empty-fragment") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/recursive-empty-fragment#",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/anonymous") {
    return sourcemeta::core::parse_json(R"JSON({
      "type": "integer"
    })JSON");
  } else if (identifier == "https://example.com/meta/1.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://example.com/meta/2.json",
      "$id": "https://example.com/meta/1.json"
    })JSON");
  } else if (identifier == "https://example.com/meta/2.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://example.com/meta/2.json"
    })JSON");
  } else {
    return sourcemeta::core::official_resolver(identifier);
  }
}

TEST(JSONSchema_bundle_draft6, no_references_no_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, const_no_references_no_id) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  const auto result = sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_bundle_draft6, simple_with_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "$id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "$id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, simple_without_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "$id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" },
      "bar": {
        "$id": "https://www.sourcemeta.com",
        "allOf": [ { "$ref": "test-2" } ]
      }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, schema_not_found) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/xxx" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_bundle_draft6, idempotency) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);
  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);
  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, pre_embedded) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "already-embedded": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-2" }
    },
    "definitions": {
      "already-embedded": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-2": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-2",
        "allOf": [ { "$ref": "test-3" } ]
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-1" } ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, taken_definitions_entry) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" },
      "extra": { "$ref": "https://www.sourcemeta.com/test-4" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": { "foo": 1 },
      "https://www.sourcemeta.com/test-4": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x": { "foo": 1 }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "test": { "$ref": "https://www.sourcemeta.com/test-1" },
      "extra": { "$ref": "https://www.sourcemeta.com/test-4" }
    },
    "definitions": {
      "https://www.sourcemeta.com/test-1": { "foo": 1 },
      "https://www.sourcemeta.com/test-1/x": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-4": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x": { "foo": 1 },
      "https://www.sourcemeta.com/test-4/x/x/x": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-4",
        "type": "boolean"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, recursive) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$ref": "https://www.sourcemeta.com/recursive"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$ref": "https://www.sourcemeta.com/recursive",
    "definitions": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/recursive",
        "properties": {
          "foo": { "$ref": "#" }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, recursive_empty_fragment) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$ref": "https://www.sourcemeta.com/recursive-empty-fragment#"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$ref": "https://www.sourcemeta.com/recursive-empty-fragment#",
    "definitions": {
      "https://www.sourcemeta.com/recursive-empty-fragment": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/recursive-empty-fragment",
        "properties": {
          "foo": { "$ref": "#" }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, anonymous_no_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$ref": "https://www.sourcemeta.com/anonymous"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver,
                           "http://json-schema.org/draft-06/schema#");

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$ref": "https://www.sourcemeta.com/anonymous",
    "definitions": {
      "https://www.sourcemeta.com/anonymous": {
        "$id": "https://www.sourcemeta.com/anonymous",
        "type": "integer"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "https://example.com/meta/2.json",
        "$id": "https://example.com/meta/1.json"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://example.com/meta/2.json"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, relative_base_uri_with_ref) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$id": "common",
    "allOf": [ { "$ref": "#reference" } ],
    "definitions": {
      "reference": {
        "$id": "#reference"
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "$id": "common",
    "allOf": [ { "$ref": "#reference" } ],
    "definitions": {
      "reference": {
        "$id": "#reference"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_bundle_draft6, hyperschema_smoke) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/hyper-schema#",
    "allOf": [
      { "$ref": "http://json-schema.org/draft-06/hyper-schema#" }
    ]
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  EXPECT_TRUE(document.is_object());
}

TEST(JSONSchema_bundle_draft6, hyperschema_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "allOf": [
      { "$ref": "http://json-schema.org/draft-06/schema#" },
      { "$ref": "http://json-schema.org/draft-06/hyper-schema#" }
    ]
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  EXPECT_TRUE(document.defines("definitions"));
  EXPECT_TRUE(document.at("definitions").is_object());
  EXPECT_EQ(document.at("definitions").size(), 2);

  EXPECT_TRUE(document.at("definitions")
                  .defines("http://json-schema.org/draft-06/schema"));
  EXPECT_TRUE(document.at("definitions")
                  .defines("http://json-schema.org/draft-06/hyper-schema"));
}

TEST(JSONSchema_bundle_draft6, hyperschema_ref_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/hyper-schema#",
    "allOf": [
      { "$ref": "http://json-schema.org/draft-06/schema#" }
    ]
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver);

  EXPECT_TRUE(document.defines("definitions"));
  EXPECT_TRUE(document.at("definitions").is_object());
  EXPECT_EQ(document.at("definitions").size(), 1);

  EXPECT_TRUE(document.at("definitions")
                  .defines("http://json-schema.org/draft-06/schema"));
}
