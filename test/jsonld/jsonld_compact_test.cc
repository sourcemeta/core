#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(JSONLD_compact, compact_to_relative_true_relativises_against_base) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/b": [ { "@id": "http://example.org/c" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": "http://example.com/b"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "http://example.org/", {},
      sourcemeta::core::JSONLDVersion::V1_1, true, true)};

  EXPECT_EQ(result.at("@id").to_string(), "a");
  EXPECT_EQ(result.at("b").at("@id").to_string(), "c");
}

TEST(JSONLD_compact, compact_to_relative_false_keeps_absolute_against_base) {
  const auto input = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.org/a",
      "http://example.com/b": [ { "@id": "http://example.org/c" } ]
    }
  ])");

  const auto context = sourcemeta::core::parse_json(R"({
    "b": "http://example.com/b"
  })");

  const auto result{sourcemeta::core::jsonld_compact(
      input, context, "http://example.org/", {},
      sourcemeta::core::JSONLDVersion::V1_1, true, false)};

  EXPECT_EQ(result.at("@id").to_string(), "http://example.org/a");
  EXPECT_EQ(result.at("b").at("@id").to_string(), "http://example.org/c");
}
