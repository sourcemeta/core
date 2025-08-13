#include <gtest/gtest.h>

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

TEST(JSONSchema_format, example_2) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "default": 1,
    "$ref": "other",
    "x-foo": [ "bar", "baz" ],
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             sourcemeta::core::schema_format_compare);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "default": 1,
  "x-foo": [ "bar", "baz" ],
  "$ref": "other"
})JSON");
}

TEST(JSONSchema_format, example_3) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "description": "test",
    "title_extension": "qux",
    "title": "foo",
    "title:extension": "baz",
    "title-extension": "bar",
    "properties": {}
  })JSON");

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             sourcemeta::core::schema_format_compare);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "foo",
  "title-extension": "bar",
  "title:extension": "baz",
  "title_extension": "qux",
  "description": "test",
  "properties": {}
})JSON");
}

TEST(JSONSchema_format, example_4) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "abc": true,
      "id_key": false
    }
  })JSON");

  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream,
                             sourcemeta::core::schema_format_compare);

  // `id_key` must go _after_ `abc` given alphabetic ordering
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "properties": {
    "abc": true,
    "id_key": false
  }
})JSON");
}

TEST(JSONSchema_format, compare_known_vs_unknown) {
  EXPECT_TRUE(sourcemeta::core::schema_format_compare("", "$id", "foo"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare("", "foo", "$id"));
}

TEST(JSONSchema_format, compare_unknown_vs_unknown) {
  // Alphabetic ordering applies here
  EXPECT_TRUE(sourcemeta::core::schema_format_compare("", "bar", "foo"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare("", "foo", "bar"));
}

TEST(JSONSchema_format, compare_properties_like_keywords) {
  EXPECT_TRUE(sourcemeta::core::schema_format_compare("", "items",
                                                      "additionalProperties"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare(
      "", "additionalProperties", "items"));

  EXPECT_TRUE(sourcemeta::core::schema_format_compare(
      "properties", "additionalProperties", "items"));
  EXPECT_FALSE(sourcemeta::core::schema_format_compare("properties", "items",
                                                       "additionalProperties"));
}
