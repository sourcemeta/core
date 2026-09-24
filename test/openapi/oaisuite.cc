#include <sourcemeta/core/json.h>
#include <sourcemeta/core/openapi.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <iostream>   // std::cerr
#include <set>        // std::set
#include <sstream>    // std::ostringstream
#include <string>     // std::string

namespace {

// These fixtures test the meta-schema, not the specification text. Section 4
// settles which answers for the format: "This text is the only normative
// description of the format. A JSON Schema is hosted on spec.openapis.org for
// informational purposes. If the JSON Schema differs from this section, then
// this section MUST be considered authoritative". So wherever the two differ
// we follow the text, and every fixture that costs us is named below with the
// reason it costs us.
//
// What the text asks and no keyword can express, so the meta-schema takes the
// document and we turn it down
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::set<std::string> KNOWN_DIVERGENCES{
    // A `path` parameter with a `content` and no `required`. Section 4.8.12
    // requires that field of every `path` parameter, while the meta-schema
    // only asks for it alongside a `schema`
    "3_1:style_defaults",
    // `/pets/{id}` under a parameter named `petId`. Section 3.5 has each
    // template expression "correspond to a path parameter", and Section
    // 4.8.12 has such a parameter correspond to a template expression back
    // the other way, so this fails both ways round. Neither requirement can
    // be written as a keyword, as it means comparing a property name against
    // a value nested under it, which is why the meta-schema takes it. It is
    // one of the specification's own published examples
    "operation_object_example",
    // `/user/{username}` under a parameter named `usernames`, which fails the
    // same two requirements the same two ways, and is likewise published as
    // an example
    "parameter_object_examples",
    // A Link Object naming `getThing`, which no operation in the document
    // declares. Section 4.8.20 has such a name "MUST be resolved within the
    // scope of the OpenAPI Description", and no keyword can compare a value
    // against the operation identifiers scattered through a document, so the
    // meta-schema takes it
    "path_item_servers_parameters",
    // A Media Type Object carrying a `description`, which the Media Type
    // Object field table of Section 4.14 does not define in either 3.2.0 or
    // 3.2.1, while both published schemas of this revision admit it and this
    // very fixture relies on it. The text is what we follow, so we turn the
    // field down. Unlike the others here the meta-schema is not laxer by
    // oversight but ahead of the text, so this entry is the one to revisit
    // when a revision that defines the field goes out
    "3_2:media_type_examples"};

// What the meta-schema asks and the text does not, which is the other way
// round from the set above. Section 4 leaves the text authoritative either
// way, so a rule that only the schema carries is one we do not enforce
const std::set<std::string> META_SCHEMA_ONLY{
    // A header parameter named `Bad[Header]`, and a `headers` map keyed
    // `Bad=Header`. The published schema holds both to
    // `^[a-zA-Z0-9!#$%&'*+.^_`|~-]+$`,
    // which is the `token` production of RFC 9110 Section 5.6.2, while
    // neither the Parameter Object `name` row of Section 4.12 nor the Header
    // Object of Section 4.21 says anything of the form such a name may take
    "3_2:parameter_object_header_name", "3_2:header_object_name"};

// And what the text asks of a Schema Object rather than of the shell around
// it. This module locates Schema Objects and hands them off, so a rule about
// what one holds is for whatever reads inside one to enforce
const std::set<std::string> SCHEMA_OBJECT_INTERIOR{
    // Section 4.26 says of the XML Object `attribute` and of `wrapped` alike:
    // "If `nodeType` is present, this field MUST NOT be present". Both are
    // keywords of the dialect this specification publishes, which is where
    // that rule is written down and where it is checked
    "3_2:xml_attr_exclusion", "3_2:xml_wrapped_exclusion"};

// A description names the dialect its Schema Objects are written against, and
// Section 4.8.1.1 asks only that the name "be in the form of a URI". These
// name one that was never published, as an OpenAPI schema spells its own
// identifier with a WORK-IN-PROGRESS placeholder until the day it goes out
// under a date. Such a description is valid and framing the shell of it holds
// up, yet the schemas within it are written in a dialect nobody can produce,
// which is the one thing this runner cannot ask for
const std::set<std::string> UNPUBLISHED_DIALECTS{"json_schema_dialect"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

auto run_pass_case(const sourcemeta::core::JSON &document,
                   const sourcemeta::core::OpenAPIVersion version) -> void {
  const sourcemeta::core::OpenAPIFrame frame{document,
                                             sourcemeta::core::schema_walker,
                                             sourcemeta::core::schema_resolver};
  EXPECT_EQ(frame.version(), version);
}

auto run_fail_case(const sourcemeta::core::JSON &document) -> void {
  try {
    [[maybe_unused]] const sourcemeta::core::OpenAPIFrame frame{
        document, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver};
    FAIL();
  } catch (const sourcemeta::core::OpenAPIError &error) {
    // Which rule turns the document down is the business of the unit tests.
    // What matters here is only that it is turned down at all
    EXPECT_TRUE(error.what() != nullptr);
  }
}

// What the two corpora share is a set of names rather than a set of documents,
// so a fixture of one name may hold what one revision publishes and what the
// other does without the two saying the same thing. A name written above with
// a revision in front of it stands for the case of that revision alone, and a
// bare one stands for the case wherever it appears
auto excluded(const std::set<std::string> &names, const std::string &revision,
              const std::string &name) -> bool {
  return names.contains(name) || names.contains(revision + ":" + name);
}

// Each revision publishes a corpus of its own, and the two share most of
// their fixture names, so which revision a case belongs to is part of what it
// is called
auto register_tests(const std::filesystem::path &directory,
                    const std::string &revision,
                    const sourcemeta::core::OpenAPIVersion version,
                    const bool expect_success) -> std::size_t {
  std::size_t count{0};
  // A corpus that is not there at all, which is what a vendored dependency
  // nobody pulled comes to, would otherwise take the whole binary down before
  // the count below has a chance to say which one is missing
  if (!std::filesystem::exists(directory)) {
    return 0;
  }

  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator{directory}) {
    if (!entry.is_regular_file() || entry.path().extension() != ".yaml") {
      continue;
    }

    const auto suite{entry.path().parent_path().filename().string()};
    std::ostringstream name;
    for (const auto character : entry.path().stem().string()) {
      name << (character == '-' ? '_' : character);
    }

    if (excluded(KNOWN_DIVERGENCES, revision, name.str()) ||
        excluded(UNPUBLISHED_DIALECTS, revision, name.str()) ||
        excluded(META_SCHEMA_ONLY, revision, name.str()) ||
        excluded(SCHEMA_OBJECT_INTERIOR, revision, name.str())) {
      continue;
    }

    std::string group{"OpenAPISuite_"};
    group.append(revision).append("_").append(suite);

    const auto document{sourcemeta::core::read_yaml(entry.path())};
    sourcemeta::core::test_register(
        group, name.str(), __FILE__, __LINE__,
        [document, expect_success, version]() -> void {
          if (expect_success) {
            run_pass_case(document, version);
          } else {
            run_fail_case(document);
          }
        });
    count += 1;
  }

  return count;
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path first{OPENAPI_SUITE_PATH_3_1};
  const std::filesystem::path second{OPENAPI_SUITE_PATH_3_2};
  const auto registered{
      register_tests(first / "pass", "3_1",
                     sourcemeta::core::OpenAPIVersion::OPENAPI_3_1, true) *
      register_tests(first / "fail", "3_1",
                     sourcemeta::core::OpenAPIVersion::OPENAPI_3_1, false) *
      register_tests(second / "pass", "3_2",
                     sourcemeta::core::OpenAPIVersion::OPENAPI_3_2, true) *
      register_tests(second / "fail", "3_2",
                     sourcemeta::core::OpenAPIVersion::OPENAPI_3_2, false)};
  // A corpus in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice. Every one of the four has to have found
  // something, which a product of their counts is zero unless they all did
  if (registered == 0) {
    std::cerr << "No OpenAPI suite fixtures found at " << OPENAPI_SUITE_PATH_3_1
              << " and " << OPENAPI_SUITE_PATH_3_2 << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}
