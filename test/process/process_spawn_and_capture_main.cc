#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdio>      // std::fread, std::fwrite, std::fflush, std::FILE
#include <cstdlib>     // std::strtol, std::strtoll
#include <filesystem>  // std::filesystem::current_path
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view

#if defined(_WIN32)
#include <fcntl.h>  // _O_BINARY
#include <io.h>     // _setmode, _fileno
#include <stdlib.h> // _environ
#else
#include <csignal> // std::raise, SIGKILL
extern char **environ;
#endif

namespace {

auto environment_entries() -> char ** {
#if defined(_WIN32)
  return _environ;
#else
  return environ;
#endif
}

auto environment_name_matches(const std::string_view left,
                              const std::string_view right) -> bool {
#if defined(_WIN32)
  // Windows compares environment variable names without regard to case, and
  // spells the search path "Path" rather than "PATH"
  return sourcemeta::core::equals_ignore_case(left, right);
#else
  return left == right;
#endif
}

// Scanning the block avoids the lookup the Microsoft runtime deprecates, and
// keeps one way of reading the environment across every platform
auto find_environment(const std::string_view name) -> const char * {
  for (char **entry = environment_entries();
       entry != nullptr && ((*entry) != nullptr); ++entry) {
    const std::string_view current{*entry};
    const auto separator{current.find('=')};
    if (separator != std::string_view::npos &&
        environment_name_matches(current.substr(0, separator), name)) {
      return *entry + separator + 1;
    }
  }

  return nullptr;
}

auto write_all(std::FILE *stream, const std::string_view payload) -> void {
  if (!payload.empty()) {
    std::fwrite(payload.data(), 1, payload.size(), stream);
  }

  std::fflush(stream);
}

auto read_all(std::FILE *stream) -> std::string {
  std::string result;
  std::array<char, 4096> buffer{};
  std::size_t count{0};
  while ((count = std::fread(buffer.data(), 1, buffer.size(), stream)) > 0) {
    result.append(buffer.data(), count);
  }

  return result;
}

// A payload whose bytes vary, so that a truncated or reordered transfer cannot
// pass by accident
auto payload_of(const std::size_t size) -> std::string {
  std::string result;
  result.reserve(size);
  for (std::size_t index = 0; index < size; ++index) {
    result.push_back(static_cast<char>('a' + (index % 26)));
  }

  return result;
}

auto to_size(const char *value) -> std::size_t {
  return static_cast<std::size_t>(std::strtoll(value, nullptr, 10));
}

} // namespace

auto main(int argc, char *argv[]) -> int {
#if defined(_WIN32)
  // Otherwise the C runtime rewrites every line feed on the way out, and the
  // captured bytes stop matching what the program wrote
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  _setmode(_fileno(stderr), _O_BINARY);
#endif

  if (argc < 2) {
    return 2;
  }

  const std::string_view command{argv[1]};

  if (command == "exit") {
    return static_cast<int>(std::strtol(argv[2], nullptr, 10));
  }

  if (command == "stdout") {
    write_all(stdout, argv[2]);
    return 0;
  }

  if (command == "stderr") {
    write_all(stderr, argv[2]);
    return 0;
  }

  if (command == "both") {
    write_all(stdout, argv[2]);
    write_all(stderr, argv[3]);
    return 0;
  }

  if (command == "silent") {
    return 0;
  }

  if (command == "cat") {
    write_all(stdout, read_all(stdin));
    return 0;
  }

  if (command == "count-stdin") {
    write_all(stdout, std::to_string(read_all(stdin).size()));
    return 0;
  }

  // Fills a stream before touching the input, which deadlocks any parent that
  // writes the whole input before reading anything
  if (command == "flood-stderr-then-cat") {
    write_all(stderr, payload_of(to_size(argv[2])));
    write_all(stdout, read_all(stdin));
    return 0;
  }

  if (command == "flood-both") {
    const auto payload{payload_of(to_size(argv[2]))};
    write_all(stdout, payload);
    write_all(stderr, payload);
    return 0;
  }

  // Exits without reading anything, so a parent still writing its input meets a
  // pipe with no reader
  if (command == "ignore-stdin") {
    write_all(stdout, "done");
    return 0;
  }

  if (command == "environment") {
    std::string result;
    for (char **entry = environment_entries();
         entry != nullptr && ((*entry) != nullptr); ++entry) {
      result.append(*entry);
      result.push_back('\n');
    }

    write_all(stdout, result);
    return 0;
  }

  // The commands below report through the exit code alone, so that a caller
  // that does not capture anything can still assert on what the program saw
  if (command == "expect-stdin") {
    return read_all(stdin) == argv[2] ? 0 : 1;
  }

  // Checks a payload too large to hand over as an argument
  if (command == "expect-stdin-size") {
    return read_all(stdin).size() == to_size(argv[2]) ? 0 : 1;
  }

  if (command == "expect-environment") {
    const char *value{find_environment(argv[2])};
    return value != nullptr && std::string_view{value} == argv[3] ? 0 : 1;
  }

  if (command == "expect-no-environment") {
    return find_environment(argv[2]) == nullptr ? 0 : 1;
  }

  if (command == "expect-directory") {
    return std::filesystem::current_path() == std::filesystem::path{argv[2]}
               ? 0
               : 1;
  }

  if (command == "directory") {
    write_all(stdout, std::filesystem::current_path().string());
    return 0;
  }

  if (command == "arguments") {
    std::string result;
    for (int index = 2; index < argc; ++index) {
      result.append(argv[index]);
      result.push_back('\n');
    }

    write_all(stdout, result);
    return 0;
  }

#if !defined(_WIN32)
  // Writes first, so that a caller can tell whether output survives a program
  // that never gets to exit
  if (command == "terminate") {
    write_all(stdout, "partial");
    std::raise(SIGKILL);
  }
#endif

  return 2;
}
