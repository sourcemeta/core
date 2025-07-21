#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <tuple>       // std::tuple
#include <vector>      // std::vector

#define EXPECT_DEPENDENCY(index, expected_origin, expected_pointer,            \
                          expected_dependency)                                 \
  EXPECT_EQ(std::get<0>(dependencies.at(index)).value(), (expected_origin));   \
  EXPECT_EQ(sourcemeta::core::to_string(std::get<1>(dependencies.at(index))),  \
            (expected_pointer));                                               \
  EXPECT_EQ(std::get<2>(dependencies.at(index)), (expected_dependency));

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://www.sourcemeta.com/test-1") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/test-1",
      "type": "string"
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

TEST(JSONSchema_dependencies, example_1) {
  std::vector<std::tuple<std::optional<std::string>, sourcemeta::core::Pointer,
                         std::string>>
      dependencies;

  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/test-1",
    "type": "string"
  })JSON")};

  sourcemeta::core::dependencies(
      schema, sourcemeta::core::schema_official_walker, test_resolver,
      [&dependencies](const auto &origin, const auto &pointer,
                      const auto &dependency) {
        dependencies.emplace_back(origin, pointer, dependency);
      });

  EXPECT_EQ(dependencies.size(), 21);

  EXPECT_DEPENDENCY(0, "https://www.sourcemeta.com/test-1", "/$schema",
                    "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(1, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/0/$ref",
                    "https://json-schema.org/draft/2020-12/meta/core");
  EXPECT_DEPENDENCY(2, "https://json-schema.org/draft/2020-12/meta/core",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(3, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/1/$ref",
                    "https://json-schema.org/draft/2020-12/meta/applicator");
  EXPECT_DEPENDENCY(4, "https://json-schema.org/draft/2020-12/meta/applicator",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(5, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/2/$ref",
                    "https://json-schema.org/draft/2020-12/meta/unevaluated");
  EXPECT_DEPENDENCY(6, "https://json-schema.org/draft/2020-12/meta/unevaluated",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(7, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/3/$ref",
                    "https://json-schema.org/draft/2020-12/meta/validation");
  EXPECT_DEPENDENCY(8, "https://json-schema.org/draft/2020-12/meta/validation",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(9, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/4/$ref",
                    "https://json-schema.org/draft/2020-12/meta/meta-data");
  EXPECT_DEPENDENCY(10, "https://json-schema.org/draft/2020-12/meta/meta-data",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(
      11, "https://json-schema.org/draft/2020-12/schema", "/allOf/5/$ref",
      "https://json-schema.org/draft/2020-12/meta/format-annotation");
  EXPECT_DEPENDENCY(
      12, "https://json-schema.org/draft/2020-12/meta/format-annotation",
      "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(13, "https://json-schema.org/draft/2020-12/schema",
                    "/allOf/6/$ref",
                    "https://json-schema.org/draft/2020-12/meta/content");
  EXPECT_DEPENDENCY(14, "https://json-schema.org/draft/2020-12/meta/content",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(15, "https://json-schema.org/draft/2020-12/schema",
                    "/properties/$recursiveAnchor/$ref",
                    "https://json-schema.org/draft/2020-12/meta/core");
  EXPECT_DEPENDENCY(16, "https://json-schema.org/draft/2020-12/meta/core",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(17, "https://json-schema.org/draft/2020-12/schema",
                    "/properties/$recursiveRef/$ref",
                    "https://json-schema.org/draft/2020-12/meta/core");
  EXPECT_DEPENDENCY(18, "https://json-schema.org/draft/2020-12/meta/core",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
  EXPECT_DEPENDENCY(
      19, "https://json-schema.org/draft/2020-12/schema",
      "/properties/dependencies/additionalProperties/anyOf/1/$ref",
      "https://json-schema.org/draft/2020-12/meta/validation");
  EXPECT_DEPENDENCY(20, "https://json-schema.org/draft/2020-12/meta/validation",
                    "/$schema", "https://json-schema.org/draft/2020-12/schema");
}
