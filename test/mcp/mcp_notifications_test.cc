#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <optional> // std::optional, std::nullopt
#include <vector>   // std::vector

TEST(parse_subscription_filter_all_events) {
  // The parsed filter borrows the resource URIs from the parameters, so the
  // parameters outlive it here
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": {
      "toolsListChanged": true,
      "promptsListChanged": true,
      "resourcesListChanged": true,
      "resourceSubscriptions": [ "file:///a", "file:///b" ]
    }
  })JSON")};
  const auto filter{sourcemeta::core::mcp_parse_subscription_filter(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters)};
  EXPECT_TRUE(filter.has_value());
  EXPECT_TRUE(filter->tools_list_changed.value());
  EXPECT_TRUE(filter->prompts_list_changed.value());
  EXPECT_TRUE(filter->resources_list_changed.value());
  EXPECT_EQ(filter->resource_subscriptions->size(), 2);
  EXPECT_EQ(filter->resource_subscriptions->at(0), "file:///a");
  EXPECT_EQ(filter->resource_subscriptions->at(1), "file:///b");
}

// Omitting a field is equivalent to not subscribing to that notification type
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(parse_subscription_filter_empty_notifications) {
  const auto filter{sourcemeta::core::mcp_parse_subscription_filter(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": {}
  })JSON"))};
  EXPECT_TRUE(filter.has_value());
  EXPECT_FALSE(filter->tools_list_changed.has_value());
  EXPECT_FALSE(filter->prompts_list_changed.has_value());
  EXPECT_FALSE(filter->resources_list_changed.has_value());
  EXPECT_FALSE(filter->resource_subscriptions.has_value());
}

TEST(parse_subscription_filter_preserves_explicit_false) {
  const auto filter{sourcemeta::core::mcp_parse_subscription_filter(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "toolsListChanged": false }
  })JSON"))};
  EXPECT_TRUE(filter.has_value());
  EXPECT_TRUE(filter->tools_list_changed.has_value());
  EXPECT_FALSE(filter->tools_list_changed.value());
}

TEST(parse_subscription_filter_preserves_empty_resource_subscriptions) {
  const auto filter{sourcemeta::core::mcp_parse_subscription_filter(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
      sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "resourceSubscriptions": [] }
  })JSON"))};
  EXPECT_TRUE(filter.has_value());
  EXPECT_TRUE(filter->resource_subscriptions.has_value());
  EXPECT_TRUE(filter->resource_subscriptions->empty());
}

// Subscriptions replaced the former resource subscribe request and the HTTP GET
// endpoint in 2026-07-28
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(parse_subscription_filter_rejects_2025_11_25) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "toolsListChanged": true }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_missing_metadata) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(
                       R"JSON({
                         "notifications": { "toolsListChanged": true }
                       })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_missing_notifications) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_object_notifications) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": []
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_boolean_tools_list_changed) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "toolsListChanged": 1 }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_boolean_prompts_list_changed) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "promptsListChanged": "yes" }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_boolean_resources_list_changed) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "resourcesListChanged": null }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_array_resource_subscriptions) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "resourceSubscriptions": "file:///a" }
  })JSON"))
                   .has_value());
}

TEST(parse_subscription_filter_rejects_non_string_resource_subscription) {
  EXPECT_FALSE(sourcemeta::core::mcp_parse_subscription_filter(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "notifications": { "resourceSubscriptions": [ false ] }
  })JSON"))
                   .has_value());
}

TEST(intersect_subscription_filters_both_opted_in) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = true;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = true;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_TRUE(result.tools_list_changed.has_value());
  EXPECT_TRUE(result.tools_list_changed.value());
  EXPECT_FALSE(result.prompts_list_changed.has_value());
  EXPECT_FALSE(result.resources_list_changed.has_value());
  EXPECT_FALSE(result.resource_subscriptions.has_value());
}

// A notification type the server does not support is omitted from the honored
// subset rather than carried as false
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(intersect_subscription_filters_omits_unsupported_event) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = true;
  const sourcemeta::core::MCPSubscriptionFilter supported;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_FALSE(result.tools_list_changed.has_value());
}

