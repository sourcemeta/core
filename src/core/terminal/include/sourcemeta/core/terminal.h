#ifndef SOURCEMETA_CORE_TERMINAL_H_
#define SOURCEMETA_CORE_TERMINAL_H_

#ifndef SOURCEMETA_CORE_TERMINAL_EXPORT
#include <sourcemeta/core/terminal_export.h>
#endif

#include <cstdint>     // std::uint8_t
#include <iosfwd>      // std::ostream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <type_traits> // std::underlying_type_t

/// @defgroup terminal Terminal
/// @brief Terminal detection, coloring policy, and ANSI styling utilities
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/terminal.h>
/// ```

namespace sourcemeta::core {

/// @ingroup terminal
///
/// Standard I/O streams defined by POSIX.1-2017 (<unistd.h>).
///
/// @see https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/unistd.h.html
enum class TerminalStream : std::uint8_t {
  /// Standard input stream (POSIX.1-2017 STDIN_FILENO, 0).
  /// @see https://pubs.opengroup.org/onlinepubs/9699919799/functions/stdin.html
  Stdin,
  /// Standard output stream (POSIX.1-2017 STDOUT_FILENO, 1).
  /// @see
  /// https://pubs.opengroup.org/onlinepubs/9699919799/functions/stdout.html
  Stdout,
  /// Standard error stream (POSIX.1-2017 STDERR_FILENO, 2).
  /// @see
  /// https://pubs.opengroup.org/onlinepubs/9699919799/functions/stderr.html
  Stderr
};

/// @ingroup terminal
///
/// Color policy governing ANSI styling output.
enum class TerminalColorPolicy : std::uint8_t {
  /// Color is applied when the destination stream is connected to an
  /// interactive terminal device.
  ///
  /// @see
  /// https://pubs.opengroup.org/onlinepubs/9699919799/functions/isatty.html
  WhenInteractive,
  /// Styling is unconditionally suppressed.
  Disabled
};

/// @ingroup terminal
///
/// Text styles and foreground colors defined by ECMA-48 (5th Edition, 1991),
/// Section 8.3.117 "SGR - SELECT GRAPHIC RENDITION".
///
/// Bitwise operators allow combining `TerminalStyle::Bold` with a foreground
/// color (for example `TerminalStyle::Bold | TerminalStyle::Red`). Only one
/// foreground color may be active at a time. If multiple foreground colors are
/// combined, the first matching color in declaration order (Red > Green >
/// Yellow > Blue > Cyan) takes precedence.
///
/// @see
/// https://ecma-international.org/publications-and-standards/standards/ecma-48/
enum class TerminalStyle : std::uint8_t {
  /// Normal display / no style (plain text).
  None = 0,
  /// Bold or increased intensity (ECMA-48 SGR parameter 1).
  Bold = 1 << 0,
  /// Red foreground color (ECMA-48 SGR parameter 31).
  Red = 1 << 1,
  /// Green foreground color (ECMA-48 SGR parameter 32).
  Green = 1 << 2,
  /// Yellow foreground color (ECMA-48 SGR parameter 33).
  Yellow = 1 << 3,
  /// Blue foreground color (ECMA-48 SGR parameter 34).
  Blue = 1 << 4,
  /// Cyan foreground color (ECMA-48 SGR parameter 36).
  Cyan = 1 << 5
};

/// @ingroup terminal
///
/// Combine two styles using bitwise OR.
[[nodiscard]] constexpr auto operator|(TerminalStyle lhs,
                                       TerminalStyle rhs) noexcept
    -> TerminalStyle {
  using Underlying = std::underlying_type_t<TerminalStyle>;
  return static_cast<TerminalStyle>(static_cast<Underlying>(lhs) |
                                    static_cast<Underlying>(rhs));
}

/// @ingroup terminal
///
/// Intersect two styles using bitwise AND.
[[nodiscard]] constexpr auto operator&(TerminalStyle lhs,
                                       TerminalStyle rhs) noexcept
    -> TerminalStyle {
  using Underlying = std::underlying_type_t<TerminalStyle>;
  return static_cast<TerminalStyle>(static_cast<Underlying>(lhs) &
                                    static_cast<Underlying>(rhs));
}

/// @ingroup terminal
///
/// Invert a style using bitwise NOT.
[[nodiscard]] constexpr auto operator~(TerminalStyle val) noexcept
    -> TerminalStyle {
  using Underlying = std::underlying_type_t<TerminalStyle>;
  return static_cast<TerminalStyle>(~static_cast<Underlying>(val));
}

/// @ingroup terminal
///
/// XOR two styles.
[[nodiscard]] constexpr auto operator^(TerminalStyle lhs,
                                       TerminalStyle rhs) noexcept
    -> TerminalStyle {
  using Underlying = std::underlying_type_t<TerminalStyle>;
  return static_cast<TerminalStyle>(static_cast<Underlying>(lhs) ^
                                    static_cast<Underlying>(rhs));
}

/// @ingroup terminal
///
/// Check whether the specified stream is connected to an interactive terminal.
///
/// @see https://pubs.opengroup.org/onlinepubs/9699919799/functions/isatty.html
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_is_interactive(TerminalStream stream) noexcept -> bool;

/// @ingroup terminal
///
/// Check whether the specified file descriptor is connected to an interactive
/// terminal.
///
/// @see https://pubs.opengroup.org/onlinepubs/9699919799/functions/isatty.html
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_is_interactive(int file_descriptor) noexcept -> bool;

/// @ingroup terminal
///
/// Set the global color policy across all streams.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_set_color_policy(TerminalColorPolicy policy) noexcept -> void;

/// @ingroup terminal
///
/// Set the color policy for a specific stream.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_set_color_policy(TerminalStream stream,
                               TerminalColorPolicy policy) noexcept -> void;

