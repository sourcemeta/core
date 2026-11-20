#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <format> // std::format

#include <optional>      // std::nullopt
#include <sstream>       // std::ostringstream
#include <stdexcept>     // std::logic_error
#include <string>        // std::string, std::stoull
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair
#include <variant>       // std::variant
#include <vector>        // std::vector

static auto test_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://sourcemeta.com/optional-core") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/optional-core",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": false
      }
    })JSON");
  }
  if (identifier == "https://sourcemeta.com/no-core") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/no-core",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/validation": true
      }
    })JSON");
  }
  if (identifier == "https://sourcemeta.com/empty-vocabularies") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/empty-vocabularies",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": {}
    })JSON");
  }
  if (identifier == "https://sourcemeta.com/array-vocabularies") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/array-vocabularies",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": [
        "https://json-schema.org/draft/2020-12/vocab/core"
      ]
    })JSON");
  }
  if (identifier == "https://sourcemeta.com/not-a-schema-object") {
    return sourcemeta::core::JSON{"neither an object nor a boolean"};
  }
  if (identifier == "https://sourcemeta.com/string-vocabulary-value") {
    return sourcemeta::core::parse_json(R"JSON({
      "$id": "https://sourcemeta.com/string-vocabulary-value",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$vocabulary": {
        "https://json-schema.org/draft/2020-12/vocab/core": "true"
      }
    })JSON");
  }
  return sourcemeta::core::schema_resolver(identifier);
}

static auto vocabularies(const sourcemeta::core::JSON &document,
                         const sourcemeta::core::SchemaResolver &resolver,
                         const std::string_view default_dialect = "")
    -> sourcemeta::core::SchemaVocabularies {
  sourcemeta::core::SchemaFrame frame{sourcemeta::core::SchemaFrame::Mode::Root,
                                      document, sourcemeta::core::schema_walker,
                                      resolver, default_dialect};
  return frame.vocabularies(frame.root_location().value().get(), resolver);
}

static auto endless_metaschema_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  constexpr std::string_view PREFIX{"https://www.sourcemeta.com/endless/"};
  if (!identifier.starts_with(PREFIX)) {
    return sourcemeta::core::schema_resolver(identifier);
  }

  // A resolver that never runs out of links would otherwise keep an unbounded
  // chain lookup going until the process runs out of memory, so this stops
  // just past the depth a chain is allowed rather than waiting for that
  const auto link{std::stoull(std::string{identifier.substr(PREFIX.size())})};
  if (link > 100) {
    throw std::logic_error{"The chain lookup was never cut short"};
  }

  return sourcemeta::core::parse_json(std::format(R"JSON({{
    "$id": "{}{}",
    "$schema": "{}{}"
  }})JSON",
                                                  PREFIX, link, PREFIX,
                                                  link + 1));
}

static auto no_answers(const std::string_view)
    -> sourcemeta::core::SchemaResolverResult {
  return std::nullopt;
}

static auto draft7_answered_as_2020_12(const std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "http://json-schema.org/draft-07/schema" ||
      identifier == "http://json-schema.org/draft-07/schema#") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

// Reading a vocabulary set hands each entry to a callback, and the type of
// that callback is what the iteration is written out for, so one type shared
// by every case below is what makes the arms each case takes add up
struct VocabularyCollector {
  std::vector<std::pair<std::string, bool>> &seen;

  auto operator()(const sourcemeta::core::SchemaVocabularies::URI &uri,
                  const bool required) const -> void {
    this->seen.emplace_back(std::format("{}", uri), required);
  }
};

TEST(core_vocabularies_boolean_without_default) {
  const sourcemeta::core::JSON document{true};
  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

TEST(unresolvable_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://non-existent.com/dialect"
  })JSON");
  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}

TEST(override_unresolvable) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-sourcemeta-dialect-override-subschema":
      "https://non-existent.com/dialect"
  })JSON");
  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}

TEST(known_vocabulary_to_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

#define EXPECT_VOCABULARY_URI(vocabulary, expected_uri)                        \
  {                                                                            \
    std::ostringstream stream;                                                 \
    stream << vocabulary;                                                      \
    EXPECT_EQ(stream.str(), expected_uri);                                     \
  }

  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_0,
                        "http://json-schema.org/draft-00/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_0_HYPER,
                        "http://json-schema.org/draft-00/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_1,
                        "http://json-schema.org/draft-01/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_1_HYPER,
                        "http://json-schema.org/draft-01/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_2,
                        "http://json-schema.org/draft-02/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_2_HYPER,
                        "http://json-schema.org/draft-02/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_3,
                        "http://json-schema.org/draft-03/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_3_HYPER,
                        "http://json-schema.org/draft-03/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_4,
                        "http://json-schema.org/draft-04/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_4_HYPER,
                        "http://json-schema.org/draft-04/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_6,
                        "http://json-schema.org/draft-06/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_6_HYPER,
                        "http://json-schema.org/draft-06/hyper-schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_7,
                        "http://json-schema.org/draft-07/schema#");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_DRAFT_7_HYPER,
                        "http://json-schema.org/draft-07/hyper-schema#");

  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_2019_09_CORE,
                        "https://json-schema.org/draft/2019-09/vocab/core");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2019_09_APPLICATOR,
      "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2019_09_VALIDATION,
      "https://json-schema.org/draft/2019-09/vocab/validation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2019_09_META_DATA,
      "https://json-schema.org/draft/2019-09/vocab/meta-data");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_2019_09_FORMAT,
                        "https://json-schema.org/draft/2019-09/vocab/format");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_2019_09_CONTENT,
                        "https://json-schema.org/draft/2019-09/vocab/content");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2019_09_HYPER_SCHEMA,
      "https://json-schema.org/draft/2019-09/vocab/hyper-schema");

  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_2020_12_CORE,
                        "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_APPLICATOR,
      "https://json-schema.org/draft/2020-12/vocab/applicator");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_UNEVALUATED,
      "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_VALIDATION,
      "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_META_DATA,
      "https://json-schema.org/draft/2020-12/vocab/meta-data");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_FORMAT_ANNOTATION,
      "https://json-schema.org/draft/2020-12/vocab/format-annotation");
  EXPECT_VOCABULARY_URI(
      Known::JSON_SCHEMA_2020_12_FORMAT_ASSERTION,
      "https://json-schema.org/draft/2020-12/vocab/format-assertion");
  EXPECT_VOCABULARY_URI(Known::JSON_SCHEMA_2020_12_CONTENT,
                        "https://json-schema.org/draft/2020-12/vocab/content");

  EXPECT_VOCABULARY_URI(Known::OPENAPI_3_1_BASE,
                        "https://spec.openapis.org/oas/3.1/vocab/base");
  EXPECT_VOCABULARY_URI(Known::OPENAPI_3_2_BASE,
                        "https://spec.openapis.org/oas/3.2/vocab/base");
  EXPECT_VOCABULARY_URI(Known::OPENAPI_3_0_BASE,
                        "tag:spec.openapis.org,2017:oas/3.0/vocab/base");

