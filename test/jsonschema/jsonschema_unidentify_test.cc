#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

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

TEST(JSONSchema_unidentify, 2020_12) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
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

  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/recursive#/properties/foo"
      },
      "baz": {
        "$ref": "https://example.com/baz-anchor#baz"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, 2020_12_bundle) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
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

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_2020_12);
  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
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

TEST(JSONSchema_unidentify, 2020_12_bundle_boolean_subschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_2020_12);
  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, 2020_12_bundle_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_2020_12);
  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2020_12);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "#/$defs/https:~1~1example.com~1meta~11.json",
    "type": "string",
    "$defs": {
      "https://example.com/meta/1.json": {
        "$schema": "#/$defs/https:~1~1example.com~1meta~12.json",
        "$vocabulary": { "https://json-schema.org/draft/2020-12/vocab/core": true }
      },
      "https://example.com/meta/2.json": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": { "https://json-schema.org/draft/2020-12/vocab/core": true }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, 2019_09) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
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

  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2019_09);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/recursive#/properties/foo"
      },
      "baz": {
        "$ref": "https://example.com/baz-anchor#baz"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, 2019_09_bundle) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
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

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_2019_09);
  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2019_09);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
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

TEST(JSONSchema_unidentify, 2019_09_bundle_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_2019_09);
  sourcemeta::core::unidentify(document,
                               sourcemeta::core::schema_official_walker,
                               test_resolver_2019_09);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "#/$defs/https:~1~1example.com~1meta~11.json",
    "type": "string",
    "$defs": {
      "https://example.com/meta/1.json": {
        "$schema": "#/$defs/https:~1~1example.com~1meta~12.json",
        "$vocabulary": { "https://json-schema.org/draft/2019-09/vocab/core": true }
      },
      "https://example.com/meta/2.json": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$vocabulary": { "https://json-schema.org/draft/2019-09/vocab/core": true }
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft7) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft7);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/recursive#/properties/foo"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft7_bundle) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft7);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft7);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
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

TEST(JSONSchema_unidentify, draft7_bundle_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft7);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft7);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "#/definitions/https:~1~1example.com~1meta~11.json",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "#/definitions/https:~1~1example.com~1meta~12.json"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-07/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft6) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft6);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/recursive#/properties/foo"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft6_bundle) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft6);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft6);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
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

TEST(JSONSchema_unidentify, draft6_bundle_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft6);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft6);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "#/definitions/https:~1~1example.com~1meta~11.json",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "#/definitions/https:~1~1example.com~1meta~12.json"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-06/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft4);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/recursive#/properties/foo"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(JSONSchema_unidentify, draft4_bundle) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://www.sourcemeta.com/top-level",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "properties": {
      "foo": {
        "$ref": "recursive#/properties/foo"
      }
    }
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft4);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft4);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
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

TEST(JSONSchema_unidentify, draft4_bundle_metaschema) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta/1.json",
    "type": "string"
  })JSON");

  sourcemeta::core::bundle(document, sourcemeta::core::schema_official_walker,
                           test_resolver_draft4);
  sourcemeta::core::unidentify(
      document, sourcemeta::core::schema_official_walker, test_resolver_draft4);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "#/definitions/https:~1~1example.com~1meta~11.json",
    "type": "string",
    "definitions": {
      "https://example.com/meta/1.json": {
        "$schema": "#/definitions/https:~1~1example.com~1meta~12.json"
      },
      "https://example.com/meta/2.json": {
        "$schema": "http://json-schema.org/draft-04/schema#"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}
