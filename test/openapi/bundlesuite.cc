#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/openapi.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <algorithm>   // std::ranges::find
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <optional>    // std::nullopt
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
    "document",       "defaultBase",  "openapiResolver",
    "schemaResolver", "maxLocations", "namer",
    "result",         "inserted",     "named",
    "reads",          "schemaReads",  "standalone",
    "relocatable",    "operations",   "error"};

// Every key an entry of what bundling reported embedding may declare
const std::vector<std::string> KNOWN_INSERTION_KEYS{"from", "at"};

// And every key an entry of what the hook that picks names was handed may
// declare
const std::vector<std::string> KNOWN_NAMING_KEYS{"from", "container"};

// Every exception that bundling throws, named after the class itself so that a
// fixture states which one it means rather than leaving the runner to guess
// from the fields that happen to be there
const std::vector<std::string> KNOWN_ERROR_TYPES{"OpenAPIError",
                                                 "OpenAPIResolutionError",
                                                 "OpenAPIReferenceError",
                                                 "OpenAPIBundleLimitError",
                                                 "SchemaResolutionError",
                                                 "SchemaAnchorCollisionError",
                                                 "SchemaError"};

const std::vector<std::string> KNOWN_ERROR_KEYS{
    "type", "message", "location", "base", "identifier", "limit", "other"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

// The two resolvers answer for different halves of a description. This one
// hands back the other documents the shell of it is split across, each of
// which is an OpenAPI Description of its own. How many times it was asked for
// each of them is kept as bundling runs, as a document that answers several
// places of a description is one a caller pays to produce once
auto make_openapi_resolver(const sourcemeta::core::JSON &test,
                           sourcemeta::core::JSON &reads)
    -> sourcemeta::core::OpenAPIResolver {
  const auto &registry{test.at("openapiResolver")};
  return [registry, &reads](const std::string_view identifier)
             -> sourcemeta::core::OpenAPIResolverResult {
    const sourcemeta::core::JSON::String key{identifier};
    const auto *seen{reads.try_at(key)};
    const sourcemeta::core::JSON::Integer count{
        seen == nullptr ? 0 : seen->to_integer()};
    reads.assign(key, sourcemeta::core::JSON{count + 1});

    const auto *match{registry.try_at(key)};
    if (match != nullptr) {
      return *match;
    }

    return std::nullopt;
  };
}

// And this one answers for what sits inside a Schema Object, which is the
// dialects it is written against and the schemas it reaches for. A description
// may be written against a dialect that nobody publishes, which a fixture says
// where to find by writing it down
auto make_schema_resolver(const sourcemeta::core::JSON &test,
                          sourcemeta::core::JSON &reads)
    -> sourcemeta::core::SchemaResolver {
  const auto &registry{test.at("schemaResolver")};
  return [registry, &reads](const std::string_view identifier)
             -> sourcemeta::core::SchemaResolverResult {
    const sourcemeta::core::JSON::String key{identifier};
    const auto *match{registry.try_at(key)};
    // Only what a fixture writes down is counted. The dialects that come with
    // this library answer for themselves and say nothing about the
    // description, so counting those would say how bundling reads a
    // meta-schema rather than how it reads the schemas of a description
    if (match == nullptr) {
      return sourcemeta::core::schema_resolver(identifier);
    }

    const auto *seen{reads.try_at(key)};
    const sourcemeta::core::JSON::Integer count{
        seen == nullptr ? 0 : seen->to_integer()};
    reads.assign(key, sourcemeta::core::JSON{count + 1});
    return *match;
  };
}

// How many locations a fixture is talking about, which it has to say as a
// whole count for the runner to read it as one. Anything else reads back as a
// number so large that it stands for no bound at all, which would leave a
// fixture written to pin down what a bound does passing without one
auto make_location_count(const sourcemeta::core::JSON &value) -> std::uint64_t {
  EXPECT_TRUE(value.is_integer());
  EXPECT_GE(value.to_integer(), 0);
  return static_cast<std::uint64_t>(value.to_integer());
}

// What bundling is allowed to analyse, which a fixture only writes down when
// it is there to say what running out of it comes to
auto make_options(const sourcemeta::core::JSON &test,
                  const sourcemeta::core::JSON::String &base)
    -> sourcemeta::core::OpenAPIBundleOptions {
  sourcemeta::core::OpenAPIBundleOptions options{.default_base = base};
  const auto *limit{test.try_at("maxLocations")};
  if (limit != nullptr) {
    options.max_locations = make_location_count(*limit);
  }

  return options;
}

// What a fixture wants an embedded Object to be called, which stands in for
// the hook a caller reaches for when the name bundling would pick itself is
// not the one they want. What that hook is handed is written down as it runs,
// as a caller that decides by where the Object is headed only can when it is
// told where that is
auto install_namer(const sourcemeta::core::JSON &test,
                   sourcemeta::core::OpenAPIBundleOptions &options,
                   sourcemeta::core::JSON &named) -> void {
  const auto *chosen{test.try_at("namer")};
  if (chosen == nullptr) {
    return;
  }

  options.namer = [name = chosen->to_string(),
                   &named](const sourcemeta::core::JSON::StringView source,
                           const sourcemeta::core::JSON::StringView container)
      -> sourcemeta::core::JSON::String {
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("from", sourcemeta::core::JSON{source});
    entry.assign("container", sourcemeta::core::JSON{container});
    named.push_back(std::move(entry));
    return name;
  };
}

// Bundling keeps a view into the base for as long as it runs, so the caller
// owns it
auto make_default_base(const sourcemeta::core::JSON &test)
    -> sourcemeta::core::JSON::String {
  const auto *base{test.try_at("defaultBase")};
  return base == nullptr ? sourcemeta::core::JSON::String{} : base->to_string();
}

// Each revision has a corpus of its own, and a fixture filed under one holds
// that revision throughout, whether or not a reference reaches every document
// it registers. What a description made of documents of differing revisions
// comes to is what the corpus beside them is for, so those are left to say it
// for themselves. A document that declares no revision this module recognises,
// or none at all, is left to whichever fixture is there to account for it
auto check_revisions_agree(const sourcemeta::core::JSON &test,
                           const std::string &corpus) -> void {
  const auto version{sourcemeta::core::openapi_version(test.at("document"))};
  if (!version.has_value() || corpus == "mixed") {
    return;
  }

  for (const auto &entry : test.at("openapiResolver").as_object()) {
    const auto other{sourcemeta::core::openapi_version(entry.second)};
    if (other.has_value()) {
      EXPECT_EQ(other.value(), version.value());
    }
  }
}

// Every key a fixture declares has to be one the runner reads, a fixture says
// either what bundling produces or how it refuses and never both, and an error
// carries the fields that the class it names actually has
auto check_shape(const sourcemeta::core::JSON &test, const std::string &corpus)
    -> void {
  for (const auto &entry : test.as_object()) {
    EXPECT_TRUE(std::ranges::find(KNOWN_KEYS, entry.first) !=
                KNOWN_KEYS.cend());
  }

  EXPECT_TRUE(test.defines("document"));
  EXPECT_TRUE(test.defines("openapiResolver"));
  EXPECT_TRUE(test.defines("schemaResolver"));
  EXPECT_TRUE(test.defines("result") != test.defines("error"));
  // What bundling embedded is only something a fixture that gets an answer
  // out of it can record, and so is how many times each document had to be
  // produced along the way
  EXPECT_EQ(test.defines("inserted"), test.defines("result"));
  EXPECT_EQ(test.defines("reads"), test.defines("result"));
  EXPECT_EQ(test.defines("schemaReads"), test.defines("result"));
  EXPECT_EQ(test.defines("standalone"), test.defines("result"));
  EXPECT_EQ(test.defines("relocatable"), test.defines("result"));
  // A fixture that picks the names itself says which ones the hook was handed.
  // Picking them is something a fixture does to get a particular answer out of
  // bundling, so a refusal has no use for it
  EXPECT_EQ(test.defines("named"), test.defines("namer"));
  if (test.defines("namer")) {
    EXPECT_TRUE(test.defines("result"));
    for (const auto &entry : test.at("named").as_array()) {
      for (const auto &field : entry.as_object()) {
        EXPECT_TRUE(std::ranges::find(KNOWN_NAMING_KEYS, field.first) !=
                    KNOWN_NAMING_KEYS.cend());
      }

      EXPECT_TRUE(entry.defines("from"));
      EXPECT_TRUE(entry.defines("container"));
    }
  }

  if (test.defines("inserted")) {
    for (const auto &entry : test.at("inserted").as_array()) {
      for (const auto &field : entry.as_object()) {
        EXPECT_TRUE(std::ranges::find(KNOWN_INSERTION_KEYS, field.first) !=
                    KNOWN_INSERTION_KEYS.cend());
      }

      EXPECT_TRUE(entry.defines("from"));
      EXPECT_TRUE(entry.defines("at"));
    }
  }

  if (test.defines("error")) {
    const auto &error{test.at("error")};
    for (const auto &entry : error.as_object()) {
      EXPECT_TRUE(std::ranges::find(KNOWN_ERROR_KEYS, entry.first) !=
                  KNOWN_ERROR_KEYS.cend());
    }

    const auto &type{error.at("type").to_string()};
    EXPECT_TRUE(std::ranges::find(KNOWN_ERROR_TYPES, type) !=
                KNOWN_ERROR_TYPES.cend());
    EXPECT_TRUE(error.defines("message"));
    // Only a refusal that names a place in the description carries one.
    // Running out of an allowance is a property of the whole description, what
    // a Schema Object reaches for is a place a JSON Schema implementation
    // names by its identifier rather than by where it sits, and two schemas
    // answering to one identifier name no single place either
    EXPECT_EQ(error.defines("location"), type != "OpenAPIBundleLimitError" &&
                                             type != "SchemaResolutionError" &&
                                             type != "SchemaError");
    EXPECT_EQ(error.defines("identifier"),
              type == "OpenAPIResolutionError" ||
                  type == "OpenAPIReferenceError" ||
                  type == "SchemaResolutionError" ||
                  type == "SchemaAnchorCollisionError");
    EXPECT_EQ(error.defines("limit"), type == "OpenAPIBundleLimitError");
    // Both of the places that claim the anchor, as either one alone says
    // nothing about the collision
    EXPECT_EQ(error.defines("other"), type == "SchemaAnchorCollisionError");
  }

  check_revisions_agree(test, corpus);
}

auto run_pass_test(const sourcemeta::core::JSON &test) -> void {
  auto reads{sourcemeta::core::JSON::make_object()};
  auto schema_reads{sourcemeta::core::JSON::make_object()};
  const auto resolver{make_openapi_resolver(test, reads)};
  const auto schema_resolver{make_schema_resolver(test, schema_reads)};
  const auto base{make_default_base(test)};
  auto options{make_options(test, base)};
  const auto &expected{test.at("result")};

  // Bundling renames what it embeds, so which place became which component is
  // only something it says rather than something the result shows
  auto inserted{sourcemeta::core::JSON::make_array()};
  options.callback =
      [&inserted](const sourcemeta::core::JSON::StringView source,
                  const sourcemeta::core::Pointer &destination) -> void {
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("from", sourcemeta::core::JSON{source});
    entry.assign(
        "at", sourcemeta::core::JSON{sourcemeta::core::to_string(destination)});
    inserted.push_back(std::move(entry));
  };

  auto named{sourcemeta::core::JSON::make_array()};
  install_namer(test, options, named);

  const auto result{sourcemeta::core::openapi_bundle(
      test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
      resolver, options)};
  EXPECT_EQ(result, expected);
  EXPECT_EQ(inserted, test.at("inserted"));

  // Every run below asks for documents and for names all over again, so what
  // one run of bundling came to is read back before any of them disturbs it
  EXPECT_EQ(reads, test.at("reads"));
  EXPECT_EQ(schema_reads, test.at("schemaReads"));
  const auto *pairs{test.try_at("named")};
  if (pairs != nullptr) {
    EXPECT_EQ(named, *pairs);
  }

  // A caller picks between the overload that mutates and the one that returns
  // for reasons that have nothing to do with what bundling produces
  auto mutated{test.at("document")};
  auto without{make_options(test, base)};
  install_namer(test, without, named);
  sourcemeta::core::openapi_bundle(mutated, sourcemeta::core::schema_walker,
                                   schema_resolver, resolver, without);
  EXPECT_EQ(mutated, expected);

  // Bundling is what makes a description whole, so what it produces holds
  // every place it went and fetched, and framing reads it back rather than
  // turning it down.
  //
  // What it does not make whole is a reference that names the document making
  // it and lands nowhere, which there is nothing to go and fetch for. Section
  // 4.8.23 holds a `$ref` to the form of a URI and says nothing about it
  // having to resolve, so such a reference is left exactly as it was written
  // and what comes back stands alone no more than what went in did
  const sourcemeta::core::OpenAPIFrame frame{
      result, sourcemeta::core::schema_walker, schema_resolver, base};
  EXPECT_EQ(frame.standalone(), test.at("standalone").to_boolean());

  // What a description exposes is what it is for, and everything else here
  // asks after the shape of the result rather than after that. Two Objects
  // that bundling moves can leave every reference resolving and still leave an
  // operation with other parameters in force, other servers to reach it at,
  // another credential to present, or a tag it never carried. Section 4.1.2.3
  // is what makes the last of those reachable without the reference touching
  // the operation at all, as a name there resolves from the entry document and
  // bundling is what fills it
  if (test.defines("operations")) {
    EXPECT_EQ(frame.to_json().at("operations"), test.at("operations"));
  }

  // And one that goes on holding it wherever it is kept. Section 4.6 settles a
  // relative reference against a base that "is usually the retrieval URI of
  // the document", so an output that names every place it holds by a fragment
  // of itself answers for itself from any address.
  //
  // A reference out of a Schema Object that declares an identifier of its own
  // is what a fixture writes this down as false for. Section 4.6 has such a
  // reference count from "the nearest parent `$id`" rather than from the
  // document, so a fragment there would name a place within that identifier
  // and the document has to be named in full instead
  // Where it is kept instead is an address that shares nothing with the one it
  // came from. A neighbour of the original would keep the host and the
  // directory alike, and one of the same name would keep even a bare filename
  // landing, so this differs in every one of the three. A reference that leans
  // on any of them is the one thing this is here to catch. RFC 2606 Section 2
  // reserves a name for standing in like this: "test" is "recommended for use
  // in testing of current or new DNS related code"
  const sourcemeta::core::JSON::String elsewhere{
      "https://relocated.test/moved/elsewhere.json"};

  const sourcemeta::core::OpenAPIFrame moved{
      result, sourcemeta::core::schema_walker, schema_resolver, elsewhere};
  EXPECT_EQ(moved.standalone(), test.at("relocatable").to_boolean());

  // Bundling that keeps finding work to do on its own output never settles
  EXPECT_EQ(
      sourcemeta::core::openapi_bundle(result, sourcemeta::core::schema_walker,
                                       schema_resolver, resolver, without),
      result);
}

// Where a refusal says it is, and which document it is in, which every way of
// turning a description down but running out of an allowance reports
auto check_where(const sourcemeta::core::JSON &expected,
                 const sourcemeta::core::Pointer &location,
                 const sourcemeta::core::JSON::StringView base,
                 const sourcemeta::core::JSON::String &bundled) -> void {
  // A document with more than one problem reports whichever check runs first,
  // and which one that is a fixture has to pin down
  EXPECT_EQ(sourcemeta::core::to_string(location),
            expected.at("location").to_string());

  // A fixture that names no document expects the one that was bundled
  const auto *declared{expected.try_at("base")};
  EXPECT_EQ(base, declared == nullptr ? bundled : declared->to_string());
}

auto run_fail_test(const sourcemeta::core::JSON &test) -> void {
  // How many times each document was asked for is not something a run that
  // was turned down partway through settles, so nothing reads this back
  auto reads{sourcemeta::core::JSON::make_object()};
  const auto resolver{make_openapi_resolver(test, reads)};
  auto schema_reads{sourcemeta::core::JSON::make_object()};
  const auto schema_resolver{make_schema_resolver(test, schema_reads)};
  const auto base{make_default_base(test)};
  const auto options{make_options(test, base)};
  const auto &expected{test.at("error")};
  const auto &type{expected.at("type").to_string()};

  if (type == "OpenAPIResolutionError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIResolutionError &error) {
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      // Which document nothing could produce, so that a caller is told what to
      // go and find rather than left to work it out from a pointer
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      check_where(expected, error.location(), error.base(), base);
    }
  } else if (type == "OpenAPIReferenceError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIReferenceError &error) {
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      // What the reference names, resolved and canonicalised, which is not
      // what the document spells at the place the pointer leads to
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      check_where(expected, error.location(), error.base(), base);
    }
  } else if (type == "SchemaResolutionError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::SchemaResolutionError &error) {
      // What a Schema Object reaches for that neither the shell nor a JSON
      // Schema implementation can produce, which only the identifier says
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
    }
  } else if (type == "SchemaError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::SchemaError &error) {
      // Two schemas answering to one identifier, which carries nothing beyond
      // what it says, as neither copy is more the owner of the name than the
      // other
      EXPECT_EQ(error.what(), expected.at("message").to_string());
    }
  } else if (type == "SchemaAnchorCollisionError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::SchemaAnchorCollisionError &error) {
      // What bundling cannot rename its way out of, as the name of an anchor
      // is what a reference to it goes by
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      EXPECT_EQ(error.identifier(), expected.at("identifier").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.location()),
                expected.at("location").to_string());
      EXPECT_EQ(sourcemeta::core::to_string(error.other()),
                expected.at("other").to_string());
    }
  } else if (type == "OpenAPIBundleLimitError") {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIBundleLimitError &error) {
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      // What the caller allowed, rather than whatever was left of it wherever
      // bundling happened to run out
      EXPECT_EQ(error.limit(), make_location_count(expected.at("limit")));
    }
  } else {
    try {
      [[maybe_unused]] const auto result{sourcemeta::core::openapi_bundle(
          test.at("document"), sourcemeta::core::schema_walker, schema_resolver,
          resolver, options)};
      FAIL();
    } catch (const sourcemeta::core::OpenAPIError &error) {
      EXPECT_EQ(error.what(), expected.at("message").to_string());
      check_where(expected, error.location(), error.base(), base);
    }
  }
}

auto register_tests(const std::filesystem::path &directory) -> std::size_t {
  std::size_t count{0};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::recursive_directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".json") {
      continue;
    }

    const auto version{entry.path().parent_path().filename().string()};
    std::ostringstream name;
    for (const auto character : entry.path().stem().string()) {
      name << (character == '-' ? '_' : character);
    }

    std::string suite{"OpenAPIBundle_"};
    suite.append(version);

    const auto test{sourcemeta::core::read_json(entry.path())};
    sourcemeta::core::test_register(suite, name.str(), __FILE__, __LINE__,
                                    [test, version]() -> void {
                                      check_shape(test, version);
                                      if (test.defines("error")) {
                                        run_fail_test(test);
                                      } else {
                                        run_pass_test(test);
                                      }
                                    });
    count += 1;
  }

  return count;
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{BUNDLE_SUITE_PATH};
  // A fixture in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice
  if (register_tests(base) == 0) {
    std::cerr << "No bundle fixtures found at " << BUNDLE_SUITE_PATH << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}
