#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/openapi.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <algorithm>   // std::ranges::find
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <functional>  // std::less
#include <iostream>    // std::cerr
#include <optional>    // std::nullopt, std::optional
#include <set>         // std::set
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

// Every key a fixture may declare. Anything else is a mistake that would
// otherwise go unnoticed, as the runner would simply not read it
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::vector<std::string> KNOWN_KEYS{"document", "defaultBase",
                                          "schemaResolver", "frame", "error"};
const std::vector<std::string> KNOWN_ERROR_KEYS{
    "message", "location", "base", "identifier", "keyword", "value"};
const std::vector<std::string> KNOWN_METHODS{"get",    "put",     "post",
                                             "delete", "options", "head",
                                             "patch",  "trace",   "query"};
// The serialised names of the Objects a location may hold, and of the routes
// an operation may be reached through. With the enums private these names are
// the public contract, so a rename or an omission has to fail rather than pass
// every fixture that happens not to cover it
const std::vector<std::string> KNOWN_TYPES{"openapi",
                                           "path-item",
                                           "parameter",
                                           "request-body",
                                           "response",
                                           "example",
                                           "header",
                                           "link",
                                           "callback",
                                           "security-scheme",
                                           "info",
                                           "contact",
                                           "license",
                                           "server",
                                           "server-variable",
                                           "components",
                                           "paths",
                                           "operation",
                                           "external-documentation",
                                           "media-type",
                                           "encoding",
                                           "responses",
                                           "tag",
                                           "reference",
                                           "schema",
                                           "oauth-flows",
                                           "oauth-flow",
                                           "security-requirement"};
