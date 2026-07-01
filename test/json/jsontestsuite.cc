#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <exception>  // std::exception
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <ios>        // std::ios::binary, std::ios_base::badbit
#include <string>     // std::string, std::char_traits

namespace {
enum class JSONTestType { Accept, Reject };

auto run_json_test_case(const std::filesystem::path &path,
                        const JSONTestType type) -> void {
  std::ifstream stream{path, std::ios::binary};
  stream.exceptions(std::ios_base::badbit);
  if (type == JSONTestType::Accept) {
    sourcemeta::core::parse_json(stream);
    // Core consumes up to the end of a valid document in the stream. Force it
    // to continue until the end to cover certain test cases.
    while (stream.peek() != std::char_traits<char>::eof()) {
      switch (stream.get()) {
        case '\r':
        case '\t':
        case '\n':
        case ' ':
          break;
        default:
          FAIL();
      }
    }
  } else {
    // Core consumes up to the end of a valid document in the stream. Force it
    // to continue until the end to cover certain test cases.
    try {
      while (!stream.eof()) {
        sourcemeta::core::parse_json(stream);
      }
      FAIL();
    } catch (const sourcemeta::core::JSONParseError &) {
      // A malformed document is expected to be rejected
    } catch (...) {
      FAIL();
    }
  }
}
} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path test_parsing_path{
      std::filesystem::path{JSONTESTSUITE_PATH} / "test_parsing"};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator{test_parsing_path}) {
    const std::filesystem::path test_path{entry.path()};
    const char front{test_path.filename().string().front()};
    const JSONTestType type{front == 'n' || front == 'i'
                                ? JSONTestType::Reject
                                : JSONTestType::Accept};
    sourcemeta::core::test_register(
        test_path.filename().string(),
        [test_path, type]() -> void { run_json_test_case(test_path, type); });
  }

  return sourcemeta::core::test_run(argc, argv);
}
