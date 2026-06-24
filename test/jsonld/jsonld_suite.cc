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

struct JSONLDExpandCase {
  std::filesystem::path suite_root;
  sourcemeta::core::JSON::String base_prefix;
  std::filesystem::path input;
  std::filesystem::path expect;
  sourcemeta::core::JSON::String error_code;
  sourcemeta::core::JSON::String base_iri;
  sourcemeta::core::JSONLDVersion version;
  bool negative;
  std::optional<std::filesystem::path> expand_context;
};

class JSONLDExpandTest : public testing::Test {
public:
  explicit JSONLDExpandTest(JSONLDExpandCase test_case)
      : test_case_{std::move(test_case)} {}

  auto TestBody() -> void override {
    // TODO: Remove this guard once jsonld_expand is implemented. The harness
    // below already drives the full upstream expansion suite, so dropping the
    // skip will run every case for real
    if (this->pending_) {
      GTEST_SKIP() << "jsonld_expand is not yet implemented";
    }

    const auto &test_case{this->test_case_};
    const sourcemeta::core::JSONLDResolver resolver =
        [&test_case](const sourcemeta::core::JSON::StringView identifier)
        -> std::optional<sourcemeta::core::JSON> {
      if (!identifier.starts_with(test_case.base_prefix)) {
        return std::nullopt;
      }
      const auto path{test_case.suite_root /
                      identifier.substr(test_case.base_prefix.size())};
      if (!std::filesystem::exists(path)) {
        return std::nullopt;
      }
      return sourcemeta::core::read_json(path);
    };

    const auto input{sourcemeta::core::read_json(test_case.input)};

    if (test_case.negative) {
      try {
        if (test_case.expand_context.has_value()) {
          const auto context{
              sourcemeta::core::read_json(test_case.expand_context.value())};
          static_cast<void>(sourcemeta::core::jsonld_expand(
              input, context, test_case.base_iri, resolver, test_case.version));
        } else {
          static_cast<void>(sourcemeta::core::jsonld_expand(
              input, test_case.base_iri, resolver, test_case.version));
        }
        FAIL() << "Expected error code: " << test_case.error_code;
      } catch (const sourcemeta::core::JSONLDError &error) {
        EXPECT_EQ(test_case.error_code, error.code());
      }
    } else {
      const auto expected{sourcemeta::core::read_json(test_case.expect)};
      if (test_case.expand_context.has_value()) {
        const auto context{
            sourcemeta::core::read_json(test_case.expand_context.value())};
        EXPECT_EQ(sourcemeta::core::jsonld_expand(input, context,
                                                  test_case.base_iri, resolver,
                                                  test_case.version),
                  expected);
      } else {
        EXPECT_EQ(sourcemeta::core::jsonld_expand(input, test_case.base_iri,
                                                  resolver, test_case.version),
                  expected);
      }
    }
  }

private:
  JSONLDExpandCase test_case_;
  bool pending_{true};
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
  bool negative{false};
  for (const auto &type : entry.at("@type").as_array()) {
    if (type.to_string() == "jld:NegativeEvaluationTest") {
      negative = true;
    }
  }

  const auto &input_relative{entry.at("input").to_string()};

  JSONLDExpandCase test_case;
  test_case.suite_root = suite_root;
  test_case.base_prefix = base_prefix;
  test_case.input = suite_root / input_relative;
  test_case.base_iri = base_prefix + input_relative;
  test_case.version = sourcemeta::core::JSONLDVersion::V1_1;
  test_case.negative = negative;

  if (entry.defines("option")) {
    const auto &option{entry.at("option")};
    if (option.defines("base")) {
      test_case.base_iri = option.at("base").to_string();
    }
    if ((option.defines("specVersion") &&
         option.at("specVersion").to_string() == "json-ld-1.0") ||
        (option.defines("processingMode") &&
         option.at("processingMode").to_string() == "json-ld-1.0")) {
      test_case.version = sourcemeta::core::JSONLDVersion::V1_0;
    }
    if (option.defines("expandContext")) {
      test_case.expand_context =
          suite_root / option.at("expandContext").to_string();
    }
  }

  if (negative) {
    test_case.error_code = entry.at("expectErrorCode").to_string();
  } else {
    test_case.expect = suite_root / entry.at("expect").to_string();
  }

  testing::RegisterTest(
      "JSONLD_expand", sanitize(entry.at("@id").to_string()).c_str(), nullptr,
      nullptr, __FILE__, __LINE__, [test_case]() -> testing::Test * {
        return new JSONLDExpandTest(test_case);
      });
}

} // namespace

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);

  const std::filesystem::path suite_root{JSONLD_SUITE_PATH};
  const auto manifest{
      sourcemeta::core::read_json(suite_root / "expand-manifest.jsonld")};
  const auto &base_prefix{manifest.at("baseIri").to_string()};

  for (const auto &entry : manifest.at("sequence").as_array()) {
    register_case(entry, suite_root, base_prefix);
  }

  return RUN_ALL_TESTS();
}
