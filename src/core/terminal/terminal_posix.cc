#include "terminal_internal.h"

#include <unistd.h> // isatty, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO

namespace sourcemeta::core::internal {

auto is_interactive_stream(TerminalStream stream) noexcept -> bool {
  int file_descriptor{STDOUT_FILENO};
  switch (stream) {
    case TerminalStream::Stdin:
      file_descriptor = STDIN_FILENO;
      break;
    case TerminalStream::Stdout:
      file_descriptor = STDOUT_FILENO;
      break;
    case TerminalStream::Stderr:
      file_descriptor = STDERR_FILENO;
      break;
  }
  return ::isatty(file_descriptor) == 1;
}

auto is_interactive_fd(int file_descriptor) noexcept -> bool {
  if (file_descriptor < 0) {
    return false;
  }
  return ::isatty(file_descriptor) == 1;
}

auto enable_virtual_terminal_stream(TerminalStream /*stream*/) noexcept
    -> void {
  // POSIX terminals interpret ANSI escape sequences natively.
}

} // namespace sourcemeta::core::internal
