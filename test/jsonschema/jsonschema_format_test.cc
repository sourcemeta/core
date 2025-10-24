#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include <sstream>

TEST(JSONSchema_format, example_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "title": "My example schema",
    "type": "object",
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::schema_format(document);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "My example schema",
  "type": "object"
})JSON");
}

TEST(JSONSchema_format, example_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "default": 1,
    "$ref": "other",
    "x-foo": [ "bar", "baz" ],
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");

  sourcemeta::core::schema_format(document);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
  EXPECT_EQ(stream.str(), R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "default": 1,
  "x-foo": [ "bar", "baz" ],
  "$ref": "other"
})JSON");
}

TEST(JSONSchema_format, example_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "description": "test",
    "title_extension": "qux",
    "title": "foo",
    "title:extension": "baz",
    "title-extension": "bar",
    "properties": {}
  })JSON");

  sourcemeta::core::schema_format(document);
  std::ostringstream stream;
  sourcemeta::core::prettify(document, stream);
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

TEST(JSONSchema_format, nested_objects) {
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

  sourcemeta::core::schema_format(document);
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
