#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <sstream>

TEST(JSONSchema_format, example_1) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "title": "My example schema",
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             sourcemeta::core::schema_format_compare);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "My example schema",
  "type": "object"
})JSON");
}

TEST(JSONSchema_format, compare_known_vs_unknown) {
  EXPECT_TRUE(sourcemeta::core::schema_format_compare("$id", "foo"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare("foo", "$id"));
}

TEST(JSONSchema_format, compare_unknown_vs_unknown) {
  // Alphabetic ordering applies here
  EXPECT_TRUE(sourcemeta::core::schema_format_compare("bar", "foo"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare("foo", "bar"));
}
