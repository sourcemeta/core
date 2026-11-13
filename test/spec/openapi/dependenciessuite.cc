#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/openapi.h>
#include <sourcemeta/core/test.h>

#include <algorithm>   // std::ranges::find
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem
#include <optional>    // std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

// Every key a fixture may declare. Anything else is a mistake that would
// otherwise go unnoticed, as the runner would simply not read it
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::vector<std::string> KNOWN_KEYS{
    "document",       "defaultBase",  "openapiResolver",
    "schemaResolver", "maxLocations", "expected",
    "locations",      "fetched",      "error"};

// Every key an entry of what analysis reported declares, the last of them
// being whether the entry is the one that had analysis resolve the document it
// names, which is what hands the document over
const std::vector<std::string> KNOWN_DEPENDENCY_KEYS{
    "origin", "pointer", "destination", "kind", "resolved"};

// Every exception that analysis throws, named after the class itself so that a
// fixture states which one it means rather than leaving the runner to guess
// from the fields that happen to be there
const std::vector<std::string> KNOWN_ERROR_TYPES{"OpenAPIResolutionError",
                                                 "OpenAPIReferenceError",
                                                 "OpenAPIFrameLimitError",
                                                 "SchemaResolutionError",
                                                 "SchemaReferenceError",
                                                 "SchemaDialectImpossibleError",
                                                 "OpenAPIError"};

const std::vector<std::string> KNOWN_ERROR_KEYS{
    "type", "message",  "identifier", "limit",
    "base", "location", "keyword",    "dialect"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

using Asked = std::vector<sourcemeta::core::JSON::String>;

// Every document the walk asked the OpenAPI resolver for, answered or not, in
// order and with repeats. One it already holds never arrives here, so this is
// what a caller paying per fetch is charged for
auto make_openapi_resolver(const sourcemeta::core::JSON &test,
                           const std::shared_ptr<Asked> &asked)
    -> sourcemeta::core::OpenAPIResolver {
  const auto registry{test.defines("openapiResolver")
                          ? test.at("openapiResolver")
                          : sourcemeta::core::JSON::make_object()};
  return [registry, asked](const std::string_view identifier)
             -> sourcemeta::core::OpenAPIResolverResult {
    if (asked != nullptr) {
      asked->emplace_back(identifier);
    }

    const auto *match{
        registry.try_at(sourcemeta::core::JSON::String{identifier})};
    if (match != nullptr) {
      return *match;
    }

    return std::nullopt;
  };
}

// And this one answers for what sits inside a Schema Object. Only what a
// fixture writes down is served from it, as the dialects this library ships
// answer for themselves and say nothing about the description
auto make_schema_resolver(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::SchemaResolver {
  const auto registry{test.defines("schemaResolver")
                          ? test.at("schemaResolver")
                          : sourcemeta::core::JSON::make_object()};
  return [registry](const std::string_view identifier)
             -> sourcemeta::core::SchemaResolverResult {
    const auto *match{
        registry.try_at(sourcemeta::core::JSON::String{identifier})};
    if (match != nullptr) {
      return *match;
    }

    return sourcemeta::core::schema_resolver(identifier);
  };
}

auto make_options(const sourcemeta::core::JSON &test,
                  const sourcemeta::core::JSON::String &base)
    -> sourcemeta::core::OpenAPIDependencyOptions {
  sourcemeta::core::OpenAPIDependencyOptions options;
  options.default_base = base;
  if (test.defines("maxLocations")) {
    options.max_locations =
        static_cast<std::uint64_t>(test.at("maxLocations").to_integer());
  }

  return options;
}

// The edges land in what the caller hands over, so the ones that analysis
// reported before it threw are still there to be read afterwards
auto collect(const sourcemeta::core::JSON &test,
             const sourcemeta::core::SchemaResolver &schema_resolver,
             const sourcemeta::core::OpenAPIResolver &resolver,
             const sourcemeta::core::JSON::String &base,
             sourcemeta::core::JSON &result) -> std::uint64_t {
  return sourcemeta::core::openapi_dependencies(
      test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
      resolver,
      [&result](const auto &dependency) -> void {
        auto entry{sourcemeta::core::JSON::make_object()};
        entry.assign("origin", sourcemeta::core::JSON{dependency.origin});
        entry.assign("pointer",
                     sourcemeta::core::JSON{
                         sourcemeta::core::to_string(dependency.pointer)});
        entry.assign("destination",
                     sourcemeta::core::JSON{dependency.destination});
        entry.assign("kind",
                     sourcemeta::core::JSON{sourcemeta::core::openapi_kind_name(
                         dependency.expected)});
        entry.assign("resolved",
                     sourcemeta::core::JSON{dependency.document != nullptr});

        result.push_back(std::move(entry));
      },
      make_options(test, base));
}

// Every key a fixture declares has to be one the runner reads, and a fixture
// has to say what analysis reports whether or not it also says how it failed
auto check_shape(const sourcemeta::core::JSON &test) -> void {
  for (const auto &entry : test.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_KEYS, entry.first) !=
                KNOWN_KEYS.cend());
  }

  EXPECT_TRUE(test.defines("document"));
  EXPECT_TRUE(test.defines("expected"));
  EXPECT_TRUE(test.at("expected").is_array());
  for (const auto &dependency : test.at("expected").as_array()) {
    EXPECT_TRUE(dependency.is_object());
    EXPECT_EQ(dependency.size(), KNOWN_DEPENDENCY_KEYS.size());
    for (const auto &entry : dependency.as_object()) {
      EXPECT_TRUE(std::ranges::find(KNOWN_DEPENDENCY_KEYS, entry.first) !=
                  KNOWN_DEPENDENCY_KEYS.cend());
      EXPECT_TRUE(entry.first == "resolved" ? entry.second.is_boolean()
                                            : entry.second.is_string());
    }
  }

  // Analysis that throws reports no count at all, so a fixture that expects
  // one states the edges it got to before that and nothing more
  if (test.defines("error")) {
    EXPECT_FALSE(test.defines("locations"));
  } else {
    EXPECT_TRUE(test.defines("locations"));
  }

  EXPECT_TRUE(test.defines("fetched"));
  EXPECT_TRUE(test.at("fetched").is_array());
  for (const auto &identifier : test.at("fetched").as_array()) {
    EXPECT_TRUE(identifier.is_string());
  }

  if (!test.defines("error")) {
    return;
  }

  const auto &error{test.at("error")};
  EXPECT_TRUE(error.is_object());
  for (const auto &entry : error.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_ERROR_KEYS, entry.first) !=
                KNOWN_ERROR_KEYS.cend());
  }

  const auto &type{error.at("type").to_string()};
  EXPECT_TRUE(std::ranges::find(KNOWN_ERROR_TYPES, type) !=
              KNOWN_ERROR_TYPES.cend());
  EXPECT_TRUE(error.defines("message"));
  EXPECT_EQ(error.defines("identifier"), type == "OpenAPIResolutionError" ||
                                             type == "OpenAPIReferenceError" ||
                                             type == "SchemaResolutionError" ||
                                             type == "SchemaReferenceError");
  EXPECT_EQ(error.defines("limit"), type == "OpenAPIFrameLimitError");
  const auto carries_base{type == "OpenAPIResolutionError" ||
                          type == "OpenAPIReferenceError"};
  EXPECT_EQ(error.defines("base"), carries_base);
  EXPECT_EQ(error.defines("location"),
            carries_base || type == "SchemaReferenceError" ||
                type == "SchemaDialectImpossibleError");
  EXPECT_EQ(error.defines("keyword"), type == "SchemaDialectImpossibleError");
  EXPECT_EQ(error.defines("dialect"), type == "SchemaDialectImpossibleError");
}

