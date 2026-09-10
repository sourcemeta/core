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

// These fixtures test the meta-schema, not the specification text. Where the
// two differ the text is normative and stricter, and so are we
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::set<std::string> KNOWN_DIVERGENCES{
    // A `path` parameter with a `content` and no `required`. Section 4.8.12
    // requires that field of every `path` parameter, while the meta-schema
    // only asks for it alongside a `schema`
    "style_defaults",
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
    "path_item_servers_parameters"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

auto run_pass_case(const sourcemeta::core::JSON &document) -> void {
  const sourcemeta::core::OpenAPIFrame frame{document, nullptr};
  EXPECT_EQ(frame.version(), sourcemeta::core::OpenAPIVersion::OPENAPI_3_1);
}

auto run_fail_case(const sourcemeta::core::JSON &document) -> void {
  try {
    [[maybe_unused]] const sourcemeta::core::OpenAPIFrame frame{document,
                                                                nullptr};
    FAIL();
  } catch (const sourcemeta::core::OpenAPIError &error) {
    // Which rule turns the document down is the business of the unit tests.
    // What matters here is only that it is turned down at all
    EXPECT_TRUE(error.what() != nullptr);
  }
}

auto register_tests(const std::filesystem::path &directory,
                    const bool expect_success) -> std::size_t {
  std::size_t count{0};
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

    if (KNOWN_DIVERGENCES.contains(name.str())) {
      continue;
    }

    const auto document{sourcemeta::core::read_yaml(entry.path())};
    sourcemeta::core::test_register("OpenAPISuite_" + suite, name.str(),
                                    __FILE__, __LINE__,
                                    [document, expect_success]() -> void {
                                      if (expect_success) {
                                        run_pass_case(document);
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
  const std::filesystem::path base{OPENAPI_SUITE_PATH};
  const auto passing{register_tests(base / "pass", true)};
  const auto failing{register_tests(base / "fail", false)};
  // A fixture in the wrong place, or with the wrong extension, would otherwise
  // never run and nobody would notice
  if (passing == 0 || failing == 0) {
    std::cerr << "No OpenAPI suite fixtures found at " << OPENAPI_SUITE_PATH
              << "\n";
    return 1;
  }

  return sourcemeta::core::test_run(argc, argv);
}
