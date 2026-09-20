#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <sstream>

TEST(format_example_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "title": "My example schema",
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "My example schema",
  "type": "object"
})JSON");
}

TEST(format_example_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "default": 1,
    "$ref": "other",
    "x-foo": [ "bar", "baz" ],
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "default": 1,
  "x-foo": [ "bar", "baz" ],
  "$ref": "other"
})JSON");
}

TEST(format_nested_objects) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "name": {
        "type": "string",
        "description": "The name",
        "title": "Name"
      }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "properties": {
    "name": {
      "title": "Name",
      "description": "The name",
      "type": "string"
    }
  }
})JSON");
}

TEST(format_no_dialect_with_default) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "string",
    "description": "test"
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver,
      "https://json-schema.org/draft/2020-12/schema"};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "description": "test",
  "type": "string"
})JSON");
}

TEST(format_properties_container_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "minimum": { "description": "min", "type": "string" },
      "maximum": { "description": "max", "type": "number" },
      "pattern": { "description": "pat", "type": "boolean" }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": {
    "minimum": {
      "description": "min",
      "type": "string"
    },
    "maximum": {
      "description": "max",
      "type": "number"
    },
    "pattern": {
      "description": "pat",
      "type": "boolean"
    }
  }
})JSON");
}

TEST(format_enum_objects_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "enum": [
      { "type": 1, "title": 2, "$schema": 3 }
    ]
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "string",
  "enum": [
    {
      "type": 1,
      "title": 2,
      "$schema": 3
    }
  ]
})JSON");
}

TEST(format_const_object_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "const": { "properties": 1, "description": 2, "$id": 3 }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "const": {
    "properties": 1,
    "description": 2,
    "$id": 3
  }
})JSON");
}

TEST(format_default_object_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "default": { "enum": 1, "const": 2, "type": 3 }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "default": {
    "enum": 1,
    "const": 2,
    "type": 3
  },
  "type": "object"
})JSON");
}

TEST(format_examples_objects_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "examples": [
      { "pattern": 1, "minimum": 2, "maximum": 3 }
    ]
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "examples": [
    {
      "pattern": 1,
      "minimum": 2,
      "maximum": 3
    }
  ],
  "type": "object"
})JSON");
}

TEST(format_typo_keyword_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "additionalProperty": {
      "type": "string",
      "description": "This is a typo"
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "additionalProperty": {
    "type": "string",
    "description": "This is a typo"
  }
})JSON");
}

TEST(format_correct_keyword_is_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "additionalProperties": {
      "type": "string",
      "description": "This is correct"
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "additionalProperties": {
    "description": "This is correct",
    "type": "string"
  }
})JSON");
}

TEST(format_nested_properties_subschemas_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "properties": {
      "zebra": {
        "type": "object",
        "description": "A zebra",
        "properties": {
          "stripes": {
            "type": "number",
            "description": "Number of stripes"
          }
        }
      }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "properties": {
    "zebra": {
      "description": "A zebra",
      "type": "object",
      "properties": {
        "stripes": {
          "description": "Number of stripes",
          "type": "number"
        }
      }
    }
  }
})JSON");
}

TEST(format_pattern_properties_container_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "patternProperties": {
      "items": { "description": "i", "type": "string" },
      "enum": { "description": "e", "type": "number" },
      "const": { "description": "c", "type": "boolean" }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "patternProperties": {
    "items": {
      "description": "i",
      "type": "string"
    },
    "enum": {
      "description": "e",
      "type": "number"
    },
    "const": {
      "description": "c",
      "type": "boolean"
    }
  }
})JSON");
}

TEST(format_definitions_container_not_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "allOf": { "type": "string", "description": "A" },
      "anyOf": { "type": "number", "description": "A" },
      "$id": { "type": "boolean", "description": "I" }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$defs": {
    "allOf": {
      "description": "A",
      "type": "string"
    },
    "anyOf": {
      "description": "A",
      "type": "number"
    },
    "$id": {
      "description": "I",
      "type": "boolean"
    }
  }
})JSON");
}

TEST(format_embedded_resource_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/root",
    "$defs": {
      "myResource": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://example.com/embedded",
        "properties": { "foo": { "type": "string" } },
        "type": "object",
        "description": "An embedded schema resource",
        "title": "Embedded Resource"
      }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://example.com/root",
  "$defs": {
    "myResource": {
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://example.com/embedded",
      "title": "Embedded Resource",
      "description": "An embedded schema resource",
      "type": "object",
      "properties": {
        "foo": {
          "type": "string"
        }
      }
    }
  }
})JSON");
}

TEST(format_boolean_subschema_does_not_crash) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "additionalProperties": true,
    "properties": {
      "foo": { "type": "string", "description": "Foo property" }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "properties": {
    "foo": {
      "description": "Foo property",
      "type": "string"
    }
  },
  "additionalProperties": true
})JSON");
}

TEST(format_reorder_does_not_invalidate_child_pointers) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "not": {},
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "not": {}
})JSON");
}

TEST(format_boolean_schema) {
  sourcemeta::core::JSON document{true};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver,
      "https://json-schema.org/draft/2020-12/schema"};

  sourcemeta::core::schema_format(document, frame);

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), "true");
}

TEST(format_root_mode_frame_only_reorders_the_root) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "name": {
        "type": "string",
        "title": "Name"
      }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "properties": {
    "name": {
      "type": "string",
      "title": "Name"
    }
  }
})JSON");
}

TEST(format_deeply_nested_siblings_reordered) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$defs": {
      "left": {
        "properties": {
          "deep": {
            "items": {
              "additionalProperties": { "type": "string", "title": "Left" },
              "type": "array",
              "description": "Left items",
              "$comment": "left"
            },
            "type": "object",
            "title": "Left deep"
          }
        },
        "type": "object",
        "description": "Left branch"
      },
      "right": {
        "properties": {
          "deep": {
            "items": {
              "additionalProperties": { "type": "number", "title": "Right" },
              "type": "array",
              "description": "Right items",
              "$comment": "right"
            },
            "type": "object",
            "title": "Right deep"
          }
        },
        "type": "object",
        "description": "Right branch"
      }
    },
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  sourcemeta::core::schema_format(document, frame);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "$defs": {
    "left": {
      "description": "Left branch",
      "type": "object",
      "properties": {
        "deep": {
          "title": "Left deep",
          "type": "object",
          "items": {
            "description": "Left items",
            "$comment": "left",
            "type": "array",
            "additionalProperties": {
              "title": "Left",
              "type": "string"
            }
          }
        }
      }
    },
    "right": {
      "description": "Right branch",
      "type": "object",
      "properties": {
        "deep": {
          "title": "Right deep",
          "type": "object",
          "items": {
            "description": "Right items",
            "$comment": "right",
            "type": "array",
            "additionalProperties": {
              "title": "Right",
              "type": "number"
            }
          }
        }
      }
    }
  }
})JSON");
}
