#include "terminal_internal.h"
#include <sourcemeta/core/terminal.h>

#include <array>       // std::array
#include <atomic>      // std::atomic, std::memory_order_relaxed
#include <cstddef>     // std::size_t
#include <ostream>     // std::ostream
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// Indices match TerminalStream: Stdin = 0, Stdout = 1, Stderr = 2
std::array<std::atomic<sourcemeta::core::TerminalColorPolicy>, 3>
    g_stream_policies{{sourcemeta::core::TerminalColorPolicy::WhenInteractive,
                       sourcemeta::core::TerminalColorPolicy::WhenInteractive,
                       sourcemeta::core::TerminalColorPolicy::WhenInteractive}};

constexpr auto stream_index(sourcemeta::core::TerminalStream stream) noexcept
    -> std::size_t {
  switch (stream) {
    case sourcemeta::core::TerminalStream::Stdin:
      return 0;
    case sourcemeta::core::TerminalStream::Stdout:
      return 1;
    case sourcemeta::core::TerminalStream::Stderr:
      return 2;
  }
  return 1;
}

} // namespace

namespace sourcemeta::core {

auto terminal_is_interactive(TerminalStream stream) noexcept -> bool {
  return internal::is_interactive_stream(stream);
}

auto terminal_is_interactive(int file_descriptor) noexcept -> bool {
  return internal::is_interactive_fd(file_descriptor);
}

auto terminal_set_color_policy(TerminalColorPolicy policy) noexcept -> void {
  for (auto &stream_policy : g_stream_policies) {
    stream_policy.store(policy, std::memory_order_relaxed);
  }
}

auto terminal_set_color_policy(TerminalStream stream,
                               TerminalColorPolicy policy) noexcept -> void {
  g_stream_policies[stream_index(stream)].store(policy,
                                                std::memory_order_relaxed);
}

auto terminal_color_policy(TerminalStream stream) noexcept
    -> TerminalColorPolicy {
  return g_stream_policies[stream_index(stream)].load(
      std::memory_order_relaxed);
}

auto terminal_color_enabled(TerminalStream stream) noexcept -> bool {
  if (terminal_color_policy(stream) == TerminalColorPolicy::Disabled) {
    return false;
  }
  return terminal_is_interactive(stream);
}

auto terminal_sgr_reset() noexcept -> std::string_view { return "\033[0m"; }

auto terminal_sgr_sequence(TerminalStyle style) noexcept -> std::string_view {
  if (style == TerminalStyle::None) {
    return {};
  }

  const bool has_bold{(style & TerminalStyle::Bold) != TerminalStyle::None};

  if ((style & TerminalStyle::Red) != TerminalStyle::None) {
    return has_bold ? "\033[1;31m" : "\033[31m";
  }
  if ((style & TerminalStyle::Green) != TerminalStyle::None) {
    return has_bold ? "\033[1;32m" : "\033[32m";
  }
  if ((style & TerminalStyle::Yellow) != TerminalStyle::None) {
    return has_bold ? "\033[1;33m" : "\033[33m";
  }
  if ((style & TerminalStyle::Blue) != TerminalStyle::None) {
    return has_bold ? "\033[1;34m" : "\033[34m";
  }
  if ((style & TerminalStyle::Cyan) != TerminalStyle::None) {
    return has_bold ? "\033[1;36m" : "\033[36m";
  }

  if (has_bold) {
    return "\033[1m";
  }

  return {};
}

auto terminal_paint(std::string_view text, TerminalStyle style, bool enabled)
    -> std::string {
  if (!enabled || style == TerminalStyle::None || text.empty()) {
    return std::string{text};
  }

  const std::string_view sequence{terminal_sgr_sequence(style)};
  if (sequence.empty()) {
    return std::string{text};
  }

  const std::string_view reset{terminal_sgr_reset()};
  std::string styled_text;
  styled_text.reserve(sequence.size() + text.size() + reset.size());
  styled_text.append(sequence);
  styled_text.append(text);
  styled_text.append(reset);
  return styled_text;
}

auto terminal_paint(TerminalStream stream, std::string_view text,
                    TerminalStyle style) -> std::string {
  const bool enabled{terminal_color_enabled(stream)};
  if (enabled && style != TerminalStyle::None && !text.empty()) {
    internal::enable_virtual_terminal_stream(stream);
  }
  return terminal_paint(text, style, enabled);
}

auto terminal_paint(std::ostream &output, std::string_view text,
                    TerminalStyle style, bool enabled) -> std::ostream & {
  if (!enabled || style == TerminalStyle::None || text.empty()) {
    output << text;
    return output;
  }

  const std::string_view sequence{terminal_sgr_sequence(style)};
  if (sequence.empty()) {
    output << text;
    return output;
  }

  output << sequence << text << terminal_sgr_reset();
  return output;
}

auto terminal_paint(std::ostream &output, TerminalStream stream,
                    std::string_view text, TerminalStyle style)
    -> std::ostream & {
  const bool enabled{terminal_color_enabled(stream)};
  if (enabled && style != TerminalStyle::None && !text.empty()) {
    internal::enable_virtual_terminal_stream(stream);
  }
  return terminal_paint(output, text, style, enabled);
}

} // namespace sourcemeta::core
