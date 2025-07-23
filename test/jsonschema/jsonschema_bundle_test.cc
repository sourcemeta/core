#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <tuple>       // std::tuple
#include <vector>      // std::vector

#define EXPECT_TRACE(expected_traces, expected_index, expected_origin,         \
                     expected_pointer, expected_target, expected_key)          \
  EXPECT_EQ(std::get<0>((expected_traces).at(expected_index)),                 \
            std::optional<sourcemeta::core::JSON::String>{(expected_origin)}); \
  EXPECT_EQ(sourcemeta::core::to_string(                                       \
                std::get<1>((expected_traces).at(expected_index))),            \
            (expected_pointer));                                               \
  EXPECT_EQ(std::get<2>((expected_traces).at(expected_index)),                 \
            (expected_target));                                                \
  EXPECT_EQ(sourcemeta::core::to_string(                                       \
                std::get<3>((expected_traces).at(expected_index))),            \
            (expected_key));

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/test-1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-2") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2019-09/schema",
      "$id": "https://www.sourcemeta.com/test-2",
      "$ref": "test-3"
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-3") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-3",
      "allOf": [ { "$ref": "test-4" } ]
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/test-4") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-04/schema#",
      "id": "https://www.sourcemeta.com/test-4",
      "type": "string"
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/no-dialect") {
    return sourcemeta::core::parse_json(R"JSON({
      "foo": 1
    })JSON");
  } else if (identifier == "https://www.sourcemeta.com/array") {
    return sourcemeta::core::parse_json(R"JSON([
      "foo", "bar", "baz"
    ])JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

TEST(JSONSchema_bundle, multiple_refs) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/test-3"
      },
      "bar": {
        "$ref": "https://www.sourcemeta.com/test-1"
      }
    }
  })JSON");

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      std::nullopt, std::nullopt, std::nullopt,
      {sourcemeta::core::empty_pointer},
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": {
        "$ref": "https://www.sourcemeta.com/test-3"
      },
      "bar": {
        "$ref": "https://www.sourcemeta.com/test-1"
      }
    },
    "$defs": {
      "https://www.sourcemeta.com/test-4": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "string"
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-4" } ]
      },
      "https://www.sourcemeta.com/test-1": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 3);

  EXPECT_TRACE(traces, 0, "https://www.example.com", "/properties/bar/$ref",
               "https://www.sourcemeta.com/test-1",
               "/$defs/https:~1~1www.sourcemeta.com~1test-1");
  EXPECT_TRACE(traces, 1, "https://www.sourcemeta.com/test-3", "/allOf/0/$ref",
               "https://www.sourcemeta.com/test-4",
               "/$defs/https:~1~1www.sourcemeta.com~1test-4");
  EXPECT_TRACE(traces, 2, "https://www.example.com", "/properties/foo/$ref",
               "https://www.sourcemeta.com/test-3",
               "/$defs/https:~1~1www.sourcemeta.com~1test-3");
}

TEST(JSONSchema_bundle, across_dialects) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" }
  })JSON");

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      std::nullopt, std::nullopt, std::nullopt,
      {sourcemeta::core::empty_pointer},
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" },
    "$defs": {
      "https://www.sourcemeta.com/test-2": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$id": "https://www.sourcemeta.com/test-2",
        "$ref": "test-3"
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-4" } ]
      },
      "https://www.sourcemeta.com/test-4": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 3);

  EXPECT_TRACE(traces, 0, "https://www.sourcemeta.com/test-3", "/allOf/0/$ref",
               "https://www.sourcemeta.com/test-4",
               "/$defs/https:~1~1www.sourcemeta.com~1test-4");
  EXPECT_TRACE(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
               "https://www.sourcemeta.com/test-3",
               "/$defs/https:~1~1www.sourcemeta.com~1test-3");
  EXPECT_TRACE(traces, 2, "https://www.example.com", "/items/$ref",
               "https://www.sourcemeta.com/test-2",
               "/$defs/https:~1~1www.sourcemeta.com~1test-2");
}

TEST(JSONSchema_bundle, across_dialects_const) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" }
  })JSON");

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  const auto result = sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      std::nullopt, std::nullopt, std::nullopt,
      {sourcemeta::core::empty_pointer},
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" },
    "$defs": {
      "https://www.sourcemeta.com/test-2": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$id": "https://www.sourcemeta.com/test-2",
        "$ref": "test-3"
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-4" } ]
      },
      "https://www.sourcemeta.com/test-4": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(result, expected);

  EXPECT_EQ(traces.size(), 3);

  EXPECT_TRACE(traces, 0, "https://www.sourcemeta.com/test-3", "/allOf/0/$ref",
               "https://www.sourcemeta.com/test-4",
               "/$defs/https:~1~1www.sourcemeta.com~1test-4");
  EXPECT_TRACE(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
               "https://www.sourcemeta.com/test-3",
               "/$defs/https:~1~1www.sourcemeta.com~1test-3");
  EXPECT_TRACE(traces, 2, "https://www.example.com", "/items/$ref",
               "https://www.sourcemeta.com/test-2",
               "/$defs/https:~1~1www.sourcemeta.com~1test-2");
}

