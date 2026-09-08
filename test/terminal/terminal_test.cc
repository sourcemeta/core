#include <sourcemeta/core/terminal.h>
#include <sourcemeta/core/test.h>

#include <array>   // std::array
#include <sstream> // std::ostringstream
#include <string>  // std::string

#if defined(_WIN32)
#include <fcntl.h> // _O_BINARY
#include <io.h>    // _pipe, _close
#else
#include <unistd.h> // pipe, close
#endif

TEST(stream_integer_constants) {
  EXPECT_EQ(static_cast<int>(sourcemeta::core::TerminalStream::Stdin), 0);
  EXPECT_EQ(static_cast<int>(sourcemeta::core::TerminalStream::Stdout), 1);
  EXPECT_EQ(static_cast<int>(sourcemeta::core::TerminalStream::Stderr), 2);
}

TEST(sgr_sequence_none) {
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::None),
            "");
}

TEST(sgr_sequence_bold) {
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold),
            "\033[1m");
}

TEST(sgr_sequence_foreground_colors) {
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Red),
            "\033[31m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Green),
            "\033[32m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Yellow),
            "\033[33m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Blue),
            "\033[34m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Cyan),
            "\033[36m");
}

TEST(sgr_sequence_bold_combined_with_colors) {
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold |
                sourcemeta::core::TerminalStyle::Red),
            "\033[1;31m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold |
                sourcemeta::core::TerminalStyle::Green),
            "\033[1;32m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold |
                sourcemeta::core::TerminalStyle::Yellow),
            "\033[1;33m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold |
                sourcemeta::core::TerminalStyle::Blue),
            "\033[1;34m");
  EXPECT_EQ(sourcemeta::core::terminal_sgr_sequence(
                sourcemeta::core::TerminalStyle::Bold |
                sourcemeta::core::TerminalStyle::Cyan),
            "\033[1;36m");
}

TEST(sgr_reset_sequence) {
  EXPECT_EQ(sourcemeta::core::terminal_sgr_reset(), "\033[0m");
}

TEST(style_validity) {
  EXPECT_TRUE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::None));
  EXPECT_TRUE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::Bold));
  EXPECT_TRUE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::Red));
  EXPECT_TRUE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::Bold |
      sourcemeta::core::TerminalStyle::Red));
  EXPECT_FALSE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::Red |
      sourcemeta::core::TerminalStyle::Green));
  EXPECT_FALSE(sourcemeta::core::terminal_style_is_valid(
      sourcemeta::core::TerminalStyle::Red |
      sourcemeta::core::TerminalStyle::Blue |
      sourcemeta::core::TerminalStyle::Bold));
  EXPECT_FALSE(sourcemeta::core::terminal_style_is_valid(
      static_cast<sourcemeta::core::TerminalStyle>(1 << 7)));
}

TEST(paint_string_when_enabled) {
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "Hello", sourcemeta::core::TerminalStyle::Red, true),
            "\033[31mHello\033[0m");
  const auto bold_green{sourcemeta::core::TerminalStyle::Bold |
                        sourcemeta::core::TerminalStyle::Green};
  EXPECT_EQ(sourcemeta::core::terminal_paint("Success", bold_green, true),
            "\033[1;32mSuccess\033[0m");
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "Info", sourcemeta::core::TerminalStyle::Cyan, true),
            "\033[36mInfo\033[0m");
}

TEST(paint_string_when_disabled) {
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "Hello", sourcemeta::core::TerminalStyle::Red, false),
            "Hello");
  const auto bold_green{sourcemeta::core::TerminalStyle::Bold |
                        sourcemeta::core::TerminalStyle::Green};
  EXPECT_EQ(sourcemeta::core::terminal_paint("Success", bold_green, false),
            "Success");
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "NoneStyle", sourcemeta::core::TerminalStyle::None, true),
            "NoneStyle");
}

TEST(paint_string_default_argument) {
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "DefaultEnabled", sourcemeta::core::TerminalStyle::Red),
            "\033[31mDefaultEnabled\033[0m");
}

