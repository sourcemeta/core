#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>

#include <sourcemeta/core/test.h>

#include <array>
#include <stdexcept>
#include <vector>

#include <cstddef>  // std::size_t
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <optional> // std::optional, std::nullopt
#include <sstream>
#include <string>
#include <utility> // std::move

namespace {
using namespace sourcemeta::core;

constexpr auto CURRENT{MCPProtocolVersion::V_2026_07_28};

auto parameters() -> JSON {
  return parse_json(
      R"({
  "_meta": {
    "io.modelcontextprotocol/protocolVersion": "2026-07-28",
    "io.modelcontextprotocol/clientCapabilities": {}
  }
})");
}

} // namespace

TEST(method_notifications_initialized) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_NOTIFICATIONS_INITIALIZED,
            "notifications/initialized");
}

TEST(is_request_method_notifications_initialized_is_not_request) {
  EXPECT_FALSE(sourcemeta::core::mcp_is_request_method(
      sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
      "notifications/initialized"));
}

TEST(protocol_era_predicates_supports_subscriptions_listen) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_03_26));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_06_18));
  EXPECT_FALSE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2025_11_25));
  EXPECT_TRUE(
      sourcemeta::core::mcp_supports_subscriptions_listen(version_2026_07_28));
}

TEST(method_subscriptions_listen) {
  EXPECT_EQ(sourcemeta::core::MCP_METHOD_SUBSCRIPTIONS_LISTEN,
            "subscriptions/listen");
}

TEST(method_notifications_subscriptions_acknowledged) {
  EXPECT_EQ(
      sourcemeta::core::MCP_METHOD_NOTIFICATIONS_SUBSCRIPTIONS_ACKNOWLEDGED,
      "notifications/subscriptions/acknowledged");
}

TEST(subscriptions_acknowledged_notification) {
  const sourcemeta::core::MCPSubscriptionFilter notifications{
      .tools_list_changed = true};

  const auto envelope{
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "sub-42",
          notifications, notifications)};

  EXPECT_EQ(envelope.at("jsonrpc").to_string(), "2.0");
  EXPECT_EQ(envelope.at("method").to_string(),
            "notifications/subscriptions/acknowledged");
  EXPECT_EQ(envelope.at("params")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_string(),
            "sub-42");
  EXPECT_TRUE(envelope.at("params")
                  .at("notifications")
                  .at("toolsListChanged")
                  .to_boolean());

  // Also test integer subscription ID
  const sourcemeta::core::MCPSubscriptionFilter notifications_integer;
  const auto envelope_integer{
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{123}, notifications_integer,
          notifications_integer)};
  EXPECT_EQ(envelope_integer.at("params")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_integer(),
            123);
}

TEST(subscriptions_close_result) {
  const auto identifier{sourcemeta::core::JSON{60}};
  const auto envelope{sourcemeta::core::mcp_make_subscription_close_result(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier)};

  EXPECT_EQ(envelope.at("result").at("resultType").to_string(), "complete");
  EXPECT_EQ(envelope.at("result")
                .at("_meta")
                .at("io.modelcontextprotocol/subscriptionId")
                .to_integer(),
            60);
}

TEST(subscriptions_id_accessor) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *sub_id{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification)};
  EXPECT_NE(sub_id, nullptr);
  EXPECT_EQ(sub_id->to_string(), "stream-99");

  const auto close_result{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *close_sub_id{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, close_result)};
  EXPECT_NE(close_sub_id, nullptr);
  EXPECT_EQ(close_sub_id->to_string(), "stream-99");

  // Number subscription ID
  const auto num_envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": 42
      }
    }
  })JSON")};
  const auto *num_sub_id{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, num_envelope)};
  EXPECT_NE(num_sub_id, nullptr);
  EXPECT_EQ(num_sub_id->to_integer(), 42);
}