TEST(intersect_subscription_filters_omits_declined_event) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = true;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = false;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_FALSE(result.tools_list_changed.has_value());
}

TEST(intersect_subscription_filters_omits_unrequested_event) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = false;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = true;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_FALSE(result.tools_list_changed.has_value());
}

TEST(intersect_subscription_filters_omits_absent_event) {
  const sourcemeta::core::MCPSubscriptionFilter requested;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = true;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_FALSE(result.tools_list_changed.has_value());
}

TEST(intersect_subscription_filters_resource_subscriptions_overlap) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a", "file:///b"};
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///b", "file:///c"};
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_TRUE(result.resource_subscriptions.has_value());
  EXPECT_EQ(result.resource_subscriptions->size(), 1);
  EXPECT_EQ(result.resource_subscriptions->front(), "file:///b");
}

TEST(intersect_subscription_filters_resource_subscriptions_without_overlap) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///b"};
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_TRUE(result.resource_subscriptions.has_value());
  EXPECT_TRUE(result.resource_subscriptions->empty());
}

TEST(intersect_subscription_filters_omits_unsupported_resource_subscriptions) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  const sourcemeta::core::MCPSubscriptionFilter supported;
  const auto result{sourcemeta::core::mcp_intersect_subscription_filters(
      requested, supported)};
  EXPECT_FALSE(result.resource_subscriptions.has_value());
}

TEST(make_subscription_acknowledged_notification_with_string_identifier) {
  sourcemeta::core::MCPSubscriptionFilter filter;
  filter.tools_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "sub-42",
                filter, filter),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": "sub-42" },
      "notifications": { "toolsListChanged": true }
    }
  })JSON"));
}

TEST(make_subscription_acknowledged_notification_with_integer_identifier) {
  sourcemeta::core::MCPSubscriptionFilter filter;
  filter.prompts_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{123}, filter, filter),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 123 },
      "notifications": { "promptsListChanged": true }
    }
  })JSON"));
}

TEST(
    make_subscription_acknowledged_notification_with_integral_real_identifier) {
  sourcemeta::core::MCPSubscriptionFilter filter;
  const sourcemeta::core::JSON identifier{42.0};
  const auto notification{
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
          filter, filter)};
  EXPECT_EQ(notification.at("params").at("_meta").at(
                "io.modelcontextprotocol/subscriptionId"),
            identifier);
}

TEST(make_subscription_acknowledged_notification_with_every_event) {
  sourcemeta::core::MCPSubscriptionFilter filter;
  filter.tools_list_changed = true;
  filter.prompts_list_changed = true;
  filter.resources_list_changed = true;
  filter.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{1}, filter, filter),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 1 },
      "notifications": {
        "toolsListChanged": true,
        "promptsListChanged": true,
        "resourcesListChanged": true,
        "resourceSubscriptions": [ "file:///a" ]
      }
    }
  })JSON"));
}

// The acknowledged filter reflects the subset the server agreed to honor, and
// notification types the server does not support are omitted
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(make_subscription_acknowledged_notification_omits_unsupported_event) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = true;
  requested.prompts_list_changed = true;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{1}, requested, supported),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 1 },
      "notifications": { "toolsListChanged": true }
    }
  })JSON"));
}

TEST(make_subscription_acknowledged_notification_omits_declined_event) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.tools_list_changed = true;
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.tools_list_changed = false;
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{1}, requested, supported),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 1 },
      "notifications": {}
    }
  })JSON"));
}

TEST(make_subscription_acknowledged_notification_narrows_resource_uris) {
  sourcemeta::core::MCPSubscriptionFilter requested;
  requested.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a", "file:///b"};
  sourcemeta::core::MCPSubscriptionFilter supported;
  supported.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///b"};
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_acknowledged_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{1}, requested, supported),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 1 },
      "notifications": { "resourceSubscriptions": [ "file:///b" ] }
    }
  })JSON"));
}

TEST(make_subscription_acknowledged_notification_rejects_2025_11_25) {
  const sourcemeta::core::MCPSubscriptionFilter filter;
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
          sourcemeta::core::JSON{1}, filter, filter),
      "Subscriptions require MCP 2026-07-28");
}

