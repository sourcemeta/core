#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/test.h>

#include <algorithm>  // std::sort
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <string>     // std::string
#include <vector>     // std::vector

namespace {

auto values_match(const std::vector<sourcemeta::core::JSONPath::Node> &nodes,
                  const sourcemeta::core::JSON &expected) -> bool {
  const auto &array{expected.as_array()};
  if (nodes.size() != array.size()) {
    return false;
  }

  for (std::size_t index = 0; index < nodes.size(); ++index) {
    if (!(*nodes[index].value == expected.at(index))) {
      return false;
    }
  }

  return true;
}

auto paths_match(const std::vector<sourcemeta::core::JSONPath::Node> &nodes,
                 const sourcemeta::core::JSON &expected) -> bool {
  const auto &array{expected.as_array()};
  if (nodes.size() != array.size()) {
    return false;
  }

  for (std::size_t index = 0; index < nodes.size(); ++index) {
    if (sourcemeta::core::JSONPath::normalize(nodes[index].location) !=
        expected.at(index).to_string()) {
      return false;
    }
  }

  return true;
}

auto run_jsonpath_test_case(const sourcemeta::core::JSON &test_case) -> void {
  const auto &selector{test_case.at("selector").to_string()};
  const auto *invalid{test_case.try_at("invalid_selector")};
  if (invalid != nullptr && invalid->is_boolean() && invalid->to_boolean()) {
    try {
      const sourcemeta::core::JSONPath path{selector};
      FAIL();
    } catch (const sourcemeta::core::JSONPathParseError &error) {
      // An invalid selector is expected to be rejected
      EXPECT_TRUE(error.column() > 0);
    }

    return;
  }

  const sourcemeta::core::JSONPath path{selector};
  const auto nodes{path.evaluate(test_case.at("document"))};
  const auto *result{test_case.try_at("result")};
  if (result != nullptr) {
    EXPECT_TRUE(values_match(nodes, *result));
    const auto *result_paths{test_case.try_at("result_paths")};
    if (result_paths != nullptr) {
      EXPECT_TRUE(paths_match(nodes, *result_paths));
    }

    return;
  }

  // Queries whose result order legitimately varies enumerate every
  // acceptable outcome, with paths aligned by index when present
  const auto &results{test_case.at("results")};
  const auto *results_paths{test_case.try_at("results_paths")};
  bool matched{false};
  for (std::size_t index = 0; index < results.as_array().size(); ++index) {
    if (values_match(nodes, results.at(index)) &&
        (results_paths == nullptr ||
         paths_match(nodes, results_paths->at(index)))) {
      matched = true;
      break;
    }
  }

  EXPECT_TRUE(matched);
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{std::filesystem::path{JSONPATH_SUITE_PATH} /
                                   "tests"};
  std::vector<std::filesystem::path> files;
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator{base}) {
    if (entry.is_regular_file() && entry.path().extension() == ".json") {
      files.push_back(entry.path());
    }
  }

  std::sort(files.begin(), files.end());
  for (const auto &file : files) {
    const auto document{sourcemeta::core::read_json(file)};
    auto prefix{std::filesystem::relative(file, base)};
    prefix.replace_extension();
    for (const auto &test_case : document.at("tests").as_array()) {
      sourcemeta::core::test_register(
          prefix.generic_string() + ": " + test_case.at("name").to_string(),
          [test_case]() -> void { run_jsonpath_test_case(test_case); });
    }
  }

  return sourcemeta::core::test_run(argc, argv);
}
