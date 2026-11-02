#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <cstddef> // std::size_t
#include <filesystem> // std::filesystem::canonical, std::filesystem::temp_directory_path
#include <map>         // std::map
#include <string>      // std::string
#include <string_view> // std::string_view

// Spawning takes the program by string reference, so the path to the
// helper needs storage of its own
// NOLINTNEXTLINE(cert-err58-cpp,bugprone-throwing-static-initialization)
static const std::string HELPER{HELPER_BINARY};

static auto payload_of(const std::size_t size) -> std::string {
  std::string result;
  result.reserve(size);
  for (std::size_t index = 0; index < size; ++index) {
    result.push_back(static_cast<char>('a' + (index % 26)));
  }

  return result;
}

TEST(standard_input_is_delivered_without_capturing) {
  const sourcemeta::core::ProcessInput input{.standard_input = "the payload"};
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-stdin", "the payload"}, input)};
  EXPECT_EQ(exit_code, 0);
}

TEST(a_mismatched_standard_input_is_visible_to_the_program) {
  const sourcemeta::core::ProcessInput input{.standard_input =
                                                 "something else"};
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-stdin", "the payload"}, input)};
  EXPECT_EQ(exit_code, 1);
}

TEST(an_absent_standard_input_leaves_the_stream_alone) {
  const int exit_code{sourcemeta::core::spawn(HELPER, {"exit", "7"})};
  EXPECT_EQ(exit_code, 7);
}

TEST(a_large_standard_input_is_delivered_without_capturing) {
  const auto payload{payload_of(std::size_t{1024} * 1024)};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-stdin-size", "1048576"}, input)};
  EXPECT_EQ(exit_code, 0);
}

TEST(a_short_standard_input_is_measured_exactly_without_capturing) {
  const sourcemeta::core::ProcessInput input{.standard_input = "12345"};
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-stdin-size", "5"}, input)};
  EXPECT_EQ(exit_code, 0);
}

TEST(a_program_that_never_reads_a_large_input_does_not_kill_the_caller) {
  const auto payload{payload_of(std::size_t{1024} * 1024)};
  const sourcemeta::core::ProcessInput input{.standard_input = payload};
  const int exit_code{sourcemeta::core::spawn(HELPER, {"exit", "3"}, input)};
  EXPECT_EQ(exit_code, 3);
}

TEST(an_explicit_environment_is_delivered_without_capturing) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_ONE", "1"}}};
  const int exit_code{sourcemeta::core::spawn(
      HELPER, {"expect-environment", "SOURCEMETA_ONE", "1"}, input)};
  EXPECT_EQ(exit_code, 0);
}

TEST(an_explicit_environment_with_a_different_value_is_visible) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_ONE", "2"}}};
  const int exit_code{sourcemeta::core::spawn(
      HELPER, {"expect-environment", "SOURCEMETA_ONE", "1"}, input)};
  EXPECT_EQ(exit_code, 1);
}

TEST(the_environment_is_inherited_by_default_without_capturing) {
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-no-environment", "PATH"})};
  EXPECT_EQ(exit_code, 1);
}

TEST(the_working_directory_is_honoured_without_capturing) {
  const auto directory{
      std::filesystem::canonical(std::filesystem::temp_directory_path())};
  const sourcemeta::core::ProcessInput input{.directory = directory};
  const int exit_code{sourcemeta::core::spawn(
      HELPER, {"expect-directory", directory.string()}, input)};
  EXPECT_EQ(exit_code, 0);
}

TEST(a_program_reports_a_working_directory_that_does_not_match) {
  const auto directory{
      std::filesystem::canonical(std::filesystem::temp_directory_path())};
  const sourcemeta::core::ProcessInput input{.directory = directory};
  const int exit_code{
      sourcemeta::core::spawn(HELPER, {"expect-directory", "/nowhere"}, input)};
  EXPECT_EQ(exit_code, 1);
}

TEST(the_environment_and_the_standard_input_combine) {
  const sourcemeta::core::ProcessInput input{
      .environment =
          std::map<std::string_view, std::string_view>{{"SOURCEMETA_ONE", "1"}},
      .standard_input = "ignored"};
  const int exit_code{sourcemeta::core::spawn(
      HELPER, {"expect-environment", "SOURCEMETA_ONE", "1"}, input)};
  EXPECT_EQ(exit_code, 0);
}

#if !defined(_WIN32)

TEST(an_explicit_environment_replaces_rather_than_extends_without_capturing) {
  const sourcemeta::core::ProcessInput input{
      .environment = std::map<std::string_view, std::string_view>{
          {"SOURCEMETA_ONE", "1"}}};
  const int exit_code{sourcemeta::core::spawn(
      HELPER, {"expect-no-environment", "PATH"}, input)};
  EXPECT_EQ(exit_code, 0);
}

#endif
