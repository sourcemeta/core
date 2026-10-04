#include <sourcemeta/core/mcp.h>

#include "validation.h"

#include <algorithm>
#include <cmath>

namespace {
using sourcemeta::core::JSON;

auto optional_string(const JSON &value, const JSON::StringView name) -> bool {
  const auto *field{value.try_at(name)};
  return !field || field->is_string();
}

auto opt_in(const std::optional<bool> requested,
            const std::optional<bool> supported) -> std::optional<bool> {
  return requested.has_value()
             ? std::optional<bool>{*requested && supported.value_or(false)}
             : std::nullopt;
}
} // namespace

namespace sourcemeta::core {
auto mcp_parse_subscription_filter(const MCPProtocolVersion version,
                                   const JSON &parameters)
    -> std::optional<MCPSubscriptionFilter> {
  if (!mcp_supports_subscriptions_listen(version) ||
      mcp_validate_request_parameters(parameters).first !=
          MCPRequestMetaStatus::Valid) {
    return std::nullopt;
  }
  const auto *filter{parameters.try_at("notifications")};
  if (!filter || !filter->is_object()) {
    return std::nullopt;
  }
  MCPSubscriptionFilter result;
  for (const auto name :
       {"toolsListChanged", "promptsListChanged", "resourcesListChanged"}) {
    const auto *value{filter->try_at(name)};
    if (!value) {
      continue;
    }
    if (!value->is_boolean()) {
      return std::nullopt;
    }
    if (JSON::StringView{name} == "toolsListChanged") {
      result.tools_list_changed = value->to_boolean();
    } else if (JSON::StringView{name} == "promptsListChanged") {
      result.prompts_list_changed = value->to_boolean();
    } else {
      result.resources_list_changed = value->to_boolean();
    }
  }
  if (const auto *uris{filter->try_at("resourceSubscriptions")}; uris) {
    if (!uris->is_array()) {
      return std::nullopt;
    }
    result.resource_subscriptions.emplace();
    for (const auto &uri : uris->as_array()) {
      if (!uri.is_string()) {
        return std::nullopt;
      }
      result.resource_subscriptions->push_back(uri.to_string());
    }
  }
  return result;
}

auto mcp_intersect_subscription_filters(const MCPSubscriptionFilter &requested,
                                        const MCPSubscriptionFilter &supported)
    -> MCPSubscriptionFilter {
  MCPSubscriptionFilter result;
  result.tools_list_changed =
      opt_in(requested.tools_list_changed, supported.tools_list_changed);
  result.prompts_list_changed =
      opt_in(requested.prompts_list_changed, supported.prompts_list_changed);
  result.resources_list_changed = opt_in(requested.resources_list_changed,
                                         supported.resources_list_changed);
  if (requested.resource_subscriptions) {
    result.resource_subscriptions.emplace();
    for (const auto uri : *requested.resource_subscriptions) {
      if (supported.resource_subscriptions &&
          std::find(supported.resource_subscriptions->begin(),
                    supported.resource_subscriptions->end(),
                    uri) != supported.resource_subscriptions->end()) {
        result.resource_subscriptions->push_back(uri);
      }
    }
  }
  return result;
}

auto mcp_result_type(const JSON &result) -> std::optional<JSON::StringView> {
  if (!result.is_object()) {
    return std::nullopt;
  }
  const auto *type{result.try_at("resultType")};
  return !type ? std::optional<JSON::StringView>{"complete"}
         : type->is_string()
             ? std::optional<JSON::StringView>{type->to_string()}
             : std::nullopt;
}

auto mcp_make_notification(const MCPProtocolVersion version,
                           const bool from_server,
                           const JSON::StringView method, JSON parameters,
                           std::optional<JSON> subscription_id,
                           const MCPSubscriptionFilter *acknowledged,
                           const MCPRequestMeta *request_context) -> JSON {
  internal::require(from_server
                        ? mcp_is_server_notification_method(version, method)
                        : mcp_is_client_notification_method(version, method),
                    "Notification has an invalid revision or direction");
  internal::require(parameters.is_object() && internal::valid_meta(parameters),
                    "Invalid notification parameters");
  if (version == MCPProtocolVersion::V_2026_07_28) {
    if (const auto *meta{parameters.try_at("_meta")}; meta) {
      internal::require(internal::valid_metadata_object(*meta),
                        "Invalid notification metadata");
    }
  }
  if (version == MCPProtocolVersion::V_2026_07_28 && from_server) {
    if (method == MCP_METHOD_NOTIFICATIONS_MESSAGE) {
      internal::require(request_context &&
                            request_context->protocol_version == version &&
                            request_context->log_level,
                        "Logging requires an explicit per-request opt-in");
    } else if (method == MCP_METHOD_NOTIFICATIONS_PROGRESS) {
      const auto *token{parameters.try_at("progressToken")};
      internal::require(request_context &&
                            request_context->protocol_version == version &&
                            request_context->progress_token && token &&
                            *token == *request_context->progress_token,
                        "Progress must echo the current request's token");
    }
  }
  if (subscription_id) {
    internal::require(version == MCPProtocolVersion::V_2026_07_28 &&
                          from_server && internal::valid_id(*subscription_id),
                      "Invalid subscription notification context");
  }
  if (method == MCP_METHOD_NOTIFICATIONS_PROGRESS) {
    const auto *token{parameters.try_at("progressToken")};
    const auto *progress{parameters.try_at("progress")};
    const auto *total{parameters.try_at("total")};
    internal::require(token && internal::valid_id(*token) && progress &&
                          progress->is_number() &&
                          std::isfinite(progress->as_real()) &&
                          (!total || (total->is_number() &&
                                      std::isfinite(total->as_real()))) &&
                          optional_string(parameters, "message"),
                      "Invalid progress notification");
  } else if (method == MCP_METHOD_NOTIFICATIONS_MESSAGE) {
    const auto *level{parameters.try_at("level")};
    internal::require(level && level->is_string() &&
                          internal::valid_log_level(level->to_string()) &&
                          parameters.defines("data") &&
                          optional_string(parameters, "logger"),
                      "Invalid logging notification");
  } else if (method == MCP_METHOD_NOTIFICATIONS_CANCELLED) {
    const auto *identifier{parameters.try_at("requestId")};
    internal::require(identifier && internal::valid_id(*identifier) &&
                          optional_string(parameters, "reason"),
                      "Invalid cancellation notification");
    if (version == MCPProtocolVersion::V_2026_07_28 && from_server) {
      internal::require(
          subscription_id && *subscription_id == *identifier,
          "Servers may only cancel the originating listen request");
    }
  } else if (method == MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED) {
    internal::require(parameters.defines("uri") &&
                          parameters.at("uri").is_string(),
                      "Resource update requires a URI");
  } else if (method == MCP_METHOD_NOTIFICATIONS_ELICITATION_COMPLETE) {
    internal::require(parameters.defines("elicitationId") &&
                          parameters.at("elicitationId").is_string(),
                      "Elicitation completion requires an ID");
  } else {
    // Task status and acknowledgment have specialized payloads and must use
    // their owning extension or the acknowledgment builder.
    internal::require(
        method == MCP_METHOD_NOTIFICATIONS_INITIALIZED ||
            method == MCP_METHOD_NOTIFICATIONS_ROOTS_LIST_CHANGED ||
            method == MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED ||
            method == MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED ||
            method == MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED,
        "Use the specialized builder for this notification");
  }
  if (version == MCPProtocolVersion::V_2026_07_28 &&
      (method == MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED ||
       method == MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED ||
       method == MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED ||
       method == MCP_METHOD_NOTIFICATIONS_RESOURCES_UPDATED)) {
    internal::require(subscription_id && acknowledged,
                      "Updates require an acknowledged subscription");
    const bool enabled{
        method == MCP_METHOD_NOTIFICATIONS_TOOLS_LIST_CHANGED
            ? acknowledged->tools_list_changed.value_or(false)
        : method == MCP_METHOD_NOTIFICATIONS_PROMPTS_LIST_CHANGED
            ? acknowledged->prompts_list_changed.value_or(false)
        : method == MCP_METHOD_NOTIFICATIONS_RESOURCES_LIST_CHANGED
            ? acknowledged->resources_list_changed.value_or(false)
            : acknowledged->resource_subscriptions &&
                  std::find(acknowledged->resource_subscriptions->begin(),
                            acknowledged->resource_subscriptions->end(),
                            parameters.at("uri").to_string()) !=
                      acknowledged->resource_subscriptions->end()};
    internal::require(enabled, "Notification was not acknowledged");
  }
  if (subscription_id) {
    auto *meta{parameters.try_at("_meta")};
    if (!meta) {
      parameters.assign_assume_new("_meta", JSON::make_object());
      meta = parameters.try_at("_meta");
    }
    meta->assign("io.modelcontextprotocol/subscriptionId",
                 std::move(*subscription_id));
  }
  auto result{JSON::make_object()};
  result.assign_assume_new("jsonrpc", JSON{"2.0"});
  result.assign_assume_new("method", JSON{method});
  result.assign_assume_new("params", std::move(parameters));
  return result;
}
auto mcp_validate_continuation(
    const MCPProtocolVersion version, const JSON &parameters,
    const JSON &input_requests,
    const std::optional<JSON::StringView> expected_state) -> bool {
  if (!mcp_supports_mrtr(version) || !input_requests.is_object() ||
      mcp_validate_request_parameters(parameters).first !=
          MCPRequestMetaStatus::Valid) {
    return false;
  }
  const auto *state{parameters.try_at("requestState")};
  const auto *responses{parameters.try_at("inputResponses")};
  if ((!state && !responses) || (state && !state->is_string()) ||
      (expected_state && (!state || state->to_string() != *expected_state)) ||
      (responses && !responses->is_object())) {
    return false;
  }
  if (!responses) {
    return true;
  }
  for (const auto &entry : responses->as_object()) {
    const auto *prior{input_requests.try_at(entry.first)};
    // MRTR permits unrecognized response information to be ignored.
    if (!prior) {
      continue;
    }
    if (!prior->is_object()) {
      return false;
    }
    const auto *method{prior->try_at("method")};
    if (!method || !method->is_string() ||
        !internal::valid_input_response(method->to_string(), entry.second)) {
      return false;
    }
  }
  return true;
}

} // namespace sourcemeta::core