TEST(JSONSchema_bundle, with_default_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "test-2" }
  })JSON");

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      std::nullopt, "https://www.sourcemeta.com/default", std::nullopt,
      {sourcemeta::core::empty_pointer},
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "test-2" },
    "$defs": {
      "https://www.sourcemeta.com/test-2": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$id": "https://www.sourcemeta.com/test-2",
        "$ref": "test-3"
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-4" } ]
      },
      "https://www.sourcemeta.com/test-4": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 3);

  EXPECT_TRACE(traces, 0, "https://www.sourcemeta.com/test-3", "/allOf/0/$ref",
               "https://www.sourcemeta.com/test-4",
               "/$defs/https:~1~1www.sourcemeta.com~1test-4");
  EXPECT_TRACE(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
               "https://www.sourcemeta.com/test-3",
               "/$defs/https:~1~1www.sourcemeta.com~1test-3");
  EXPECT_TRACE(traces, 2, "https://www.sourcemeta.com/default", "/items/$ref",
               "https://www.sourcemeta.com/test-2",
               "/$defs/https:~1~1www.sourcemeta.com~1test-2");
}

TEST(JSONSchema_bundle, with_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      std::nullopt, {sourcemeta::core::empty_pointer},
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" }
    },
    "$defs": {
      "https://www.sourcemeta.com/test-1": {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$id": "https://www.sourcemeta.com/test-1",
        "type": "string"
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 1);

  EXPECT_TRACE(traces, 0, std::nullopt, "/properties/foo/$ref",
               "https://www.sourcemeta.com/test-1",
               "/$defs/https:~1~1www.sourcemeta.com~1test-1");
}

TEST(JSONSchema_bundle, without_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_bundle, target_no_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/no-dialect" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaReferenceError);
}

TEST(JSONSchema_bundle, target_array) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/array" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::bundle(
          document, sourcemeta::core::schema_official_walker, test_resolver),
      sourcemeta::core::SchemaReferenceError);
}

TEST(JSONSchema_bundle, custom_paths_no_external) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "string"
      }
    }
  })JSON")};

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      sourcemeta::core::Pointer{"components"},
      {
          sourcemeta::core::Pointer{"wrapper"},
          sourcemeta::core::Pointer{"common", "test"},
          sourcemeta::core::Pointer{"common", "with-id"},
      },
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "string"
      }
    }
  })JSON")};

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 0);
}

TEST(JSONSchema_bundle, custom_paths_with_externals) {
  auto document{sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "test-2"
      }
    }
  })JSON")};

  std::vector<std::tuple<
      std::optional<sourcemeta::core::JSON::String>, sourcemeta::core::Pointer,
      sourcemeta::core::JSON::String, sourcemeta::core::Pointer>>
      traces;
  sourcemeta::core::bundle(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      sourcemeta::core::Pointer{"components"},
      {
          sourcemeta::core::Pointer{"wrapper"},
          sourcemeta::core::Pointer{"common", "test"},
          sourcemeta::core::Pointer{"common", "with-id"},
      },
      [&traces](const std::optional<sourcemeta::core::JSON::String> &origin,
                const sourcemeta::core::Pointer &pointer,
                const sourcemeta::core::JSON::String &target,
                const sourcemeta::core::Pointer &key) {
        traces.emplace_back(origin, pointer, target, key);
      });

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "wrapper": {
      "$ref": "#/common/test"
    },
    "common": {
      "test": {
        "$ref": "#/common/with-id"
      },
      "with-id": {
        "$id": "https://www.sourcemeta.com/schema",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "test-2"
      }
    },
    "components": {
      "https://www.sourcemeta.com/test-2": {
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$id": "https://www.sourcemeta.com/test-2",
        "$ref": "test-3"
      },
      "https://www.sourcemeta.com/test-3": {
        "$schema": "http://json-schema.org/draft-06/schema#",
        "$id": "https://www.sourcemeta.com/test-3",
        "allOf": [ { "$ref": "test-4" } ]
      },
      "https://www.sourcemeta.com/test-4": {
        "$schema": "http://json-schema.org/draft-04/schema#",
        "id": "https://www.sourcemeta.com/test-4",
        "type": "string"
      }
    }
  })JSON")};

  EXPECT_EQ(document, expected);

  EXPECT_EQ(traces.size(), 3);

  EXPECT_TRACE(traces, 0, "https://www.sourcemeta.com/test-3", "/allOf/0/$ref",
               "https://www.sourcemeta.com/test-4",
               "/components/https:~1~1www.sourcemeta.com~1test-4");
  EXPECT_TRACE(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
               "https://www.sourcemeta.com/test-3",
               "/components/https:~1~1www.sourcemeta.com~1test-3");
  // TODO: We should be getting the nested identifier here
  EXPECT_TRACE(traces, 2, std::nullopt, "/common/with-id/$ref",
               "https://www.sourcemeta.com/test-2",
               "/components/https:~1~1www.sourcemeta.com~1test-2");
}
