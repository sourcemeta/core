#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// The helper bounds every scenario with an alarm, so a broken pipe-wiring or
// cleanup path fails rather than leaving the suite waiting indefinitely.
static auto check_resources(const std::string_view scenario) -> void {
  const auto result{sourcemeta::core::spawn_and_capture(
      std::string{RESOURCES_BINARY}, {scenario, HELPER_BINARY})};
  EXPECT_TRUE(result.exit_code.has_value());
  EXPECT_EQ(result.exit_code.value(), 0);
  EXPECT_TRUE(result.standard_output.empty());
  EXPECT_TRUE(result.standard_error.empty());
}

TEST(input_is_delivered_when_the_caller_closed_standard_input) {
  check_resources("closed-input");
}

TEST(output_is_captured_when_the_caller_closed_standard_output) {
  check_resources("closed-output");
}

TEST(error_is_captured_when_the_caller_closed_standard_error) {
  check_resources("closed-error");
}

TEST(input_and_both_outputs_work_when_all_standard_descriptors_are_closed) {
  check_resources("closed-all");
}

TEST(a_failed_first_pipe_relocation_closes_both_endpoints) {
  check_resources("exhaust-first");
}

TEST(a_failed_second_pipe_relocation_closes_the_relocated_endpoint) {
  check_resources("exhaust-second");
}

TEST(a_failed_output_pipe_closes_the_previously_created_input_pipe) {
  check_resources("exhaust-output");
}

TEST(a_failed_error_pipe_closes_the_previously_created_pipes) {
  check_resources("exhaust-error");
}

TEST(failed_spawns_release_descriptors_and_leave_no_child_to_reap) {
  check_resources("failed-spawns");
}

TEST(an_unavailable_exit_status_reports_an_error_without_leaking_descriptors) {
  check_resources("ignored-child");
}

TEST(a_pending_broken_pipe_signal_still_belongs_to_the_caller) {
  check_resources("pending-signal");
}

#if defined(__linux__)
TEST(unavailable_process_measurements_are_absent_and_recover) {
  check_resources("telemetry");
}
#endif

TEST(capture_preserves_output_across_acknowledged_signal_interruptions) {
  check_resources("interrupt-capture");
}

TEST(child_waiting_completes_across_acknowledged_signal_interruptions) {
  check_resources("interrupt-wait");
}
