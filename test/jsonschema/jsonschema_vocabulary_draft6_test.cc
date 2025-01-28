#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string> // std::string

static auto test_resolver(std::string_view identifier)
    -> std::optional<sourcemeta::core::JSON> {
  if (identifier == "https://sourcemeta.com/one-hop") {
    return sourcemeta::core::parse(R"JSON({
      "$id": "https://sourcemeta.com/one-hop",
      "$schema": "http://json-schema.org/draft-06/schema#"
    })JSON");
  } else if (identifier == "https://sourcemeta.com/with-vocabulary") {
    return sourcemeta::core::parse(R"JSON({
      "$id": "https://sourcemeta.com/with-vocabulary",
      "$schema": "http://json-schema.org/draft-06/schema#",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": true,
        "https://json-schema.org/draft/2020-12/vocab/applicator": true,
        "https://json-schema.org/draft/2020-12/vocab/validation": false
      }
    })JSON");
  } else {
    return sourcemeta::core::official_resolver(identifier);
  }
}

#define EXPECT_VOCABULARY_REQUIRED(vocabularies, vocabulary)                   \
  EXPECT_TRUE((vocabularies).contains(vocabulary));                            \
  EXPECT_TRUE((vocabularies).at(vocabulary));

#define EXPECT_VOCABULARY_OPTIONAL(vocabularies, vocabulary)                   \
  EXPECT_TRUE((vocabularies).contains(vocabulary));                            \
  EXPECT_FALSE((vocabularies).at(vocabulary));

TEST(JSONSchema_vocabulary_draft6, schema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-06/schema#",
    "type": "object"
  })JSON");
  const std::map<std::string, bool> vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_vocabulary_draft6, hyperschema) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "http://json-schema.org/draft-06/hyper-schema#",
    "type": "object"
  })JSON");
  const std::map<std::string, bool> vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-06/hyper-schema#");
}

TEST(JSONSchema_vocabulary_draft6, one_hop) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://sourcemeta.com/one-hop",
    "type": "object"
  })JSON");
  const std::map<std::string, bool> vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-06/schema#");
}

TEST(JSONSchema_vocabulary_draft6, ignore_vocabulary) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse(R"JSON({
    "$schema": "https://sourcemeta.com/with-vocabulary",
    "type": "object"
  })JSON");
  const std::map<std::string, bool> vocabularies{
      sourcemeta::core::vocabularies(document, test_resolver)};
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_VOCABULARY_REQUIRED(vocabularies,
                             "http://json-schema.org/draft-06/schema#");
}
