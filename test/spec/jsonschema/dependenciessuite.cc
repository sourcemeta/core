#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm>   // std::ranges::find
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <limits>      // std::numeric_limits
#include <memory>      // std::shared_ptr, std::make_shared
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace {

// Every key a fixture may declare. Anything else is a mistake that would
// otherwise go unnoticed, as the runner would simply not read it
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::vector<std::string> KNOWN_KEYS{
    "schema",       "resolver",    "defaultDialect",
    "defaultId",    "defaultBase", "paths",
    "maxLocations", "accounted",   "accountedReported",
    "dependencies", "locations",   "fetched",
    "error"};

// Every exception that analysis throws, named after the class itself so that
// a fixture states which one it means rather than leaving the runner to guess
// from the fields that happen to be there
const std::vector<std::string> KNOWN_ERROR_TYPES{
    "SchemaResolutionError", "SchemaRelativeMetaschemaResolutionError",
    "SchemaReferenceError", "SchemaUnknownBaseDialectError",
    "SchemaFrameLimitError"};

const std::vector<std::string> KNOWN_ERROR_KEYS{"type", "message", "identifier",
                                                "location", "limit"};

// Every key an entry of what analysis reported declares, the last of them
// being whether the entry is the one that had analysis resolve the document it
// names, which is what hands the document over
const std::vector<std::string> KNOWN_DEPENDENCY_KEYS{"origin", "pointer",
                                                     "destination", "resolved"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

using Asked = std::vector<sourcemeta::core::JSON::String>;

// Every document the walk asked the resolver for, answered or not, in order
// and with repeats. One the walk already holds never arrives here, so this is
// what a caller paying per fetch is charged for rather than every question the
// walk asked itself, and a question nothing answers is charged for too.
// Official meta-schemas are left out, as framing reads those whatever the walk
// does
auto make_resolver(const sourcemeta::core::JSON &test,
                   const std::shared_ptr<Asked> &asked)
    -> sourcemeta::core::SchemaResolver {
  const auto registry{test.defines("resolver")
                          ? test.at("resolver")
                          : sourcemeta::core::JSON::make_object()};
  return [registry, asked](const std::string_view identifier)
             -> sourcemeta::core::SchemaResolverResult {
    if (asked != nullptr && !sourcemeta::core::schema_is_official(identifier)) {
      asked->emplace_back(identifier);
    }

    const auto *match{
        registry.try_at(sourcemeta::core::JSON::String{identifier})};
    if (match != nullptr) {
      return *match;
    }

    return sourcemeta::core::schema_resolver(identifier);
  };
}

// Analysis keeps views into the default dialect, the default identifier and
// the default base for as long as it runs, and the paths it walks point into
// pointers that the caller owns, so the caller owns all of it
struct Inputs {
  sourcemeta::core::JSON::String default_dialect;
  sourcemeta::core::JSON::String default_id;
  sourcemeta::core::JSON::String default_base;
  std::vector<sourcemeta::core::Pointer> path_storage;
  sourcemeta::core::SchemaFrame::Paths paths;
  std::uint64_t max_locations{std::numeric_limits<std::uint64_t>::max()};
  std::vector<sourcemeta::core::JSON::String> accounted;
};

auto make_inputs(const sourcemeta::core::JSON &test) -> Inputs {
  Inputs inputs;
  const auto *raw_dialect{test.try_at("defaultDialect")};
  if (raw_dialect != nullptr) {
    inputs.default_dialect = raw_dialect->to_string();
  }

  const auto *raw_id{test.try_at("defaultId")};
  if (raw_id != nullptr) {
    inputs.default_id = raw_id->to_string();
  }

  const auto *raw_base{test.try_at("defaultBase")};
  if (raw_base != nullptr) {
    inputs.default_base = raw_base->to_string();
  }

  const auto *raw_limit{test.try_at("maxLocations")};
  if (raw_limit != nullptr) {
    inputs.max_locations = static_cast<std::uint64_t>(raw_limit->to_integer());
  }

  for (const auto &identifier : test.at("accounted").as_array()) {
    inputs.accounted.push_back(identifier.to_string());
  }

  const auto *raw_paths{test.try_at("paths")};
  if (raw_paths == nullptr) {
    inputs.paths.push_back(sourcemeta::core::EMPTY_WEAK_POINTER);
    return inputs;
  }

  // Every pointer has to be in its final place before anything points into it
  inputs.path_storage.reserve(raw_paths->size());
  for (const auto &path : raw_paths->as_array()) {
    inputs.path_storage.push_back(
        sourcemeta::core::to_pointer(path.to_string()));
  }

  inputs.paths.reserve(inputs.path_storage.size());
  for (const auto &path : inputs.path_storage) {
    inputs.paths.push_back(sourcemeta::core::to_weak_pointer(path));
  }

  return inputs;
}

// The views and paths that the options carry point into what the fixture owns,
// so the result never outlives the inputs it came from
auto make_options(const Inputs &inputs)
    -> sourcemeta::core::SchemaDependenciesOptions {
  sourcemeta::core::SchemaDependenciesOptions options;
  options.default_dialect = inputs.default_dialect;
  options.default_id = inputs.default_id;
  options.paths = inputs.paths;
  options.default_base = inputs.default_base;
  options.max_locations = inputs.max_locations;
  options.accounted = [&inputs](const std::string_view identifier) -> bool {
    return std::ranges::find(inputs.accounted, identifier) !=
           inputs.accounted.cend();
  };
  return options;
}

// The edges land in what the caller hands over, so the ones that analysis
// reported before it threw are still there to be read afterwards
auto collect(const sourcemeta::core::JSON &schema,
             const sourcemeta::core::SchemaResolver &resolver,
             const sourcemeta::core::SchemaResolver &unlogged,
             const Inputs &inputs, sourcemeta::core::JSON &result,
             sourcemeta::core::JSON &accounted) -> std::uint64_t {
  auto options{make_options(inputs)};
  options.accounted_callback =
      [&accounted](const std::string_view identifier) -> void {
    accounted.push_back(sourcemeta::core::JSON{identifier});
  };

  return sourcemeta::core::schema_dependencies(
      schema, sourcemeta::core::schema_walker, resolver,
      [&unlogged, &result](const auto &dependency) -> void {
        // What answers to the destination is handed over alongside it, so a
        // fixture never has to repeat the document it already registered
        if (dependency.document != nullptr) {
          const auto remote{unlogged(dependency.destination)};
          EXPECT_TRUE(remote.has_value());
          EXPECT_EQ(*dependency.document, remote.value());
        }

        auto entry{sourcemeta::core::JSON::make_object()};
        entry.assign("origin", sourcemeta::core::JSON{dependency.origin});
        entry.assign("pointer",
                     sourcemeta::core::JSON{
                         sourcemeta::core::to_string(dependency.pointer)});
        entry.assign("destination",
                     sourcemeta::core::JSON{dependency.destination});
        entry.assign("resolved",
                     sourcemeta::core::JSON{dependency.document != nullptr});

        result.push_back(std::move(entry));
      },
      options);
}

// Every key a fixture declares has to be one the runner reads, a fixture has
// to say either what analysis reports or how it fails but never both, and an
// error has to carry the fields that the class it names actually has
auto check_shape(const sourcemeta::core::JSON &test) -> void {
  for (const auto &entry : test.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_KEYS, entry.first) !=
                KNOWN_KEYS.cend());
  }

  EXPECT_TRUE(test.defines("schema"));
  EXPECT_TRUE(test.defines("dependencies") || test.defines("error"));

  // Analysis that throws reports no count at all, so a fixture that expects
  // one states the edges it got to before that and nothing more. One that
  // returns has both required of it
  EXPECT_TRUE(test.defines("dependencies"));
  if (test.defines("error")) {
    EXPECT_FALSE(test.defines("locations"));
  } else {
    EXPECT_TRUE(test.defines("locations"));
  }
  if (test.defines("locations")) {
    EXPECT_TRUE(test.at("locations").is_integer());
    EXPECT_TRUE(test.at("locations").to_integer() >= 0);
  }

  EXPECT_TRUE(test.defines("fetched"));
  EXPECT_TRUE(test.at("fetched").is_array());
  EXPECT_TRUE(test.defines("accounted"));
  EXPECT_TRUE(test.at("accounted").is_array());
  EXPECT_TRUE(test.defines("accountedReported"));
  EXPECT_TRUE(test.at("accountedReported").is_array());
  for (const auto &identifier : test.at("fetched").as_array()) {
    EXPECT_TRUE(identifier.is_string());
  }

  if (test.defines("dependencies")) {
    const auto &dependencies{test.at("dependencies")};
    EXPECT_TRUE(dependencies.is_array());
    for (const auto &dependency : dependencies.as_array()) {
      EXPECT_TRUE(dependency.is_object());
      EXPECT_EQ(dependency.size(), KNOWN_DEPENDENCY_KEYS.size());
      for (const auto &entry : dependency.as_object()) {
        EXPECT_TRUE(std::ranges::find(KNOWN_DEPENDENCY_KEYS, entry.first) !=
                    KNOWN_DEPENDENCY_KEYS.cend());
        EXPECT_TRUE(entry.first == "resolved" ? entry.second.is_boolean()
                                              : entry.second.is_string());
      }
    }
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
  EXPECT_EQ(error.defines("identifier"),
            type == "SchemaResolutionError" ||
                type == "SchemaRelativeMetaschemaResolutionError" ||
                type == "SchemaReferenceError");
  EXPECT_EQ(error.defines("location"), type == "SchemaReferenceError");
  EXPECT_EQ(error.defines("limit"), type == "SchemaFrameLimitError");
}

auto check_error(const sourcemeta::core::JSON &schema,
                 const sourcemeta::core::SchemaResolver &resolver,
                 const sourcemeta::core::SchemaResolver &unlogged,
                 const Inputs &inputs, const sourcemeta::core::JSON &expected,
                 sourcemeta::core::JSON &reported,
                 sourcemeta::core::JSON &accounted) -> void {
  const auto &type{expected.at("type").to_string()};
  const auto &message{expected.at("message").to_string()};

  if (type == "SchemaRelativeMetaschemaResolutionError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(schema, resolver, unlogged, inputs, reported, accounted)};
      FAIL();
    } catch (const sourcemeta::core::SchemaRelativeMetaschemaResolutionError
                 &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
    }
  } else if (type == "SchemaResolutionError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(schema, resolver, unlogged, inputs, reported, accounted)};
      FAIL();
    } catch (
        const sourcemeta::core::SchemaRelativeMetaschemaResolutionError &) {
      // Naming the wider class will not do for a throw that was the narrower
      FAIL();
    } catch (const sourcemeta::core::SchemaResolutionError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
    }
  } else if (type == "SchemaReferenceError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(schema, resolver, unlogged, inputs, reported, accounted)};
      FAIL();
    } catch (const sourcemeta::core::SchemaReferenceError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
    }
  } else if (type == "SchemaUnknownBaseDialectError") {
    try {
      [[maybe_unused]] const auto locations{
          collect(schema, resolver, unlogged, inputs, reported, accounted)};
      FAIL();
    } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
    }
  } else {
    try {
      [[maybe_unused]] const auto locations{
          collect(schema, resolver, unlogged, inputs, reported, accounted)};
      FAIL();
    } catch (const sourcemeta::core::SchemaFrameLimitError &error) {
      EXPECT_STREQ(error.what(), message.c_str());
      EXPECT_EQ(error.limit(),
                static_cast<std::uint64_t>(expected.at("limit").to_integer()));
    }
  }
}