#undef EXPECT_VOCABULARY_URI
}

TEST(throw_if_any_unsupported_all_supported_by_enum) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, false}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE, Known::JSON_SCHEMA_2020_12_APPLICATOR};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_all_supported_custom_by_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab-1", true);
  vocabularies.insert("https://example.com/custom-vocab-2", true);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE,
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab-1"},
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab-2"}};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_mixed_known_enum_and_custom_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE, Known::JSON_SCHEMA_2020_12_APPLICATOR,
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab"}};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_missing_required_known) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, true}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE, Known::JSON_SCHEMA_2020_12_APPLICATOR};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(),
              "https://json-schema.org/draft/2020-12/vocab/validation");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(throw_if_any_unsupported_missing_required_custom) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(), "https://example.com/custom-vocab");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(throw_if_any_unsupported_optional_not_checked) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, false}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_empty_vocabularies) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_known_vocab_as_string_in_vocabularies) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{};
  vocabularies.insert("https://json-schema.org/draft/2020-12/vocab/core", true);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_custom_supported_by_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE,
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab"}};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(contains_any_empty_vocabularies) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{};

  EXPECT_FALSE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE}));
  EXPECT_FALSE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE,
                                 Known::JSON_SCHEMA_2020_12_APPLICATOR}));
}

TEST(contains_any_single_match) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_TRUE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE}));
  EXPECT_TRUE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE,
                                 Known::JSON_SCHEMA_2020_12_APPLICATOR}));
  EXPECT_TRUE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_APPLICATOR,
                                         Known::JSON_SCHEMA_2020_12_CORE}));
}

TEST(contains_any_no_match) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_FALSE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_APPLICATOR}));
  EXPECT_FALSE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_APPLICATOR,
                                          Known::JSON_SCHEMA_2020_12_VALIDATION,
                                          Known::JSON_SCHEMA_2020_12_CONTENT}));
}

TEST(contains_any_multiple_vocabularies) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, false}};

  EXPECT_TRUE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE}));
  EXPECT_TRUE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_APPLICATOR}));
  EXPECT_TRUE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_VALIDATION}));
  EXPECT_TRUE(vocabularies.contains_any(
      {Known::JSON_SCHEMA_2020_12_CONTENT, Known::JSON_SCHEMA_2020_12_CORE}));
  EXPECT_FALSE(vocabularies.contains_any(
      {Known::JSON_SCHEMA_2020_12_CONTENT, Known::JSON_SCHEMA_DRAFT_7}));
}

TEST(contains_any_empty_list) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_FALSE(vocabularies.contains_any({}));
}

TEST(contains_any_matches_both_required_and_optional) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, false}};

  EXPECT_TRUE(vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_CORE}));
  EXPECT_TRUE(
      vocabularies.contains_any({Known::JSON_SCHEMA_2020_12_VALIDATION}));
}

TEST(uri_to_string_known_variant) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;
  using URI = sourcemeta::core::SchemaVocabularies::URI;

  const URI vocabulary{Known::JSON_SCHEMA_2020_12_CORE};
  std::ostringstream stream;
  stream << vocabulary;
  EXPECT_EQ(stream.str(), "https://json-schema.org/draft/2020-12/vocab/core");
}

TEST(uri_to_string_custom_variant) {
  using URI = sourcemeta::core::SchemaVocabularies::URI;

  const URI vocabulary{
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab"}};
  std::ostringstream stream;
  stream << vocabulary;
  EXPECT_EQ(stream.str(), "https://example.com/custom-vocab");
}

TEST(format_known) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  EXPECT_EQ(std::format("{}", Known::JSON_SCHEMA_2020_12_CORE),
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_EQ(std::format("{}", Known::JSON_SCHEMA_2019_09_APPLICATOR),
            "https://json-schema.org/draft/2019-09/vocab/applicator");
  EXPECT_EQ(std::format("{}", Known::JSON_SCHEMA_DRAFT_7),
            "http://json-schema.org/draft-07/schema#");
  EXPECT_EQ(std::format("{}", Known::OPENAPI_3_1_BASE),
            "https://spec.openapis.org/oas/3.1/vocab/base");
  EXPECT_EQ(std::format("{}", Known::OPENAPI_3_2_BASE),
            "https://spec.openapis.org/oas/3.2/vocab/base");
  EXPECT_EQ(std::format("{}", Known::OPENAPI_3_0_BASE),
            "tag:spec.openapis.org,2017:oas/3.0/vocab/base");
}

TEST(format_uri_known_variant) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;
  using URI = sourcemeta::core::SchemaVocabularies::URI;

  const URI vocabulary{Known::JSON_SCHEMA_2020_12_VALIDATION};
  EXPECT_EQ(std::format("{}", vocabulary),
            "https://json-schema.org/draft/2020-12/vocab/validation");
}

