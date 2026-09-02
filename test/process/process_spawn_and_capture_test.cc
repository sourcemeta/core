#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <map>         // std::map
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::thread
#include <vector>      // std::vector

// Spawning takes the program by string reference, so the path to the
// helper needs storage of its own
// NOLINTNEXTLINE(cert-err58-cpp,bugprone-throwing-static-initialization)
static const std::string HELPER{HELPER_BINARY};

// Enough to overrun the pipe buffer of every platform this builds on, so that a
// transfer that does not interleave reading and writing cannot complete
static constexpr std::size_t FLOOD_SIZE{std::size_t{1024} * 1024};

// Mirrors what the helper program writes, so that a truncated or reordered
// transfer cannot pass by accident
static auto payload_of(const std::size_t size) -> std::string {
  std::string result;
  result.reserve(size);
  for (std::size_t index = 0; index < size; ++index) {
    result.push_back(static_cast<char>('a' + (index % 26)));
  }

  return result;
}

TEST(exit_code_zero_is_reported) {
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, {"exit", "0"})};
  EXPECT_TRUE(result.exit_code.has_value());
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "");
  EXPECT_EQ(result.standard_error, "");
}

TEST(exit_code_non_zero_is_reported) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"exit", "42"})};
  EXPECT_TRUE(result.exit_code.has_value());
  EXPECT_EQ(result.exit_code.value(), 42);
}

TEST(exit_code_one_is_reported) {
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, {"exit", "1"})};
  EXPECT_TRUE(result.exit_code.has_value());
  EXPECT_EQ(result.exit_code.value(), 1);
}

TEST(standard_output_is_captured) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"stdout", "hello"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "hello");
  EXPECT_EQ(result.standard_error, "");
}

TEST(standard_error_is_captured) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"stderr", "problem"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "");
  EXPECT_EQ(result.standard_error, "problem");
}

TEST(both_streams_are_captured_separately) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"both", "out", "err"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "out");
  EXPECT_EQ(result.standard_error, "err");
}

TEST(a_silent_program_captures_nothing) {
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, {"silent"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "");
  EXPECT_EQ(result.standard_error, "");
  EXPECT_TRUE(result.standard_output.empty());
  EXPECT_TRUE(result.standard_error.empty());
}

TEST(output_without_a_trailing_newline_is_preserved) {
  const auto result{sourcemeta::core::spawn_and_capture(
      HELPER, {"stdout", "no newline here"})};
  EXPECT_EQ(result.standard_output, "no newline here");
  EXPECT_EQ(result.standard_output.size(), 15);
}

TEST(line_endings_in_output_are_not_rewritten) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"stdout", "one\r\ntwo\n"})};
  EXPECT_EQ(result.standard_output, "one\r\ntwo\n");
  EXPECT_EQ(result.standard_output.size(), 9);
}

TEST(arguments_are_passed_in_order) {
  const auto result{sourcemeta::core::spawn_and_capture(
      HELPER, {"arguments", "alpha", "beta", "gamma"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "alpha\nbeta\ngamma\n");
}

TEST(an_argument_with_spaces_stays_a_single_argument) {
  const auto result{sourcemeta::core::spawn_and_capture(
      HELPER, {"arguments", "alpha beta gamma"})};
  EXPECT_EQ(result.standard_output, "alpha beta gamma\n");
}

TEST(an_argument_with_quotes_is_preserved) {
  const auto result{sourcemeta::core::spawn_and_capture(
      HELPER, {"arguments", "say \"hello\"", "back\\slash"})};
  EXPECT_EQ(result.standard_output, "say \"hello\"\nback\\slash\n");
}

TEST(an_empty_argument_is_preserved) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"arguments", "", "after"})};
  EXPECT_EQ(result.standard_output, "\nafter\n");
}

TEST(the_span_overload_accepts_a_dynamic_argument_list) {
  const std::vector<std::string_view> arguments{"arguments", "one", "two"};
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, arguments)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "one\ntwo\n");
}

TEST(standard_input_is_delivered) {
  const sourcemeta::core::ProcessInput input{.standard_input = "hello input"};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "hello input");
}

TEST(an_absent_standard_input_is_an_immediate_end_of_input) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"count-stdin"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "0");
}

TEST(an_empty_standard_input_is_an_immediate_end_of_input) {
  const sourcemeta::core::ProcessInput input{.standard_input = ""};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"count-stdin"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "0");
}

TEST(standard_input_carrying_null_bytes_round_trips) {
  const std::string payload{std::string{"before"} + std::string(1, '\0') +
                            std::string{"after"}};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output.size(), 12);
  EXPECT_EQ(result.standard_output, payload);
}

TEST(a_large_standard_input_round_trips) {
  const auto payload{payload_of(FLOOD_SIZE)};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output.size(), FLOOD_SIZE);
  EXPECT_EQ(result.standard_output, payload);
}

