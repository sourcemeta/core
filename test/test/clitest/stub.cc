// A program for the script runner to run. It does only what a script asks of
// it, so that a fixture can exercise a command without depending on any real
// tool being present

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/options.h>

#include <algorithm>   // std::replace
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <cstdio>      // stdout
#include <cstdlib>     // EXIT_SUCCESS, EXIT_FAILURE, std::getenv
#include <filesystem>  // std::filesystem::current_path
#include <iostream>    // std::cin, std::cout, std::cerr
#include <string>      // std::string
#include <string_view> // std::string_view

#if defined(_WIN32)
#include <fcntl.h> // _O_BINARY
#include <io.h>    // _setmode, _fileno
#endif

// Emit an arbitrary byte, so that a fixture can hand the runner something that
// is not text at all. Whether it did so is reported, as an odd number of
// digits would otherwise quietly emit one byte fewer than the fixture meant
static auto write_bytes(const std::string_view hex) -> bool {
  if (hex.size() % 2 != 0) {
    return false;
  }

  for (std::size_t index{0}; index < hex.size(); index += 2) {
    std::cout.put(static_cast<char>(
        std::stoi(std::string{hex.substr(index, 2)}, nullptr, 16)));
  }

  return true;
}

auto main(int argc, char *argv[]) -> int {
#if defined(_WIN32)
  // Say exactly what the fixture asked for. The standard output of a program is
  // otherwise in text mode here, which turns every line feed into a carriage
  // return and a line feed, so a fixture asking for one of its own would get
  // two
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  sourcemeta::core::Options application;
  application.option("out", {});
  application.option("err", {});
  application.option("code", {});
  application.option("env", {});
  application.option("bytes", {});
  application.option("crlf", {});
  application.option("gunzip", {});
  application.option("unterminated", {});
  application.flag("cat", {});
  application.flag("pwd", {});
  application.parse(argc, argv);

  for (const auto line : application.at("out")) {
    std::cout << line << "\n";
  }

  if (application.contains("pwd")) {
    auto directory{std::filesystem::current_path().string()};
    std::replace(directory.begin(), directory.end(), '\\', '/');
    std::cout << directory << "\n";
  }

  for (const auto name : application.at("env")) {
    const char *const value{std::getenv(std::string{name}.c_str())};
    std::cout << name << "=" << (value == nullptr ? "" : value) << "\n";
  }

  for (const auto line : application.at("crlf")) {
    std::cout << line << "\r\n";
  }

  for (const auto hex : application.at("bytes")) {
    if (!write_bytes(hex)) {
      std::cerr << "not a whole number of bytes: " << hex << "\n";
      return EXIT_FAILURE;
    }
  }

  for (const auto name : application.at("gunzip")) {
    auto stream{sourcemeta::core::read_file(std::string{name})};
    const auto compressed{sourcemeta::core::read_to_string(stream)};
    std::cout << sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(compressed.data()),
        compressed.size());
  }

  // Written without a closing terminator, which no command of the language
  // produces but a program under test certainly can
  for (const auto name : application.at("unterminated")) {
    sourcemeta::core::write_file(std::filesystem::path{std::string{name}},
                                 std::string_view{"gamma\nbeta\nalpha"});
  }

  if (application.contains("cat")) {
    std::cout << std::cin.rdbuf();
  }

  for (const auto argument : application.positional()) {
    std::cout << "argument: " << argument << "\n";
  }

  for (const auto line : application.at("err")) {
    std::cerr << line << "\n";
  }

  std::cout.flush();
  std::cerr.flush();

  if (application.contains("code")) {
    const auto code{
        sourcemeta::core::to_int64_t(application.at("code").front())};
    if (!code.has_value()) {
      std::cerr << "not an exit code: " << application.at("code").front()
                << "\n";
      return EXIT_FAILURE;
    }

    return static_cast<int>(code.value());
  }

  return EXIT_SUCCESS;
}
