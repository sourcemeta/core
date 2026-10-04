#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace sourcemeta::core;
constexpr MCPProtocolVersion revisions[]{
    MCPProtocolVersion::V_2025_03_26, MCPProtocolVersion::V_2025_06_18,
    MCPProtocolVersion::V_2025_11_25, MCPProtocolVersion::V_2026_07_28};
constexpr JSON::StringView wires[]{"2025-03-26", "2025-06-18", "2025-11-25",
                                   "2026-07-28"};
constexpr auto current{MCPProtocolVersion::V_2026_07_28};

template <typename Callback> void rejects(Callback &&callback) {
  bool rejected = false;
  try {
    callback();
  } catch (const std::invalid_argument &) {
    rejected = true;
  }
  EXPECT_TRUE(rejected);
}

auto parameters() -> JSON {
  return parse_json(
      R"({"_meta":{"io.modelcontextprotocol/protocolVersion":"2026-07-28","io.modelcontextprotocol/clientCapabilities":{}}})");
}

auto request(const JSON::StringView method = "tools/list") -> JSON {
  auto result{parse_json(R"({"jsonrpc":"2.0","id":0})")};
  result.assign("method", JSON{method});
  result.assign("params", parameters());
  return result;
}

void corpus(const MCPProtocolVersion version, const JSON::StringView definition,
            const JSON &value) {
  EXPECT_TRUE(value.is_object());
  std::string path;
#if defined(_MSC_VER)
  char *environment_value{nullptr};
  std::size_t size{0};
  if (::_dupenv_s(&environment_value, &size, "SOURCEMETA_MCP_CORPUS") == 0 &&
      environment_value) {
    path = environment_value;
  }
  std::free(environment_value);
#else
  if (const auto *environment_value{std::getenv("SOURCEMETA_MCP_CORPUS")}) {
    path = environment_value;
  }
#endif
  if (path.empty()) {
    return;
  }
  std::ofstream stream{path, std::ios::app};
  EXPECT_TRUE(stream.good());
  auto entry{JSON::make_object()};
  entry.assign("version", JSON{mcp_protocol_version_string(version)});
  entry.assign("definition", JSON{definition});
  entry.assign("value", JSON{value});
  stringify(entry, stream);
  stream << '\n';
}
} // namespace

TEST(conformance_invalid_success_ids) {
  for (const auto version : revisions) {
    for (const auto &id : {JSON{nullptr}, JSON{true}, JSON{1.5},
                           JSON::make_array(), JSON::make_object()}) {
      rejects([&] { mcp_make_empty_result(version, id); });
      rejects([&] { mcp_make_tool_error(version, id, "error"); });
    }
    EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
    EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
  }
}

TEST(conformance_errors_and_transport_context) {
  const JSON bad{true};
  for (const auto version : revisions) {
    const auto transport{
        mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad",
                       std::nullopt, MCPErrorContext::Transport)};
    if (mcp_protocol_version_at_least(version,
                                      MCPProtocolVersion::V_2025_11_25)) {
      EXPECT_FALSE(transport.defines("id"));
    } else {
      EXPECT_TRUE(transport.at("id").is_null());
      rejects([&] {
        mcp_make_error(version, &bad, JSONRPC_CODE_INVALID_REQUEST, "bad");
      });
    }
    const auto idless{mcp_make_error(version, nullptr,
                                     JSONRPC_CODE_INVALID_REQUEST, "bad",
                                     std::nullopt, MCPErrorContext::Transport)};
    EXPECT_FALSE(idless.defines("id"));
  }
  rejects([] {
    mcp_make_error(current, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
  });
  rejects([] {
    mcp_make_error(current, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  });
}

TEST(conformance_initialization_supported_subset) {
  const JSON::StringView only[]{"2025-03-26"};
  const auto init{parse_json(
      R"({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"protocolVersion":"unknown","capabilities":{},"clientInfo":{"name":"client","version":"1"}}})")};
  const auto result{
      mcp_make_initialize_result(init, {}, {"server", "1"}, only)};
  EXPECT_EQ(result.at("result").at("protocolVersion"), JSON{"2025-03-26"});
  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  EXPECT_EQ(mcp_make_initialize_result(invalid, {}, {"server", "1"}, only)
                .at("error")
                .at("code"),
            JSON{-32602});
  invalid.assign("id", JSON{});
  rejects(
      [&] { mcp_make_initialize_result(invalid, {}, {"server", "1"}, only); });
  const JSON::StringView modern_only[]{"2026-07-28"};
  rejects([&] {
    mcp_make_initialize_result(init, {}, {"server", "1"}, modern_only);
  });
}

TEST(conformance_metadata_borrowing_and_shapes) {
  auto value{parameters()};
  const auto parsed{mcp_validate_request_parameters(value)};
  EXPECT_EQ(parsed.first, MCPRequestMetaStatus::Valid);
  EXPECT_EQ(&parsed.second->meta_object, &value.at("_meta"));
  EXPECT_EQ(
      &parsed.second->client_capabilities,
      &value.at("_meta").at("io.modelcontextprotocol/clientCapabilities"));
  for (const auto &bad : {JSON{nullptr}, JSON{true}, JSON::make_array()}) {
    EXPECT_FALSE(mcp_validate_request_parameters(bad).second.has_value());
    EXPECT_FALSE(mcp_validate_request_meta(bad).second.has_value());
  }
  for (const auto text :
       {R"({"roots":true})", R"({"sampling":{"tools":false}})",
        R"({"elicitation":{"url":true}})", R"({"extensions":{"bad":{}}})",
        R"({"experimental":{"extension":false}})"}) {
    value = parameters();
    value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                             parse_json(text));
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
    rejects([&] { mcp_parse_client_capabilities(current, parse_json(text)); });
  }
  for (const auto text :
       {R"({"name":"client","version":"1","websiteUrl":false})",
        R"({"name":"client","version":"1","icons":[{"src":5}]})"}) {
    value = parameters();
    value.at("_meta").assign("io.modelcontextprotocol/clientInfo",
                             parse_json(text));
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  value = parameters();
  value.at("_meta").assign("progressToken", JSON{1.5});
  EXPECT_EQ(mcp_validate_request_parameters(value).first,
            MCPRequestMetaStatus::InvalidProgressToken);
  value = parameters();
  value.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"verbose"});
  EXPECT_EQ(mcp_validate_request_parameters(value).first,
            MCPRequestMetaStatus::InvalidLogLevel);
}

