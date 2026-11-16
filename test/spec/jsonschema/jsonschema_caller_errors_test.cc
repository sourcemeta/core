#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string_view>

// Neither fixture suite can express a resolver or a walker of the caller's
// that throws, and what these pin is that such an error arrives as it was
// thrown. Both operations reach for both components of their own accord,
// inside handlers that translate failures of their own making, so an error of
// the caller's that spells the same as one of those is the case that tells
// them apart
//
// The shape is the same throughout: the entry document names a remote, and the
// remote declares a dialect the caller's resolver refuses to answer for, which
// is what puts the error inside the lookup each operation runs to decide
// whether it can read what it just fetched
static constexpr std::string_view ENTRY{R"JSON({
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$id": "https://www.sourcemeta.com/entry",
  "items": {
    "$ref": "https://www.sourcemeta.com/remote"
  }
})JSON"};

static auto remote_of(const std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://www.sourcemeta.com/custom-dialect",
      "$id": "https://www.sourcemeta.com/remote",
      "type": "string"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

static auto resolution_throwing_resolver(const std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/custom-dialect") {
    throw sourcemeta::core::SchemaResolutionError(
        "The caller could not reach this one",
        "https://www.sourcemeta.com/custom-dialect");
  }

  return remote_of(identifier);
}

static auto base_dialect_throwing_resolver(const std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/custom-dialect") {
    throw sourcemeta::core::SchemaUnknownBaseDialectError();
  }

  return remote_of(identifier);
}

static auto keyword_throwing_resolver(const std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/custom-dialect") {
    throw sourcemeta::core::SchemaKeywordError(
        "A keyword error of the caller's own", "x-caller", "42");
  }

  return remote_of(identifier);
}

static auto
limit_throwing_walker(const std::string_view keyword,
                      const sourcemeta::core::SchemaVocabularies &vocabularies)
    -> const sourcemeta::core::SchemaWalkerResult & {
  if (keyword == "items") {
    throw sourcemeta::core::SchemaFrameLimitError{789};
  }

  return sourcemeta::core::schema_walker(keyword, vocabularies);
}

TEST(dependencies_a_resolution_error_of_the_callers_own_is_unchanged) {
  const auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, resolution_throwing_resolver,
        [](const auto &) -> void {});
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "The caller could not reach this one");
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/custom-dialect");
  }
}

TEST(dependencies_a_base_dialect_error_of_the_callers_own_is_unchanged) {
  const auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker,
        base_dialect_throwing_resolver, [](const auto &) -> void {});
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

TEST(dependencies_a_keyword_error_of_the_callers_own_is_unchanged) {
  const auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, keyword_throwing_resolver,
        [](const auto &) -> void {});
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_STREQ(error.what(), "A keyword error of the caller's own");
    EXPECT_EQ(error.keyword(), "x-caller");
    EXPECT_EQ(error.value(), "42");
  }
}

TEST(dependencies_a_limit_error_from_the_walker_keeps_its_own_allowance) {
  const auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_dependencies(document, limit_throwing_walker,
                                          remote_of,
                                          [](const auto &) -> void {}, {});
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 789);
  }
}

TEST(bundle_a_resolution_error_of_the_callers_own_is_unchanged) {
  auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    resolution_throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "The caller could not reach this one");
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/custom-dialect");
  }
}

TEST(bundle_a_base_dialect_error_of_the_callers_own_is_unchanged) {
  auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_bundle(document, sourcemeta::core::schema_walker,
                                    base_dialect_throwing_resolver);
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }
}

TEST(bundle_a_limit_error_from_the_walker_keeps_its_own_allowance) {
  auto document{sourcemeta::core::parse_json(ENTRY)};

  try {
    sourcemeta::core::schema_bundle(document, limit_throwing_walker, remote_of);
    FAIL();
  } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
    EXPECT_EQ(error.limit(), 789);
  }
}
