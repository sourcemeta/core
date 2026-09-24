#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/yaml.h>

#include <algorithm>  // std::min
#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem
#include <istream>    // std::basic_istream
#include <ostream>    // std::ostream
#include <sstream>    // std::ostringstream, std::istringstream
#include <string>     // std::string
#include <vector>     // std::vector

namespace {
enum class YAMLTestType : std::uint8_t { Success, Error };

// Reads every document a stream holds and writes each one back out with the
// formatting it was read with, gathering the documents into an array so that
// how many there are is compared along with what each one holds.
//
// A stream that trails off into comments, or into a bare document end marker,
// holds no further document, which the parser reports the same way it reports
// an input with no document at all. Any other failure to read a case is
// already reported by the test that checks it against its expected value
auto roundtrip_all(std::basic_istream<char> &stream, std::ostream &output)
    -> sourcemeta::core::JSON {
  auto documents{sourcemeta::core::JSON::make_array()};
  while (stream.peek() != EOF) {
    sourcemeta::core::YAMLRoundTrip metadata;
    try {
      documents.push_back(sourcemeta::core::parse_yaml(stream, metadata));
    } catch (const sourcemeta::core::YAMLParseError &) {
      break;
    }

    sourcemeta::core::stringify_yaml(documents.back(), output, metadata);
  }

  return documents;
}

// A YAML document carries presentation that its JSON value does not, so the
// round-trip emitter is only correct if the text it writes reads back as the
// very same documents. See
// https://yaml.org/spec/1.2.2/#321-representation-graph
auto run_yaml_roundtrip_case(const std::filesystem::path &yaml_path) -> void {
  auto input{sourcemeta::core::read_file(yaml_path)};
  std::ostringstream first_pass;
  const auto documents{roundtrip_all(input, first_pass)};

  std::istringstream emitted{first_pass.str()};
  std::ostringstream second_pass;
  const auto reread{roundtrip_all(emitted, second_pass)};

  // Numbers of different types compare equal to each other, so the documents
  // are compared as JSON text, which tells them apart and says what differs
  std::ostringstream expected;
  sourcemeta::core::stringify(documents, expected);
  std::ostringstream actual;
  sourcemeta::core::stringify(reread, actual);

  EXPECT_EQ(actual.str(), expected.str());

  // Writing out what was just written has nothing left to change
  EXPECT_EQ(second_pass.str(), first_pass.str());
}

auto run_yaml_test_case(const std::filesystem::path &test_directory,
                        const YAMLTestType type) -> void {
  const auto yaml_path{test_directory / "in.yaml"};

  if (type == YAMLTestType::Success) {
    const auto json_path{test_directory / "in.json"};

    auto yaml_stream{sourcemeta::core::read_file(yaml_path)};
    auto json_stream{sourcemeta::core::read_file(json_path)};

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

  const auto yaml_path{test_directory / "in.yaml"};
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