TEST(paint_string_empty) {
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "", sourcemeta::core::TerminalStyle::Red, /*enabled=*/true),
            "");
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                "", sourcemeta::core::TerminalStyle::Red, /*enabled=*/false),
            "");
}

TEST(paint_stream_when_enabled) {
  std::ostringstream stream_output;
  sourcemeta::core::terminal_paint(stream_output, "Notice",
                                   sourcemeta::core::TerminalStyle::Red, true);
  EXPECT_EQ(stream_output.str(), "\033[31mNotice\033[0m");

  std::ostringstream bold_stream_output;
  const auto bold_cyan{sourcemeta::core::TerminalStyle::Bold |
                       sourcemeta::core::TerminalStyle::Cyan};
  sourcemeta::core::terminal_paint(bold_stream_output, "Alert", bold_cyan,
                                   true);
  EXPECT_EQ(bold_stream_output.str(), "\033[1;36mAlert\033[0m");
}

TEST(paint_stream_when_disabled) {
  std::ostringstream stream_output;
  sourcemeta::core::terminal_paint(stream_output, "Plain",
                                   sourcemeta::core::TerminalStyle::Red, false);
  EXPECT_EQ(stream_output.str(), "Plain");

  std::ostringstream none_stream_output;
  sourcemeta::core::terminal_paint(none_stream_output, "NoneStyle",
                                   sourcemeta::core::TerminalStyle::None, true);
  EXPECT_EQ(none_stream_output.str(), "NoneStyle");
}

TEST(paint_stream_default_argument) {
  std::ostringstream stream_output;
  sourcemeta::core::terminal_paint(stream_output, "StreamDefault",
                                   sourcemeta::core::TerminalStyle::Red);
  EXPECT_EQ(stream_output.str(), "\033[31mStreamDefault\033[0m");
}

TEST(style_bitwise_or) {
  const auto combined{sourcemeta::core::TerminalStyle::Bold |
                      sourcemeta::core::TerminalStyle::Blue};
  EXPECT_EQ((combined & sourcemeta::core::TerminalStyle::Bold),
            sourcemeta::core::TerminalStyle::Bold);
  EXPECT_EQ((combined & sourcemeta::core::TerminalStyle::Blue),
            sourcemeta::core::TerminalStyle::Blue);
}

TEST(style_bitwise_and) {
  const auto combined{sourcemeta::core::TerminalStyle::Bold |
                      sourcemeta::core::TerminalStyle::Blue};
  EXPECT_EQ((combined & sourcemeta::core::TerminalStyle::Bold),
            sourcemeta::core::TerminalStyle::Bold);
  EXPECT_EQ((combined & sourcemeta::core::TerminalStyle::Red),
            sourcemeta::core::TerminalStyle::None);
}

TEST(style_bitwise_not) {
  const auto inverted{~sourcemeta::core::TerminalStyle::None};
  EXPECT_EQ((inverted & sourcemeta::core::TerminalStyle::Bold),
            sourcemeta::core::TerminalStyle::Bold);
}

TEST(style_bitwise_xor) {
  const auto cleared{sourcemeta::core::TerminalStyle::Bold ^
                     sourcemeta::core::TerminalStyle::Bold};
  EXPECT_EQ(cleared, sourcemeta::core::TerminalStyle::None);

  const auto preserved{sourcemeta::core::TerminalStyle::Bold ^
                       sourcemeta::core::TerminalStyle::None};
  EXPECT_EQ(preserved, sourcemeta::core::TerminalStyle::Bold);
}

TEST(policy_default_argument) {
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(),
            sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stdout));
}