TEST(format_uri_custom_variant) {
  using URI = sourcemeta::core::SchemaVocabularies::URI;

  const URI vocabulary{
      sourcemeta::core::JSON::String{"https://example.com/my-vocab"}};
  EXPECT_EQ(std::format("{}", vocabulary), "https://example.com/my-vocab");
}

TEST(has_unknown_empty_vocabularies) {
  const sourcemeta::core::SchemaVocabularies vocabularies{};
  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(has_unknown_only_known_required) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true}};

  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(has_unknown_only_known_optional) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, false}};

  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(has_unknown_with_custom_required) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab", true);

  EXPECT_TRUE(vocabularies.has_unknown());
}

TEST(has_unknown_with_custom_optional) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab", false);

  EXPECT_TRUE(vocabularies.has_unknown());
}

TEST(has_unknown_with_multiple_custom) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab-1", true);
  vocabularies.insert("https://example.com/custom-vocab-2", false);

  EXPECT_TRUE(vocabularies.has_unknown());
}

TEST(contains_any_openapi_3_1_base) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}, {Known::OPENAPI_3_1_BASE, true}};

  EXPECT_TRUE(vocabularies.contains_any({Known::OPENAPI_3_1_BASE}));
  EXPECT_TRUE(vocabularies.contains_any(
      {Known::OPENAPI_3_1_BASE, Known::OPENAPI_3_2_BASE}));
  EXPECT_FALSE(vocabularies.contains_any({Known::OPENAPI_3_2_BASE}));
}

TEST(contains_any_openapi_3_2_base) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}, {Known::OPENAPI_3_2_BASE, true}};

  EXPECT_TRUE(vocabularies.contains_any({Known::OPENAPI_3_2_BASE}));
  EXPECT_TRUE(vocabularies.contains_any(
      {Known::OPENAPI_3_1_BASE, Known::OPENAPI_3_2_BASE}));
  EXPECT_FALSE(vocabularies.contains_any({Known::OPENAPI_3_1_BASE}));
}

TEST(insert_openapi_3_1_base_by_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://spec.openapis.org/oas/3.1/vocab/base", true);

  EXPECT_EQ(vocabularies.size(), 2);
  EXPECT_TRUE(vocabularies.contains(Known::OPENAPI_3_1_BASE));
  EXPECT_TRUE(vocabularies.get(Known::OPENAPI_3_1_BASE).value());
  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(insert_openapi_3_2_base_by_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://spec.openapis.org/oas/3.2/vocab/base", false);

  EXPECT_EQ(vocabularies.size(), 2);
  EXPECT_TRUE(vocabularies.contains(Known::OPENAPI_3_2_BASE));
  EXPECT_FALSE(vocabularies.get(Known::OPENAPI_3_2_BASE).value());
  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(throw_if_any_unsupported_openapi_required_not_in_supported) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}, {Known::OPENAPI_3_1_BASE, true}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(), "https://spec.openapis.org/oas/3.1/vocab/base");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(throw_if_any_unsupported_openapi_optional_not_checked) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::OPENAPI_3_1_BASE, false}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_openapi_in_supported) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}, {Known::OPENAPI_3_1_BASE, true}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE, Known::OPENAPI_3_1_BASE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(has_unknown_with_openapi_vocabularies) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::OPENAPI_3_1_BASE, true},
      {Known::OPENAPI_3_2_BASE, false}};

  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(embedded_custom_metaschema_wrong_container) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$id": "https://example.com/schema",
    "$defs": {
      "https://example.com/meta": {
        "$id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "object"
      }
    }
  })JSON");

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta");
  } catch (...) {
    FAIL();
  }
}

TEST(for_each_empty) {
  const sourcemeta::core::SchemaVocabularies vocabularies;
  std::vector<std::pair<std::string, bool>> seen;
  vocabularies.for_each(VocabularyCollector{seen});

  EXPECT_TRUE(seen.empty());
}

TEST(for_each_known_required_and_optional) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;
  sourcemeta::core::SchemaVocabularies vocabularies;
  vocabularies.insert(Known::JSON_SCHEMA_2020_12_CORE, true);
  vocabularies.insert(Known::JSON_SCHEMA_2020_12_VALIDATION, false);

  std::vector<std::pair<std::string, bool>> seen;
  vocabularies.for_each(VocabularyCollector{seen});

  EXPECT_EQ(seen.size(), 2);
  EXPECT_EQ(seen.at(0).first,
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(seen.at(0).second);
  EXPECT_EQ(seen.at(1).first,
            "https://json-schema.org/draft/2020-12/vocab/validation");
  EXPECT_FALSE(seen.at(1).second);
}

