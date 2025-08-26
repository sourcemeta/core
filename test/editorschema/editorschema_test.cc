#include <gtest/gtest.h>

#include <sourcemeta/core/editorschema.h>

static auto test_resolver_2020_12(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://example.com/foo/bar") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://example.com/foo/bar",
      "$anchor": "baz"
    })JSON");
  } else if (identifier == "https://example.com/baz-anchor") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://example.com/baz-anchor",
      "$defs": {
        "baz": {
          "$anchor": "baz",
          "type": "string"
        }
      }
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier == "https://example.com/meta/1.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://example.com/meta/2.json",
      "$id": "https://example.com/meta/1.json",
      "$vocabulary": { "https://json-schema.org/draft/2020-12/vocab/core": true }
    })JSON");
  } else if (identifier == "https://example.com/meta/2.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://example.com/meta/2.json",
      "$vocabulary": { "https://json-schema.org/draft/2020-12/vocab/core": true }
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

static auto test_resolver_2019_09(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://example.com/foo/bar") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2019-09/schema",
      "$id": "https://example.com/foo/bar",
      "$anchor": "baz"
    })JSON");
  } else if (identifier == "https://example.com/baz-anchor") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2019-09/schema",
      "$id": "https://example.com/baz-anchor",
      "$defs": {
        "baz": {
          "$anchor": "baz",
          "type": "string"
        }
      }
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2019-09/schema",
      "$id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier == "https://example.com/meta/1.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://example.com/meta/2.json",
      "$id": "https://example.com/meta/1.json",
      "$vocabulary": { "https://json-schema.org/draft/2019-09/vocab/core": true }
    })JSON");
  } else if (identifier == "https://example.com/meta/2.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2019-09/schema",
      "$id": "https://example.com/meta/2.json",
      "$vocabulary": { "https://json-schema.org/draft/2019-09/vocab/core": true }
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

static auto test_resolver_draft7(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier == "https://example.com/meta/1.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://example.com/meta/2.json",
      "$id": "https://example.com/meta/1.json"
    })JSON");
  } else if (identifier == "https://example.com/meta/2.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-07/schema#",
      "$id": "https://example.com/meta/2.json"
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

static auto test_resolver_draft6(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
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
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

static auto test_resolver_draft4(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/recursive") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/recursive",
      "properties": {
        "foo": { "$ref": "#" }
      }
    })JSON");
  } else if (identifier == "https://example.com/meta/1.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://example.com/meta/2.json",
      "id": "https://example.com/meta/1.json"
    })JSON");
  } else if (identifier == "https://example.com/meta/2.json") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://example.com/meta/2.json"
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

TEST(EditorSchema, 2020_12_bundle) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      },
      "baz": {
        "$ref": "https://example.com/baz-anchor#baz"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/$defs/https:~1~1www.sourcemeta.com~1recursive/properties/foo"
      },
      "baz": {
        "$ref": "#/$defs/https:~1~1example.com~1baz-anchor/$defs/baz"
      }
    },
    "$defs": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "properties": {
          "foo": {
            "$ref": "#/$defs/https:~1~1www.sourcemeta.com~1recursive"
          }
        }
      },
      "https://example.com/baz-anchor": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$defs": {
          "baz": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_static_dynamic_reference) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$dynamicRef": "https://example.com/foo/bar"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#/$defs/https:~1~1example.com~1foo~1bar"
      }
    },
    "$defs": {
      "https://example.com/foo/bar": {
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_dynamic_reference_to_static_anchor) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "foo",
    "properties": {
      "foo": {
        "$dynamicRef": "#foo"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_bundle_boolean_subschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_default_base_dialect) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "properties": {
      "foo": true
    }
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_2020_12,
      "https://json-schema.org/draft/2020-12/schema");

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_default_dialect) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "properties": {
      "foo": true
    }
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_2020_12,
      "https://example.com/meta/1.json");

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2020_12_bundle_metaschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "string",
    "$defs": {
      "https://example.com/meta/1.json": {
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      },
      "https://example.com/meta/2.json": {
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2019_09_bundle) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      },
      "baz": {
        "$ref": "https://example.com/baz-anchor#baz"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2019_09);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$ref": "#/$defs/https:~1~1www.sourcemeta.com~1recursive/properties/foo"
      },
      "baz": {
        "$ref": "#/$defs/https:~1~1example.com~1baz-anchor/$defs/baz"
      }
    },
    "$defs": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "properties": {
          "foo": {
            "$ref": "#/$defs/https:~1~1www.sourcemeta.com~1recursive"
          }
        }
      },
      "https://example.com/baz-anchor": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$defs": {
          "baz": {
            "type": "string"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2019_09_bundle_metaschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2019_09);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "string",
    "$defs": {
      "https://example.com/meta/1.json": {
        "$schema": "https://json-schema.org/draft/2019-09/schema"
      },
      "https://example.com/meta/2.json": {
        "$schema": "https://json-schema.org/draft/2019-09/schema"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft7_bundle) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft7);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "foo": {
        "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive/properties/foo"
      }
    },
    "definitions": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "properties": {
          "foo": {
            "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, 2019_09_static_recursive_reference) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$recursiveRef": "#"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$ref": "#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft7_bundle_metaschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft7);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "http://json-schema.org/draft-07/schema#"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-07/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft6_bundle) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft6);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": {
        "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive/properties/foo"
      }
    },
    "definitions": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "properties": {
          "foo": {
            "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft6_bundle_metaschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft6);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "http://json-schema.org/draft-06/schema#"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-06/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft4_bundle) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft4);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive/properties/foo"
      }
    },
    "definitions": {
      "https://www.sourcemeta.com/recursive": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "properties": {
          "foo": {
            "$ref": "#/definitions/https:~1~1www.sourcemeta.com~1recursive"
          }
        }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(EditorSchema, draft4_bundle_metaschema) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::for_editor(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft4);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "http://json-schema.org/draft-04/schema#"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-04/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}
