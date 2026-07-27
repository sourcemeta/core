#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

// Turn a W3C manifest identifier into a valid test name.
auto suite_test_name(const std::string_view identifier) -> std::string {
  std::string result;
  for (const auto character : identifier) {
    if (character == '#') {
      continue;
    }
    result += sourcemeta::core::is_alphanum(character) ? character : '_';
  }
  return result;
}

// A resolver that loads vendored suite documents living under the base prefix,
// ignoring any fragment or query when dereferencing.
auto suite_resolver(std::filesystem::path suite_root,
                    sourcemeta::core::JSON::String base_prefix)
    -> sourcemeta::core::JSONLDResolver {
  return [suite_root = std::move(suite_root),
          base_prefix = std::move(base_prefix)](
             const sourcemeta::core::JSON::StringView identifier)
             -> std::optional<sourcemeta::core::JSON> {
    if (!identifier.starts_with(base_prefix)) {
      return std::nullopt;
    }
    const auto suffix{identifier.substr(base_prefix.size())};
    const auto path{suite_root / suffix.substr(0, suffix.find_first_of("#?"))};
    if (!std::filesystem::exists(path)) {
      return std::nullopt;
    }
    return sourcemeta::core::read_json(path);
  };
}

struct JSONLDExpandCase {
  std::filesystem::path suite_root;
  sourcemeta::core::JSON::String base_prefix;
  std::filesystem::path input;
  std::filesystem::path expect;
  sourcemeta::core::JSON::String error_code;
  sourcemeta::core::JSON::String base_iri;
  sourcemeta::core::JSONLDVersion version;
  bool negative;
  bool valid_expanded_output;
  std::optional<std::filesystem::path> expand_context;
};

// A few expand fixtures are correct expansion output yet not valid expanded
// documents, because expansion builds IRIs by concatenation without validation
// (JSON-LD 1.1 API Section 5.2). Their output still matches the fixture, but
// the validator must reject it rather than accept it as expanded form.
auto produces_valid_expanded_output(const std::string_view identifier) -> bool {
  // A property IRI with a second "#" inside the fragment, which is not a valid
  // IRI
  return identifier != "#t0111" &&
         // The same pathological property IRI with a relative vocabulary base
         identifier != "#t0112" &&
         // A keyword-form IRI is ignored, leaving a null @id (the manifest
         // entry is non-normative and documents this)
         identifier != "#t0122";
}

auto run_expand_case(const JSONLDExpandCase &test_case) -> void {
  const auto resolver{
      suite_resolver(test_case.suite_root, test_case.base_prefix)};

  const auto input{sourcemeta::core::read_json(test_case.input)};

  if (test_case.negative) {
    try {
      if (test_case.expand_context.has_value()) {
        const auto context{
            sourcemeta::core::read_json(test_case.expand_context.value())};
        [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(
            input, context, test_case.base_iri, resolver, test_case.version)};
      } else {
        [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(
            input, test_case.base_iri, resolver, test_case.version)};
      }
      FAIL();
    } catch (const sourcemeta::core::JSONLDError &error) {
      // The implementation capitalises the first letter of every error code,
      // whereas the upstream suite expresses them in lower case.
      std::string actual_code{error.what()};
      if (!actual_code.empty()) {
        actual_code.front() =
            sourcemeta::core::to_lowercase(actual_code.front());
      }
      EXPECT_EQ(static_cast<std::string_view>(test_case.error_code),
                actual_code);
    }
  } else {
    const auto expected{sourcemeta::core::read_json(test_case.expect)};
    const auto actual{
        test_case.expand_context.has_value()
            ? sourcemeta::core::jsonld_expand(
                  input,
                  sourcemeta::core::read_json(test_case.expand_context.value()),
                  test_case.base_iri, resolver, test_case.version)
            : sourcemeta::core::jsonld_expand(input, test_case.base_iri,
                                              resolver, test_case.version)};
    EXPECT_EQ(actual, expected);
    // Every expansion output, and every expected fixture, is by definition a
    // valid expanded document, save for the few that are correct output yet
    // not valid documents, which the validator must reject instead.
    if (test_case.valid_expanded_output) {
      EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(actual));
      EXPECT_TRUE(sourcemeta::core::jsonld_is_expanded(expected));
    } else {
      EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(actual));
      EXPECT_FALSE(sourcemeta::core::jsonld_is_expanded(expected));
    }
  }
}

