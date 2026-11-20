#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

// Besides whatever a document says of itself, it answers to what the caller
// says it is, and the fixture suite has no way to hand one a name of its own

// A document that is nothing but a boolean says nothing, and there is nowhere
// on it to write a name down, so the name the caller handed over stands for it
// without being written into it
TEST(a_boolean_document_handed_a_name_of_its_own) {
  auto document = sourcemeta::core::parse_json("true");

  const auto spent{sourcemeta::core::schema_bundle(
      document, sourcemeta::core::schema_walker,
      sourcemeta::core::schema_resolver,
      "https://json-schema.org/draft/2020-12/schema",
      "https://example.com/handed")};

  EXPECT_EQ(document, sourcemeta::core::parse_json("true"));
  EXPECT_TRUE(spent > 0);
}
