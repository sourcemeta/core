#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>

TEST(JSONSchema_frame_draft6, empty_schema) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(static_frame.size(), 1);
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/schema"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/schema"),
            "https://www.sourcemeta.com/schema");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/schema"),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/schema"),
            "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_frame_draft6, one_level_applicators_without_identifiers) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "items": { "type": "string" },
    "properties": {
      "foo": { "type": "number" }
    }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(static_frame.size(), 1);
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/schema"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/schema"),
            "https://www.sourcemeta.com/schema");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/schema"),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/schema"),
            "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_frame_draft6, one_level_applicators_with_identifiers) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/test/qux",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "items": { "$id": "../foo", "type": "string" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(static_frame.size(), 2);

  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/test/qux"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/test/qux"),
            "https://www.sourcemeta.com/test/qux");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/test/qux"),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/test/qux"),
            "http://json-schema.org/draft-06/schema#");

  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/foo"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/foo"),
            "https://www.sourcemeta.com/test/qux");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/foo"),
            sourcemeta::jsontoolkit::Pointer{"items"});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/foo"),
            "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_frame_draft6, subschema_absolute_identifier) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "items": {
      "$id": "https://www.sourcemeta.com/foo",
      "type": "string"
     }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver)
      .wait();

  EXPECT_EQ(static_frame.size(), 2);

  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/schema"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/schema"),
            "https://www.sourcemeta.com/schema");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/schema"),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/schema"),
            "http://json-schema.org/draft-06/schema#");

  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/foo"));
  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/foo"),
            "https://www.sourcemeta.com/schema");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/foo"),
            sourcemeta::jsontoolkit::Pointer{"items"});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/foo"),
            "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_frame_draft6, id_override) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-06/schema#",
    "items": { "$id": "schema" }
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  EXPECT_THROW(sourcemeta::jsontoolkit::frame(
                   document, static_frame,
                   sourcemeta::jsontoolkit::default_schema_walker,
                   sourcemeta::jsontoolkit::official_resolver)
                   .wait(),
               sourcemeta::jsontoolkit::SchemaError);
}

TEST(JSONSchema_frame_draft6, explicit_argument_id_same) {
  const sourcemeta::jsontoolkit::JSON document =
      sourcemeta::jsontoolkit::parse(R"JSON({
    "$id": "https://www.sourcemeta.com/schema",
    "$schema": "http://json-schema.org/draft-06/schema#"
  })JSON");

  sourcemeta::jsontoolkit::ReferenceFrame static_frame;
  sourcemeta::jsontoolkit::frame(document, static_frame,
                                 sourcemeta::jsontoolkit::default_schema_walker,
                                 sourcemeta::jsontoolkit::official_resolver,
                                 "http://json-schema.org/draft-06/schema#",
                                 "https://www.sourcemeta.com/schema")
      .wait();

  EXPECT_EQ(static_frame.size(), 1);
  EXPECT_TRUE(static_frame.defines("https://www.sourcemeta.com/schema"));

  EXPECT_EQ(static_frame.base("https://www.sourcemeta.com/schema"),
            "https://www.sourcemeta.com/schema");
  EXPECT_EQ(static_frame.pointer("https://www.sourcemeta.com/schema"),
            sourcemeta::jsontoolkit::Pointer{});
  EXPECT_EQ(static_frame.dialect("https://www.sourcemeta.com/schema"),
            "http://json-schema.org/draft-06/schema#");
}