auto run_dependencies_test(const sourcemeta::core::JSON &test) -> void {
  check_shape(test);

  const auto asked{std::make_shared<Asked>()};
  const auto resolver{make_resolver(test, asked)};
  const auto unlogged{make_resolver(test, nullptr)};
  const auto inputs{make_inputs(test)};

  auto reported{sourcemeta::core::JSON::make_array()};
  auto accounted{sourcemeta::core::JSON::make_array()};

  if (test.defines("error")) {
    check_error(test.at("schema"), resolver, unlogged, inputs, test.at("error"),
                reported, accounted);
    EXPECT_EQ(reported, test.at("dependencies"));
  } else {
    const auto locations{collect(test.at("schema"), resolver, unlogged, inputs,
                                 reported, accounted)};
    EXPECT_EQ(reported, test.at("dependencies"));
    EXPECT_EQ(locations,
              static_cast<std::uint64_t>(test.at("locations").to_integer()));
  }

  // What the walk asked the resolver for is what a caller tracking files has
  // to be able to trust, so every fixture pins it, including the ones that
  // say how far the walk got before it threw
  auto actual{sourcemeta::core::JSON::make_array()};
  for (const auto &identifier : *asked) {
    actual.push_back(sourcemeta::core::JSON{identifier});
  }

  EXPECT_EQ(actual, test.at("fetched"));

  // Whatever analysis came to account for is what a caller stitching several
  // documents into one span answers the predicate above from, so every
  // fixture pins it alongside what the walk asked for
  EXPECT_EQ(accounted, test.at("accountedReported"));
}

auto register_tests(const std::filesystem::path &directory) -> std::size_t {
  std::size_t count{0};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::recursive_directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".json") {
      continue;
    }

    const auto suite{entry.path().parent_path().filename().string()};
    std::ostringstream name;
    for (const auto character : entry.path().stem().string()) {
      name << (character == '-' ? '_' : character);
    }

    const auto test{sourcemeta::core::read_json(entry.path())};
    sourcemeta::core::test_register(
        "DependenciesSuite_" + suite, name.str(), __FILE__, __LINE__,
        [test]() -> void { run_dependencies_test(test); });
    count += 1;
  }

  return count;
}

} // namespace

auto main(int argc, char **argv) -> int {
  const auto count{
      register_tests(std::filesystem::path{DEPENDENCIES_SUITE_PATH})};
  // A fixture in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice
  if (count == 0) {
    std::cerr << "No dependency fixtures found at " << DEPENDENCIES_SUITE_PATH
              << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}