const std::vector<std::string> KNOWN_OPERATION_TYPES{"path", "webhook",
                                                     "callback"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

// A description may be written against a dialect that nobody publishes, which
// a fixture says where to find by writing it down. Everything else comes from
// the dialects that are published
auto make_schema_resolver(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::SchemaResolver {
  if (!test.defines("schemaResolver")) {
    return sourcemeta::core::schema_resolver;
  }

  const auto &registry{test.at("schemaResolver")};
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

// A frame keeps a view into the base it was given, so the base has to outlive
// it. The caller owns this, as anything built inside the analysis would dangle
// on return and only misbehave later, when the frame is read back
auto make_default_base(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::JSON::String {
  const auto *base{test.try_at("defaultBase")};
  return base == nullptr ? sourcemeta::core::JSON::String{} : base->to_string();
}

// A fixture cannot name the location it will be read from, so it writes
// `[PATH]` where the directory holding it belongs and the runner fills that in
// with the URI of that directory. That makes the runner stand in for whatever
// retrieved the document, which is what OpenAPI 3.1 leaves to the caller.
//
// It is a URI rather than a path because a path is not one. A Windows path
// carries a drive letter and backslashes, neither of which a URI reference
// admits, so a fixture spelling the scheme itself and having the runner paste a
// path after it produces something that parses nowhere but on POSIX
auto expand(const sourcemeta::core::JSON::String &value,
            const std::filesystem::path &directory)
    -> sourcemeta::core::JSON::String {
  constexpr auto PLACEHOLDER{"[PATH]"};
  const auto replacement{
      sourcemeta::core::URI::from_path(directory).recompose()};

  sourcemeta::core::JSON::String result{value};
  auto position{result.find(PLACEHOLDER)};
  while (position != sourcemeta::core::JSON::String::npos) {
    result.replace(position, std::string_view{PLACEHOLDER}.size(), replacement);
    position = result.find(PLACEHOLDER, position);
  }

  return result;
}

// Whether one string names the directory a fixture sits in. What follows the
// name has to end the reference or begin a new segment of it, as a sibling
// directory whose name merely starts the same way names somewhere else
auto names(const sourcemeta::core::JSON::String &value,
           const sourcemeta::core::JSON::String &directory) -> bool {
  auto position{value.find(directory)};
  while (position != sourcemeta::core::JSON::String::npos) {
    const auto next{position + directory.size()};
    if (next == value.size() || value.at(next) == '/') {
      return true;
    }

    position = value.find(directory, position + 1);
  }

  return false;
}

// Which string of a fixture spells that directory out rather than standing in
// for it, if any does. Such a fixture passes on whichever machine wrote it and
// nowhere else, which is what recording what a run reported, without putting
// the placeholder back, comes to
auto spelled_out_by(const sourcemeta::core::JSON &value,
                    const sourcemeta::core::JSON::String &directory)
    -> std::optional<sourcemeta::core::JSON::String> {
  if (value.is_string()) {
    if (names(value.to_string(), directory)) {
      return value.to_string();
    }
  } else if (value.is_array()) {
    for (const auto &entry : value.as_array()) {
      const auto match{spelled_out_by(entry, directory)};
      if (match.has_value()) {
        return match;
      }
    }
  } else if (value.is_object()) {
    for (const auto &entry : value.as_object()) {
      if (names(entry.first, directory)) {
        return entry.first;
      }

      const auto match{spelled_out_by(entry.second, directory)};
      if (match.has_value()) {
        return match;
      }
    }
  }

  return std::nullopt;
}

// The placeholder stands anywhere a path may be written, including the keys a
// frame is reported under, so every string in the fixture goes through it
auto expand_all(const sourcemeta::core::JSON &value,
                const std::filesystem::path &directory)
    -> sourcemeta::core::JSON {
  if (value.is_string()) {
    return sourcemeta::core::JSON{expand(value.to_string(), directory)};
  }

  if (value.is_array()) {
    auto result{sourcemeta::core::JSON::make_array()};
    for (const auto &entry : value.as_array()) {
      result.push_back(expand_all(entry, directory));
    }

    return result;
  }

  if (value.is_object()) {
    auto result{sourcemeta::core::JSON::make_object()};
    for (const auto &entry : value.as_object()) {
      result.assign(expand(entry.first, directory),
                    expand_all(entry.second, directory));
    }

    return result;
  }

  return value;
}

auto check_known_keys(const sourcemeta::core::JSON &test) -> void {
  for (const auto &entry : test.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_KEYS, entry.first) !=
                KNOWN_KEYS.cend());
  }

  EXPECT_TRUE(test.defines("document"));
}

// Whether everything the Schema Objects reference is among the places framing
// them found, which is what standing alone comes to on that side. The dialect
// a schema names is the exception, as a schema is under no obligation to
// carry the meta-schema it is written against
auto schemas_stand_alone(const sourcemeta::core::JSON &schemas) -> bool {
  const auto &locations{schemas.at("locations")};
  for (const auto &reference : schemas.at("references").as_array()) {
    if (reference.at("origin").to_string().ends_with("/$schema")) {
      continue;
    }

    const auto &destination{reference.at("destination").to_string()};
    if (!locations.at("static").defines(destination) &&
        !locations.at("dynamic").defines(destination)) {
      return false;
    }
  }

  return true;
}

// A frame is a graph written down as text, and every edge in it is a key into
// the same map. These hold whatever the description was, so the suite asserts
// them on every fixture rather than on the handful that thought to look
auto check_frame_invariants(const sourcemeta::core::JSON &frame) -> void {
  const auto &locations{frame.at("locations")};

  // The entry document is an Object like any other, so the base names it, and
  // what it names is the OpenAPI Object at its root
  const auto &base{frame.at("base").to_string()};
  EXPECT_TRUE(locations.defines(base));
  if (locations.defines(base)) {
    EXPECT_EQ(locations.at(base).at("type").to_string(), "openapi");
  }

  for (const auto &entry : locations.as_object()) {
    // A location carries its pointer, and its key is that pointer hung off the
    // document it sits in as a properly escaped fragment. So the key has to
    // parse as a URI and its fragment has to decode back to that same pointer,
    // which is what a path template or a callback expression would otherwise
    // break
    const auto &pointer{entry.second.at("pointer").to_string()};
    if (pointer.empty()) {
      EXPECT_EQ(entry.first.find('#'), sourcemeta::core::JSON::String::npos);
    } else {
      const sourcemeta::core::URI uri{entry.first};
      const auto recovered{sourcemeta::core::fragment_to_pointer(uri)};
      EXPECT_TRUE(recovered.has_value());
      if (recovered.has_value()) {
        EXPECT_EQ(sourcemeta::core::to_string(recovered.value()), pointer);
      }
    }

    // Only the root of a document has no parent, and every other parent is a
    // key into this same map
    const auto &parent{entry.second.at("parent")};
    EXPECT_EQ(parent.is_null(), pointer.empty());
    if (parent.is_string()) {
      EXPECT_TRUE(locations.defines(parent.to_string()));
    }

    const auto type{entry.second.at("type").to_string()};
    EXPECT_TRUE(std::ranges::find(KNOWN_TYPES, type) != KNOWN_TYPES.cend());

    // The dialect in force is recorded where a JSON Schema implementation
    // needs it, which is the root of a document and each Schema Object
    EXPECT_EQ(entry.second.defines("dialect"),
              type == "openapi" || type == "schema");

    // A Schema Object position carries the base to resolve against too, and
    // the base a document keys its locations by is the part of every one of
    // those keys that comes before the fragment
    EXPECT_EQ(entry.second.defines("base"), type == "schema");
    if (type == "schema") {
      const auto &schema_base{entry.second.at("base").to_string()};
      EXPECT_TRUE(entry.first.starts_with(schema_base));
      EXPECT_TRUE(entry.first.size() > schema_base.size() &&
                  entry.first.at(schema_base.size()) == '#');

      // The frame builds a key by appending the escaped pointer to the base,
      // which is what resolving the two through a URI comes to, and the two
      // have to keep coming to the same thing for a schema frame of the same
      // document to be addressable by these very keys
      EXPECT_EQ(entry.first,
                sourcemeta::core::to_uri(sourcemeta::core::to_pointer(pointer),
                                         sourcemeta::core::URI{schema_base})
                    .recompose());

      // Which is what lets one name the other. Where a Schema Object begins is
      // where framing it begins, and both sides address that place the same
      // way, so every position this reports is a place the schema frame holds
      EXPECT_TRUE(frame.at("schemas")
                      .at("locations")
                      .at("static")
                      .defines(entry.first));
    }

    // A reference is written down on the Object that makes it, which is a
    // Reference Object, a Path Item Object declaring a `$ref`, or a Link
    // Object declaring an `operationRef`
    EXPECT_EQ(entry.second.defines("original"),
              entry.second.defines("destination"));
    if (!entry.second.defines("destination")) {
      continue;
    }

    EXPECT_TRUE(type == "reference" || type == "path-item" || type == "link");

    // Where a reference lands is a key into this same map, and a reference
    // says of itself whether it is one of those, which is what the walk was
    // never in a position to read
    const auto &destination{entry.second.at("destination").to_string()};
    EXPECT_EQ(entry.second.at("dangling").to_boolean(),
              !locations.defines(destination));
  }

  // A description stands alone when nothing it references leaves it, which
  // counts what its Schema Objects reference as much as what the shell around
  // them does. This asks the locations again rather than carrying an answer
  // out of the loop above, so that a check added there can never quietly
  // narrow what this covers
  EXPECT_EQ(frame.at("standalone").to_boolean(),
            schemas_stand_alone(frame.at("schemas")) &&
                std::ranges::none_of(
                    locations.as_object(), [](const auto &entry) -> bool {
                      return entry.second.defines("dangling") &&
                             entry.second.at("dangling").to_boolean();
                    }));

  for (const auto &operation : frame.at("operations").as_array()) {
    // Every operation the description exposes is an Operation Object that the
    // walk read, and what is in force where it sits was read too
    const auto &origin{operation.at("origin").to_string()};
    EXPECT_TRUE(locations.defines(origin));
    EXPECT_EQ(locations.at(origin).at("type").to_string(), "operation");
    // 3.2 lets a Path Item name a method of its own, so the closed set only
    // accounts for the ones this specification defines as fields. Anything
    // else has to have come from `additionalOperations`, which is where the
    // method name is written rather than implied
    //
    // The pointer is what this compares against rather than the key, as a key
    // writes the pointer out as a URI fragment and a method token may hold a
    // character that becomes a percent encoded triplet there. A token holds
    // neither a slash nor a tilde, so it needs no escaping of its own
    const auto &method{operation.at("method").to_string()};
    EXPECT_FALSE(method.empty());
    if (std::ranges::find(KNOWN_METHODS, method) == KNOWN_METHODS.cend()) {
      sourcemeta::core::JSON::String suffix{"/additionalOperations/"};
      suffix.append(method);
      EXPECT_TRUE(
          locations.at(origin).at("pointer").to_string().ends_with(suffix));
    }
    EXPECT_TRUE(std::ranges::find(KNOWN_OPERATION_TYPES,
                                  operation.at("type").to_string()) !=
                KNOWN_OPERATION_TYPES.cend());

    // An Operation Object only ever sits directly inside a Path Item Object
    const auto &container{locations.at(origin).at("parent")};
    EXPECT_TRUE(container.is_string() &&
                locations.defines(container.to_string()));
    if (container.is_string() && locations.defines(container.to_string())) {
      EXPECT_EQ(locations.at(container.to_string()).at("type").to_string(),
                "path-item");
    }

    // The position that exposes an operation is a Path Item too, and it is the
    // one that gives it a URL rather than the one that defines it
    const auto &endpoint{operation.at("endpoint").to_string()};
    EXPECT_TRUE(locations.defines(endpoint));
    EXPECT_EQ(locations.at(endpoint).at("type").to_string(), "path-item");

    for (const auto &server : operation.at("servers").as_array()) {
      EXPECT_TRUE(locations.defines(server.to_string()));
      EXPECT_EQ(locations.at(server.to_string()).at("type").to_string(),
                "server");
    }

    for (const auto &requirement : operation.at("security").as_array()) {
      EXPECT_TRUE(locations.defines(requirement.to_string()));
      EXPECT_EQ(locations.at(requirement.to_string()).at("type").to_string(),
                "security-requirement");
    }

    // A tag names a Tag Object the entry document declares, or nothing at all,
    // which the specification permits
    for (const auto &tag : operation.at("tags").as_array()) {
      if (tag.is_null()) {
        continue;
      }

      EXPECT_TRUE(locations.defines(tag.to_string()));
      EXPECT_EQ(locations.at(tag.to_string()).at("type").to_string(), "tag");
    }

    // A parameter position holds a Parameter Object, or a Reference Object
    // standing in for one
    for (const auto &parameter : operation.at("parameters").as_array()) {
      EXPECT_TRUE(locations.defines(parameter.to_string()));
      const auto type{
          locations.at(parameter.to_string()).at("type").to_string()};
      EXPECT_TRUE(type == "parameter" || type == "reference");
    }
  }
}

auto run_pass_test(const sourcemeta::core::JSON &test) -> void {
  check_known_keys(test);
  EXPECT_TRUE(test.defines("frame"));

  const auto default_base{make_default_base(test)};

  const sourcemeta::core::OpenAPIFrame frame{
      test.at("document"), sourcemeta::core::schema_walker,
      make_schema_resolver(test), default_base};
  // The invariants come first because a failed expectation aborts the test. A
  // frame that contradicts itself is a deeper failure than one that merely
  // differs from what a fixture recorded, so it is the one worth reporting
  const auto result{frame.to_json()};
  check_frame_invariants(result);
  EXPECT_EQ(result, test.at("frame"));
}

// What a Schema Object holds is JSON Schema's to make sense of, so what it
// turns down is reported in its own terms rather than reworded here, and it
// points within the schema rather than within the description
auto check_schema_refusal(const sourcemeta::core::JSON &test,
                          const std::string_view message) -> void {
  EXPECT_EQ(message, test.at("error").at("message").to_string());
  EXPECT_FALSE(test.at("error").defines("location"));
  EXPECT_FALSE(test.at("error").defines("base"));
}

auto run_fail_test(const sourcemeta::core::JSON &test) -> void {
  check_known_keys(test);
  EXPECT_TRUE(test.defines("error"));
  EXPECT_TRUE(test.at("error").defines("message"));
  for (const auto &entry : test.at("error").as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_ERROR_KEYS, entry.first) !=
                KNOWN_ERROR_KEYS.cend());
  }

  const auto default_base{make_default_base(test)};

  // Whether anything was turned down is settled outside the handlers, as the
  // way this runner reports a failed expectation is an exception of its own
  // and a handler broad enough to hold a Schema Object error would swallow it
  bool refused{false};
  try {
    [[maybe_unused]] const sourcemeta::core::OpenAPIFrame frame{
        test.at("document"), sourcemeta::core::schema_walker,
        make_schema_resolver(test), default_base};
  } catch (const sourcemeta::core::OpenAPIError &error) {
    refused = true;
    EXPECT_EQ(error.what(), test.at("error").at("message").to_string());

    // A document with more than one problem reports whichever check runs
    // first, and which one that is a fixture has to pin down
    EXPECT_TRUE(test.at("error").defines("location"));
    EXPECT_EQ(sourcemeta::core::to_string(error.location()),
              test.at("error").at("location").to_string());

    // An error says which document it is in, and a fixture that names none
    // expects the one that was framed
    const auto *base{test.at("error").try_at("base")};
    EXPECT_EQ(error.base(),
              base == nullptr ? make_default_base(test) : base->to_string());
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    refused = true;
    check_schema_refusal(test, error.what());

    // Which meta-schema could not be produced, so that turning one down is
    // never mistaken for turning down another
    EXPECT_TRUE(test.at("error").defines("identifier"));
    EXPECT_EQ(error.identifier(),
              test.at("error").at("identifier").to_string());
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    refused = true;
    check_schema_refusal(test, error.what());

    // Which keyword holds the problem and what it holds, neither of which the
    // message says on its own
    EXPECT_TRUE(test.at("error").defines("keyword"));
    EXPECT_EQ(error.keyword(), test.at("error").at("keyword").to_string());
    EXPECT_TRUE(test.at("error").defines("value"));
    EXPECT_EQ(error.value(), test.at("error").at("value").to_string());
  } catch (const std::exception &error) {
    // Every way a description can be turned down is named above, so anything
    // else is a refusal this runner has no account of rather than one to let
    // through. What it was is printed on the way out, as naming it here too
    // is the fix
    std::cerr << "Unaccounted refusal: " << error.what() << "\n";
    FAIL();
  }

  EXPECT_TRUE(refused);
}

