#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string> // std::string

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://sourcemeta.com/one-hop") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/one-hop",
      "$schema": "http://json-schema.org/draft-02/schema#"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/with-vocabulary") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/with-vocabulary",
      "$schema": "http://json-schema.org/draft-02/schema#",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": true,
        "https://json-schema.org/draft/2020-12/vocab/applicator": true,
        "https://json-schema.org/draft/2020-12/vocab/validation": false
      }
    })JSON");
  } else {
    return sourcemeta::core::schema_official_resolver(identifier);
  }
}

#define EXPECT_VOCABULARY_REQUIRED(vocabularies, vocabulary)                   \
  EXPECT_TRUE((vocabularies).contains(vocabulary));                            \
  EXPECT_TRUE((vocabularies).get(vocabulary).value());

#define EXPECT_VOCABULARY_OPTIONAL(vocabularies, vocabulary)                   \
  EXPECT_TRUE((vocabularies).contains(vocabulary));                            \
  EXPECT_FALSE((vocabularies).get(vocabulary).value());

TEST(JSONSchema_vocabulary_draft2, schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-02/schema#",
    "type": "object"
  })JSON");
  const sourcemeta::core::Vocabularies vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-02/schema#");
}

TEST(JSONSchema_vocabulary_draft2, hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-02/hyper-schema#",
    "type": "object"
  })JSON");
  const sourcemeta::core::Vocabularies vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-02/hyper-schema#");
}

TEST(JSONSchema_vocabulary_draft2, one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/one-hop",
    "type": "object"
  })JSON");
  const sourcemeta::core::Vocabularies vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-02/hyper-schema#");
}

TEST(JSONSchema_vocabulary_draft2, ignore_vocabulary) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/with-vocabulary",
    "type": "object"
  })JSON");
  const sourcemeta::core::Vocabularies vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-02/hyper-schema#");
}
