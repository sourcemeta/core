#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include <stdexcept>

namespace {
using namespace sourcemeta::core;

auto log_notification(const MCPLogLevel threshold, const MCPLogLevel level)
    -> JSON {
  const auto parameters{parse_json(R"({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })")};
  auto context{mcp_validate_request_parameters(parameters).second.value()};
  context.log_level = mcp_log_level_string(threshold);
  auto payload{parse_json(R"({"data":"hello"})")};
  payload.assign("level", JSON{mcp_log_level_string(level)});
  return mcp_make_notification(MCPProtocolVersion::V_2026_07_28, true,
                               "notifications/message", payload, std::nullopt,
                               nullptr, &context);
}
} // namespace

TEST(log_level_roundtrip_debug) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Debug)),
            MCPLogLevel::Debug);
}

TEST(log_level_roundtrip_info) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Info)),
            MCPLogLevel::Info);
}

TEST(log_level_roundtrip_notice) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Notice)),
            MCPLogLevel::Notice);
}

TEST(log_level_roundtrip_warning) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Warning)),
            MCPLogLevel::Warning);
}

TEST(log_level_roundtrip_error) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Error)),
            MCPLogLevel::Error);
}

TEST(log_level_roundtrip_critical) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Critical)),
            MCPLogLevel::Critical);
}

TEST(log_level_roundtrip_alert) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Alert)),
            MCPLogLevel::Alert);
}

TEST(log_level_roundtrip_emergency) {
  EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(MCPLogLevel::Emergency)),
            MCPLogLevel::Emergency);
}

TEST(logging_debug_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Debug)));
}

TEST(logging_info_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Info)));
}

TEST(logging_notice_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Notice)));
}

TEST(logging_warning_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Warning)));
}

TEST(logging_error_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Error)));
}

TEST(logging_critical_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_debug_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Debug, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_info_threshold) {
  try {
    log_notification(MCPLogLevel::Info, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Info)));
}

TEST(logging_notice_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Notice)));
}

TEST(logging_warning_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Warning)));
}

TEST(logging_error_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Error)));
}

TEST(logging_critical_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_info_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Info, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_notice_threshold) {
  try {
    log_notification(MCPLogLevel::Notice, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_notice_threshold) {
  try {
    log_notification(MCPLogLevel::Notice, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Notice)));
}

TEST(logging_warning_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Warning)));
}

TEST(logging_error_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Error)));
}

TEST(logging_critical_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_notice_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Notice, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_warning_threshold) {
  try {
    log_notification(MCPLogLevel::Warning, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_warning_threshold) {
  try {
    log_notification(MCPLogLevel::Warning, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_warning_threshold) {
  try {
    log_notification(MCPLogLevel::Warning, MCPLogLevel::Notice);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_warning_with_warning_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Warning, MCPLogLevel::Warning)));
}

TEST(logging_error_with_warning_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Warning, MCPLogLevel::Error)));
}

TEST(logging_critical_with_warning_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Warning, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_warning_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Warning, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_warning_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Warning, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_error_threshold) {
  try {
    log_notification(MCPLogLevel::Error, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_error_threshold) {
  try {
    log_notification(MCPLogLevel::Error, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_error_threshold) {
  try {
    log_notification(MCPLogLevel::Error, MCPLogLevel::Notice);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_warning_with_error_threshold) {
  try {
    log_notification(MCPLogLevel::Error, MCPLogLevel::Warning);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_error_with_error_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Error, MCPLogLevel::Error)));
}

TEST(logging_critical_with_error_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Error, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_error_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Error, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_error_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Error, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_critical_threshold) {
  try {
    log_notification(MCPLogLevel::Critical, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_critical_threshold) {
  try {
    log_notification(MCPLogLevel::Critical, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_critical_threshold) {
  try {
    log_notification(MCPLogLevel::Critical, MCPLogLevel::Notice);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_warning_with_critical_threshold) {
  try {
    log_notification(MCPLogLevel::Critical, MCPLogLevel::Warning);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_error_with_critical_threshold) {
  try {
    log_notification(MCPLogLevel::Critical, MCPLogLevel::Error);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_critical_with_critical_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Critical, MCPLogLevel::Critical)));
}

TEST(logging_alert_with_critical_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Critical, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_critical_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Critical, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Notice);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_warning_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Warning);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_error_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Error);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_critical_with_alert_threshold) {
  try {
    log_notification(MCPLogLevel::Alert, MCPLogLevel::Critical);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_alert_with_alert_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Alert, MCPLogLevel::Alert)));
}

TEST(logging_emergency_with_alert_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Alert, MCPLogLevel::Emergency)));
}

TEST(logging_debug_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Debug);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_info_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Info);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_notice_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Notice);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_warning_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Warning);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_error_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Error);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_critical_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Critical);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_alert_with_emergency_threshold) {
  try {
    log_notification(MCPLogLevel::Emergency, MCPLogLevel::Alert);
    FAIL();
  } catch (const std::invalid_argument &) {
    // A level below the threshold is expected to be rejected
  }
}

TEST(logging_emergency_with_emergency_threshold) {
  EXPECT_TRUE(jsonrpc_is_notification(
      log_notification(MCPLogLevel::Emergency, MCPLogLevel::Emergency)));
}

TEST(logging_rejects_invalid_threshold) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  auto parameters{parse_json(
      R"({
  "_meta": {
    "io.modelcontextprotocol/protocolVersion": "2026-07-28",
    "io.modelcontextprotocol/clientCapabilities": {},
    "io.modelcontextprotocol/logLevel": "warning"
  }
})")};
  const auto context{mcp_validate_request_parameters(parameters).second};
  EXPECT_TRUE(context.has_value());

  auto bad_context{*context};
  bad_context.log_level = "verbose";
  try {
    mcp_make_notification(VERSION, true, "notifications/message",
                          parse_json(R"({"level":"emergency","data":"hello"})"),
                          std::nullopt, nullptr, &bad_context);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(logging_rejects_subscription_scope) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  auto parameters{parse_json(
      R"({
  "_meta": {
    "io.modelcontextprotocol/protocolVersion": "2026-07-28",
    "io.modelcontextprotocol/clientCapabilities": {},
    "io.modelcontextprotocol/logLevel": "warning"
  }
})")};
  const auto context{mcp_validate_request_parameters(parameters).second};
  EXPECT_TRUE(context.has_value());

  auto bad_context{*context};
  try {
    mcp_make_notification(VERSION, true, "notifications/message",
                          parse_json(R"({"level":"emergency","data":"hello"})"),
                          std::optional<JSON>{JSON{1}}, nullptr, &bad_context);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}