auto register_tests(const std::filesystem::path &directory,
                    const bool expect_success) -> std::size_t {
  std::size_t count{0};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::recursive_directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".json") {
      continue;
    }

    const auto version{
        entry.path().parent_path().parent_path().filename().string()};
    const auto outcome{entry.path().parent_path().filename().string()};
    std::ostringstream name;
    for (const auto character : entry.path().stem().string()) {
      name << (character == '-' ? '_' : character);
    }

    std::string suite{"OpenAPIFrame_"};
    suite.append(version).append("_").append(outcome);

    const auto original{sourcemeta::core::read_json(entry.path())};
    const auto spelled_out{spelled_out_by(
        original, sourcemeta::core::URI::from_path(entry.path().parent_path())
                      .recompose())};
    const auto test{expand_all(original, entry.path().parent_path())};
    sourcemeta::core::test_register(
        suite, name.str(), __FILE__, __LINE__,
        [test, expect_success, spelled_out]() -> void {
          if (spelled_out.has_value()) {
            std::cerr << "Spells out the directory it sits in: "
                      << spelled_out.value() << "\n";
          }

          EXPECT_FALSE(spelled_out.has_value());
          if (expect_success) {
            run_pass_test(test);
          } else {
            run_fail_test(test);
          }
        });
    count += 1;
  }

  return count;
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{FRAME_SUITE_PATH};
  std::size_t passing{0};
  std::size_t failing{0};
  for (const std::filesystem::directory_entry &version :
       std::filesystem::directory_iterator{base}) {
    if (!version.is_directory()) {
      continue;
    }

    passing += register_tests(version.path() / "pass", true);
    failing += register_tests(version.path() / "fail", false);
  }

  // A fixture in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice
  if (passing == 0 || failing == 0) {
    std::cerr << "No frame fixtures found at " << FRAME_SUITE_PATH << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}
