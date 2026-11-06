#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <string_view>
#include <unordered_set>

// The fixture suite answers for a fixed list of URIs, so what it cannot
// express is a caller whose answer changes while the walk runs, which is what
// a caller stitching several documents into one span does. Only then does a
// reference that nothing answered for reach the end of the walk and settle
// against what the caller has since come to hold

static auto sibling_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/sibling") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/sibling",
      "$defs": {
        "inner": {
          "$id": "https://www.sourcemeta.com/sibling-inner",
          "type": "string"
        }
      }
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_reference_the_caller_comes_to_account_for_settles_at_the_end) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/elsewhere" },
    "contains": { "$ref": "https://www.sourcemeta.com/sibling" }
  })JSON");

  // Reading the sibling is what leaves the caller holding the document that
  // the other reference names, which it only knows once the walk says so
  std::unordered_set<std::string> held;
  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted = [&held](const std::string_view identifier) -> bool {
    return held.contains(std::string{identifier});
  };
  options.accounted_callback =
      [&held](const std::string_view identifier) -> void {
    held.emplace(identifier);
    if (identifier == "https://www.sourcemeta.com/sibling-inner") {
      held.emplace("https://www.sourcemeta.com/elsewhere");
    }
  };

  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, sibling_resolver,
      [](const auto &) -> void {}, options)};

  EXPECT_TRUE(held.contains("https://www.sourcemeta.com/elsewhere"));
}

TEST(a_reference_the_caller_never_accounts_for_ends_analysis) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/elsewhere" },
    "contains": { "$ref": "https://www.sourcemeta.com/sibling" }
  })JSON");

  std::unordered_set<std::string> held;
  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted = [&held](const std::string_view identifier) -> bool {
    return held.contains(std::string{identifier});
  };
  options.accounted_callback =
      [&held](const std::string_view identifier) -> void {
    held.emplace(identifier);
  };

  try {
    [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, sibling_resolver,
        [](const auto &) -> void {}, options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/elsewhere");
  }
}
