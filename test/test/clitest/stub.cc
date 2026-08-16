// A program for the script runner to run. It does only what a script asks of
// it, so that a fixture can exercise a command without depending on any real
// tool being present

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/options.h>

#include <cstdint>    // std::uint8_t
#include <cstdlib>    // EXIT_SUCCESS, std::getenv
#include <filesystem> // std::filesystem::current_path
#include <iostream>   // std::cin, std::cout, std::cerr
#include <string>     // std::string

// Emit an arbitrary byte, so that a fixture can hand the runner something that
// is not text at all
static auto write_bytes(const std::string_view hex) -> void {
  for (std::size_t index{0}; index + 1 < hex.size(); index += 2) {
    std::cout.put(static_cast<char>(
        std::stoi(std::string{hex.substr(index, 2)}, nullptr, 16)));
  }
}

auto main(int argc, char *argv[]) -> int {
  sourcemeta::core::Options application;
  application.option("out", {});
  application.option("err", {});
  application.option("code", {});
  application.option("env", {});
  application.option("bytes", {});
  application.option("crlf", {});
  application.option("gunzip", {});
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
    write_bytes(hex);
  }

  for (const auto name : application.at("gunzip")) {
    auto stream{sourcemeta::core::read_file(std::string{name})};
    const auto compressed{sourcemeta::core::read_to_string(stream)};
    std::cout << sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(compressed.data()),
        compressed.size());
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
    return std::stoi(std::string{application.at("code").front()});
  }

  return EXIT_SUCCESS;
}