auto register_expand_case(const sourcemeta::core::JSON &entry,
                          const std::filesystem::path &suite_root,
                          const sourcemeta::core::JSON::String &base_prefix)
    -> void {
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
  test_case.valid_expanded_output =
      produces_valid_expanded_output(entry.at("@id").to_string());

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

  sourcemeta::core::test_register(
      "JSONLD_expand", suite_test_name(entry.at("@id").to_string()), __FILE__,
      __LINE__, [test_case]() -> void { run_expand_case(test_case); });
}

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
  bool compact_to_relative;
};

auto run_compact_case(const JSONLDCompactCase &test_case) -> void {
  const auto resolver{
      suite_resolver(test_case.suite_root, test_case.base_prefix)};

  const auto input{sourcemeta::core::read_json(test_case.input)};
  const auto context{sourcemeta::core::read_json(test_case.context)};

  if (test_case.negative) {
    try {
      const auto expanded{sourcemeta::core::jsonld_expand(
          input, test_case.base_iri, resolver, test_case.version)};
      [[maybe_unused]] const auto result{sourcemeta::core::jsonld_compact(
          expanded, context, test_case.base_iri, resolver, test_case.version,
          test_case.compact_arrays, test_case.compact_to_relative)};
      FAIL();
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
                  test_case.version, test_case.compact_arrays,
                  test_case.compact_to_relative),
              expected);
  }
}

auto register_compact_case(const sourcemeta::core::JSON &entry,
                           const std::filesystem::path &suite_root,
                           const sourcemeta::core::JSON::String &base_prefix)
    -> void {
  // A document declared as JSON-LD 1.0 exercises compaction behaviour that
  // changed for a 1.1 processor (notably that an expanded term definition is no
  // longer usable as a compact IRI prefix), per the JSON-LD 1.0 Errata. The
  // reference implementations skip these tests when run as a 1.1 processor, so
  // neither do we:
  //
  //   https://github.com/digitalbazaar/jsonld.js/blob/main/tests/test.js
  //   https://github.com/digitalbazaar/pyld/blob/master/tests/runtests.py
  if (entry.defines("option") && entry.at("option").defines("specVersion") &&
      entry.at("option").at("specVersion").to_string() == "json-ld-1.0") {
    return;
  }

  // Compaction requires input in valid expanded document form. A few suite
  // inputs expand into documents whose property IRIs are not valid IRIs (which
  // expansion does not validate), so they cannot serve as compaction input
  // without violating that precondition. The excluded input below has property
  // IRIs ending in "[" and "]", which are not valid IRIs.
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
  test_case.compact_to_relative = true;

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
    if (option.defines("compactToRelative")) {
      test_case.compact_to_relative =
          option.at("compactToRelative").to_boolean();
    }
  }

  if (negative) {
    test_case.error_code = entry.at("expectErrorCode").to_string();
  } else {
    test_case.expect = suite_root / entry.at("expect").to_string();
  }

  sourcemeta::core::test_register(
      "JSONLD_compact", suite_test_name(entry.at("@id").to_string()), __FILE__,
      __LINE__, [test_case]() -> void { run_compact_case(test_case); });
}

struct JSONLDFlattenCase {
  std::filesystem::path suite_root;
  sourcemeta::core::JSON::String base_prefix;
  std::filesystem::path input;
  std::filesystem::path expect;
  std::filesystem::path context;
  bool has_context;
  sourcemeta::core::JSON::String error_code;
  sourcemeta::core::JSON::String base_iri;
  sourcemeta::core::JSONLDVersion version;
  bool negative;
  bool compact_arrays;
};

