#include <gtest/gtest.h>

#include <cassert>
#include <filesystem>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include <sourcemeta/jsontoolkit/json.h>
#include <sourcemeta/jsontoolkit/jsonpointer.h>
#include <sourcemeta/jsontoolkit/jsonschema.h>
#include <sourcemeta/jsontoolkit/uri.h>

static auto
dereference(const std::string &uri,
            const std::map<std::string, std::pair<sourcemeta::jsontoolkit::JSON,
                                                  std::string>> &registry)
    -> std::optional<std::pair<sourcemeta::jsontoolkit::JSON, std::string>> {
  if (!registry.contains(uri)) {
    return std::nullopt;
  }

  return registry.at(uri);
}

class ReferencingTest : public testing::Test {
public:
  explicit ReferencingTest(sourcemeta::jsontoolkit::JSON test_suite,
                           sourcemeta::jsontoolkit::JSON test_document,
                           std::string default_dialect)
      // Ubuntu's ClangFormat gets a bit lost on this one
      // clang-format off
    : suite{std::move(test_suite)}, test{std::move(test_document)},
      dialect{std::move(default_dialect)} {}
  // clang-format on

  auto TestBody() -> void override {
    // (1) Pre-populate the registry
    EXPECT_TRUE(this->suite.defines("registry"));
    EXPECT_TRUE(this->suite.at("registry").is_object());
    for (const auto &[uri, schema] : this->suite.at("registry").as_object()) {
      assert(sourcemeta::jsontoolkit::is_schema(schema));
      this->registry.insert({uri, {schema, uri}});
    }

    // (2) Frame every schema and re-populate registry
    std::map<std::string, std::pair<sourcemeta::jsontoolkit::JSON, std::string>>
        new_entries;
    for (const auto &[uri, schema] : this->registry) {
      sourcemeta::jsontoolkit::ReferenceFrame frame;
      sourcemeta::jsontoolkit::ReferenceMap references;
      sourcemeta::jsontoolkit::frame(
          schema.first, frame, references,
          sourcemeta::jsontoolkit::default_schema_walker,
          sourcemeta::jsontoolkit::official_resolver, this->dialect, uri)
          .wait();
      for (const auto &[key, entry] : frame) {
        new_entries.insert(
            {key.second,
             {sourcemeta::jsontoolkit::get(schema.first, entry.pointer),
              entry.base}});
      }
    }

    // We don't insert into the main registry on the above loop,
    // otherwise we would affect the loop itself.
    for (auto pair : new_entries) {
      this->registry.insert(std::move(pair));
    }

    // (3) Run test
    this->assert_case(this->test, std::nullopt);
  }

  auto assert_case(const sourcemeta::jsontoolkit::JSON &current,
                   const std::optional<std::string> &default_base_uri) -> void {
    const bool is_error{current.defines("error") &&
                        current.at("error").is_boolean() &&
                        current.at("error").to_boolean()};

    const std::optional<std::string> base_uri{
        current.defines("base_uri") ? current.at("base_uri").to_string()
                                    : default_base_uri};

    assert(current.defines("ref"));
    assert(current.at("ref").is_string());
    const std::string ref_string{current.at("ref").to_string()};

    try {
      sourcemeta::jsontoolkit::URI ref_uri{ref_string};
      if (base_uri.has_value()) {
        ref_uri.resolve_from(base_uri.value());
      }

      ref_uri.canonicalize();
      const auto result{dereference(ref_uri.recompose(), this->registry)};

      if (is_error) {
        EXPECT_FALSE(result.has_value());
      } else {
        EXPECT_TRUE(current.defines("target"));
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value().first, current.at("target"));
        if (current.defines("then")) {
          assert_case(current.at("then"), result.value().second);
        }
      }
    } catch (const sourcemeta::jsontoolkit::URIParseError &) {
      EXPECT_TRUE(is_error);
    }
  }

private:
  const sourcemeta::jsontoolkit::JSON suite;
  const sourcemeta::jsontoolkit::JSON test;
  const std::string dialect;
  std::map<std::string, std::pair<sourcemeta::jsontoolkit::JSON, std::string>>
      registry;
};

static auto register_tests(const std::filesystem::path &subdirectory,
                           const std::string &suite_name,
                           const std::string &default_dialect) -> void {
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator(
           std::filesystem::path{REFERENCING_SUITE_PATH} / subdirectory)) {
    const std::string name{entry.path().stem().string()};
    const sourcemeta::jsontoolkit::JSON test =
        sourcemeta::jsontoolkit::from_file(entry.path());
    assert(test.is_object());
    assert(test.defines("tests"));
    assert(test.at("tests").is_array());
    for (unsigned int index = 0; index < test.at("tests").size(); index++) {
      std::ostringstream test_name;
      for (const auto character : name) {
        // Hyphens break `--gtest_filter`
        test_name << (character == '-' ? '_' : character);
      }

      test_name << '_' << index;
      testing::RegisterTest(
          suite_name.c_str(), test_name.str().c_str(), nullptr, nullptr,
          __FILE__, __LINE__, [=]() -> ReferencingTest * {
            return new ReferencingTest(test, test.at("tests").at(index),
                                       default_dialect);
          });
    }
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  // 2020-12
  register_tests("json-schema-draft-2020-12",
                 "JSONSchemaReferencingSuite_2020_12",
                 "https://json-schema.org/draft/2020-12/schema");

  // 2019-09
  register_tests("json-schema-draft-2019-09",
                 "JSONSchemaReferencingSuite_2019_09",
                 "https://json-schema.org/draft/2019-09/schema");

  // Draft7
  register_tests("json-schema-draft-07", "JSONSchemaReferencingSuite_Draft7",
                 "http://json-schema.org/draft-07/schema#");

  // Draft6
  register_tests("json-schema-draft-06", "JSONSchemaReferencingSuite_Draft6",
                 "http://json-schema.org/draft-06/schema#");

  // Draft4
  register_tests("json-schema-draft-04", "JSONSchemaReferencingSuite_Draft4",
                 "http://json-schema.org/draft-04/schema#");

  // Draft3
  register_tests("json-schema-draft-03", "JSONSchemaReferencingSuite_Draft3",
                 "http://json-schema.org/draft-03/schema#");

  return RUN_ALL_TESTS();
}
