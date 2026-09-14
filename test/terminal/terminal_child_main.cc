#include <sourcemeta/core/options.h>
#include <sourcemeta/core/terminal.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string_view>

#if defined(_WIN32)
#include <fcntl.h> // _O_BINARY
#include <io.h>    // _setmode, _fileno
#endif

namespace {

auto run_mode(sourcemeta::core::TerminalColorPolicy policy) -> int {
  sourcemeta::core::terminal_set_color_policy(
      sourcemeta::core::TerminalStream::Stdout, policy);
  assert(!sourcemeta::core::terminal_is_interactive(
      sourcemeta::core::TerminalStream::Stdout));
  sourcemeta::core::terminal_paint(
      std::cout, sourcemeta::core::TerminalStream::Stdout, "streamed",
      sourcemeta::core::TerminalStyle::Red);
  std::cout << "\n"
            << sourcemeta::core::terminal_paint(
                   sourcemeta::core::TerminalStream::Stdout, "painted",
                   sourcemeta::core::TerminalStyle::Bold |
                       sourcemeta::core::TerminalStyle::Green)
            << "\n"
            << std::flush;
  return EXIT_SUCCESS;
}

} // namespace

auto main(int argc, char **argv) -> int {
#if defined(_WIN32)
  // Ensure standard output is binary on Windows so newlines are not converted
  // to CRLF (\r\n), which would alter captured byte stream comparisons.
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  sourcemeta::core::Options application;
  application.option("color", {});
  application.parse(argc, argv);

  auto policy{sourcemeta::core::TerminalColorPolicy::WhenInteractive};
  if (application.contains("color")) {
    const auto &values{application.at("color")};
    if (values.empty()) {
      std::cerr << "missing value for --color\n";
      return EXIT_FAILURE;
    }

    const auto value{values.front()};
    if (value == "always") {
      policy = sourcemeta::core::TerminalColorPolicy::Always;
    } else if (value == "auto" || value == "when-interactive") {
      policy = sourcemeta::core::TerminalColorPolicy::WhenInteractive;
    } else if (value == "never" || value == "disabled") {
      policy = sourcemeta::core::TerminalColorPolicy::Disabled;
    } else {
      std::cerr << "unknown --color option: " << value << "\n";
      return EXIT_FAILURE;
    }
  }

  return run_mode(policy);
}
