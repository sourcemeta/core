#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

#include <algorithm>  // std::min
#include <exception>  // std::exception
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <string>     // std::string
#include <vector>     // std::vector

enum class YAMLTestType { Success, Error };

class YAMLTest : public testing::Test {
public:
  explicit YAMLTest(std::filesystem::path directory,
                    const YAMLTestType category)
      : test_directory{std::move(directory)}, type{category} {}

  auto TestBody() -> void override {
    const auto yaml_path{this->test_directory / "in.yaml"};

    if (this->type == YAMLTestType::Success) {
      const auto json_path{this->test_directory / "in.json"};

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
    } else if (this->type == YAMLTestType::Error) {
      try {
        sourcemeta::core::read_yaml(yaml_path);
        FAIL() << "Expected a YAML parse error but parsing succeeded";
      } catch (const sourcemeta::core::YAMLParseError &) {
        SUCCEED();
      } catch (const std::exception &error) {
        FAIL() << "Expected a YAML parse error but got: " << error.what();
      }
    } else {
      FAIL() << "Invalid test type";
    }
  }

private:
  const std::filesystem::path test_directory;
  const YAMLTestType type;
};

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path test_suite_path{YAMLTESTSUITE_PATH};

  for (const auto &entry :
       std::filesystem::directory_iterator(test_suite_path)) {
    if (!entry.is_directory()) {
      continue;
    }

    const auto test_directory{entry.path()};
    const auto test_name{test_directory.filename().string()};

    const auto json_file{test_directory / "in.json"};
    const auto error_file{test_directory / "error"};

    YAMLTestType type;
    if (std::filesystem::exists(json_file)) {
      type = YAMLTestType::Success;
    } else if (std::filesystem::exists(error_file)) {
      type = YAMLTestType::Error;
    } else {
      continue;
    }

    testing::RegisterTest("YAMLTestSuite", test_name.c_str(), nullptr, nullptr,
                          __FILE__, __LINE__, [=]() -> YAMLTest * {
                            return new YAMLTest(test_directory, type);
                          });
  }

  return RUN_ALL_TESTS();
}