TEST(conformance_tracing_metadata) {
  auto value{parameters()};
  for (const auto key : {"traceparent", "tracestate", "baggage"}) {
    value = parameters();
    value.at("_meta").assign(key, JSON{false});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  for (const auto parent :
       {"00-00000000000000000000000000000000-0123456789abcdef-01",
        "00-0123456789abcdef0123456789abcdef-0000000000000000-01",
        "ff-0123456789abcdef0123456789abcdef-0123456789abcdef-01",
        "00-0123456789abcdef0123456789abcdef-0123456789abcdef-01-extra"}) {
    value = parameters();
    value.at("_meta").assign("traceparent", JSON{parent});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  value = parameters();
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate",
                           JSON{"vendor=state,1tenant@system=value"});
  value.at("_meta").assign("baggage",
                           JSON{"user=alice;property=value,region=us%20east"});
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  for (const auto state : {"a=b,a=c", "UPPER=value", "a=value=wrong"}) {
    value.at("_meta").assign("tracestate", JSON{state});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  value = parameters();
  value.at("_meta").assign("baggage", JSON{"a=%ZZ"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  // These tracing names are not reserved fields in legacy result metadata.
  const auto legacy{parse_json(R"({"_meta":{"traceparent":false}})")};
  EXPECT_EQ(mcp_decorate_result(MCPProtocolVersion::V_2025_03_26, legacy),
            legacy);
  rejects([&] { mcp_decorate_result(current, legacy); });
}

TEST(conformance_header_error_ordering) {
  auto value{request()};
  EXPECT_EQ(mcp_validate_request_headers(current, std::nullopt, "tools/list",
                                         std::nullopt, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32020});
  value.at("params").erase("_meta");
  EXPECT_EQ(mcp_validate_request_headers(current, std::nullopt, std::nullopt,
                                         std::nullopt, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32602});
  value = request();
  value.at("params").at("_meta").assign(
      "io.modelcontextprotocol/protocolVersion", JSON{"unknown"});
  EXPECT_EQ(mcp_validate_request_headers(current, "2026-07-28", "tools/list",
                                         std::nullopt, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32020});
  EXPECT_EQ(mcp_validate_request_headers(current, "unknown", "tools/list",
                                         std::nullopt, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32022});
  constexpr JSON::StringView disabled[]{"2025-11-25"};
  value = request();
  EXPECT_EQ(mcp_validate_request_headers(current, "2026-07-28", "tools/list",
                                         std::nullopt, value, disabled)
                ->at("error")
                .at("code"),
            JSON{-32022});
  EXPECT_EQ(mcp_validate_request_headers(current, "2026-07-28", "tools/list",
                                         "unexpected", value, wires)
                ->at("error")
                .at("code"),
            JSON{-32020});
  value.assign("id", JSON{true});
  const auto invalid{mcp_validate_request_headers(
      MCPProtocolVersion::V_2025_03_26, std::nullopt, std::nullopt,
      std::nullopt, value, wires)};
  EXPECT_TRUE(invalid->at("id").is_null());
}

TEST(conformance_parameter_header_roundtrip) {
  const auto schema{parse_json(
      R"({"type":"object","properties":{"nested":{"type":"object","properties":{"text":{"type":"string","x-mcp-header":"Label"}}},"number":{"type":"integer","x-mcp-header":"Count"},"enabled":{"type":"boolean","x-mcp-header":"Enabled"}}})")};
  const auto descriptors{mcp_header_parameters(current, schema)};
  EXPECT_TRUE(descriptors.has_value());
  EXPECT_EQ(descriptors->size(), 3);
  const auto args{parse_json(
      R"({"nested":{"text":" café "},"number":42,"enabled":false})")};
  const auto headers{mcp_make_parameter_headers(*descriptors, args)};
  std::vector<std::pair<JSON::StringView, JSON::StringView>> views;
  for (const auto &header : headers) {
    views.emplace_back(header.first, header.second);
  }
  EXPECT_FALSE(mcp_validate_parameter_headers(current, JSON{0}, *descriptors,
                                              args, views)
                   .has_value());
  for (auto &header : views) {
    if (header.first == "Mcp-Param-Count") {
      header.second = "42.0";
    }
  }
  EXPECT_FALSE(mcp_validate_parameter_headers(current, JSON{0}, *descriptors,
                                              args, views)
                   .has_value());
  views.emplace_back("mcp-param-count", "42");
  EXPECT_EQ(mcp_validate_parameter_headers(current, JSON{0}, *descriptors, args,
                                           views)
                ->at("error")
                .at("code"),
            JSON{-32020});
  EXPECT_TRUE(
      mcp_make_parameter_headers(*descriptors, parse_json(R"({"number":null})"))
          .empty());
  rejects([&] {
    mcp_make_parameter_headers(*descriptors,
                               parse_json(R"({"number":9007199254740992})"));
  });
  rejects([&] {
    mcp_make_parameter_headers(*descriptors, parse_json(R"({"number":1.5})"));
  });
}

TEST(conformance_parameter_header_schema_rejections) {
  for (
      const auto text :
      {R"({"type":"object","x-mcp-header":"Root"})",
       R"({"properties":{"a":{"type":"number","x-mcp-header":"Number"}}})",
       R"({"properties":{"a":{"type":"string","x-mcp-header":"Bad Name"}}})",
       R"({"properties":{"a":{"type":"string","x-mcp-header":"Name"},"b":{"type":"string","x-mcp-header":"name"}}})",
       R"({"allOf":[{"properties":{"a":{"type":"string","x-mcp-header":"Name"}}}]})"}) {
    EXPECT_FALSE(mcp_header_parameters(current, parse_json(text)).has_value());
  }
  EXPECT_TRUE(
      mcp_header_parameters(
          current,
          parse_json(R"({"default":{"x-mcp-header":"NotAnAnnotation"}})"))
          ->empty());
  const std::array<MCPHeaderParameter, 1> forged{
      {{"bad\r\nInjected", {"x"}, JSON::Type::String}}};
  rejects([&] { mcp_make_parameter_headers(forged, JSON::make_object()); });
  rejects([&] {
    mcp_validate_parameter_headers(current, JSON{0}, forged,
                                   JSON::make_object(), {});
  });
}

TEST(conformance_result_types_and_cache_scope) {
  EXPECT_EQ(mcp_result_type(JSON::make_object()), "complete");
  EXPECT_EQ(
      mcp_result_type(parse_json(R"({"resultType":"example/extension"})")),
      "example/extension");
  EXPECT_FALSE(mcp_result_type(JSON{nullptr}).has_value());
  EXPECT_FALSE(
      mcp_result_type(parse_json(R"({"resultType":false})")).has_value());
  for (const auto text :
       {R"({"resultType":"input_required","requestState":"state"})",
        R"({"resultType":"unknown"})"}) {
    rejects([&] { mcp_decorate_result(current, parse_json(text)); });
    rejects([&] {
      mcp_decorate_cacheable_result(current, parse_json(text), {},
                                    "resources/read");
    });
  }
  rejects([] {
    mcp_decorate_cacheable_result(current, JSON::make_object(), {},
                                  "tools/call");
  });
  EXPECT_EQ(mcp_decorate_cacheable_result(current, JSON::make_object(),
                                          {-4, MCPCacheScope::Private},
                                          "resources/read")
                .at("ttlMs"),
            JSON{0});
  const auto invalid_info{parse_json(
      R"({"_meta":{"io.modelcontextprotocol/serverInfo":{"name":"server"}}})")};
  rejects([&] { mcp_decorate_result(current, invalid_info); });
}

TEST(conformance_content_and_schemas) {
  for (const auto version : revisions) {
    rejects([&] {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"string"})"));
    });
    rejects([&] {
      mcp_make_resources_list_result(version, parse_json(R"([{"uri":"x"}])"),
                                     std::nullopt, {});
    });
    rejects([&] {
      mcp_make_prompts_list_result(
          version,
          parse_json(R"([{"name":"prompt","arguments":[{"name":3}]}])"),
          std::nullopt, {});
    });
    rejects([&] {
      mcp_make_resources_read_result(
          version, parse_json(R"([{"uri":"x","blob":false}])"), {});
    });
    rejects([&] {
      mcp_make_tool_success(version, JSON{0}, JSON::make_object(),
                            parse_json(R"([{"type":"text","text":false}])"));
    });
    rejects([&] {
      mcp_make_prompts_get_result(
          version, "",
          parse_json(
              R"([{"role":"system","content":{"type":"text","text":"hi"}}])"));
    });
  }
  for (const auto version :
       {MCPProtocolVersion::V_2025_06_18, MCPProtocolVersion::V_2025_11_25}) {
    rejects(
        [&] { mcp_make_tool_success(version, JSON{0}, JSON::make_array()); });
    rejects([&] {
      mcp_make_tool_descriptor(version, "tool", "",
                               parse_json(R"({"type":"object"})"),
                               parse_json(R"({"type":"string"})"));
    });
  }
  for (const auto &value :
       {JSON{nullptr}, JSON{true}, JSON{5}, JSON{"text"}, JSON::make_array()}) {
    EXPECT_EQ(mcp_make_tool_success(current, JSON{0}, value)
                  .at("result")
                  .at("structuredContent"),
              value);
  }
  auto many{JSON::make_array()};
  for (std::size_t index = 0; index < 101; ++index) {
    many.push_back(JSON{"value"});
  }
  rejects([&] {
    mcp_make_completion_result(current, many, std::nullopt, std::nullopt);
  });
  rejects([] {
    mcp_make_completion_result(current, parse_json(R"(["value"] )"), 0, false);
  });
}

TEST(conformance_mrtr_nested_requests) {
  MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  for (
      const auto text :
      {R"({"r":{"method":"roots/list"}})",
       R"({"r":{"method":"elicitation/create","params":{"mode":"url","message":"visit","url":"https://example.com"}}})",
       R"({"r":{"method":"elicitation/create","params":{"message":"choose","requestedSchema":{"type":"object","properties":{"choice":{"type":"array","items":{"type":"string","enum":["a"]}}}}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[{"role":"assistant","content":{"type":"tool_use","id":"call","name":"tool","input":{}}},{"role":"user","content":{"type":"tool_result","toolUseId":"call","content":[{"type":"text","text":"ok"}]}}],"tools":[{"name":"tool","inputSchema":{"type":"object"}}],"toolChoice":{"mode":"auto"}}}})"}) {
    const auto result{mcp_make_input_required_result(
        current, "tools/call", JSON{0}, parse_json(text), std::nullopt,
        capabilities)};
    corpus(current, "InputRequiredResult", result.at("result"));
  }
  for (
      const auto text :
      {R"({"r":{"method":"roots/list","params":false}})",
       R"({"r":{"method":"elicitation/create","params":{"message":"choose","requestedSchema":{"type":"object","properties":{"a":{"type":"object"}}}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[{"role":"user","content":{"type":"resource_link","uri":"x","name":"x"}}]}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"temperature":false}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"modelPreferences":{"costPriority":2}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"toolChoice":{"mode":"bad"}}}})"}) {
    rejects([&] {
      mcp_make_input_required_result(current, "tools/call", JSON{0},
                                     parse_json(text), std::nullopt,
                                     capabilities);
    });
  }
  rejects([&] {
    mcp_make_input_required_result(current, "tools/list", JSON{0}, std::nullopt,
                                   "token", capabilities);
  });
  for (const auto version : revisions) {
    if (version != current) {
      rejects([&] {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
      });
    }
  }
  const auto continuation{parse_json(
      R"({"jsonrpc":"2.0","id":0,"method":"tools/call","params":{"requestState":"s","inputResponses":{}}})")};
  EXPECT_TRUE(mcp_request_state(current, continuation).has_value());
  EXPECT_FALSE(mcp_request_state(MCPProtocolVersion::V_2025_11_25, continuation)
                   .has_value());
  auto unrelated{continuation};
  unrelated.assign("method", JSON{"tools/list"});
  EXPECT_EQ(mcp_request_input_responses(current, unrelated), nullptr);
}

TEST(conformance_subscription_filters_and_notifications) {
  auto value{parameters()};
  value.assign(
      "notifications",
      parse_json(
          R"({"toolsListChanged":true,"resourcesListChanged":false,"resourceSubscriptions":["a","b"]})"));
  const auto requested{mcp_parse_subscription_filter(current, value)};
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
  EXPECT_FALSE(mcp_parse_subscription_filter(current, malformed).has_value());
  malformed = value;
  malformed.at("notifications")
      .assign("resourceSubscriptions", parse_json("[false]"));
  EXPECT_FALSE(mcp_parse_subscription_filter(current, malformed).has_value());
  const auto ack{mcp_make_subscription_acknowledged_notification(
      current, JSON{0}, *requested, supported)};
  corpus(current, "SubscriptionsAcknowledgedNotification", ack);
  EXPECT_FALSE(ack.defines("id"));
  const auto update{
      mcp_make_notification(current, true, "notifications/resources/updated",
                            parse_json(R"({"uri":"b"})"), JSON{0}, &allowed)};
  corpus(current, "ResourceUpdatedNotification", update);
  EXPECT_EQ(*mcp_request_subscription_id(current, update), JSON{0});
  rejects([&] {
    mcp_make_notification(current, true, "notifications/resources/updated",
                          parse_json(R"({"uri":"a"})"), JSON{0}, &allowed);
  });
  rejects([&] {
    mcp_make_notification(current, true, "notifications/tools/list_changed",
                          JSON::make_object(), std::nullopt, &allowed);
  });
  rejects([&] {
    mcp_make_notification(current, true, "notifications/prompts/list_changed",
                          JSON::make_object(), JSON{0}, &allowed);
  });
  rejects([] {
    mcp_make_notification(current, true, "notifications/cancelled",
                          parse_json(R"({"requestId":1})"), JSON{0});
  });
  rejects([] {
    mcp_make_notification(current, true, "notifications/roots/list_changed",
                          JSON::make_object(), std::nullopt);
  });
}

TEST(conformance_notification_shapes) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};
  for (const auto version : revisions) {
    const auto log{mcp_make_notification(
        version, true, "notifications/message",
        parse_json(R"({"level":"info","data":{"message":"ok"}})"), std::nullopt,
        nullptr, &*context.second)};
    corpus(version, "LoggingMessageNotification", log);
    const auto progress{mcp_make_notification(
        version, true, "notifications/progress",
        parse_json(
            R"({"progressToken":0,"progress":1,"total":2,"message":"working"})"),
        std::nullopt, nullptr, &*context.second)};
    corpus(version, "ProgressNotification", progress);
    const auto cancel{mcp_make_notification(
        version, false, "notifications/cancelled",
        parse_json(R"({"requestId":0,"reason":"done"})"), std::nullopt)};
    corpus(version, "CancelledNotification", cancel);
    rejects([&] {
      mcp_make_notification(version, true, "notifications/message",
                            parse_json(R"({"level":"invalid","data":null})"),
                            std::nullopt);
    });
    rejects([&] {
      mcp_make_notification(
          version, true, "notifications/progress",
          parse_json(R"({"progressToken":false,"progress":1})"), std::nullopt);
    });
  }
}