TEST(make_subscription_acknowledged_notification_rejects_null_identifier) {
  const sourcemeta::core::MCPSubscriptionFilter filter;
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_subscription_acknowledged_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{nullptr}, filter, filter),
      "Invalid subscription identifier");
}

// A graceful close answers the long-lived listen request and names the
// subscription it ends
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(make_subscription_close_result) {
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_close_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{60}),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 60,
    "result": {
      "resultType": "complete",
      "_meta": { "io.modelcontextprotocol/subscriptionId": 60 }
    }
  })JSON"));
}

TEST(make_subscription_close_result_with_string_identifier) {
  EXPECT_EQ(sourcemeta::core::mcp_make_subscription_close_result(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{"listen"}),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "listen",
    "result": {
      "resultType": "complete",
      "_meta": { "io.modelcontextprotocol/subscriptionId": "listen" }
    }
  })JSON"));
}

TEST(make_subscription_close_result_rejects_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_subscription_close_result(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         sourcemeta::core::JSON{1}),
                     "Subscriptions require MCP 2026-07-28");
}

TEST(make_subscription_close_result_rejects_null_identifier) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_subscription_close_result(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON{nullptr}),
                     "Invalid subscription identifier");
}

TEST(request_subscription_id_from_notification) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/subscriptions/acknowledged",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *identifier{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification)};
  EXPECT_NE(identifier, nullptr);
  EXPECT_EQ(*identifier, sourcemeta::core::JSON{"stream-99"});
}

TEST(request_subscription_id_from_result) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "resultType": "complete",
      "_meta": {
        "io.modelcontextprotocol/subscriptionId": "stream-99"
      }
    }
  })JSON")};
  const auto *identifier{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, response)};
  EXPECT_NE(identifier, nullptr);
  EXPECT_EQ(*identifier, sourcemeta::core::JSON{"stream-99"});
}

TEST(request_subscription_id_integer) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 42 }
    }
  })JSON")};
  const auto *identifier{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification)};
  EXPECT_NE(identifier, nullptr);
  EXPECT_EQ(*identifier, sourcemeta::core::JSON{42});
}

TEST(request_subscription_id_integral_real) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "params": { "_meta": { "io.modelcontextprotocol/subscriptionId": 42.0 } }
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_subscription_id(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification),
      &notification.at("params").at("_meta").at(
          "io.modelcontextprotocol/subscriptionId"));
}

TEST(request_subscription_id_integral_decimal) {
  const auto response{sourcemeta::core::parse_json(R"JSON({
    "result": { "_meta": {
      "io.modelcontextprotocol/subscriptionId": 18446744073709551617
    } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_subscription_id(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, response),
            &response.at("result").at("_meta").at(
                "io.modelcontextprotocol/subscriptionId"));
}

TEST(request_subscription_id_rejects_fractional_decimal) {
  const auto metadata{sourcemeta::core::parse_json(R"JSON({
    "_meta": { "io.modelcontextprotocol/subscriptionId": 42.00000000000000000001 }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_subscription_id(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, metadata),
            nullptr);
}

TEST(request_subscription_id_from_bare_metadata) {
  const auto metadata{sourcemeta::core::parse_json(
      R"JSON({
        "_meta": { "io.modelcontextprotocol/subscriptionId": 7 }
      })JSON")};
  const auto *identifier{sourcemeta::core::mcp_request_subscription_id(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, metadata)};
  EXPECT_NE(identifier, nullptr);
  EXPECT_EQ(*identifier, sourcemeta::core::JSON{7});
}

TEST(request_subscription_id_on_2025_11_25) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 42 }
    }
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_subscription_id(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, notification),
      nullptr);
}

TEST(request_subscription_id_absent) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/tools/list_changed",
    "params": {}
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_subscription_id(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification),
      nullptr);
}

TEST(request_subscription_id_of_wrong_type) {
  const auto notification{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": false }
    }
  })JSON")};
  EXPECT_EQ(
      sourcemeta::core::mcp_request_subscription_id(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, notification),
      nullptr);
}

TEST(request_subscription_id_of_non_object) {
  EXPECT_EQ(sourcemeta::core::mcp_request_subscription_id(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::parse_json(R"JSON([])JSON")),
            nullptr);
}