TEST(for_each_unknown) {
  sourcemeta::core::SchemaVocabularies vocabularies;
  vocabularies.insert(sourcemeta::core::JSON::String{"https://example.com/one"},
                      true);

  std::vector<std::pair<std::string, bool>> seen;
  vocabularies.for_each(VocabularyCollector{seen});

  EXPECT_EQ(seen.size(), 1);
  EXPECT_EQ(seen.at(0).first, "https://example.com/one");
  EXPECT_TRUE(seen.at(0).second);
}

TEST(for_each_known_and_unknown) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;
  sourcemeta::core::SchemaVocabularies vocabularies;
  vocabularies.insert(Known::JSON_SCHEMA_2020_12_CORE, true);
  vocabularies.insert(sourcemeta::core::JSON::String{"https://example.com/one"},
                      false);

  std::vector<std::pair<std::string, bool>> seen;
  vocabularies.for_each(VocabularyCollector{seen});

  EXPECT_EQ(seen.size(), vocabularies.size());
  EXPECT_EQ(seen.size(), 2);
  // Known vocabularies always come before unknown ones
  EXPECT_EQ(seen.at(0).first,
            "https://json-schema.org/draft/2020-12/vocab/core");
  EXPECT_TRUE(seen.at(0).second);
  EXPECT_EQ(seen.at(1).first, "https://example.com/one");
  EXPECT_FALSE(seen.at(1).second);
}

TEST(vocabularies_known_2020_12_core) {
  const auto vocabulary{
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE};
  const auto result{sourcemeta::core::to_json(vocabulary)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 21);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::Known>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(vocabulary, back.value());
}

TEST(vocabularies_known_2020_12_applicator) {
  const auto vocabulary{sourcemeta::core::SchemaVocabularies::Known::
                            JSON_SCHEMA_2020_12_APPLICATOR};
  const auto result{sourcemeta::core::to_json(vocabulary)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 22);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::Known>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(vocabulary, back.value());
}

TEST(vocabularies_known_draft_0) {
  const auto vocabulary{
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_0};
  const auto result{sourcemeta::core::to_json(vocabulary)};
  EXPECT_TRUE(result.is_integer());
  EXPECT_EQ(result.to_integer(), 0);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::Known>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(vocabulary, back.value());
}

TEST(vocabularies_known_from_json_invalid_type) {
  const sourcemeta::core::JSON input{"not-an-integer"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::Known>(
          input)};
  EXPECT_FALSE(result.has_value());
}

TEST(vocabularies_uri_with_known) {
  const sourcemeta::core::SchemaVocabularies::URI uri{
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE};
  const auto result{sourcemeta::core::to_json(uri)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 0);
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_EQ(result.at(1).to_integer(), 21);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(uri, back.value());
}

TEST(vocabularies_uri_with_custom_string) {
  const sourcemeta::core::SchemaVocabularies::URI uri{
      sourcemeta::core::JSON::String{"https://example.com/my-custom-vocab"}};
  const auto result{sourcemeta::core::to_json(uri)};
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_TRUE(result.at(1).is_string());
  EXPECT_EQ(result.at(1).to_string(), "https://example.com/my-custom-vocab");
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(uri, back.value());
}

TEST(vocabularies_uri_from_json_invalid_type) {
  const sourcemeta::core::JSON input{"not-an-array"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          input)};
  EXPECT_FALSE(result.has_value());
}

TEST(vocabularies_uri_from_json_invalid_size) {
  auto input{sourcemeta::core::JSON::make_array()};
  input.push_back(sourcemeta::core::JSON{0});
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          input)};
  EXPECT_FALSE(result.has_value());
}

TEST(vocabularies_uri_from_json_invalid_index) {
  auto input{sourcemeta::core::JSON::make_array()};
  input.push_back(sourcemeta::core::JSON{99});
  input.push_back(sourcemeta::core::JSON{0});
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          input)};
  EXPECT_FALSE(result.has_value());
}

TEST(core_cannot_be_optional) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/optional-core"
  })JSON");

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaError &error) {
    EXPECT_STREQ(error.what(), "The core vocabulary must always be required");
  }
}

TEST(core_must_be_declared) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/no-core"
  })JSON");

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaError &error) {
    EXPECT_STREQ(error.what(), "The core vocabulary must always be present");
  }
}

TEST(core_must_be_declared_even_where_nothing_is) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/empty-vocabularies"
  })JSON");

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaError &error) {
    EXPECT_STREQ(error.what(), "The core vocabulary must always be present");
  }
}

TEST(contains_custom_vocabulary_by_uri) {
  sourcemeta::core::SchemaVocabularies vocabularies;
  vocabularies.insert("https://example.com/required-vocab", true);
  vocabularies.insert("https://example.com/optional-vocab", false);
  const sourcemeta::core::JSON::String required{
      "https://example.com/required-vocab"};
  const sourcemeta::core::JSON::String optional{
      "https://example.com/optional-vocab"};
  const sourcemeta::core::JSON::String missing{
      "https://example.com/missing-vocab"};
  EXPECT_TRUE(vocabularies.contains(required));
  EXPECT_TRUE(vocabularies.contains(optional));
  EXPECT_FALSE(vocabularies.contains(missing));
}

TEST(get_custom_vocabulary_by_uri) {
  sourcemeta::core::SchemaVocabularies vocabularies;
  vocabularies.insert("https://example.com/required-vocab", true);
  vocabularies.insert("https://example.com/optional-vocab", false);
  const sourcemeta::core::JSON::String required{
      "https://example.com/required-vocab"};
  const sourcemeta::core::JSON::String optional{
      "https://example.com/optional-vocab"};
  const sourcemeta::core::JSON::String missing{
      "https://example.com/missing-vocab"};
  EXPECT_TRUE(vocabularies.get(required).has_value());
  EXPECT_TRUE(vocabularies.get(required).value());
  EXPECT_TRUE(vocabularies.get(optional).has_value());
  EXPECT_FALSE(vocabularies.get(optional).value());
  EXPECT_FALSE(vocabularies.get(missing).has_value());
}

