#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_wrap, identifier_without_fragment) {
  const auto identifier{"https://www.example.com"};
  const auto result{sourcemeta::core::wrap(identifier)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.example.com"
  })JSON")};

  EXPECT_EQ(result, expected);
}

TEST(JSONSchema_wrap, identifier_with_fragment) {
  const auto identifier{"https://www.example.com#/foo/bar"};
  const auto result{sourcemeta::core::wrap(identifier)};

  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$ref": "https://www.example.com#/foo/bar"
  })JSON")};

  EXPECT_EQ(result, expected);
}