TEST(make_notification_client_cancellation_2026_07_28) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, false,
                "notifications/cancelled",
                sourcemeta::core::parse_json(
                    R"JSON({ "requestId": 0, "reason": "done" })JSON"),
                std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/cancelled",
    "params": { "requestId": 0, "reason": "done" }
  })JSON"));
}

TEST(make_notification_client_cancellation_2025_03_26) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, false,
                "notifications/cancelled",
                sourcemeta::core::parse_json(
                    R"JSON({ "requestId": 0, "reason": "done" })JSON"),
                std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/cancelled",
    "params": { "requestId": 0, "reason": "done" }
  })JSON"));
}

TEST(make_notification_cancellation_without_reason) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, false,
                "notifications/cancelled",
                sourcemeta::core::parse_json(R"JSON({ "requestId": 0 })JSON"),
                std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/cancelled",
    "params": { "requestId": 0 }
  })JSON"));
}

TEST(make_notification_cancellation_rejects_missing_request_id) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/cancelled",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Invalid cancellation notification");
}

TEST(make_notification_cancellation_rejects_unreadable_request_id) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, false,
          "notifications/cancelled",
          sourcemeta::core::parse_json(R"JSON({ "requestId": null })JSON"),
          std::nullopt),
      "Invalid cancellation notification");
}

TEST(make_notification_cancellation_rejects_non_string_reason) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/cancelled",
                         sourcemeta::core::parse_json(
                             R"JSON({ "requestId": 0, "reason": 1 })JSON"),
                         std::nullopt),
                     "Invalid cancellation notification");
}

// A server may only cancel the listen request that opened the stream it is
// serving
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(make_notification_server_cancellation_names_its_subscription) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/cancelled",
          sourcemeta::core::parse_json(R"JSON({ "requestId": "listen" })JSON"),
          sourcemeta::core::JSON{"listen"}),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/cancelled",
    "params": {
      "requestId": "listen",
      "_meta": { "io.modelcontextprotocol/subscriptionId": "listen" }
    }
  })JSON"));
}

TEST(make_notification_server_cancellation_rejects_other_subscription) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/cancelled",
          sourcemeta::core::parse_json(R"JSON({ "requestId": 1 })JSON"),
          sourcemeta::core::JSON{2}),
      "Servers may only cancel the originating listen request");
}

TEST(make_notification_server_cancellation_rejects_missing_subscription) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/cancelled",
          sourcemeta::core::parse_json(R"JSON({ "requestId": 1 })JSON"),
          std::nullopt),
      "Servers may only cancel the originating listen request");
}

// Only clients announce that their roots changed, and the notification is gone
// from 2026-07-28 altogether
// https://modelcontextprotocol.io/specification/2026-07-28/deprecated
TEST(make_notification_roots_list_changed_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, false,
                "notifications/roots/list_changed",
                sourcemeta::core::parse_json(R"JSON({})JSON"), std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/roots/list_changed",
    "params": {}
  })JSON"));
}

TEST(make_notification_roots_list_changed_rejects_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/roots/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Notification has an invalid revision or direction");
}

TEST(make_notification_roots_list_changed_rejects_server_direction) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/roots/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Notification has an invalid revision or direction");
}

// The initialization handshake is gone in the stateless revision
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#statelessness
TEST(make_notification_initialized_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, false,
                "notifications/initialized",
                sourcemeta::core::parse_json(R"JSON({})JSON"), std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/initialized",
    "params": {}
  })JSON"));
}

TEST(make_notification_initialized_rejects_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/initialized",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Notification has an invalid revision or direction");
}

TEST(make_notification_rejects_non_object_parameters) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/cancelled",
                         sourcemeta::core::parse_json(R"JSON([])JSON"),
                         std::nullopt),
                     "Invalid notification parameters");
}

TEST(make_notification_rejects_non_object_metadata) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/cancelled",
                         sourcemeta::core::parse_json(
                             R"JSON({ "requestId": 1, "_meta": [] })JSON"),
                         std::nullopt),
                     "Invalid notification parameters");
}