TEST(base_dialect_to_string) {
  EXPECT_EQ(std::format(
                "{}", sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12),
            "https://json-schema.org/draft/2020-12/schema");
  EXPECT_EQ(std::format(
                "{}", sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_4),
            "http://json-schema.org/draft-04/schema#");
}

TEST(base_dialect_stream) {
  std::ostringstream stream;
  stream << sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7;
  EXPECT_EQ(stream.str(), "http://json-schema.org/draft-07/schema#");
}

TEST(throw_if_any_unsupported_known_required_supported_as_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_APPLICATOR, true}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      sourcemeta::core::JSON::String{
          "https://json-schema.org/draft/2020-12/vocab/core"},
      sourcemeta::core::JSON::String{
          "https://json-schema.org/draft/2020-12/vocab/applicator"}};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

TEST(throw_if_any_unsupported_known_required_partially_supported_as_string) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true},
      {Known::JSON_SCHEMA_2020_12_VALIDATION, true}};

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      sourcemeta::core::JSON::String{
          "https://json-schema.org/draft/2020-12/vocab/core"}};

  try {
    vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(),
              "https://json-schema.org/draft/2020-12/vocab/validation");
    EXPECT_STREQ(error.what(), "Unsupported vocabulary");
  }
}

TEST(pre_vocabulary_dialect_draft0) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/schema#"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_0));
}

TEST(pre_vocabulary_dialect_draft0_hyper) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-00/hyper-schema#"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_0_HYPER));
}

TEST(pre_vocabulary_dialect_draft7_hyper) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/hyper-schema#"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_7_HYPER));
}

TEST(pre_vocabulary_dialect_draft4_hyper) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/hyper-schema#"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4_HYPER));
}

TEST(openapi_3_0_dialect_2024_10_18) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "string",
    "nullable": true
  })JSON");
  const auto result{
      vocabularies(document, sourcemeta::core::schema_resolver,
                   "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::OPENAPI_3_0_BASE));
  EXPECT_FALSE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4));
}

TEST(openapi_3_0_dialect_2021_09_28) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "type": "string",
    "nullable": true
  })JSON");
  const auto result{
      vocabularies(document, sourcemeta::core::schema_resolver,
                   "tag:spec.openapis.org,2021-09-28:oas/3.0/dialect")};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::OPENAPI_3_0_BASE));
  EXPECT_FALSE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4));
}

TEST(openapi_3_0_dialect_via_subschema_override) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-sourcemeta-dialect-override-subschema":
      "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect",
    "type": "string",
    "nullable": true
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::OPENAPI_3_0_BASE));
}

TEST(openapi_3_0_dialect_document_is_itself_draft4) {
  const auto dialect{sourcemeta::core::schema_resolver(
      "tag:spec.openapis.org,2024-10-18:oas/3.0/dialect")};
  EXPECT_TRUE(dialect.has_value());
  const auto result{
      vocabularies(dialect.value(), sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4));
  EXPECT_FALSE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::OPENAPI_3_0_BASE));
}

// Asking by URI is a documented way of asking, and naming one of the
// vocabularies this module already knows that way used to abort a debug build
// rather than answer.
TEST(contains_a_known_vocabulary_by_its_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_TRUE(result.contains(sourcemeta::core::JSON::String{
      "https://json-schema.org/draft/2020-12/vocab/core"}));
  EXPECT_FALSE(result.contains(
      sourcemeta::core::JSON::String{"https://example.com/vocab/absent"}));
}

TEST(get_a_known_vocabulary_by_its_uri) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON");
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  const auto status{result.get(sourcemeta::core::JSON::String{
      "https://json-schema.org/draft/2020-12/vocab/core"})};
  EXPECT_TRUE(status.has_value());
  EXPECT_TRUE(status.value());
  EXPECT_FALSE(result
                   .get(sourcemeta::core::JSON::String{
                       "https://example.com/vocab/absent"})
                   .has_value());
}

// Section 8.1.2 requires the value to be an object whose property values are
// booleans. Reading a metaschema that breaks either as declaring nothing
// quietly hands back a core-only answer for a document that never said so.
TEST(vocabularies_of_a_metaschema_must_be_an_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/array-vocabularies"
  })JSON");

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(), "https://sourcemeta.com/array-vocabularies");
    EXPECT_STREQ(error.what(),
                 "The vocabularies of a metaschema must be an object");
  } catch (...) {
    FAIL();
  }
}

TEST(vocabularies_of_a_metaschema_must_be_declared_with_booleans) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://sourcemeta.com/string-vocabulary-value"
  })JSON");

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaVocabularyError &error) {
    EXPECT_EQ(error.uri(), "https://sourcemeta.com/string-vocabulary-value");
    EXPECT_STREQ(
        error.what(),
        "Every vocabulary of a metaschema must be declared with a boolean");
  } catch (...) {
    FAIL();
  }
}

// Naming a vocabulary this module does not know, where the dialect declared no
// unknown vocabularies either, leaves nothing to answer with
TEST(get_an_unknown_uri_where_no_unknown_vocabularies_were_declared) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_FALSE(result.has_unknown());
  EXPECT_FALSE(result.get("https://example.com/not-a-vocabulary").has_value());
}

