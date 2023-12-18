#include <gtest/gtest.h>

#include <set>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

#include "jsonschema_test_utils.h"

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

  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/test"));
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/foo"));
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/bar"));

  EXPECT_FRAME(static_frame, "https://www.sourcemeta.com/test",
               "https://www.sourcemeta.com/test", "",
               "https://json-schema.org/draft/2020-12/schema");
  EXPECT_FRAME(static_frame, "https://www.sourcemeta.com/foo",
               "https://www.sourcemeta.com/test", "/$defs/foo",
               "http://json-schema.org/draft-04/schema#");
  EXPECT_FRAME(static_frame, "https://www.sourcemeta.com/bar",
               "https://www.sourcemeta.com/test", "/$defs/foo/definitions/bar",
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

TEST(JSONSchema_frame, no_id_with_default) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "type": "string" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver,
                                 "https://json-schema.org/draft/2020-12/schema",
                                 "https://www.sourcemeta.com/schema")
      .wait();

  EXPECT_EQ(static_frame.size(), 1);
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/schema"));
  EXPECT_FRAME(static_frame, "https://www.sourcemeta.com/schema",
               "https://www.sourcemeta.com/schema", "",
               "https://json-schema.org/draft/2020-12/schema");
}

TEST(JSONSchema_frame, uri_iterators) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": {
      "$id": "test",
      "$anchor": "foo",
      "type": "string"
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  std::set<std::string> uris;
  for (const auto &uri : static_frame) {
    uris.insert(uri);
  }

  EXPECT_EQ(static_frame.size(), 3);
  EXPECT_EQ(uris.size(), 3);
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/schema"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test"));
  EXPECT_TRUE(uris.contains("https://www.sourcemeta.com/test#foo"));
}

TEST(JSONSchema_frame, reference_frame_uri_canonicalize) {
  sourcemeta::jsontoolkit::ReferenceFrame frame;
  frame.store("https://example.com#", "https://example.com#",
              sourcemeta::jsontoolkit::empty_pointer,
              "https://json-schema.org/draft/2020-12/schema");
  EXPECT_EQ(frame.size(), 1);
  EXPECT_TRUE(frame.defines("https://example.com"));
  // This is canonicalized
  EXPECT_TRUE(frame.defines("https://example.com#"));
  // This must not be canonicalized
  EXPECT_EQ(frame.base("https://example.com"), "https://example.com#");
}