TEST(make_notification_rejects_malformed_metadata_key_on_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         false, "notifications/cancelled",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "requestId": 1,
                               "_meta": { "invalid key": 1 }
                             })JSON"),
                         std::nullopt),
                     "Invalid notification metadata");
}

TEST(make_notification_rejects_subscription_identifier_on_2025_11_25) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/tools/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         sourcemeta::core::JSON{1}),
                     "Invalid subscription notification context");
}

TEST(make_notification_rejects_subscription_identifier_from_client) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, false,
          "notifications/cancelled",
          sourcemeta::core::parse_json(R"JSON({ "requestId": 1 })JSON"),
          sourcemeta::core::JSON{1}),
      "Invalid subscription notification context");
}

TEST(make_notification_rejects_unreadable_subscription_identifier) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.tools_list_changed = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/tools/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         sourcemeta::core::JSON{nullptr}, &acknowledged),
                     "Invalid subscription notification context");
}

TEST(make_notification_tools_list_changed_on_an_acknowledged_subscription) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.tools_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/tools/list_changed",
                sourcemeta::core::parse_json(R"JSON({})JSON"),
                sourcemeta::core::JSON{5}, &acknowledged),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/tools/list_changed",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 5 }
    }
  })JSON"));
}

TEST(make_notification_prompts_list_changed_on_an_acknowledged_subscription) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.prompts_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/prompts/list_changed",
                sourcemeta::core::parse_json(R"JSON({})JSON"),
                sourcemeta::core::JSON{5}, &acknowledged),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/prompts/list_changed",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 5 }
    }
  })JSON"));
}

TEST(make_notification_resources_list_changed_on_an_acknowledged_subscription) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.resources_list_changed = true;
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/resources/list_changed",
                sourcemeta::core::parse_json(R"JSON({})JSON"),
                sourcemeta::core::JSON{5}, &acknowledged),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/resources/list_changed",
    "params": {
      "_meta": { "io.modelcontextprotocol/subscriptionId": 5 }
    }
  })JSON"));
}

TEST(make_notification_resources_updated_on_an_acknowledged_uri) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/resources/updated",
          sourcemeta::core::parse_json(R"JSON({ "uri": "file:///a" })JSON"),
          sourcemeta::core::JSON{5}, &acknowledged),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/resources/updated",
    "params": {
      "uri": "file:///a",
      "_meta": { "io.modelcontextprotocol/subscriptionId": 5 }
    }
  })JSON"));
}

// The server must not send notification types the client has not explicitly
// requested
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/subscriptions
TEST(make_notification_rejects_unacknowledged_tools_list_changed) {
  const sourcemeta::core::MCPSubscriptionFilter acknowledged;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/tools/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         sourcemeta::core::JSON{5}, &acknowledged),
                     "Notification was not acknowledged");
}

TEST(make_notification_rejects_unacknowledged_resource_uri) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///b"};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/resources/updated",
          sourcemeta::core::parse_json(R"JSON({ "uri": "file:///a" })JSON"),
          sourcemeta::core::JSON{5}, &acknowledged),
      "Notification was not acknowledged");
}

TEST(make_notification_list_changed_requires_a_subscription) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.tools_list_changed = true;
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/tools/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt, &acknowledged),
                     "Updates require an acknowledged subscription");
}

TEST(make_notification_list_changed_requires_an_acknowledged_filter) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/tools/list_changed",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         sourcemeta::core::JSON{5}),
                     "Updates require an acknowledged subscription");
}

TEST(make_notification_resources_updated_requires_a_uri) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/resources/updated",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         sourcemeta::core::JSON{5}, &acknowledged),
                     "Resource update requires a URI");
}

TEST(make_notification_resources_updated_rejects_non_string_uri) {
  sourcemeta::core::MCPSubscriptionFilter acknowledged;
  acknowledged.resource_subscriptions =
      std::vector<sourcemeta::core::JSON::StringView>{"file:///a"};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/resources/updated",
          sourcemeta::core::parse_json(R"JSON({ "uri": 1 })JSON"),
          sourcemeta::core::JSON{5}, &acknowledged),
      "Resource update requires a URI");
}