// With the far end of that chain unknown, there is no base dialect to report
// rather than a guess at one
TEST(base_dialect_unresolved_where_the_chain_leaves_the_known) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta-1",
    "$defs": {
      "meta-1": {
        "$id": "https://example.com/meta-1",
        "$schema": "https://example.com/meta-2",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};

  // The chain is abandoned rather than reported step by step, so what the
  // refusal names is the dialect that was asked about and not the one further
  // along that could not be fetched
  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta-1");
  }
}

// An embedded meta-schema may hand its chain over to the resolver, which can
// answer every link with a fresh one, so walking the chain is held to the same
// depth as the dialect lookup that started it
TEST(base_dialect_an_embedded_chain_the_resolver_extends_for_ever) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://www.sourcemeta.com/endless/1",
    "$defs": {
      "endless-1": {
        "$id": "https://www.sourcemeta.com/endless/1",
        "$schema": "https://www.sourcemeta.com/endless/2"
      }
    }
  })JSON")};

  try {
    vocabularies(document, endless_metaschema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

// Per JSON Schema 2020-12 Section 4.3.5, a URI naming a resource with an empty
// fragment is "considered to be non-canonical", so the identifiers of the
// drafts before 2019-09 name the same schema with that fragment dropped. That
// equivalence is the media type's rather than the generic one, which Section
// 8.2.1 is explicit about, so it is settled here rather than left to a URI
// library and a document spelling one of those identifiers without the
// fragment needs no resolver to be read
TEST(base_dialect_a_fragmentless_draft7_without_a_resolver) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema"
  })JSON")};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root, document,
      sourcemeta::core::schema_walker, no_answers, ""};
  EXPECT_TRUE(frame.root_location().has_value());
  EXPECT_EQ(frame.root_location().value().get().base_dialect,
            sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7);
}

TEST(base_dialect_a_declared_draft7_spelling_without_a_resolver) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#"
  })JSON")};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root, document,
      sourcemeta::core::schema_walker, no_answers, ""};
  EXPECT_TRUE(frame.root_location().has_value());
  EXPECT_EQ(frame.root_location().value().get().base_dialect,
            sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7);
}

// A spelling that settles the chain is never taken as a link, so an answer the
// resolver holds for it, whatever dialect that answer declares, cannot steer
// the determination away from the official dialect the spelling names
TEST(base_dialect_a_fragmentless_draft7_despite_an_overriding_answer) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema"
  })JSON")};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root, document,
      sourcemeta::core::schema_walker, draft7_answered_as_2020_12, ""};
  EXPECT_TRUE(frame.root_location().has_value());
  EXPECT_EQ(frame.root_location().value().get().base_dialect,
            sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_DRAFT_7);
}

// A spelling that settles the chain is never taken as a link either, so a copy
// of that schema the document embeds is never read through, and however its
// own chain cycles onwards, the official dialect the spelling names settles
TEST(base_dialect_a_respelled_official_root_with_an_embedded_cyclic_copy) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "HTTPS://JSON-SCHEMA.ORG/draft/2020-12/schema",
    "$defs": {
      "copy": {
        "$id": "https://json-schema.org/draft/2020-12/schema",
        "$schema": "https://www.sourcemeta.com/custom-a"
      },
      "a": {
        "$id": "https://www.sourcemeta.com/custom-a",
        "$schema": "https://www.sourcemeta.com/custom-b"
      },
      "b": {
        "$id": "https://www.sourcemeta.com/custom-b",
        "$schema": "https://www.sourcemeta.com/custom-a"
      }
    }
  })JSON")};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Root, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver, ""};
  EXPECT_TRUE(frame.root_location().has_value());
  EXPECT_EQ(frame.root_location().value().get().base_dialect,
            sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12);
}

TEST(get_known_absent_vocabulary) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_FALSE(
      vocabularies.get(Known::JSON_SCHEMA_2020_12_APPLICATOR).has_value());
}

TEST(empty_with_only_a_custom_vocabulary) {
  sourcemeta::core::SchemaVocabularies vocabularies{};
  vocabularies.insert("https://example.com/custom-vocab", true);

  EXPECT_FALSE(vocabularies.empty());
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_TRUE(vocabularies.has_unknown());
  EXPECT_TRUE(vocabularies.contains(
      sourcemeta::core::JSON::String{"https://example.com/custom-vocab"}));
  EXPECT_TRUE(vocabularies
                  .get(sourcemeta::core::JSON::String{
                      "https://example.com/custom-vocab"})
                  .has_value());
  EXPECT_TRUE(vocabularies
                  .get(sourcemeta::core::JSON::String{
                      "https://example.com/custom-vocab"})
                  .value());
}

// Every link of an embedded meta-schema chain has to be a schema in its own
// right for the chain to say anything about the base dialect, and one that is
// not leaves the chain unable to terminate anywhere official
TEST(embedded_metaschema_chain_reaching_something_that_is_no_schema) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta-1",
    "$defs": {
      "meta-1": {
        "$id": "https://example.com/meta-1",
        "$schema": "https://sourcemeta.com/not-a-schema-object",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};

  try {
    vocabularies(document, test_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

TEST(vocabularies_uri_without_an_index) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ "0", 1 ])JSON")};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(vocabularies_uri_with_an_unknown_vocabulary) {
  const auto document{sourcemeta::core::parse_json(R"JSON([ 0, "x" ])JSON")};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(vocabularies_uri_with_a_non_string_name) {
  const auto document{sourcemeta::core::parse_json("[ 1, 2 ]")};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::SchemaVocabularies::URI>(
          document)};
  EXPECT_FALSE(result.has_value());
}