TEST(conformance_borrowed_writer) {
  const auto page{parse_json(
      R"({"resources":[{"name":"test","uri":"https://example.com/a"}],"nextCursor":"","_meta":{"example.org/value":"preserved"}})")};
  const auto saved{page};
  for (const auto version : revisions) {
    std::ostringstream stream;
    mcp_write_result(stream, version, "resources/list", JSON{"quote\"\n"}, page,
                     MCPCachePolicy{});
    const auto parsed{parse_json(stream.str())};
    EXPECT_EQ(parsed.at("id"), JSON{"quote\"\n"});
    EXPECT_EQ(parsed.at("result").at("resources"), page.at("resources"));
    EXPECT_EQ(parsed.at("result").at("nextCursor"), JSON{""});
    EXPECT_EQ(parsed.at("result").at("_meta").at("example.org/value"),
              JSON{"preserved"});
    EXPECT_EQ(page, saved);
    EXPECT_EQ(stream.str().find('\n'), std::string::npos);
    corpus(version, "ListResourcesResult", parsed.at("result"));
  }
  std::ostringstream stream;
  rejects([&] {
    mcp_write_result(stream, current, "resources/list", JSON{0}, page,
                     std::nullopt);
  });
  rejects([&] {
    mcp_write_result(stream, current, "tools/call", JSON{0}, page,
                     MCPCachePolicy{});
  });
}

TEST(conformance_builder_schema_corpus) {
  const JSON id{0};
  const MCPImplementation server{"server", "1", "Title", "Description",
                                 "https://example.com"};
  for (const auto version : revisions) {
    corpus(version, "EmptyResult",
           mcp_make_empty_result(version, id).at("result"));
    corpus(
        version, "CallToolResult",
        mcp_make_tool_success(version, id, JSON::make_object()).at("result"));
    corpus(
        version, "CallToolResult",
        mcp_make_tool_success(version, id, JSON::make_object(),
                              parse_json(R"([{"type":"text","text":"hello"}])"))
            .at("result"));
    corpus(version, "CallToolResult",
           mcp_make_tool_error(version, id, "failure").at("result"));
    corpus(version, "Tool",
           mcp_make_tool_descriptor(
               version, "tool", "description",
               parse_json(R"({"type":"object","properties":{}})")));
    corpus(version, "Resource",
           mcp_make_resource("https://example.com", "resource", "text/plain",
                             "", 10, 0.5));
    corpus(version, "ResourceTemplate",
           mcp_make_resource_template("https://example.com/{name}", "template",
                                      "description", "text/plain"));
    corpus(version,
           mcp_supports_resource_link_content(version) ? "ResourceLink"
                                                       : "TextContent",
           mcp_make_resource_link(version, "https://example.com", "link"));
    corpus(version, "ListToolsResult",
           mcp_make_tools_list_result(version, JSON::make_array(), "", {}));
    corpus(version, "ListResourcesResult",
           mcp_make_resources_list_result(version, JSON::make_array(), "", {}));
    corpus(version, "ListResourceTemplatesResult",
           mcp_make_resource_templates_list_result(version, JSON::make_array(),
                                                   "", {}));
    corpus(version, "ListPromptsResult",
           mcp_make_prompts_list_result(version, JSON::make_array(), "", {}));
    corpus(version, "ReadResourceResult",
           mcp_make_resources_read_result(
               version,
               parse_json(R"([{"uri":"https://example.com","text":"hello"}])"),
               {}));
    corpus(
        version, "GetPromptResult",
        mcp_make_prompts_get_result(
            version, "description",
            parse_json(
                R"([{"role":"user","content":{"type":"text","text":"hello"}}])")));
    corpus(version, "CompleteResult",
           mcp_make_completion_result(version, parse_json(R"(["one","two"] )"),
                                      3, true));
    corpus(version, "ClientCapabilities",
           mcp_serialize_client_capabilities(version, {}));
    const auto error{
        mcp_make_error(version, &id, JSONRPC_CODE_INVALID_PARAMS, "bad")};
    corpus(
        version,
        mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25)
            ? "JSONRPCErrorResponse"
            : "JSONRPCError",
        error);
    corpus(
        version,
        mcp_protocol_version_at_least(version, MCPProtocolVersion::V_2025_11_25)
            ? "JSONRPCErrorResponse"
            : "JSONRPCError",
        mcp_make_error_resource_not_found(version, id));
    if (mcp_uses_initialization_handshake(version)) {
      auto init{parse_json(
          R"({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"capabilities":{},"clientInfo":{"name":"client","version":"1"}}})")};
      init.at("params").assign("protocolVersion",
                               JSON{mcp_protocol_version_string(version)});
      corpus(version, "InitializeResult",
             mcp_make_initialize_result(init, {}, server, wires).at("result"));
    }
  }
  corpus(current, "DiscoverResult",
         mcp_make_server_discover_result(current, id, {}, server, wires,
                                         "instructions", {})
             .at("result"));
  corpus(current, "SubscriptionsListenResult",
         mcp_make_subscription_close_result(current, id).at("result"));
  corpus(current, "InputRequiredResult",
         mcp_make_input_required_result(current, "resources/read", id,
                                        std::nullopt, "state", {})
             .at("result"));
}

TEST(conformance_continuation_validation) {
  auto value{parameters()};
  value.assign("requestState", JSON{"state"});
  value.assign(
      "inputResponses",
      parse_json(
          R"({"roots":{"roots":[{"uri":"file:///workspace"}]},"form":{"action":"accept","content":{"choice":["a"],"accepted":true}},"sample":{"role":"assistant","model":"model","content":{"type":"text","text":"hello"}},"unknown":{"ignored":true}})"));
  const auto prior{parse_json(
      R"({"roots":{"method":"roots/list"},"form":{"method":"elicitation/create"},"sample":{"method":"sampling/createMessage"}})")};
  EXPECT_TRUE(mcp_validate_continuation(current, value, prior, "state"));
  EXPECT_FALSE(mcp_validate_continuation(current, value, prior, "different"));
  EXPECT_FALSE(mcp_validate_continuation(MCPProtocolVersion::V_2025_11_25,
                                         value, prior, "state"));
  auto bad{value};
  bad.at("inputResponses")
      .at("roots")
      .at("roots")
      .at(0)
      .assign("uri", JSON{"https://example.com"});
  EXPECT_FALSE(mcp_validate_continuation(current, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("form").assign("action", JSON{"unknown"});
  EXPECT_FALSE(mcp_validate_continuation(current, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("sample").erase("model");
  EXPECT_FALSE(mcp_validate_continuation(current, bad, prior, "state"));
  bad = value;
  bad.assign("inputResponses", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(current, bad, prior, "state"));
  bad = value;
  bad.assign("requestState", JSON{1});
  EXPECT_FALSE(mcp_validate_continuation(current, bad, prior, std::nullopt));
  EXPECT_FALSE(
      mcp_validate_continuation(current, parameters(), prior, std::nullopt));
}

TEST(conformance_encoded_header_boundaries) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  for (const auto header :
       {"=?base64?!!!!?=", "=?base64?/w==?=", " café", "café", "cafe\n"}) {
    EXPECT_TRUE(mcp_validate_request_headers(current, "2026-07-28",
                                             "tools/call", header, value, wires)
                    .has_value());
  }
  for (const auto text : {"=?base64?YWJj?=", " padded ", "café", "a\tb"}) {
    value.at("params").assign("name", JSON{text});
    const auto encoded{mcp_encode_header_value(text)};
    EXPECT_FALSE(mcp_validate_request_headers(
                     current, "2026-07-28", "tools/call", encoded, value, wires)
                     .has_value());
  }
  rejects([] { mcp_encode_header_value(std::string_view{"\xff", 1}); });
}

TEST(conformance_notification_opt_in) {
  rejects([] {
    mcp_make_notification(current, true, "notifications/message",
                          parse_json(R"({"level":"info","data":"hello"})"),
                          std::nullopt);
  });
  rejects([] {
    mcp_make_notification(current, true, "notifications/progress",
                          parse_json(R"({"progressToken":0,"progress":1})"),
                          std::nullopt);
  });
  const auto cancelled{mcp_make_notification(
      current, true, "notifications/cancelled",
      parse_json(R"({"requestId":"listen"})"), JSON{"listen"})};
  EXPECT_FALSE(cancelled.defines("id"));
  corpus(current, "CancelledNotification", cancelled);
  const auto initialized{mcp_make_notification(
      MCPProtocolVersion::V_2025_11_25, false, "notifications/initialized",
      JSON::make_object(), std::nullopt)};
  EXPECT_FALSE(initialized.defines("id"));
  rejects([] {
    mcp_make_notification(current, false, "notifications/initialized",
                          JSON::make_object(), std::nullopt);
  });
}

TEST(conformance_capability_settings_roundtrip) {
  const auto version{MCPProtocolVersion::V_2025_11_25};
  const auto source{parse_json(
      R"({"roots":{"listChanged":false,"settings":{"name":"root"}},"sampling":{"tools":{"option":"value"}},"elicitation":{"form":{"setting":true}},"tasks":{"requests":{"sampling":{"createMessage":{}}}},"example.org/custom":{"value":true}})")};
  EXPECT_EQ(mcp_serialize_client_capabilities(
                version, mcp_parse_client_capabilities(version, source)),
            source);
  corpus(version, "ClientCapabilities", source);
  for (const auto text :
       {R"({"tasks":false})", R"({"tasks":{"list":false}})",
        R"({"tasks":{"requests":{"sampling":{"createMessage":true}}}})"}) {
    rejects([&] { mcp_parse_client_capabilities(version, parse_json(text)); });
  }
}

TEST(conformance_borrowed_header_collection) {
  const auto value{request()};
  std::vector<std::pair<JSON::StringView, JSON::StringView>> headers{
      {"mcp-protocol-version", "2026-07-28"}, {"MCP-METHOD", "tools/list"}};
  EXPECT_FALSE(
      mcp_validate_request_headers(current, headers, value, wires).has_value());
  headers.emplace_back("Mcp-Method", "tools/list");
  EXPECT_EQ(mcp_validate_request_headers(current, headers, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32020});
  const auto legacy{MCPProtocolVersion::V_2025_11_25};
  headers.front().second = "2025-11-25";
  EXPECT_FALSE(
      mcp_validate_request_headers(legacy, headers, value, wires).has_value());
}

TEST(conformance_stateless_revision_and_missing_capabilities) {
  auto value{request()};
  value.at("params").at("_meta").assign(
      "io.modelcontextprotocol/protocolVersion", JSON{"2025-11-25"});
  EXPECT_FALSE(mcp_validate_request_meta(value).second.has_value());
  value = request();
  value.at("params").at("_meta").erase(
      "io.modelcontextprotocol/clientCapabilities");
  EXPECT_EQ(mcp_validate_request_headers(current, std::nullopt, std::nullopt,
                                         std::nullopt, value, wires)
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(conformance_borrowed_writer_metadata_rejections) {
  std::ostringstream stream;
  const auto bad{parse_json(
      R"({"resources":[],"_meta":{"io.modelcontextprotocol/serverInfo":{"name":"missing-version"}}})")};
  rejects([&] {
    mcp_write_result(stream, current, "resources/list", JSON{0}, bad,
                     MCPCachePolicy{});
  });
  EXPECT_TRUE(stream.str().empty());
  const auto discovery{
      parse_json(R"({"supportedVersions":["2025-11-25"],"capabilities":{}})")};
  rejects([&] {
    mcp_write_result(stream, current, "server/discover", JSON{0}, discovery,
                     MCPCachePolicy{});
  });
}

// Expected membership comes from the five directional unions in the pinned
// official schemas, rather than from the implementation's dispatch tables.
TEST(conformance_official_method_matrix) {
  struct Entry {
    JSON::StringView method;
    unsigned client_request;
    unsigned server_request;
    unsigned client_notification;
    unsigned server_notification;
    unsigned input_request;
  };
  const Entry entries[]{
      {"completion/complete", 15, 0, 0, 0, 0},
      {"elicitation/create", 0, 6, 0, 0, 8},
      {"initialize", 7, 0, 0, 0, 0},
      {"logging/setLevel", 7, 0, 0, 0, 0},
      {"notifications/cancelled", 0, 0, 15, 15, 0},
      {"notifications/elicitation/complete", 0, 0, 0, 4, 0},
      {"notifications/initialized", 0, 0, 7, 0, 0},
      {"notifications/message", 0, 0, 0, 15, 0},
      {"notifications/progress", 0, 0, 7, 15, 0},
      {"notifications/prompts/list_changed", 0, 0, 0, 15, 0},
      {"notifications/resources/list_changed", 0, 0, 0, 15, 0},
      {"notifications/resources/updated", 0, 0, 0, 15, 0},
      {"notifications/roots/list_changed", 0, 0, 7, 0, 0},
      {"notifications/subscriptions/acknowledged", 0, 0, 0, 8, 0},
      {"notifications/tasks/status", 0, 0, 4, 4, 0},
      {"notifications/tools/list_changed", 0, 0, 0, 15, 0},
      {"ping", 7, 7, 0, 0, 0},
      {"prompts/get", 15, 0, 0, 0, 0},
      {"prompts/list", 15, 0, 0, 0, 0},
      {"resources/list", 15, 0, 0, 0, 0},
      {"resources/read", 15, 0, 0, 0, 0},
      {"resources/subscribe", 7, 0, 0, 0, 0},
      {"resources/templates/list", 15, 0, 0, 0, 0},
      {"resources/unsubscribe", 7, 0, 0, 0, 0},
      {"roots/list", 0, 7, 0, 0, 8},
      {"sampling/createMessage", 0, 7, 0, 0, 8},
      {"server/discover", 8, 0, 0, 0, 0},
      {"subscriptions/listen", 8, 0, 0, 0, 0},
      {"tasks/cancel", 4, 4, 0, 0, 0},
      {"tasks/get", 4, 4, 0, 0, 0},
      {"tasks/list", 4, 4, 0, 0, 0},
      {"tasks/result", 4, 4, 0, 0, 0},
      {"tools/call", 15, 0, 0, 0, 0},
      {"tools/list", 15, 0, 0, 0, 0},
      {"extension/unknown", 0, 0, 0, 0, 0},
  };
  for (std::size_t index = 0; index < std::size(revisions); ++index) {
    const auto version{revisions[index]};
    const auto bit{1U << index};
    EXPECT_EQ(mcp_supports_implementation_website_url(version), index >= 2);
    for (const auto &entry : entries) {
      EXPECT_EQ(mcp_is_request_method(version, entry.method),
                (entry.client_request & bit) != 0);
      EXPECT_EQ(mcp_is_server_request_method(version, entry.method),
                (entry.server_request & bit) != 0);
      EXPECT_EQ(mcp_is_client_notification_method(version, entry.method),
                (entry.client_notification & bit) != 0);
      EXPECT_EQ(mcp_is_server_notification_method(version, entry.method),
                (entry.server_notification & bit) != 0);
      EXPECT_EQ(
          mcp_is_notification_method(version, entry.method),
          ((entry.client_notification | entry.server_notification) & bit) != 0);
      EXPECT_EQ(mcp_is_input_request_method(version, entry.method),
                (entry.input_request & bit) != 0);
      EXPECT_EQ(mcp_supports_method(version, entry.method),
                ((entry.client_request | entry.server_request |
                  entry.client_notification | entry.server_notification |
                  entry.input_request) &
                 bit) != 0);
    }
  }
  for (const auto method : {"tools/call", "resources/read", "prompts/get",
                            "resources/subscribe", "resources/unsubscribe"}) {
    EXPECT_TRUE(mcp_is_named_request_method(method));
  }
  for (const auto method : {"tools/list", "resources/list", "server/discover",
                            "subscriptions/listen", "extension/unknown"}) {
    EXPECT_FALSE(mcp_is_named_request_method(method));
  }
}

TEST(conformance_enum_forms_and_sampling_continuations) {
  MCPClientCapabilities capabilities;
  capabilities.elicitation = true;
  capabilities.elicitation_form = true;
  const auto inputs{parse_json(
      R"({"form":{"method":"elicitation/create","params":{"mode":"form","message":"Choose options","requestedSchema":{"type":"object","properties":{"single":{"type":"string","oneOf":[{"const":"a","title":"Alpha"},{"const":"b","title":"Beta"}]},"multiple":{"type":"array","items":{"anyOf":[{"const":"a","title":"Alpha"},{"const":"b","title":"Beta"}]}}}}}}})")};
  EXPECT_EQ(mcp_make_input_required_result(current, "tools/call", JSON{0},
                                           inputs, std::nullopt, capabilities)
                .at("result")
                .at("inputRequests"),
            inputs);
  auto malformed{inputs};
  malformed.at("form")
      .at("params")
      .at("requestedSchema")
      .at("properties")
      .at("single")
      .at("oneOf")
      .at(0)
      .erase("title");
  rejects([&] {
    mcp_make_input_required_result(current, "tools/call", JSON{0}, malformed,
                                   std::nullopt, capabilities);
  });
  malformed = inputs;
  malformed.at("form")
      .at("params")
      .at("requestedSchema")
      .at("properties")
      .at("multiple")
      .at("items")
      .assign("anyOf", JSON{false});
  rejects([&] {
    mcp_make_input_required_result(current, "tools/call", JSON{0}, malformed,
                                   std::nullopt, capabilities);
  });
  malformed = inputs;
  malformed.at("form")
      .at("params")
      .at("requestedSchema")
      .at("properties")
      .at("multiple")
      .at("items")
      .at("anyOf")
      .at(1)
      .assign("const", JSON{1});
  rejects([&] {
    mcp_make_input_required_result(current, "tools/call", JSON{0}, malformed,
                                   std::nullopt, capabilities);
  });

  const auto prior{
      parse_json(R"({"sample":{"method":"sampling/createMessage"}})")};
  auto value{parameters()};
  value.assign(
      "inputResponses",
      parse_json(
          R"({"sample":{"role":"assistant","model":"model","content":[{"type":"text","text":"answer"},{"type":"tool_use","id":"call-1","name":"tool","input":{}}]}})"));
  EXPECT_TRUE(mcp_validate_continuation(current, value, prior, std::nullopt));
  value.at("inputResponses")
      .at("sample")
      .at("content")
      .at(1)
      .assign("input", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(current, value, prior, std::nullopt));
}