// Lists changed freely on legacy connections, where a client subscribed by
// capability rather than per request
// https://modelcontextprotocol.io/specification/2025-11-25/server/tools#list-changed-notification
TEST(make_notification_tools_list_changed_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
                "notifications/tools/list_changed",
                sourcemeta::core::parse_json(R"JSON({})JSON"), std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/tools/list_changed",
    "params": {}
  })JSON"));
}

TEST(make_notification_resources_updated_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/resources/updated",
          sourcemeta::core::parse_json(R"JSON({ "uri": "file:///a" })JSON"),
          std::nullopt),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/resources/updated",
    "params": { "uri": "file:///a" }
  })JSON"));
}

TEST(make_notification_elicitation_complete_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/elicitation/complete",
          sourcemeta::core::parse_json(R"JSON({ "elicitationId": "e1" })JSON"),
          std::nullopt),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/elicitation/complete",
    "params": { "elicitationId": "e1" }
  })JSON"));
}

TEST(make_notification_elicitation_complete_requires_an_identifier) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/elicitation/complete",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Elicitation completion requires an ID");
}

TEST(make_notification_elicitation_complete_rejects_2025_06_18) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_06_18, true,
          "notifications/elicitation/complete",
          sourcemeta::core::parse_json(R"JSON({ "elicitationId": "e1" })JSON"),
          std::nullopt),
      "Notification has an invalid revision or direction");
}

// The acknowledgment has its own builder because the filter it carries is the
// negotiated subset rather than caller-supplied parameters
TEST(make_notification_rejects_acknowledgment_method) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/subscriptions/acknowledged",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Use the specialized builder for this notification");
}

TEST(make_notification_rejects_task_status_method) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/tasks/status",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Use the specialized builder for this notification");
}

TEST(make_notification_rejects_unknown_method) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/unknown",
                         sourcemeta::core::parse_json(R"JSON({})JSON"),
                         std::nullopt),
                     "Notification has an invalid revision or direction");
}

// Progress is scoped to the request that opted in with a progress token
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/progress
TEST(make_notification_progress_2026_07_28) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "progressToken": 7
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/progress", sourcemeta::core::parse_json(R"JSON({
    "progressToken": 7,
    "progress": 1,
    "total": 2,
    "message": "working"
  })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/progress",
    "params": {
      "progressToken": 7,
      "progress": 1,
      "total": 2,
      "message": "working"
    }
  })JSON"));
}

TEST(make_notification_progress_without_total_and_message) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "progressToken": "token"
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
                "notifications/progress",
                sourcemeta::core::parse_json(
                    R"JSON({
                      "progressToken": "token",
                      "progress": 0.5
                    })JSON"),
                std::nullopt, nullptr, &context.second.value()),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/progress",
    "params": { "progressToken": "token", "progress": 0.5 }
  })JSON"));
}

TEST(make_notification_progress_requires_the_request_context_on_2026_07_28) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/progress",
          sourcemeta::core::parse_json(
              R"JSON({ "progressToken": 0, "progress": 1 })JSON"),
          std::nullopt),
      "Progress must echo the current request's token");
}

TEST(make_notification_progress_rejects_a_foreign_token_on_2026_07_28) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "progressToken": 7
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/progress",
          sourcemeta::core::parse_json(
              R"JSON({ "progressToken": 8, "progress": 1 })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Progress must echo the current request's token");
}

TEST(make_notification_progress_rejects_a_request_that_did_not_opt_in) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  const auto context{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, true,
          "notifications/progress",
          sourcemeta::core::parse_json(
              R"JSON({ "progressToken": 7, "progress": 1 })JSON"),
          std::nullopt, nullptr, &context.second.value()),
      "Progress must echo the current request's token");
}

TEST(make_notification_progress_2025_11_25) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
                "notifications/progress",
                sourcemeta::core::parse_json(
                    R"JSON({ "progressToken": 0, "progress": 1 })JSON"),
                std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/progress",
    "params": { "progressToken": 0, "progress": 1 }
  })JSON"));
}

TEST(make_notification_progress_rejects_an_unreadable_token) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/progress",
          sourcemeta::core::parse_json(
              R"JSON({ "progressToken": false, "progress": 1 })JSON"),
          std::nullopt),
      "Invalid progress notification");
}