TEST(policy_lifecycle_and_isolation) {
  // Global configuration affects all streams
  sourcemeta::core::terminal_set_color_policy(
      sourcemeta::core::TerminalColorPolicy::WhenInteractive);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stdout),
            sourcemeta::core::TerminalColorPolicy::WhenInteractive);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stderr),
            sourcemeta::core::TerminalColorPolicy::WhenInteractive);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stdin),
            sourcemeta::core::TerminalColorPolicy::WhenInteractive);

  // Per-stream setting isolates changes to target stream
  sourcemeta::core::terminal_set_color_policy(
      sourcemeta::core::TerminalStream::Stderr,
      sourcemeta::core::TerminalColorPolicy::Disabled);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stdout),
            sourcemeta::core::TerminalColorPolicy::WhenInteractive);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stderr),
            sourcemeta::core::TerminalColorPolicy::Disabled);

  // In disabled mode, color is unconditionally suppressed
  EXPECT_EQ(sourcemeta::core::terminal_color_enabled(
                sourcemeta::core::TerminalStream::Stderr),
            false);
  EXPECT_EQ(sourcemeta::core::terminal_paint(
                sourcemeta::core::TerminalStream::Stderr, "error",
                sourcemeta::core::TerminalStyle::Red),
            "error");

  std::ostringstream stderr_stream;
  sourcemeta::core::terminal_paint(
      stderr_stream, sourcemeta::core::TerminalStream::Stderr, "error_stream",
      sourcemeta::core::TerminalStyle::Red);
  EXPECT_EQ(stderr_stream.str(), "error_stream");

  // Restore default policy across all streams
  sourcemeta::core::terminal_set_color_policy(
      sourcemeta::core::TerminalColorPolicy::WhenInteractive);
  EXPECT_EQ(sourcemeta::core::terminal_color_policy(
                sourcemeta::core::TerminalStream::Stderr),
            sourcemeta::core::TerminalColorPolicy::WhenInteractive);
}

TEST(stream_detection_runs_safely) {
  // Exercises stream queries without throws or crashes
  [[maybe_unused]] const bool stdin_is_term{
      sourcemeta::core::terminal_is_interactive(
          sourcemeta::core::TerminalStream::Stdin)};
  [[maybe_unused]] const bool stdout_is_term{
      sourcemeta::core::terminal_is_interactive(
          sourcemeta::core::TerminalStream::Stdout)};
  [[maybe_unused]] const bool stderr_is_term{
      sourcemeta::core::terminal_is_interactive(
          sourcemeta::core::TerminalStream::Stderr)};
}

TEST(non_interactive_file_descriptors) {
  // Invalid file descriptors are never interactive terminals
  EXPECT_EQ(sourcemeta::core::terminal_is_interactive(-1), false);
  EXPECT_EQ(sourcemeta::core::terminal_is_interactive(-100), false);

  // Pipes are not interactive terminals
  std::array<int, 2> pipe_descriptors{{-1, -1}};
#if defined(_WIN32)
  const int pipe_status{_pipe(pipe_descriptors.data(), 256, _O_BINARY)};
#else
  const int pipe_status{::pipe(pipe_descriptors.data())};
#endif
  if (pipe_status == 0) {
    EXPECT_EQ(sourcemeta::core::terminal_is_interactive(pipe_descriptors[0]),
              false);
    EXPECT_EQ(sourcemeta::core::terminal_is_interactive(pipe_descriptors[1]),
              false);
#if defined(_WIN32)
    _close(pipe_descriptors[0]);
    _close(pipe_descriptors[1]);
#else
    ::close(pipe_descriptors[0]);
    ::close(pipe_descriptors[1]);
#endif
  }
}

TEST(composition_with_formatting) {
  const auto bold_red{sourcemeta::core::TerminalStyle::Bold |
                      sourcemeta::core::TerminalStyle::Red};
  const std::string styled{
      sourcemeta::core::terminal_paint("FAILED", bold_red, true)};
  const std::string text_enabled{"Status: " + styled + " [exit code: 1]"};
  EXPECT_EQ(text_enabled, "Status: \033[1;31mFAILED\033[0m [exit code: 1]");

  const std::string plain{
      sourcemeta::core::terminal_paint("FAILED", bold_red, false)};
  const std::string text_disabled{"Status: " + plain + " [exit code: 1]"};
  EXPECT_EQ(text_disabled, "Status: FAILED [exit code: 1]");
}
