#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <sstream>       // std::ostringstream
#include <unordered_set> // std::unordered_set
#include <variant>       // std::variant

TEST(JSONSchema_vocabulary, core_vocabularies_boolean_without_default) {
  const sourcemeta::core::JSON document{true};
  EXPECT_THROW(sourcemeta::core::vocabularies(
                   document, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_vocabulary, unresolvable_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://non-existent.com/dialect"
  })JSON");
  EXPECT_THROW(sourcemeta::core::vocabularies(
                   document, sourcemeta::core::schema_official_resolver),
               sourcemeta::core::SchemaResolutionError);
}

TEST(JSONSchema_vocabulary, known_vocabulary_to_string) {
  using Known = sourcemeta::core::Vocabularies::Known;

#define EXPECT_VOCABULARY_URI(vocabulary, expected_uri)                        \
  {                                                                            \
    std::ostringstream stream;                                                 \
    stream << vocabulary;                                                      \
    EXPECT_EQ(stream.str(), expected_uri);                                     \
  }

  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_0,
                        "http://json-schema.org/draft-00/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_0_Hyper,
                        "http://json-schema.org/draft-00/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_1,
                        "http://json-schema.org/draft-01/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_1_Hyper,
                        "http://json-schema.org/draft-01/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_2,
                        "http://json-schema.org/draft-02/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_2_Hyper,
                        "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_3,
                        "http://json-schema.org/draft-03/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_3_Hyper,
                        "http://json-schema.org/draft-03/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_4,
                        "http://json-schema.org/draft-04/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_4_Hyper,
                        "http://json-schema.org/draft-04/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_6,
                        "http://json-schema.org/draft-06/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_6_Hyper,
                        "http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_7,
                        "http://json-schema.org/draft-07/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_Draft_7_Hyper,
                        "http://json-schema.org/draft-07/hyper-schema#");

  EXPECT_VOCABULARY_URI(Known::JSON_Schema_2019_09_Core,
                        "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2019_09_Applicator,
      "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2019_09_Validation,
      "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2019_09_Meta_Data,
      "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_2019_09_Format,
                        "https://json-schema.org/draft/2019-09/vocab/format");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_2019_09_Content,
                        "https://json-schema.org/draft/2019-09/vocab/content");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2019_09_Hyper_Schema,
      "https://json-schema.org/draft/2019-09/vocab/hyper-schema");

  EXPECT_VOCABULARY_URI(Known::JSON_Schema_2020_12_Core,
                        "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Applicator,
      "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Unevaluated,
      "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Validation,
      "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Meta_Data,
      "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Format_Annotation,
      "https://json-schema.org/draft/2020-12/vocab/format-annotation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_Schema_2020_12_Format_Assertion,
      "https://json-schema.org/draft/2020-12/vocab/format-assertion");
  EXPECT_VOCABULARY_URI(Known::JSON_Schema_2020_12_Content,
                        "https://json-schema.org/draft/2020-12/vocab/content");

#undef EXPECT_VOCABULARY_URI
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_all_supported_by_enum) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true},
      {Known::JSON_Schema_2020_12_Applicator, true},
      {Known::JSON_Schema_2020_12_Validation, false}};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core,
                Known::JSON_Schema_2020_12_Applicator};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_all_supported_by_string) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true},
      {Known::JSON_Schema_2020_12_Applicator, true}};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{sourcemeta::core::JSON::String{
                    "https://json-schema.org/draft/2020-12/vocab/core"},
                sourcemeta::core::JSON::String{
                    "https://json-schema.org/draft/2020-12/vocab/applicator"}};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_mixed_enum_and_string) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true},
      {Known::JSON_Schema_2020_12_Applicator, true}};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core,
                sourcemeta::core::JSON::String{
                    "https://json-schema.org/draft/2020-12/vocab/applicator"}};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_missing_required_known) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true},
      {Known::JSON_Schema_2020_12_Applicator, true},
      {Known::JSON_Schema_2020_12_Validation, true}};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core,
                Known::JSON_Schema_2020_12_Applicator};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(),
              "https://json-schema.org/draft/2020-12/vocab/validation");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_missing_required_custom) {
  using Known = sourcemeta::core::Vocabularies::Known;

  sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(), "https://example.com/custom-vocab");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_optional_not_checked) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true},
      {Known::JSON_Schema_2020_12_Validation, false}};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary, throw_if_any_unsupported_empty_vocabularies) {
  using Known = sourcemeta::core::Vocabularies::Known;

  const sourcemeta::core::Vocabularies vocabularies{};

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary,
     throw_if_any_unsupported_known_vocab_as_string_in_vocabularies) {
  using Known = sourcemeta::core::Vocabularies::Known;

  sourcemeta::core::Vocabularies vocabularies{};
  vocabularies.insert("https://json-schema.org/draft/2020-12/vocab/core", true);

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{Known::JSON_Schema_2020_12_Core};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}

TEST(JSONSchema_vocabulary,
     throw_if_any_unsupported_custom_supported_by_string) {
  using Known = sourcemeta::core::Vocabularies::Known;

  sourcemeta::core::Vocabularies vocabularies{
      {Known::JSON_Schema_2020_12_Core, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  const std::unordered_set<std::variant<sourcemeta::core::JSON::String, Known>>
      supported{
          Known::JSON_Schema_2020_12_Core,
          sourcemeta::core::JSON::String{"https://example.com/custom-vocab"}};

  EXPECT_NO_THROW(vocabularies.throw_if_any_unsupported(
      supported, "Unsupported vocabulary"));
}