TEST(make_notification_progress_rejects_a_missing_progress_value) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/progress",
          sourcemeta::core::parse_json(R"JSON({ "progressToken": 0 })JSON"),
          std::nullopt),
      "Invalid progress notification");
}

TEST(make_notification_progress_rejects_a_non_numeric_progress_value) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/progress",
          sourcemeta::core::parse_json(
              R"JSON({ "progressToken": 0, "progress": "1" })JSON"),
          std::nullopt),
      "Invalid progress notification");
}

TEST(make_notification_progress_rejects_a_non_numeric_total) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/progress",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "progressToken": 0,
                               "progress": 1,
                               "total": "2"
                             })JSON"),
                         std::nullopt),
                     "Invalid progress notification");
}

TEST(make_notification_progress_rejects_a_non_string_message) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/progress",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "progressToken": 0,
                               "progress": 1,
                               "message": 2
                             })JSON"),
                         std::nullopt),
                     "Invalid progress notification");
}

TEST(make_notification_message_2025_11_25) {
  EXPECT_EQ(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "info", "data": { "message": "ok" } })JSON"),
          std::nullopt),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "info", "data": { "message": "ok" } }
  })JSON"));
}

TEST(make_notification_message_with_a_logger_name) {
  EXPECT_EQ(sourcemeta::core::mcp_make_notification(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
                "notifications/message",
                sourcemeta::core::parse_json(
                    R"JSON({
                      "level": "info",
                      "logger": "db",
                      "data": null
                    })JSON"),
                std::nullopt),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/message",
    "params": { "level": "info", "logger": "db", "data": null }
  })JSON"));
}

TEST(make_notification_message_rejects_an_unknown_level) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "invalid",
                               "data": null
                             })JSON"),
                         std::nullopt),
                     "Invalid logging notification");
}

TEST(make_notification_message_rejects_missing_data) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/message",
          sourcemeta::core::parse_json(R"JSON({ "level": "info" })JSON"),
          std::nullopt),
      "Invalid logging notification");
}

TEST(make_notification_message_rejects_a_non_string_logger) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_notification(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, true,
          "notifications/message",
          sourcemeta::core::parse_json(
              R"JSON({ "level": "info", "logger": 1, "data": null })JSON"),
          std::nullopt),
      "Invalid logging notification");
}

// Logging in 2026-07-28 is opted into per request through the log level in
// `_meta`, since there is no session to carry a level
// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging
TEST(make_notification_message_requires_the_request_context_on_2026_07_28) {
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_notification(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         true, "notifications/message",
                         sourcemeta::core::parse_json(
                             R"JSON({
                               "level": "info",
                               "data": "hello"
                             })JSON"),
                         std::nullopt),
                     "Logging requires an explicit per-request opt-in");
}

TEST(make_notification_message_requires_a_requested_level_on_2026_07_28) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
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
                     "Logging requires an explicit per-request opt-in");
}

TEST(make_notification_message_cannot_use_a_subscription_stream) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
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
                         sourcemeta::core::JSON{1}, nullptr,
                         &context.second.value()),
                     "Logging cannot be sent on a subscription stream");
}

// A client that was given no state token must not include one on the retry
// https://modelcontextprotocol.io/specification/2026-07-28/basic/patterns/mrtr#client-requirements-basic-workflow
TEST(validate_continuation_requires_an_absent_state_when_none_was_issued) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "inputResponses": { "form": { "action": "decline" } }
  })JSON")};
  const auto prior{sourcemeta::core::JSON::make_object()};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      std::nullopt));
}

TEST(validate_continuation_rejects_an_unsolicited_state_token) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": "invented",
    "inputResponses": { "form": { "action": "decline" } }
  })JSON")};
  const auto prior{sourcemeta::core::JSON::make_object()};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      std::nullopt));
}

TEST(validate_continuation_requires_a_present_empty_state_for_an_empty_echo) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    },
    "requestState": ""
  })JSON")};
  const auto prior{sourcemeta::core::JSON::make_object()};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_continuation(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, parameters, prior,
      ""));
}
