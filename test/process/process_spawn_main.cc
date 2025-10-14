#include <sourcemeta/core/process.h>

#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

auto main(int argc, char *argv[]) -> int {
  const std::filesystem::path directory{argv[1]};
  const std::string program{argv[2]};
  std::vector<std::string_view> arguments;

  for (int index = 3; index < argc; ++index) {
    arguments.emplace_back(argv[index]);
  }

  return sourcemeta::core::spawn(program, arguments, directory);
}