TEST(a_large_standard_error_does_not_deadlock_a_large_standard_input) {
  const auto payload{payload_of(FLOOD_SIZE)};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const auto result{sourcemeta::core::spawn_and_capture(
      HELPER, {"flood-stderr-then-cat", "1048576"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_error.size(), FLOOD_SIZE);
  EXPECT_EQ(result.standard_error, payload);
  EXPECT_EQ(result.standard_output.size(), FLOOD_SIZE);
  EXPECT_EQ(result.standard_output, payload);
}

TEST(both_streams_flooding_at_once_does_not_deadlock) {
  const auto payload{payload_of(FLOOD_SIZE)};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"flood-both", "1048576"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output.size(), FLOOD_SIZE);
  EXPECT_EQ(result.standard_output, payload);
  EXPECT_EQ(result.standard_error.size(), FLOOD_SIZE);
  EXPECT_EQ(result.standard_error, payload);
}

TEST(a_program_that_never_reads_its_input_does_not_kill_the_caller) {
  const auto payload{payload_of(FLOOD_SIZE)};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"ignore-stdin"}, input)};
  EXPECT_TRUE(result.exit_code.has_value());
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "done");
  EXPECT_EQ(result.standard_error, "");
}

TEST(the_environment_is_inherited_by_default) {
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"environment"})};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_FALSE(result.standard_output.empty());
}

TEST(an_explicit_environment_is_delivered) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_ALPHA", "one"}, {"SOURCEMETA_BETA", "two"}}};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"environment"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_TRUE(result.standard_output.find("SOURCEMETA_ALPHA=one\n") !=
              std::string::npos);
  EXPECT_TRUE(result.standard_output.find("SOURCEMETA_BETA=two\n") !=
              std::string::npos);
}

TEST(an_explicit_environment_with_an_empty_value_is_delivered) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_EMPTY", ""}}};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"environment"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_TRUE(result.standard_output.find("SOURCEMETA_EMPTY=\n") !=
              std::string::npos);
}

TEST(the_working_directory_is_honoured) {
  const auto directory{
      std::filesystem::canonical(std::filesystem::temp_directory_path())};
  const sourcemeta::core::ProcessInput input{.directory = directory};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"directory"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, directory.string());
}

// Every descriptor the call opens is marked close-on-exec, so a program started
// on one thread never inherits the pipes of a program started on another. A
// leaked write end would keep a stream from ever reaching its end
TEST(concurrent_calls_do_not_leak_streams_into_each_other) {
  const auto first_payload{payload_of(FLOOD_SIZE)};
  const std::string second_payload{"second"};
  const std::string third_payload{"third"};
  sourcemeta::core::ProcessOutput first;
  sourcemeta::core::ProcessOutput second;
  sourcemeta::core::ProcessOutput third;

  std::thread first_thread{[&first, &first_payload] {
    const sourcemeta::core::ProcessInput input{.standard_input = first_payload};
    first = sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input);
  }};
  std::thread second_thread{[&second, &second_payload] {
    const sourcemeta::core::ProcessInput input{.standard_input =
                                                   second_payload};
    second = sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input);
  }};
  std::thread third_thread{[&third, &third_payload] {
    const sourcemeta::core::ProcessInput input{.standard_input = third_payload};
    third = sourcemeta::core::spawn_and_capture(HELPER, {"cat"}, input);
  }};

  first_thread.join();
  second_thread.join();
  third_thread.join();

  EXPECT_EQ(first.exit_code.value(), 0);
  EXPECT_EQ(first.standard_output.size(), FLOOD_SIZE);
  EXPECT_EQ(first.standard_output, first_payload);
  EXPECT_EQ(second.exit_code.value(), 0);
  EXPECT_EQ(second.standard_output, second_payload);
  EXPECT_EQ(third.exit_code.value(), 0);
  EXPECT_EQ(third.standard_output, third_payload);
}

TEST(a_missing_program_reports_the_program_it_could_not_find) {
  const std::string program{"this_program_definitely_does_not_exist"};
  try {
    const auto result{sourcemeta::core::spawn_and_capture(program, {})};
    FAIL();
  } catch (const sourcemeta::core::ProcessProgramNotFoundError &error) {
    EXPECT_EQ(error.program(), program);
  }
}

#if !defined(_WIN32)

TEST(a_program_killed_by_a_signal_reports_no_exit_code) {
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, {"terminate"})};
  EXPECT_FALSE(result.exit_code.has_value());
}

TEST(a_program_killed_by_a_signal_still_yields_its_output) {
  const auto result{sourcemeta::core::spawn_and_capture(HELPER, {"terminate"})};
  EXPECT_EQ(result.standard_output, "partial");
  EXPECT_EQ(result.standard_error, "");
}

TEST(an_explicit_environment_replaces_rather_than_extends) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_ALPHA", "one"}, {"SOURCEMETA_BETA", "two"}}};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"environment"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output,
            "SOURCEMETA_ALPHA=one\nSOURCEMETA_BETA=two\n");
}

TEST(an_empty_explicit_environment_isolates_completely) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{}};
  const auto result{
      sourcemeta::core::spawn_and_capture(HELPER, {"environment"}, input)};
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_EQ(result.standard_output, "");
}

#endif