TEST(subscription_filters_and_notifications) {
  auto value{parameters()};
  value.assign("notifications", parse_json(
                                    R"({
  "toolsListChanged": true,
  "resourcesListChanged": false,
  "resourceSubscriptions": [
    "a",
    "b"
  ]
})"));
  const auto requested{mcp_parse_subscription_filter(CURRENT, value)};
  EXPECT_TRUE(requested.has_value());
  MCPSubscriptionFilter supported;
  supported.tools_list_changed = true;
  supported.prompts_list_changed = true;
  supported.resource_subscriptions = std::vector<JSON::StringView>{"b", "c"};
  const auto allowed{mcp_intersect_subscription_filters(*requested, supported)};
  EXPECT_TRUE(allowed.tools_list_changed.value_or(false));
  EXPECT_FALSE(allowed.prompts_list_changed.has_value());
  EXPECT_EQ(allowed.resource_subscriptions->size(), 1);
  EXPECT_EQ(allowed.resource_subscriptions->front(), "b");
  EXPECT_FALSE(
      mcp_parse_subscription_filter(MCPProtocolVersion::V_2025_11_25, value)
          .has_value());
  auto malformed{value};
  malformed.at("notifications").assign("toolsListChanged", JSON{1});
  EXPECT_FALSE(mcp_parse_subscription_filter(CURRENT, malformed).has_value());
  malformed = value;
  malformed.at("notifications")
      .assign("resourceSubscriptions", parse_json("[false]"));
  EXPECT_FALSE(mcp_parse_subscription_filter(CURRENT, malformed).has_value());
  const auto ack{mcp_make_subscription_acknowledged_notification(
      CURRENT, JSON{0}, *requested, supported)};
  EXPECT_TRUE(ack.is_object());
  EXPECT_FALSE(ack.defines("id"));
  const auto update{
      mcp_make_notification(CURRENT, true, "notifications/resources/updated",
                            parse_json(R"({"uri":"b"})"), JSON{0}, &allowed)};
  EXPECT_TRUE(update.is_object());
  EXPECT_EQ(*mcp_request_subscription_id(CURRENT, update), JSON{0});
  try {
    mcp_make_notification(CURRENT, true, "notifications/resources/updated",
                          parse_json(R"({"uri":"a"})"), JSON{0}, &allowed);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(CURRENT, true, "notifications/tools/list_changed",
                          JSON::make_object(), std::nullopt, &allowed);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(CURRENT, true, "notifications/prompts/list_changed",
                          JSON::make_object(), JSON{0}, &allowed);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(CURRENT, true, "notifications/cancelled",
                          parse_json(R"({"requestId":1})"), JSON{0});
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(CURRENT, true, "notifications/roots/list_changed",
                          JSON::make_object(), std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(notification_shapes_2025_03_26) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};

  const auto version{MCPProtocolVersion::V_2025_03_26};

  const auto log{mcp_make_notification(
      version, true, "notifications/message",
      parse_json(R"({"level":"info","data":{"message":"ok"}})"), std::nullopt,
      nullptr, &*context.second)};
  EXPECT_TRUE(log.is_object());
  const auto progress{mcp_make_notification(
      version, true, "notifications/progress",
      parse_json(
          R"({"progressToken":0,"progress":1,"total":2,"message":"working"})"),
      std::nullopt, nullptr, &*context.second)};
  EXPECT_TRUE(progress.is_object());
  const auto cancel{mcp_make_notification(
      version, false, "notifications/cancelled",
      parse_json(R"({"requestId":0,"reason":"done"})"), std::nullopt)};
  EXPECT_TRUE(cancel.is_object());
  try {
    mcp_make_notification(version, true, "notifications/message",
                          parse_json(R"({"level":"invalid","data":null})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(version, true, "notifications/progress",
                          parse_json(R"({"progressToken":false,"progress":1})"),
                          std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(notification_shapes_2025_06_18) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};

  const auto version{MCPProtocolVersion::V_2025_06_18};

  const auto log{mcp_make_notification(
      version, true, "notifications/message",
      parse_json(R"({"level":"info","data":{"message":"ok"}})"), std::nullopt,
      nullptr, &*context.second)};
  EXPECT_TRUE(log.is_object());
  const auto progress{mcp_make_notification(
      version, true, "notifications/progress",
      parse_json(
          R"({"progressToken":0,"progress":1,"total":2,"message":"working"})"),
      std::nullopt, nullptr, &*context.second)};
  EXPECT_TRUE(progress.is_object());
  const auto cancel{mcp_make_notification(
      version, false, "notifications/cancelled",
      parse_json(R"({"requestId":0,"reason":"done"})"), std::nullopt)};
  EXPECT_TRUE(cancel.is_object());
  try {
    mcp_make_notification(version, true, "notifications/message",
                          parse_json(R"({"level":"invalid","data":null})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(version, true, "notifications/progress",
                          parse_json(R"({"progressToken":false,"progress":1})"),
                          std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(notification_shapes_2025_11_25) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};

  const auto version{MCPProtocolVersion::V_2025_11_25};

  const auto log{mcp_make_notification(
      version, true, "notifications/message",
      parse_json(R"({"level":"info","data":{"message":"ok"}})"), std::nullopt,
      nullptr, &*context.second)};
  EXPECT_TRUE(log.is_object());
  const auto progress{mcp_make_notification(
      version, true, "notifications/progress",
      parse_json(
          R"({"progressToken":0,"progress":1,"total":2,"message":"working"})"),
      std::nullopt, nullptr, &*context.second)};
  EXPECT_TRUE(progress.is_object());
  const auto cancel{mcp_make_notification(
      version, false, "notifications/cancelled",
      parse_json(R"({"requestId":0,"reason":"done"})"), std::nullopt)};
  EXPECT_TRUE(cancel.is_object());
  try {
    mcp_make_notification(version, true, "notifications/message",
                          parse_json(R"({"level":"invalid","data":null})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(version, true, "notifications/progress",
                          parse_json(R"({"progressToken":false,"progress":1})"),
                          std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(notification_shapes_2026_07_28) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};

  const auto version{MCPProtocolVersion::V_2026_07_28};

  const auto log{mcp_make_notification(
      version, true, "notifications/message",
      parse_json(R"({"level":"info","data":{"message":"ok"}})"), std::nullopt,
      nullptr, &*context.second)};
  EXPECT_TRUE(log.is_object());
  const auto progress{mcp_make_notification(
      version, true, "notifications/progress",
      parse_json(
          R"({"progressToken":0,"progress":1,"total":2,"message":"working"})"),
      std::nullopt, nullptr, &*context.second)};
  EXPECT_TRUE(progress.is_object());
  const auto cancel{mcp_make_notification(
      version, false, "notifications/cancelled",
      parse_json(R"({"requestId":0,"reason":"done"})"), std::nullopt)};
  EXPECT_TRUE(cancel.is_object());
  try {
    mcp_make_notification(version, true, "notifications/message",
                          parse_json(R"({"level":"invalid","data":null})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(version, true, "notifications/progress",
                          parse_json(R"({"progressToken":false,"progress":1})"),
                          std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(notification_opt_in) {
  try {
    mcp_make_notification(CURRENT, true, "notifications/message",
                          parse_json(R"({"level":"info","data":"hello"})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_notification(CURRENT, true, "notifications/progress",
                          parse_json(R"({"progressToken":0,"progress":1})"),
                          std::nullopt);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  const auto cancelled{mcp_make_notification(
      CURRENT, true, "notifications/cancelled",
      parse_json(R"({"requestId":"listen"})"), JSON{"listen"})};
  EXPECT_FALSE(cancelled.defines("id"));
  EXPECT_TRUE(cancelled.is_object());
  const auto initialized{mcp_make_notification(
      MCPProtocolVersion::V_2025_11_25, false, "notifications/initialized",
      JSON::make_object(), std::nullopt)};
  EXPECT_FALSE(initialized.defines("id"));
  try {
    mcp_make_notification(CURRENT, false, "notifications/initialized",
                          JSON::make_object(), std::nullopt);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}