// Flatten the already expanded input, compacting it against the test context
// when one is given.
auto run_flatten(const JSONLDFlattenCase &test_case,
                 const sourcemeta::core::JSON &expanded,
                 const sourcemeta::core::JSONLDResolver &resolver)
    -> sourcemeta::core::JSON {
  if (test_case.has_context) {
    const auto context{sourcemeta::core::read_json(test_case.context)};
    return sourcemeta::core::jsonld_flatten(
        expanded, context, test_case.base_iri, resolver, test_case.version,
        test_case.compact_arrays);
  }
  return sourcemeta::core::jsonld_flatten(expanded);
}

auto run_flatten_case(const JSONLDFlattenCase &test_case) -> void {
  const auto resolver{
      suite_resolver(test_case.suite_root, test_case.base_prefix)};

  const auto input{sourcemeta::core::read_json(test_case.input)};
  const auto expanded{sourcemeta::core::jsonld_expand(
      input, test_case.base_iri, resolver, test_case.version)};

  if (test_case.negative) {
    try {
      [[maybe_unused]] const auto result{
          run_flatten(test_case, expanded, resolver)};
      FAIL();
    } catch (const sourcemeta::core::JSONLDError &error) {
      std::string actual_code{error.what()};
      sourcemeta::core::to_lowercase(actual_code);
      std::string expected_code{test_case.error_code};
      sourcemeta::core::to_lowercase(expected_code);
      EXPECT_EQ(expected_code, actual_code);
    }
  } else {
    const auto expected{sourcemeta::core::read_json(test_case.expect)};
    EXPECT_EQ(run_flatten(test_case, expanded, resolver), expected);
  }
}

auto register_flatten_case(const sourcemeta::core::JSON &entry,
                           const std::filesystem::path &suite_root,
                           const sourcemeta::core::JSON::String &base_prefix)
    -> void {
  // A document declared as JSON-LD 1.0 exercises behaviour that changed for a
  // 1.1 processor, per the JSON-LD 1.0 Errata. The reference implementations
  // skip these tests when run as a 1.1 processor, so neither do we:
  //
  //   https://github.com/digitalbazaar/jsonld.js/blob/main/tests/test.js
  //   https://github.com/digitalbazaar/pyld/blob/master/tests/runtests.py
  if (entry.defines("option") && entry.at("option").defines("specVersion") &&
      entry.at("option").at("specVersion").to_string() == "json-ld-1.0") {
    return;
  }

  bool negative{false};
  for (const auto &type : entry.at("@type").as_array()) {
    if (type.to_string() == "jld:NegativeEvaluationTest") {
      negative = true;
    }
  }

  const auto &input_relative{entry.at("input").to_string()};

  JSONLDFlattenCase test_case;
  test_case.suite_root = suite_root;
  test_case.base_prefix = base_prefix;
  test_case.input = suite_root / input_relative;
  test_case.base_iri = base_prefix + input_relative;
  test_case.version = sourcemeta::core::JSONLDVersion::V1_1;
  test_case.negative = negative;
  test_case.compact_arrays = true;
  test_case.has_context = entry.defines("context");
  if (test_case.has_context) {
    test_case.context = suite_root / entry.at("context").to_string();
  }

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

  sourcemeta::core::test_register(
      "JSONLD_flatten", suite_test_name(entry.at("@id").to_string()), __FILE__,
      __LINE__, [test_case]() -> void { run_flatten_case(test_case); });
}

auto register_suite(const std::filesystem::path &suite_root,
                    const std::string_view manifest,
                    void (*callback)(const sourcemeta::core::JSON &,
                                     const std::filesystem::path &,
                                     const sourcemeta::core::JSON::String &))
    -> void {
  const auto document{sourcemeta::core::read_json(suite_root / manifest)};
  const auto &base_prefix{document.at("baseIri").to_string()};
  for (const auto &entry : document.at("sequence").as_array()) {
    callback(entry, suite_root, base_prefix);
  }
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path suite_root{JSONLD_SUITE_PATH};
  register_suite(suite_root, "expand-manifest.jsonld", register_expand_case);
  register_suite(suite_root, "compact-manifest.jsonld", register_compact_case);
  register_suite(suite_root, "flatten-manifest.jsonld", register_flatten_case);

  return sourcemeta::core::test_run(argc, argv);
}
