#include <sourcemeta/core/json.h>
#include <sourcemeta/core/json_error.h>

#include <gtest/gtest.h>

#include <exception>  // std::exception
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <string>     // std::string, std::char_traits

enum class JSONTestType { Accept, Reject };

class JSONTest : public testing::Test {
public:
  explicit JSONTest(const std::filesystem::path path,
                    const JSONTestType category)
      : test_path{path}, type{category} {}

  void TestBody() override {
    std::ifstream stream{this->test_path};
    stream.exceptions(std::ios_base::badbit);
    if (this->type == JSONTestType::Accept) {
      sourcemeta::core::parse_json(stream);
      // Core consumes up to the end of a valid document
      // in the stream. Force it continue until the end to cover
      // certain test cases.
      while (stream.peek() != std::char_traits<char>::eof()) {
        switch (stream.get()) {
          case '\n':
          case ' ':
            break;
          default:
            FAIL() << "Unexpected character";
        }
      }
    } else if (this->type == JSONTestType::Reject) {
      try {
        // Core consumes up to the end of a valid document
        // in the stream. Force it continue until the end to cover
        // certain test cases.
        while (!stream.eof()) {
          sourcemeta::core::parse_json(stream);
        }
      } catch (const sourcemeta::core::JSONParseError &) {
        SUCCEED();
      } catch (const std::exception &) {
        FAIL() << "The parse function threw an unexpected error";
      }
    } else {
      FAIL() << "Invalid test type";
    }
  }

private:
  const std::filesystem::path test_path;
  const JSONTestType type;
};

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path test_parsing_path =
      std::filesystem::path{JSONTESTSUITE_PATH} / "test_parsing";
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator(test_parsing_path)) {
    const std::filesystem::path test_path{entry.path()};
    const char front = test_path.filename().string().front();

    JSONTestType type = front == 'n' || front == 'i' ? JSONTestType::Reject
                                                     : JSONTestType::Accept;
    testing::RegisterTest(
        "JSONTestSuite", test_path.filename().string().c_str(), nullptr,
        nullptr, __FILE__, __LINE__,
        [=]() -> JSONTest * { return new JSONTest(test_path, type); });
  }

  return RUN_ALL_TESTS();
}