TEST(empty_with_a_required_known_vocabulary) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_FALSE(vocabularies.empty());
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_TRUE(vocabularies.contains(Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_TRUE(vocabularies.get(Known::JSON_SCHEMA_2020_12_CORE).has_value());
  EXPECT_TRUE(vocabularies.get(Known::JSON_SCHEMA_2020_12_CORE).value());
  EXPECT_FALSE(vocabularies.contains(Known::JSON_SCHEMA_2020_12_CONTENT));
  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(empty_with_an_optional_known_vocabulary) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CONTENT, false}};

  EXPECT_FALSE(vocabularies.empty());
  EXPECT_EQ(vocabularies.size(), 1);
  EXPECT_TRUE(vocabularies.contains(Known::JSON_SCHEMA_2020_12_CONTENT));
  EXPECT_TRUE(vocabularies.get(Known::JSON_SCHEMA_2020_12_CONTENT).has_value());
  EXPECT_FALSE(vocabularies.get(Known::JSON_SCHEMA_2020_12_CONTENT).value());
  EXPECT_FALSE(vocabularies.contains(Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_FALSE(vocabularies.has_unknown());
}

TEST(empty_with_no_vocabulary_at_all) {
  const sourcemeta::core::SchemaVocabularies vocabularies{};

  EXPECT_TRUE(vocabularies.empty());
  EXPECT_EQ(vocabularies.size(), 0);
}

// A vocabulary this implementation knows may still be asked for by its URI,
// which is the slower of the two ways of asking and has to answer the same
TEST(contains_the_first_known_vocabulary_by_its_uri) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  const sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};

  EXPECT_TRUE(vocabularies.contains(
      "https://json-schema.org/draft/2020-12/vocab/core"));
  EXPECT_TRUE(
      vocabularies.get("https://json-schema.org/draft/2020-12/vocab/core")
          .has_value());
  EXPECT_TRUE(
      vocabularies.get("https://json-schema.org/draft/2020-12/vocab/core")
          .value());
}

// Only a vocabulary a dialect requires has to be supported, which holds for
// one this implementation does not know by name as much as for one it does
TEST(throw_if_any_unsupported_optional_custom_not_checked) {
  using Known = sourcemeta::core::SchemaVocabularies::Known;

  sourcemeta::core::SchemaVocabularies vocabularies{
      {Known::JSON_SCHEMA_2020_12_CORE, true}};
  vocabularies.insert("https://example.com/custom-vocab", false);

  const std::unordered_set<sourcemeta::core::SchemaVocabularies::URI> supported{
      Known::JSON_SCHEMA_2020_12_CORE};

  vocabularies.throw_if_any_unsupported(supported, "Unsupported vocabulary");
}

// A document may embed the meta-schema it names, and the chain that walks to
// an official base dialect holds every link to the container that base
// dialect reserves for one. Draft 7 and Draft 6 reserve `definitions` and
// introduce an identifier with `$id`
TEST(embedded_metaschema_chain_terminating_at_draft7_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-07/hyper-schema#"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_7_HYPER));
}

// This project keeps a vocabulary of its own in the registry, which a
// meta-schema may declare even though no keyword of it is read here
TEST(a_metaschema_declaring_the_extension_vocabulary_of_this_project) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true,
          "tag:sourcemeta.com,2026:extension/v1": true
        }
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::SOURCEMETA_EXTENSION_V1));
  EXPECT_FALSE(result.has_unknown());
}

// A relative identifier is held up to the one asked for by resolving it
// against the identifier of the document that carries it, which a document
// whose own identifier reads as no URI cannot anchor
TEST(an_embedded_metaschema_relative_within_a_document_naming_no_uri) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$id": "http://[",
    "definitions": {
      "meta": {
        "$id": "meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta");
  } catch (...) {
    FAIL();
  }
}

// The three oldest hyper-schema drafts reserve no place for a schema to carry
// another, so a meta-schema a document embeds can never sit where one of them
// would read it, whichever of the two names it is written under
TEST(embedded_metaschema_chain_terminating_at_draft2_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-02/hyper-schema#"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta");
  } catch (...) {
    FAIL();
  }
}

TEST(embedded_metaschema_chain_terminating_at_draft1_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-01/hyper-schema#"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta");
  } catch (...) {
    FAIL();
  }
}

TEST(embedded_metaschema_chain_terminating_at_draft0_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-00/hyper-schema#"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://example.com/meta");
  } catch (...) {
    FAIL();
  }
}

// 2019-09 and 2020-12 reserve `$defs`, and go on supporting `definitions`
// for backwards compatibility, so a meta-schema a document embeds under the
// older name stands for them as well. One declaring no vocabularies of its
// own leaves the core vocabulary of the dialect it reads by in force, which
// JSON Schema 2020-12 Core, section 8.1.2.1 requires of an implementation
// that carries on reading a meta-schema it does not recognise
TEST(embedded_metaschema_chain_under_the_older_container_of_2020_12) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_FALSE(result.has_unknown());
}

TEST(embedded_metaschema_chain_under_the_older_container_of_2019_09) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2019-09/schema"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2019_09_CORE));
  EXPECT_FALSE(result.has_unknown());
}

TEST(embedded_metaschema_chain_terminating_at_draft6_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-06/hyper-schema#"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_6_HYPER));
}

// Draft 4 and Draft 3 reserve `definitions` as well, while the keyword that
// introduces an identifier for them is `id`
TEST(embedded_metaschema_chain_terminating_at_draft4_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-04/hyper-schema#"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4_HYPER));
}

TEST(embedded_metaschema_chain_terminating_at_draft3_hyper) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-03/hyper-schema#"
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_3_HYPER));
}