auto check_error(const sourcemeta::core::JSON &test,
                 const sourcemeta::core::SchemaResolver &schema_resolver,
                 const sourcemeta::core::OpenAPIResolver &resolver,
                 const sourcemeta::core::JSON::String &base,
                 sourcemeta::core::JSON &reported) -> void {
  const auto &expected{test.at("error")};
  const auto &type{expected.at("type").to_string()};
  const auto &message{expected.at("message").to_string()};

  if (type == "OpenAPIResolutionError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIResolutionError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      EXPECT_EQ(error.base(), expected.at("base").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
    }
  } else if (type == "OpenAPIReferenceError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIReferenceError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      EXPECT_EQ(error.base(), expected.at("base").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
    }
  } else if (type == "SchemaResolutionError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::SchemaResolutionError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
    }
  } else if (type == "SchemaReferenceError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::SchemaReferenceError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
    }
  } else if (type == "SchemaDialectImpossibleError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::SchemaDialectImpossibleError &error) {
      // A declaration that asserts the conditions under which it could not
      // have been written, so both halves of it are read back
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.keyword(), expected.at("keyword").to_string());
      EXPECT_EQ(error.dialect(), expected.at("dialect").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
    }
  } else if (type == "OpenAPIFrameLimitError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIFrameLimitError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.limit(),
                static_cast<std::uint64_t>(expected.at("limit").to_integer()));
    }
  } else {
    try {
      [[maybe_unused]] const auto locations{
          collect(test, schema_resolver, resolver, base, reported)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
    }
  }
}

auto run_dependencies_test(const sourcemeta::core::JSON &test) -> void {
  check_shape(test);

  const auto asked{std::make_shared<Asked>()};
  const auto resolver{make_openapi_resolver(test, asked)};
  const auto schema_resolver{make_schema_resolver(test)};
  const sourcemeta::core::JSON::String base{
      test.defines("defaultBase") ? test.at("defaultBase").to_string()
                                  : sourcemeta::core::JSON::String{}};

  auto reported{sourcemeta::core::JSON::make_array()};
  if (test.defines("error")) {
    check_error(test, schema_resolver, resolver, base, reported);
  } else {
    const auto locations{
        collect(test, schema_resolver, resolver, base, reported)};
    EXPECT_EQ(locations,
              static_cast<std::uint64_t>(test.at("locations").to_integer()));
  }

  EXPECT_EQ(reported, test.at("expected"));

  auto actual{sourcemeta::core::JSON::make_array()};
  for (const auto &identifier : *asked) {
    actual.push_back(sourcemeta::core::JSON{identifier});
  }

  EXPECT_EQ(actual, test.at("fetched"));
}

auto register_tests(const std::filesystem::path &directory) -> void {
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".json") {
      continue;
    }

    const auto group{entry.path().parent_path().filename().string()};
    const auto name{entry.path().stem().string()};
    sourcemeta::core::test_register(
        "OpenAPIDependencies_" + group, name, __FILE__, __LINE__,
        [path = entry.path()]() -> void {
          run_dependencies_test(sourcemeta::core::read_json(path));
        });
  }
}

} // namespace

auto main(int argc, char *argv[]) -> int {
  register_tests(DEPENDENCIES_SUITE_PATH);
  return sourcemeta::core::test_run(argc, argv);
}
