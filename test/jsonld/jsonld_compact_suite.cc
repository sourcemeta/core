#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/text.h>

#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

struct JSONLDCompactCase {
  std::filesystem::path suite_root;
  sourcemeta::core::JSON::String base_prefix;
  std::filesystem::path input;
  std::filesystem::path context;
  std::filesystem::path expect;
  sourcemeta::core::JSON::String error_code;
  sourcemeta::core::JSON::String base_iri;
  sourcemeta::core::JSONLDVersion version;
  bool negative;
  bool compact_arrays;
};

class JSONLDCompactTest : public testing::Test {
public:
  explicit JSONLDCompactTest(JSONLDCompactCase test_case)
      : test_case_{std::move(test_case)} {}

  auto TestBody() -> void override {
    const auto &test_case{this->test_case_};
    const sourcemeta::core::JSONLDResolver resolver =
        [&test_case](const sourcemeta::core::JSON::StringView identifier)
        -> std::optional<sourcemeta::core::JSON> {
      if (!identifier.starts_with(test_case.base_prefix)) {
        return std::nullopt;
      }
      const auto suffix{identifier.substr(test_case.base_prefix.size())};
      const auto path{test_case.suite_root /
                      suffix.substr(0, suffix.find_first_of("#?"))};
      if (!std::filesystem::exists(path)) {
        return std::nullopt;
      }
      return sourcemeta::core::read_json(path);
    };

    const auto input{sourcemeta::core::read_json(test_case.input)};
    const auto context{sourcemeta::core::read_json(test_case.context)};

    if (test_case.negative) {
      try {
        const auto expanded{sourcemeta::core::jsonld_expand(
            input, test_case.base_iri, resolver, test_case.version)};
        [[maybe_unused]] const auto result{sourcemeta::core::jsonld_compact(
            expanded, context, test_case.base_iri, resolver, test_case.version,
            test_case.compact_arrays)};
        FAIL() << "Expected error code: " << test_case.error_code;
      } catch (const sourcemeta::core::JSONLDError &error) {
        std::string actual_code{error.what()};
        sourcemeta::core::to_lowercase(actual_code);
        std::string expected_code{test_case.error_code};
        sourcemeta::core::to_lowercase(expected_code);
        EXPECT_EQ(expected_code, actual_code);
      }
    } else {
      const auto expanded{sourcemeta::core::jsonld_expand(
          input, test_case.base_iri, resolver, test_case.version)};
      const auto expected{sourcemeta::core::read_json(test_case.expect)};
      EXPECT_EQ(sourcemeta::core::jsonld_compact(
                    expanded, context, test_case.base_iri, resolver,
                    test_case.version, test_case.compact_arrays),
                expected);
    }
  }

private:
  JSONLDCompactCase test_case_;
};

auto sanitize(const std::string_view identifier) -> std::string {
  std::string result;
  for (const auto character : identifier) {
    if (character == '#') {
      continue;
    }
    result += sourcemeta::core::is_alphanum(character) ? character : '_';
  }
  return result;
}

auto register_case(const sourcemeta::core::JSON &entry,
                   const std::filesystem::path &suite_root,
                   const sourcemeta::core::JSON::String &base_prefix) -> void {
  // A document declared as JSON-LD 1.0 exercises compaction behaviour that
  // changed for a 1.1 processor (notably that an expanded term definition is no
  // longer usable as a compact IRI prefix), per the JSON-LD 1.0 Errata. The
  // reference implementations do not run these tests against a 1.1 processor,
  // so neither do we:
  //
  //   jsonld.js: TEST_TYPES['jld:CompactTest'].skip.specVersion =
  //   ['json-ld-1.0']
  //   https://github.com/digitalbazaar/jsonld.js/blob/main/tests/test.js
  //
  //   PyLD: TEST_TYPES['jld:CompactTest']['skip']['specVersion'] =
  //   ['json-ld-1.0']
  //   https://github.com/digitalbazaar/pyld/blob/master/tests/runtests.py
  if (entry.defines("option") && entry.at("option").defines("specVersion") &&
      entry.at("option").at("specVersion").to_string() == "json-ld-1.0") {
    return;
  }

  // jsonld_compact requires input in valid expanded document form. A few suite
  // inputs expand into documents whose property IRIs are not valid IRIs (which
  // expansion does not validate), so they cannot serve as compaction input
  // without violating that precondition. tp004 uses property IRIs ending in "["
  // and "]", which are not valid IRIs.
  if (entry.at("@id").to_string() == "#tp004") {
    return;
  }

  bool negative{false};
  for (const auto &type : entry.at("@type").as_array()) {
    if (type.to_string() == "jld:NegativeEvaluationTest") {
      negative = true;
    }
  }

  const auto &input_relative{entry.at("input").to_string()};

  JSONLDCompactCase test_case;
  test_case.suite_root = suite_root;
  test_case.base_prefix = base_prefix;
  test_case.input = suite_root / input_relative;
  test_case.context = suite_root / entry.at("context").to_string();
  test_case.base_iri = base_prefix + input_relative;
  test_case.version = sourcemeta::core::JSONLDVersion::V1_1;
  test_case.negative = negative;
  test_case.compact_arrays = true;

  if (entry.defines("option")) {
    const auto &option{entry.at("option")};
    if (option.defines("base")) {
      test_case.base_iri = option.at("base").to_string();
    }
    if (option.defines("processingMode") &&
        option.at("processingMode").to_string() == "json-ld-1.0") {
      test_case.version = sourcemeta::core::JSONLDVersion::V1_0;
    }
    if (option.defines("compactArrays")) {
      test_case.compact_arrays = option.at("compactArrays").to_boolean();
    }
  }

  if (negative) {
    test_case.error_code = entry.at("expectErrorCode").to_string();
  } else {
    test_case.expect = suite_root / entry.at("expect").to_string();
  }

  testing::RegisterTest(
      "JSONLD_compact", sanitize(entry.at("@id").to_string()).c_str(), nullptr,
      nullptr, __FILE__, __LINE__, [test_case]() -> testing::Test * {
        return new JSONLDCompactTest(test_case);
      });
}

} // namespace

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);

  const std::filesystem::path suite_root{JSONLD_SUITE_PATH};
  const auto manifest{
      sourcemeta::core::read_json(suite_root / "compact-manifest.jsonld")};
  const auto &base_prefix{manifest.at("baseIri").to_string()};

  for (const auto &entry : manifest.at("sequence").as_array()) {
    register_case(entry, suite_root, base_prefix);
  }

  return RUN_ALL_TESTS();
}