/// @ingroup terminal
///
/// Retrieve the current color policy for the specified stream.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_color_policy(
    TerminalStream stream = TerminalStream::Stdout) noexcept
    -> TerminalColorPolicy;

/// @ingroup terminal
///
/// Determine whether styling is enabled for the specified stream.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_color_enabled(TerminalStream stream) noexcept -> bool;

/// @ingroup terminal
///
/// Return the ECMA-48 Select Graphic Rendition (SGR) control sequence for the
/// given style.
///
/// Returns an empty string view if `style` is `TerminalStyle::None`.
///
/// @see
/// https://ecma-international.org/publications-and-standards/standards/ecma-48/
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_sgr_sequence(TerminalStyle style) noexcept -> std::string_view;

/// @ingroup terminal
///
/// Return the ECMA-48 Select Graphic Rendition (SGR) reset control sequence
/// (`"\033[0m"`).
///
/// @see
/// https://ecma-international.org/publications-and-standards/standards/ecma-48/
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_sgr_reset() noexcept -> std::string_view;

/// @ingroup terminal
///
/// Wrap text in ANSI SGR sequences for the given style if `enabled` is `true`.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_paint(std::string_view text, TerminalStyle style,
                    bool enabled = true) -> std::string;

/// @ingroup terminal
///
/// Wrap text in ANSI SGR sequences for the given stream destination.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_paint(TerminalStream stream, std::string_view text,
                    TerminalStyle style) -> std::string;

/// @ingroup terminal
///
/// Stream styled text to an output stream without heap allocation.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_paint(std::ostream &output, std::string_view text,
                    TerminalStyle style, bool enabled = true) -> std::ostream &;

/// @ingroup terminal
///
/// Stream styled text to an output stream for the given stream destination.
SOURCEMETA_CORE_TERMINAL_EXPORT
auto terminal_paint(std::ostream &output, TerminalStream stream,
                    std::string_view text, TerminalStyle style)
    -> std::ostream &;

} // namespace sourcemeta::core

#endif
