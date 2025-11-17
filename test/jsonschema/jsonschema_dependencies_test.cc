#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <tuple>       // std::tuple
#include <vector>      // std::vector

#define EXPECT_DEPENDENCY(expected_traces, expected_index, expected_origin,    \
                          expected_pointer, expected_target)                   \
  EXPECT_EQ(std::get<0>((expected_traces).at(expected_index)),                 \
            std::optional<sourcemeta::core::JSON::String>{(expected_origin)}); \
  EXPECT_EQ(sourcemeta::core::to_string(                                       \
                std::get<1>((expected_traces).at(expected_index))),            \
            (expected_pointer));                                               \
  EXPECT_EQ(std::get<2>((expected_traces).at(expected_index)),                 \
            (expected_target));

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
  } else if (identifier == "https://www.sourcemeta.com/test-3-top-level-ref") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$id": "https://www.sourcemeta.com/test-3-top-level-ref",
      "$ref": "test-4"
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

TEST(JSONSchema_dependencies, multiple_refs) {
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

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) {
        traces.emplace_back(origin, pointer, target);
      });

  EXPECT_EQ(traces.size(), 3);

  EXPECT_DEPENDENCY(traces, 0, "https://www.example.com",
                    "/properties/bar/$ref",
                    "https://www.sourcemeta.com/test-1");
  EXPECT_DEPENDENCY(traces, 1, "https://www.example.com",
                    "/properties/foo/$ref",
                    "https://www.sourcemeta.com/test-3");
  EXPECT_DEPENDENCY(traces, 2, "https://www.sourcemeta.com/test-3",
                    "/allOf/0/$ref", "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies, across_dialects) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" }
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) {
        traces.emplace_back(origin, pointer, target);
      });

  EXPECT_EQ(traces.size(), 3);

  EXPECT_DEPENDENCY(traces, 0, "https://www.example.com", "/items/$ref",
                    "https://www.sourcemeta.com/test-2");
  EXPECT_DEPENDENCY(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
                    "https://www.sourcemeta.com/test-3");
  EXPECT_DEPENDENCY(traces, 2, "https://www.sourcemeta.com/test-3",
                    "/allOf/0/$ref", "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies, across_dialects_top_level_ref_draft) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-3-top-level-ref" }
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) {
        traces.emplace_back(origin, pointer, target);
      });

  EXPECT_EQ(traces.size(), 2);

  EXPECT_DEPENDENCY(traces, 0, "https://www.example.com", "/items/$ref",
                    "https://www.sourcemeta.com/test-3-top-level-ref");
  EXPECT_DEPENDENCY(traces, 1,
                    "https://www.sourcemeta.com/test-3-top-level-ref", "/$ref",
                    "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies,
     across_dialects_from_top_level_ref_draft_absolute) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$ref": "https://www.sourcemeta.com/test-4"
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) {
        traces.emplace_back(origin, pointer, target);
      });

  EXPECT_EQ(traces.size(), 1);

  EXPECT_DEPENDENCY(traces, 0, std::nullopt, "/$ref",
                    "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies,
     across_dialects_from_top_level_ref_draft_relative) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$ref": "test-4"
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  EXPECT_THROW(sourcemeta::core::dependencies(
                   document, sourcemeta::core::schema_official_walker,
                   test_resolver,
                   [&traces](const auto &origin, const auto &pointer,
                             const auto &target, const auto &) {
                     traces.emplace_back(origin, pointer, target);
                   }),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_dependencies,
     across_dialects_from_top_level_ref_draft_with_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$ref": "https://www.sourcemeta.com/test-4"
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) { traces.emplace_back(origin, pointer, target); },
      "http://json-schema.org/draft-07/schema#");

  EXPECT_EQ(traces.size(), 1);

  EXPECT_DEPENDENCY(traces, 0, std::nullopt, "/$ref",
                    "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies, across_dialects_const) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://www.example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/test-2" }
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) {
        traces.emplace_back(origin, pointer, target);
      });

  EXPECT_EQ(traces.size(), 3);

  EXPECT_DEPENDENCY(traces, 0, "https://www.example.com", "/items/$ref",
                    "https://www.sourcemeta.com/test-2");
  EXPECT_DEPENDENCY(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
                    "https://www.sourcemeta.com/test-3");
  EXPECT_DEPENDENCY(traces, 2, "https://www.sourcemeta.com/test-3",
                    "/allOf/0/$ref", "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies, with_default_id) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "test-2" }
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) { traces.emplace_back(origin, pointer, target); },
      std::nullopt, "https://www.sourcemeta.com/default");

  EXPECT_EQ(traces.size(), 3);

  EXPECT_DEPENDENCY(traces, 0, "https://www.sourcemeta.com/default",
                    "/items/$ref", "https://www.sourcemeta.com/test-2");
  EXPECT_DEPENDENCY(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
                    "https://www.sourcemeta.com/test-3");
  EXPECT_DEPENDENCY(traces, 2, "https://www.sourcemeta.com/test-3",
                    "/allOf/0/$ref", "https://www.sourcemeta.com/test-4");
}

