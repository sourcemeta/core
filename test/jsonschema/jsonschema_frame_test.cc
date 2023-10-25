#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

TEST(JSONSchema_frame, nested_schemas_mixing_dialects) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/test",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$defs": {
      "foo": {
        "id": "foo",
        "$schema": "http://json-schema.org/draft-04/schema#",
        "definitions": {
          "bar": {
            "id": "bar",
            "type": "string"
          }
        }
      }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(static_frame.size(), 3);

  EXPECT_TRUE(static_frame.contains("https://www.sourcemeta.com/test"));
  EXPECT_TRUE(static_frame.contains("https://www.sourcemeta.com/foo"));
  EXPECT_TRUE(static_frame.contains("https://www.sourcemeta.com/bar"));

  EXPECT_EQ(std::get<0>(static_frame.at("https://www.sourcemeta.com/test")),
            "https://www.sourcemeta.com/test");
  EXPECT_EQ(std::get<1>(static_frame.at("https://www.sourcemeta.com/test")),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(std::get<2>(static_frame.at("https://www.sourcemeta.com/test")),
            "https://json-schema.org/draft/2020-12/schema");

  EXPECT_EQ(std::get<0>(static_frame.at("https://www.sourcemeta.com/foo")),
            "https://www.sourcemeta.com/test");
  EXPECT_EQ(std::get<1>(static_frame.at("https://www.sourcemeta.com/foo")),
            sourcemeta::jsontoolkit::Pointer({"$defs", "foo"}));
  EXPECT_EQ(std::get<2>(static_frame.at("https://www.sourcemeta.com/foo")),
            "http://json-schema.org/draft-04/schema#");

  EXPECT_EQ(std::get<0>(static_frame.at("https://www.sourcemeta.com/bar")),
            "https://www.sourcemeta.com/test");
  EXPECT_EQ(
      std::get<1>(static_frame.at("https://www.sourcemeta.com/bar")),
      sourcemeta::jsontoolkit::Pointer({"$defs", "foo", "definitions", "bar"}));
  EXPECT_EQ(std::get<2>(static_frame.at("https://www.sourcemeta.com/bar")),
            "http://json-schema.org/draft-04/schema#");
}

TEST(JSONSchema_frame, no_id) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_TRUE(static_frame.empty());
}
