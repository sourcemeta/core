#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <optional> // std::nullopt

// The protocol follows the standard syslog severity levels specified in
// RFC 5424
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging#log-levels
TEST(log_level_string_debug) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Debug),
            "debug");
}

TEST(log_level_string_info) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Info),
            "info");
}

TEST(log_level_string_notice) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Notice),
            "notice");
}

TEST(log_level_string_warning) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Warning),
            "warning");
}

TEST(log_level_string_error) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Error),
            "error");
}

TEST(log_level_string_critical) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Critical),
            "critical");
}

TEST(log_level_string_alert) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Alert),
            "alert");
}

TEST(log_level_string_emergency) {
  EXPECT_EQ(sourcemeta::core::mcp_log_level_string(
                sourcemeta::core::MCPLogLevel::Emergency),
            "emergency");
}

TEST(resolve_log_level_debug) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("debug"),
            sourcemeta::core::MCPLogLevel::Debug);
}

TEST(resolve_log_level_info) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("info"),
            sourcemeta::core::MCPLogLevel::Info);
}

TEST(resolve_log_level_notice) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("notice"),
            sourcemeta::core::MCPLogLevel::Notice);
}

TEST(resolve_log_level_warning) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("warning"),
            sourcemeta::core::MCPLogLevel::Warning);
}

TEST(resolve_log_level_error) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("error"),
            sourcemeta::core::MCPLogLevel::Error);
}

TEST(resolve_log_level_critical) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("critical"),
            sourcemeta::core::MCPLogLevel::Critical);
}

TEST(resolve_log_level_alert) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("alert"),
            sourcemeta::core::MCPLogLevel::Alert);
}

TEST(resolve_log_level_emergency) {
  EXPECT_EQ(sourcemeta::core::mcp_resolve_log_level("emergency"),
            sourcemeta::core::MCPLogLevel::Emergency);
}

TEST(resolve_log_level_rejects_an_unknown_level) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_log_level("verbose").has_value());
}

TEST(resolve_log_level_rejects_an_empty_level) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_log_level("").has_value());
}

TEST(resolve_log_level_is_case_sensitive) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_log_level("Info").has_value());
}

TEST(resolve_log_level_rejects_a_padded_level) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_log_level(" info ").has_value());
}

TEST(resolve_log_level_rejects_a_numeric_severity) {
  EXPECT_FALSE(sourcemeta::core::mcp_resolve_log_level("6").has_value());
}

// A request's logging threshold admits its own level and every more severe
// level
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging#per-request-log-level
TEST(log_level_enabled_debug_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_info_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_notice_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_warning_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_error_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_critical_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_alert_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_emergency_with_debug_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Debug));
}

TEST(log_level_enabled_debug_with_info_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_info_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_notice_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_warning_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_error_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_critical_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_alert_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_emergency_with_info_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Info));
}

TEST(log_level_enabled_debug_with_notice_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_info_with_notice_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_notice_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_warning_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_error_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_critical_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_alert_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_emergency_with_notice_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Notice));
}

TEST(log_level_enabled_debug_with_warning_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_info_with_warning_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_notice_with_warning_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_warning_with_warning_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_error_with_warning_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_critical_with_warning_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_alert_with_warning_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_emergency_with_warning_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Warning));
}

TEST(log_level_enabled_debug_with_error_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_info_with_error_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_notice_with_error_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_warning_with_error_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_error_with_error_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_critical_with_error_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_alert_with_error_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_emergency_with_error_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Error));
}

TEST(log_level_enabled_debug_with_critical_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_info_with_critical_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_notice_with_critical_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_warning_with_critical_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_error_with_critical_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_critical_with_critical_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_alert_with_critical_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_emergency_with_critical_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Critical));
}

TEST(log_level_enabled_debug_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_info_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_notice_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_warning_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_error_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_critical_with_alert_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_alert_with_alert_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_emergency_with_alert_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Alert));
}

TEST(log_level_enabled_debug_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Debug,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_info_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Info,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_notice_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Notice,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_warning_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Warning,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_error_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Error,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_critical_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Critical,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_alert_with_emergency_threshold) {
  EXPECT_FALSE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Alert,
      sourcemeta::core::MCPLogLevel::Emergency));
}

TEST(log_level_enabled_emergency_with_emergency_threshold) {
  EXPECT_TRUE(sourcemeta::core::mcp_log_level_enabled(
      sourcemeta::core::MCPLogLevel::Emergency,
      sourcemeta::core::MCPLogLevel::Emergency));
}

// A server may send a notification at or above the level the request asked for,
// and the builder refuses anything below it
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging#per-request-log-level
TEST(make_notification_message_debug_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "debug", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "debug", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_info_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "info", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "info", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_notice_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "notice", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "notice", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_warning_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "warning", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "warning", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_error_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "error", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "error", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_critical_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_debug_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "debug"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "info", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "info", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_notice_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "notice", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "notice", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_warning_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "warning", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "warning", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_error_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "error", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "error", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_critical_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_info_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "notice", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "notice", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_warning_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "warning", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "warning", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_error_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "error", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "error", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_critical_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_notice_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "notice"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "notice", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_warning_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "warning", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "warning", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_error_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "error", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "error", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_critical_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_warning_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "notice", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_warning_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "warning", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_error_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "error", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "error", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_critical_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_error_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "error"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "notice", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_warning_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "warning", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_error_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "error", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_critical_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "critical", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "critical", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_alert_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_critical_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "critical"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "notice", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_warning_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "warning", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_error_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "error", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_critical_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "critical", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_alert_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "alert", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "alert", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_emergency_with_alert_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "alert"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

TEST(make_notification_message_debug_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "debug", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_info_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt, nullptr, &context.second.value()),
                     "Logging notification is below the request threshold");
}

TEST(make_notification_message_notice_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "notice", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_warning_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "warning", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_error_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "error", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_critical_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "critical", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_alert_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "alert", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Logging notification is below the request threshold");
}

TEST(make_notification_message_emergency_with_emergency_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "emergency"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({ "level": "emergency", "data": "hello" })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "emergency", "data": "hello" }
  })JSON"));
}

// An unrecognized log level in a request's metadata is rejected rather than
// treated as a threshold
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging#error-handling
TEST(make_notification_message_rejects_an_unrecognized_threshold) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters).second};
  context->log_level = "verbose";
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "emergency", "data": "hello" })JSON"),
          std::nullopt, nullptr, &context.value()),
      "Logging notification is below the request threshold");
}

TEST(validate_request_parameters_rejects_an_unrecognized_log_level) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "verbose"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(parameters).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidLogLevel);
}

TEST(validate_request_parameters_accepts_a_recognized_log_level) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "warning"
    }
  })JSON")};
  const auto result{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(result.first, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_EQ(result.second->log_level.value(), "warning");
}
