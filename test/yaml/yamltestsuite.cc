#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <algorithm>  // std::min
#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <iterator>   // std::istreambuf_iterator
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <vector>     // std::vector

namespace {
enum class YAMLTestType : std::uint8_t { Success, Error };

auto read_to_string(const std::filesystem::path &path) -> std::string {
  std::ifstream stream{path, std::ios::binary};
  stream.exceptions(std::ios_base::badbit);
  return std::string{std::istreambuf_iterator<char>{stream},
                     std::istreambuf_iterator<char>{}};
}

// Numbers of different types compare equal to each other, so the types have to
// agree too before the emitted document can be said to still hold the same
// value
auto same_value(const sourcemeta::core::JSON &left,
                const sourcemeta::core::JSON &right) -> bool {
  if (left.type() != right.type()) {
    return false;
  }

  if (left.is_array()) {
    if (left.size() != right.size()) {
      return false;
    }

    for (std::size_t index = 0; index < left.size(); index++) {
      if (!same_value(left.at(index), right.at(index))) {
        return false;
      }
    }

    return true;
  }

  if (left.is_object()) {
    if (left.size() != right.size()) {
      return false;
    }

    for (const auto &entry : left.as_object()) {
      if (!right.defines(entry.first) ||
          !same_value(entry.second, right.at(entry.first))) {
        return false;
      }
    }

    return true;
  }

  return left == right;
}

// A YAML document carries presentation that its JSON value does not, so the
// round-trip emitter is only correct if the text it writes reads back as the
// very same document. See https://yaml.org/spec/1.2.2/#321-representation-graph
auto run_yaml_roundtrip_case(const std::filesystem::path &yaml_path) -> void {
  sourcemeta::core::YAMLRoundTrip metadata;
  const auto document{
      sourcemeta::core::parse_yaml(read_to_string(yaml_path), metadata)};

  std::ostringstream output;
  sourcemeta::core::stringify_yaml(document, output, metadata);

  sourcemeta::core::YAMLRoundTrip emitted_metadata;
  const auto emitted{
      sourcemeta::core::parse_yaml(output.str(), emitted_metadata)};

  EXPECT_TRUE(same_value(emitted, document));
}

auto run_yaml_test_case(const std::filesystem::path &test_directory,
                        const YAMLTestType type) -> void {
  const auto yaml_path{test_directory / "in.yaml"};

  if (type == YAMLTestType::Success) {
    const auto json_path{test_directory / "in.json"};

    std::ifstream yaml_stream{yaml_path, std::ios::binary};
    std::ifstream json_stream{json_path, std::ios::binary};
    yaml_stream.exceptions(std::ios_base::badbit);
    json_stream.exceptions(std::ios_base::badbit);

    std::vector<sourcemeta::core::JSON> yaml_documents;
    std::vector<sourcemeta::core::JSON> json_documents;

    // Parse all YAML documents from the stream
    while (yaml_stream.peek() != EOF) {
      try {
        yaml_documents.push_back(sourcemeta::core::parse_yaml(yaml_stream));
      } catch (const sourcemeta::core::YAMLParseError &) {
        break;
      }
    }

    // Parse all JSON documents from the stream
    while (json_stream.peek() != EOF) {
      try {
        json_documents.push_back(sourcemeta::core::parse_json(json_stream));
      } catch (const sourcemeta::core::JSONParseError &) {
        break;
      }
    }

    EXPECT_EQ(yaml_documents.size(), json_documents.size());
    const auto compare_count{
        std::min(yaml_documents.size(), json_documents.size())};
    for (std::size_t index = 0; index < compare_count; index++) {
      EXPECT_EQ(yaml_documents[index], json_documents[index]);
    }
  } else {
    try {
      sourcemeta::core::read_yaml(yaml_path);
      FAIL();
    } catch (const sourcemeta::core::YAMLParseError &) {
      // A malformed document is expected to be rejected
    }
  }
}

auto register_yaml_test_case(const std::filesystem::path &test_directory,
                             const std::string &test_name) -> void {
  // A case that carries an "error" marker must be rejected even when a sample
  // "in.json" is also present, so the marker takes precedence
  YAMLTestType type;
  if (std::filesystem::exists(test_directory / "error")) {
    type = YAMLTestType::Error;
  } else if (std::filesystem::exists(test_directory / "in.json")) {
    type = YAMLTestType::Success;
  } else {
    return;
  }

  sourcemeta::core::test_register(test_name, [test_directory, type]() -> void {
    run_yaml_test_case(test_directory, type);
  });

  if (type == YAMLTestType::Error) {
    return;
  }

  // The round-trip emitter writes a single document, so a case that holds
  // several, or none at all, is out of its scope
  const auto yaml_path{test_directory / "in.yaml"};
  try {
    sourcemeta::core::read_yaml(yaml_path);
  } catch (const sourcemeta::core::YAMLError &) {
    return;
  } catch (const sourcemeta::core::YAMLParseError &) {
    return;
  }

  sourcemeta::core::test_register(
      test_name + "/roundtrip",
      [yaml_path]() -> void { run_yaml_roundtrip_case(yaml_path); });
}
} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path test_suite_path{YAMLTESTSUITE_PATH};

  for (const auto &entry :
       std::filesystem::directory_iterator(test_suite_path)) {
    if (!entry.is_directory()) {
      continue;
    }

    const auto &test_directory{entry.path()};
    const auto test_name{test_directory.filename().string()};

    // A suite entry either holds the case files directly or, for the
    // multi-document cases, contains one numbered subdirectory per case, which
    // a non-recursive scan would silently skip
    if (std::filesystem::exists(test_directory / "in.json") ||
        std::filesystem::exists(test_directory / "error")) {
      register_yaml_test_case(test_directory, test_name);
    } else {
      for (const auto &subentry :
           std::filesystem::directory_iterator(test_directory)) {
        if (subentry.is_directory()) {
          register_yaml_test_case(subentry.path(),
                                  test_name + "/" +
                                      subentry.path().filename().string());
        }
      }
    }
  }

  return sourcemeta::core::test_run(argc, argv);
}