// An entry whose identifier is of no use as one names no resource, so the
// entry that does name the resource is the one that answers for it
TEST(embedded_metaschema_candidate_whose_identifier_is_no_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "not-a-resource": {
        "$id": 5,
        "$schema": "https://json-schema.org/draft/2020-12/schema"
      },
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
}

// A container that holds no members at all reserves nothing to search, so the
// search carries on to the next one
TEST(embedded_metaschema_container_that_is_no_object) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": 5,
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true,
          "https://json-schema.org/draft/2020-12/vocab/validation": true
        }
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_TRUE(result.contains(sourcemeta::core::SchemaVocabularies::Known::
                                  JSON_SCHEMA_2020_12_VALIDATION));
}

// In 2019-09 and 2020-12 the older name for the container is still read, so a
// document that keeps its embedded meta-schema there is honoured as well
TEST(embedded_metaschema_under_the_older_container_name) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "definitions": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true,
          "https://json-schema.org/draft/2020-12/vocab/meta-data": true
        }
      }
    }
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_TRUE(result.contains(sourcemeta::core::SchemaVocabularies::Known::
                                  JSON_SCHEMA_2020_12_META_DATA));
}

// Every official dialect was published under one spelling of its scheme and
// is widely written with the other, so both name the same base dialect
TEST(alternate_spelling_of_the_2020_12_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft/2020-12/schema"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 7);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
}

TEST(alternate_spelling_of_the_2019_09_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft/2019-09/schema"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 6);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2019_09_CORE));
}

TEST(alternate_spelling_of_the_2020_12_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft/2020-12/hyper-schema"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
}

TEST(alternate_spelling_of_the_2019_09_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft/2019-09/hyper-schema"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_TRUE(result.contains(sourcemeta::core::SchemaVocabularies::Known::
                                  JSON_SCHEMA_2019_09_HYPER_SCHEMA));
}

TEST(alternate_spelling_of_the_draft7_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-07/schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_7));
}

TEST(alternate_spelling_of_the_draft7_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-07/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_7_HYPER));
}

TEST(alternate_spelling_of_the_draft6_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-06/schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_6));
}

TEST(alternate_spelling_of_the_draft6_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-06/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_6_HYPER));
}

TEST(alternate_spelling_of_the_draft4_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-04/schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4));
}

TEST(alternate_spelling_of_the_draft4_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-04/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_4_HYPER));
}

TEST(alternate_spelling_of_the_draft3_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-03/schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_3));
}

TEST(alternate_spelling_of_the_draft3_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-03/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_3_HYPER));
}

TEST(alternate_spelling_of_the_draft2_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-02/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_2_HYPER));
}

TEST(alternate_spelling_of_the_draft1_hyper_dialect) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft-01/hyper-schema#"
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_DRAFT_1_HYPER));
}

// The keyword that names a dialect from within a subschema names one only
// where it carries a string, so a value of any other shape leaves the
// declaration of the schema itself to answer
TEST(dialect_override_that_is_no_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-sourcemeta-dialect-override-subschema": 5
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 7);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
}

// An empty string names no dialect either, as a dialect is named by a URI
TEST(dialect_override_that_is_an_empty_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "x-sourcemeta-dialect-override-subschema": ""
  })JSON")};
  const auto result{vocabularies(document, sourcemeta::core::schema_resolver)};
  EXPECT_EQ(result.size(), 7);
  EXPECT_TRUE(result.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
}

// A link of an embedded meta-schema chain names what describes it with a URI,
// and a name that is no URI reaches neither another link nor the resolver
TEST(embedded_metaschema_chain_naming_something_that_is_no_uri) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "not-a-uri"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}

// A chain that reaches an official base dialect may still leave the dialect
// in force naming itself, which says nothing about what describes it
TEST(embedded_metaschema_whose_override_names_the_dialect_it_describes) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "x-sourcemeta-dialect-override-subschema": "https://example.com/meta"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

// Draft 7 reserves `definitions` for schema definitions and leaves `$defs` as
// payload, so an embedded meta-schema kept there is not one the document can
// be read against
TEST(embedded_metaschema_under_a_container_the_dialect_leaves_as_payload) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "$id": "https://example.com/meta",
        "$schema": "http://json-schema.org/draft-07/schema#"
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}

// An identifier spelled relatively resolves against the one the document
// carries, which a declaration that is not even a string does not give
TEST(embedded_metaschema_beside_a_base_identifier_that_is_no_string) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$id": 5,
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "$id": "meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}

// RFC 3986 Section 3.2.3 writes a port as digits, and one past the range a
// port number reaches reads as a URI while settling as none, which leaves the
// dialect it names unreadable
TEST(embedded_metaschema_named_by_a_uri_that_settles_as_none) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://example.com:99999999999999/meta",
    "$defs": {
      "meta": {
        "$id": "https://example.com/other",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_STREQ(error.what(), "The dialect is not a valid URI");
    EXPECT_EQ(error.keyword(), "$schema");
    EXPECT_EQ(error.value(), "http://example.com:99999999999999/meta");
  }
}

TEST(embedded_metaschema_declaring_a_uri_that_settles_as_none) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://example.com/meta",
    "$defs": {
      "meta": {
        "$id": "http://example.com:99999999999999/meta",
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$vocabulary": {
          "https://json-schema.org/draft/2020-12/vocab/core": true
        }
      }
    }
  })JSON")};

  try {
    vocabularies(document, sourcemeta::core::schema_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not resolve the metaschema of the schema");
  }
}