TEST(JSONSchema_dependencies, with_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) { traces.emplace_back(origin, pointer, target); },
      "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(traces.size(), 1);

  EXPECT_DEPENDENCY(traces, 0, std::nullopt, "/properties/foo/$ref",
                    "https://www.sourcemeta.com/test-1");
}

TEST(JSONSchema_dependencies, without_default_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/test-1" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::dependencies(
          document, sourcemeta::core::schema_official_walker, test_resolver,
          [](const auto &, const auto &, const auto &, const auto &) {}),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_dependencies, target_no_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/no-dialect" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::dependencies(
          document, sourcemeta::core::schema_official_walker, test_resolver,
          [](const auto &, const auto &, const auto &, const auto &) {}),
      sourcemeta::core::SchemaReferenceError);
}

TEST(JSONSchema_dependencies, target_array) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "foo": { "$ref": "https://www.sourcemeta.com/array" }
    }
  })JSON");

  EXPECT_THROW(
      sourcemeta::core::dependencies(
          document, sourcemeta::core::schema_official_walker, test_resolver,
          [](const auto &, const auto &, const auto &, const auto &) {}),
      sourcemeta::core::SchemaReferenceError);
}

TEST(JSONSchema_dependencies, custom_paths_no_external) {
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

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) { traces.emplace_back(origin, pointer, target); },
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      {
          sourcemeta::core::Pointer{"wrapper"},
          sourcemeta::core::Pointer{"common", "test"},
          sourcemeta::core::Pointer{"common", "with-id"},
      });

  EXPECT_EQ(traces.size(), 0);
}

TEST(JSONSchema_dependencies, custom_paths_with_externals) {
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

  std::vector<
      std::tuple<std::optional<sourcemeta::core::JSON::String>,
                 sourcemeta::core::Pointer, sourcemeta::core::JSON::String>>
      traces;

  sourcemeta::core::dependencies(
      document, sourcemeta::core::schema_official_walker, test_resolver,
      [&traces](const auto &origin, const auto &pointer, const auto &target,
                const auto &) { traces.emplace_back(origin, pointer, target); },
      "https://json-schema.org/draft/2020-12/schema", std::nullopt,
      {
          sourcemeta::core::Pointer{"wrapper"},
          sourcemeta::core::Pointer{"common", "test"},
          sourcemeta::core::Pointer{"common", "with-id"},
      });

  EXPECT_EQ(traces.size(), 3);

  // TODO: We should be getting the nested identifier here
  EXPECT_DEPENDENCY(traces, 0, std::nullopt, "/common/with-id/$ref",
                    "https://www.sourcemeta.com/test-2");
  EXPECT_DEPENDENCY(traces, 1, "https://www.sourcemeta.com/test-2", "/$ref",
                    "https://www.sourcemeta.com/test-3");
  EXPECT_DEPENDENCY(traces, 2, "https://www.sourcemeta.com/test-3",
                    "/allOf/0/$ref", "https://www.sourcemeta.com/test-4");
}
