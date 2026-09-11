#include <sourcemeta/core/terminal.h>

#include <cassert>
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
            << std::flush;
  return 0;
}

} // namespace

auto main(int argc, char **argv) -> int {
#if defined(_WIN32)
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  if (argc > 1) {
    const std::string_view mode{argv[1]};
    if (mode == "--child-noninteractive-when-interactive") {
      return run_mode(sourcemeta::core::TerminalColorPolicy::WhenInteractive);
    }

    if (mode == "--child-noninteractive-always") {
      return run_mode(sourcemeta::core::TerminalColorPolicy::Always);
    }

    if (mode == "--child-noninteractive-disabled") {
      return run_mode(sourcemeta::core::TerminalColorPolicy::Disabled);
    }
  }

  return 1;
}
