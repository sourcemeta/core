#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

#include <array>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

TEST(review_capability_overrides) {
  using namespace sourcemeta::core;
  constexpr auto LEGACY{MCPProtocolVersion::V_2025_11_25};
  auto client{mcp_parse_client_capabilities(
      LEGACY,
      parse_json(
          R"({"roots":{"listChanged":true,"custom":{}},"sampling":{"context":{"custom":true},"tools":{}},"elicitation":{"form":{},"url":{}},"experimental":{"custom":{}}})"))};
  client.roots_list_changed = false;
  client.sampling_context = false;
  client.sampling_tools = false;
  client.elicitation_form = false;
  client.elicitation_url = false;
  client.experimental.reset();
  const auto edited{mcp_serialize_client_capabilities(LEGACY, client)};
  EXPECT_FALSE(edited.at("roots").defines("listChanged"));
  EXPECT_TRUE(edited.at("roots").defines("custom"));
  EXPECT_TRUE(edited.at("sampling").empty());
  EXPECT_TRUE(edited.at("elicitation").empty());
  EXPECT_FALSE(edited.defines("experimental"));
  const auto downgraded{mcp_serialize_client_capabilities(
      MCPProtocolVersion::V_2025_06_18,
      mcp_parse_client_capabilities(
          LEGACY,
          parse_json(R"({"elicitation":{"form":{},"url":{},"custom":{}}})")))};
  EXPECT_EQ(downgraded.at("elicitation"), parse_json(R"({"custom":{}})"));
  auto modern{mcp_parse_client_capabilities(
      MCPProtocolVersion::V_2026_07_28,
      parse_json(
          R"({"extensions":{"org.example/":{}},"experimental":{"custom":{}}})"))};
  modern.extensions.reset();
  modern.experimental.reset();
  EXPECT_TRUE(mcp_serialize_client_capabilities(
                  MCPProtocolVersion::V_2026_07_28, modern)
                  .empty());
}

TEST(review_server_capability_roundtrip_and_overrides) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2025_11_25};
  const auto source{parse_json(
      R"({"resources":{"subscribe":true,"listChanged":false,"custom":{}},"tools":{"listChanged":true},"prompts":{},"logging":{"custom":{}},"completions":{},"experimental":{"custom":{}},"tasks":{"list":{}},"org.example/custom":{}})")};
  auto parsed{mcp_parse_server_capabilities(VERSION, source)};
  EXPECT_EQ(mcp_serialize_server_capabilities(VERSION, parsed), source);
  parsed.resources_subscribe = false;
  parsed.tools = false;
  parsed.tools_list_changed = false;
  parsed.experimental.reset();
  const auto result{mcp_serialize_server_capabilities(VERSION, parsed)};
  EXPECT_FALSE(result.at("resources").defines("subscribe"));
  EXPECT_TRUE(result.at("resources").defines("custom"));
  EXPECT_FALSE(result.defines("tools"));
  EXPECT_FALSE(result.defines("experimental"));
  EXPECT_TRUE(result.defines("tasks"));
  const auto modern{mcp_serialize_server_capabilities(
      MCPProtocolVersion::V_2026_07_28, parsed)};
  EXPECT_FALSE(modern.defines("tasks"));
  EXPECT_TRUE(modern.defines("org.example/custom"));
}

TEST(review_exact_parameter_integers) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};
  for (const auto *const text :
       {"42.0", "4.2e1", "9007199254740991.0", "-9007199254740991.0", "0.0"}) {
    auto arguments{JSON::make_object()};
    arguments.assign("value", parse_json(text));
    const auto headers{mcp_make_parameter_headers(descriptors, arguments)};
    EXPECT_EQ(headers.size(), 1);
    EXPECT_EQ(headers.front().second,
              std::to_string(arguments.at("value").as_integer()));
    const std::array<std::pair<JSON::StringView, JSON::StringView>, 1> views{
        {{headers.front().first, headers.front().second}}};
    EXPECT_FALSE(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                                arguments, views)
                     .has_value());
  }
  for (const auto *const text :
       {"42.00000000000000000001", "9007199254740991.1", "-9007199254740991.1",
        "9007199254740992", "-9007199254740992", "1e100", "1e-100"}) {
    auto arguments{JSON::make_object()};
    arguments.assign("value", parse_json(text));
    bool rejected = false;
    try {
      mcp_make_parameter_headers(descriptors, arguments);
    } catch (const std::invalid_argument &) {
      rejected = true;
    }
    EXPECT_TRUE(rejected);
    EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                             arguments, {})
                  ->at("error")
                  .at("code"),
              JSON{-32602});
  }
}

TEST(review_logging_thresholds_and_stream_scope) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  auto parameters{parse_json(
      R"({"_meta":{"io.modelcontextprotocol/protocolVersion":"2026-07-28","io.modelcontextprotocol/clientCapabilities":{},"io.modelcontextprotocol/logLevel":"warning"}})")};
  const auto context{mcp_validate_request_parameters(parameters).second};
  EXPECT_TRUE(context.has_value());
  constexpr std::array<MCPLogLevel, 8> LEVELS{
      {MCPLogLevel::Debug, MCPLogLevel::Info, MCPLogLevel::Notice,
       MCPLogLevel::Warning, MCPLogLevel::Error, MCPLogLevel::Critical,
       MCPLogLevel::Alert, MCPLogLevel::Emergency}};
  for (const auto threshold : LEVELS) {
    auto current_context{*context};
    current_context.log_level = mcp_log_level_string(threshold);
    for (const auto level : LEVELS) {
      EXPECT_EQ(mcp_resolve_log_level(mcp_log_level_string(level)), level);
      auto payload{parse_json(R"({"data":"hello"})")};
      payload.assign("level", JSON{mcp_log_level_string(level)});
      bool rejected = false;
      try {
        mcp_make_notification(VERSION, true, "notifications/message", payload,
                              std::nullopt, nullptr, &current_context);
      } catch (const std::invalid_argument &) {
        rejected = true;
      }
      EXPECT_EQ(rejected, level < threshold);
    }
  }
  for (const auto subscribed : {false, true}) {
    auto bad_context{*context};
    if (!subscribed) {
      bad_context.log_level = "verbose";
    }
    bool rejected = false;
    try {
      mcp_make_notification(
          VERSION, true, "notifications/message",
          parse_json(R"({"level":"emergency","data":"hello"})"),
          subscribed ? std::optional<JSON>{JSON{1}} : std::nullopt, nullptr,
          &bad_context);
    } catch (const std::invalid_argument &) {
      rejected = true;
    }
    EXPECT_TRUE(rejected);
  }
}

TEST(review_legacy_parameter_structure_and_borrowed_name) {
  using namespace sourcemeta::core;
  constexpr std::array<JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  for (const auto wire : SUPPORTED) {
    const auto version{*mcp_resolve_protocol_version(wire)};
    auto envelope{
        parse_json(R"({"jsonrpc":"2.0","id":0,"method":"tools/list"})")};
    EXPECT_FALSE(mcp_validate_request_headers(version, std::nullopt, "ignored",
                                              "ignored", envelope, SUPPORTED)
                     .has_value());
    for (const auto *const text :
         {"false", "[]", "null", R"({"_meta":true})"}) {
      envelope.assign("params", parse_json(text));
      EXPECT_EQ(mcp_validate_request_headers(version, std::nullopt,
                                             std::nullopt, std::nullopt,
                                             envelope, SUPPORTED)
                    ->at("error")
                    .at("code"),
                JSON{-32602});
    }
  }
  const auto named{
      parse_json(R"({"method":"tools/call","params":{"name":"borrowed"}})")};
  const auto name{mcp_request_name_from_body(named)};
  EXPECT_EQ(name->data(), named.at("params").at("name").to_string().data());
}

TEST(review_result_type_recognition) {
  using namespace sourcemeta::core;
  constexpr std::array<JSON::StringView, 1> EXTENSIONS{{"org.example/custom"}};
  const auto custom{parse_json(R"({"resultType":"org.example/custom"})")};
  const auto interim{parse_json(R"({"resultType":"input_required"})")};
  for (const auto version :
       {MCPProtocolVersion::V_2025_03_26, MCPProtocolVersion::V_2025_06_18,
        MCPProtocolVersion::V_2025_11_25, MCPProtocolVersion::V_2026_07_28}) {
    EXPECT_EQ(mcp_resolve_result_type(version, JSON::make_object()).value(),
              "complete");
    EXPECT_FALSE(mcp_resolve_result_type(version, custom).has_value());
    EXPECT_EQ(mcp_resolve_result_type(version, custom, EXTENSIONS).has_value(),
              version == MCPProtocolVersion::V_2026_07_28);
    EXPECT_EQ(mcp_resolve_result_type(version, interim).has_value(),
              version == MCPProtocolVersion::V_2026_07_28);
    EXPECT_FALSE(mcp_resolve_result_type(version, JSON{false}).has_value());
  }
}

namespace {
using namespace sourcemeta::core;
constexpr std::array<MCPProtocolVersion, 4> REVISIONS{
    {MCPProtocolVersion::V_2025_03_26, MCPProtocolVersion::V_2025_06_18,
     MCPProtocolVersion::V_2025_11_25, MCPProtocolVersion::V_2026_07_28}};
constexpr std::array<JSON::StringView, 4> WIRES{
    {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
constexpr auto CURRENT{MCPProtocolVersion::V_2026_07_28};

template <typename Callback> void rejects(Callback &&callback) {
  bool rejected = false;
  try {
    std::forward<Callback>(callback)();
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

auto list_result(const JSON::StringView method,
                 const MCPProtocolVersion version, JSON entries) -> JSON {
  if (method == "resources/list") {
    return mcp_make_resources_list_result(version, std::move(entries),
                                          std::nullopt, {});
  }
  if (method == "resources/templates/list") {
    return mcp_make_resource_templates_list_result(version, std::move(entries),
                                                   std::nullopt, {});
  }
  if (method == "resources/read") {
    return mcp_make_resources_read_result(version, std::move(entries), {});
  }
  if (method == "prompts/list") {
    return mcp_make_prompts_list_result(version, std::move(entries),
                                        std::nullopt, {});
  }
  return mcp_make_tools_list_result(version, std::move(entries), std::nullopt,
                                    {});
}

auto sampling_result(const JSON &messages,
                     const MCPClientCapabilities &capabilities) -> JSON {
  auto requests{parse_json(
      R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":1}}})")};
  requests.at("r").at("params").assign("messages", messages);
  return mcp_make_input_required_result(CURRENT, "tools/call", JSON{1},
                                        requests, std::nullopt, capabilities);
}
} // namespace

TEST(conformance_invalid_success_ids) {
  for (const auto version : REVISIONS) {
    for (const auto &identifier : {JSON{nullptr}, JSON{true}, JSON{1.5},
                                   JSON::make_array(), JSON::make_object()}) {
      rejects([&] { mcp_make_empty_result(version, identifier); });
      rejects([&] { mcp_make_tool_error(version, identifier, "error"); });
    }
    EXPECT_EQ(mcp_make_empty_result(version, JSON{0}).at("id"), JSON{0});
    EXPECT_EQ(mcp_make_empty_result(version, JSON{""}).at("id"), JSON{""});
  }
}

TEST(conformance_errors_and_transport_context) {
  const JSON bad{true};
  for (const auto version : REVISIONS) {
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
    mcp_make_error(CURRENT, nullptr, MCP_CODE_RESOURCE_NOT_FOUND, "retired");
  });
  rejects([] {
    mcp_make_error(CURRENT, nullptr, MCP_CODE_URL_ELICITATION_REQUIRED,
                   "retired");
  });
}

TEST(conformance_initialization_supported_subset) {
  const std::array<JSON::StringView, 1> only{{"2025-03-26"}};
  const auto init{parse_json(
      R"({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"protocolVersion":"unknown","capabilities":{},"clientInfo":{"name":"client","version":"1"}}})")};
  const auto result{mcp_make_initialize_result(
      init, {}, {.name = "server", .version = "1"}, only)};
  EXPECT_EQ(result.at("result").at("protocolVersion"), JSON{"2025-03-26"});
  auto invalid{init};
  invalid.at("params").erase("clientInfo");
  EXPECT_EQ(mcp_make_initialize_result(invalid, {}, {"server", "1"}, only)
                .at("error")
                .at("code"),
            JSON{-32602});
  invalid.assign("id", JSON{});
  rejects([&] {
    mcp_make_initialize_result(invalid, {}, {.name = "server", .version = "1"},
                               only);
  });
  const std::array<JSON::StringView, 1> modern_only{{"2026-07-28"}};
  rejects([&] {
    mcp_make_initialize_result(init, {}, {.name = "server", .version = "1"},
                               modern_only);
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
  for (const auto *const text :
       {R"({"roots":true})", R"({"sampling":{"tools":false}})",
        R"({"elicitation":{"url":true}})", R"({"extensions":{"bad":{}}})",
        R"({"experimental":{"extension":false}})"}) {
    value = parameters();
    value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                             parse_json(text));
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
    rejects([&] { mcp_parse_client_capabilities(CURRENT, parse_json(text)); });
  }
  for (const auto *const text :
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
  for (const auto *const key : {"traceparent", "tracestate", "baggage"}) {
    value = parameters();
    value.at("_meta").assign(key, JSON{false});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  for (const auto *const parent :
       {"00-00000000000000000000000000000000-0123456789abcdef-01",
        "00-0123456789abcdef0123456789abcdef-0000000000000000-01",
        "ff-0123456789abcdef0123456789abcdef-0123456789abcdef-01",
        "00-0123456789ABCDEF0123456789abcdef-0123456789abcdef-01",
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
                           JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      JSON{"\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  for (const auto *const state : {"a=b,a=c", "UPPER=value", "a=value=wrong"}) {
    value.at("_meta").assign("tracestate", JSON{state});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  for (const auto *const text : {"a=%ZZ", "a=%", "a=%2", "a=value\n"}) {
    value = parameters();
    value.at("_meta").assign("baggage", JSON{text});
    EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
  }
  // These tracing names are not reserved fields in legacy result metadata.
  const auto legacy{parse_json(R"({"_meta":{"traceparent":false}})")};
  EXPECT_EQ(mcp_decorate_result(MCPProtocolVersion::V_2025_03_26, legacy),
            legacy);
  rejects([&] { mcp_decorate_result(CURRENT, legacy); });
}

TEST(conformance_header_error_ordering) {
  auto value{request()};
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, std::nullopt, "tools/list",
                                         std::nullopt, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32020});
  value.at("params").erase("_meta");
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, std::nullopt, std::nullopt,
                                         std::nullopt, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32602});
  value = request();
  value.at("params").at("_meta").assign(
      "io.modelcontextprotocol/protocolVersion", JSON{"unknown"});
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/list",
                                         std::nullopt, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32020});
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, "unknown", "tools/list",
                                         std::nullopt, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32022});
  constexpr std::array<JSON::StringView, 1> DISABLED{{"2025-11-25"}};
  value = request();
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/list",
                                         std::nullopt, value, DISABLED)
                ->at("error")
                .at("code"),
            JSON{-32022});
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/list",
                                         "unexpected", value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32020});
  value.assign("id", JSON{true});
  const auto invalid{mcp_validate_request_headers(
      MCPProtocolVersion::V_2025_03_26, std::nullopt, std::nullopt,
      std::nullopt, value, WIRES)};
  EXPECT_TRUE(invalid->at("id").is_null());
}

TEST(conformance_parameter_header_roundtrip) {
  const std::array<MCPHeaderParameter, 3> descriptors{{
      {.name = "Label", .path = {"nested", "text"}, .type = JSON::Type::String},
      {.name = "Count", .path = {"number"}, .type = JSON::Type::Integer},
      {.name = "Enabled", .path = {"enabled"}, .type = JSON::Type::Boolean},
  }};
  const auto args{parse_json(
      R"({"nested":{"text":" café "},"number":42,"enabled":false})")};
  const auto headers{mcp_make_parameter_headers(descriptors, args)};
  std::vector<std::pair<JSON::StringView, JSON::StringView>> views;
  views.reserve(headers.size());
  for (const auto &header : headers) {
    views.emplace_back(header.first, header.second);
  }
  EXPECT_FALSE(
      mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors, args, views)
          .has_value());
  for (auto &header : views) {
    if (header.first == "Mcp-Param-Count") {
      header.second = "42.0";
    }
  }
  EXPECT_FALSE(
      mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors, args, views)
          .has_value());
  views.emplace_back("mcp-param-count", "42");
  EXPECT_EQ(
      mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors, args, views)
          ->at("error")
          .at("code"),
      JSON{-32020});
  EXPECT_TRUE(
      mcp_make_parameter_headers(descriptors, parse_json(R"({"number":null})"))
          .empty());
  rejects([&] {
    mcp_make_parameter_headers(descriptors,
                               parse_json(R"({"number":9007199254740992})"));
  });
  rejects([&] {
    mcp_make_parameter_headers(descriptors, parse_json(R"({"number":1.5})"));
  });
}

TEST(conformance_parameter_header_descriptor_rejections) {
  for (const auto &parameter :
       {MCPHeaderParameter{
            .name = "Value", .path = {}, .type = JSON::Type::String},
        MCPHeaderParameter{
            .name = "Value", .path = {"x"}, .type = JSON::Type::Real}}) {
    const std::array<MCPHeaderParameter, 1> descriptors{{parameter}};
    rejects(
        [&] { mcp_make_parameter_headers(descriptors, JSON::make_object()); });
    rejects([&] {
      mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors,
                                     JSON::make_object(), {});
    });
  }
  const std::array<MCPHeaderParameter, 2> duplicates{{
      {.name = "Value", .path = {"x"}, .type = JSON::Type::String},
      {.name = "value", .path = {"y"}, .type = JSON::Type::String},
  }};
  rejects([&] { mcp_make_parameter_headers(duplicates, JSON::make_object()); });
  rejects([&] {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, duplicates,
                                   JSON::make_object(), {});
  });
  const std::array<MCPHeaderParameter, 1> forged{
      {{.name = "bad\r\nInjected", .path = {"x"}, .type = JSON::Type::String}}};
  rejects([&] { mcp_make_parameter_headers(forged, JSON::make_object()); });
  rejects([&] {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, forged,
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
  for (const auto *const text :
       {R"({"resultType":"input_required","requestState":"state"})",
        R"({"resultType":"unknown"})"}) {
    rejects([&] { mcp_decorate_result(CURRENT, parse_json(text)); });
    rejects([&] {
      mcp_decorate_cacheable_result(CURRENT, parse_json(text), {},
                                    "resources/read");
    });
  }
  rejects([] {
    mcp_decorate_cacheable_result(CURRENT, JSON::make_object(), {},
                                  "tools/call");
  });
  EXPECT_EQ(mcp_decorate_cacheable_result(CURRENT, JSON::make_object(),
                                          {-4, MCPCacheScope::Private},
                                          "resources/read")
                .at("ttlMs"),
            JSON{0});
  const auto invalid_info{parse_json(
      R"({"_meta":{"io.modelcontextprotocol/serverInfo":{"name":"server"}}})")};
  rejects([&] { mcp_decorate_result(CURRENT, invalid_info); });
}

TEST(conformance_content_and_schemas) {
  for (const auto version : REVISIONS) {
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
    EXPECT_EQ(mcp_make_tool_success(CURRENT, JSON{0}, value)
                  .at("result")
                  .at("structuredContent"),
              value);
  }
  auto many{JSON::make_array()};
  for (std::size_t index = 0; index < 101; ++index) {
    many.push_back(JSON{"value"});
  }
  rejects([&] {
    mcp_make_completion_result(CURRENT, many, std::nullopt, std::nullopt);
  });
  rejects([] {
    mcp_make_completion_result(CURRENT, parse_json(R"(["value"] )"), 0, false);
  });
  for (const auto version :
       {MCPProtocolVersion::V_2025_03_26, MCPProtocolVersion::V_2025_06_18,
        MCPProtocolVersion::V_2025_11_25, MCPProtocolVersion::V_2026_07_28}) {
    rejects([&] {
      mcp_make_completion_result(version, JSON::make_array(), -1, false);
    });
    EXPECT_EQ(mcp_make_completion_result(version, JSON::make_array(), 0, false)
                  .at("completion")
                  .at("total"),
              JSON{0});
    EXPECT_EQ(mcp_make_completion_result(version, parse_json(R"(["value"])"), 1,
                                         false)
                  .at("completion")
                  .at("total"),
              JSON{1});
    constexpr auto MAXIMUM{std::numeric_limits<std::int64_t>::max()};
    EXPECT_EQ(mcp_make_completion_result(version, parse_json(R"(["value"])"),
                                         MAXIMUM, false)
                  .at("completion")
                  .at("total"),
              JSON{MAXIMUM});
  }
}

TEST(conformance_mrtr_nested_requests) {
  MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  capabilities.elicitation_url = true;
  capabilities.elicitation_form = true;
  for (
      const auto *const text :
      {R"({"r":{"method":"roots/list"}})",
       R"({"r":{"method":"elicitation/create","params":{"mode":"url","message":"visit","url":"https://example.com"}}})",
       R"({"r":{"method":"elicitation/create","params":{"message":"choose","requestedSchema":{"type":"object","properties":{"choice":{"type":"array","items":{"type":"string","enum":["a"]}}}}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[{"role":"assistant","content":{"type":"tool_use","id":"call","name":"tool","input":{}}},{"role":"user","content":{"type":"tool_result","toolUseId":"call","content":[{"type":"text","text":"ok"}]}}],"tools":[{"name":"tool","inputSchema":{"type":"object"}}],"toolChoice":{"mode":"auto"}}}})"}) {
    const auto result{mcp_make_input_required_result(
        CURRENT, "tools/call", JSON{0}, parse_json(text), std::nullopt,
        capabilities)};
    EXPECT_TRUE(result.at("result").is_object());
  }
  for (
      const auto *const text :
      {R"({"r":{"method":"roots/list","params":false}})",
       R"({"r":{"method":"elicitation/create","params":{"message":"choose","requestedSchema":{"type":"array"}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[{"role":"user","content":{"type":"resource_link","uri":"x","name":"x"}}]}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"temperature":false}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"modelPreferences":{"costPriority":2}}}})",
       R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":10,"messages":[],"toolChoice":{"mode":"bad"}}}})"}) {
    rejects([&] {
      mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                     parse_json(text), std::nullopt,
                                     capabilities);
    });
  }
  rejects([&] {
    mcp_make_input_required_result(CURRENT, "tools/list", JSON{0}, std::nullopt,
                                   "token", capabilities);
  });
  for (const auto version : REVISIONS) {
    if (version != CURRENT) {
      rejects([&] {
        mcp_make_input_required_result(version, "tools/call", JSON{0},
                                       std::nullopt, "token", capabilities);
      });
    }
  }
  const auto continuation{parse_json(
      R"({"jsonrpc":"2.0","id":0,"method":"tools/call","params":{"requestState":"s","inputResponses":{}}})")};
  EXPECT_TRUE(mcp_request_state(CURRENT, continuation).has_value());
  EXPECT_FALSE(mcp_request_state(MCPProtocolVersion::V_2025_11_25, continuation)
                   .has_value());
  auto unrelated{continuation};
  unrelated.assign("method", JSON{"tools/list"});
  EXPECT_EQ(mcp_request_input_responses(CURRENT, unrelated), nullptr);
}

TEST(conformance_subscription_filters_and_notifications) {
  auto value{parameters()};
  value.assign(
      "notifications",
      parse_json(
          R"({"toolsListChanged":true,"resourcesListChanged":false,"resourceSubscriptions":["a","b"]})"));
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
  rejects([&] {
    mcp_make_notification(CURRENT, true, "notifications/resources/updated",
                          parse_json(R"({"uri":"a"})"), JSON{0}, &allowed);
  });
  rejects([&] {
    mcp_make_notification(CURRENT, true, "notifications/tools/list_changed",
                          JSON::make_object(), std::nullopt, &allowed);
  });
  rejects([&] {
    mcp_make_notification(CURRENT, true, "notifications/prompts/list_changed",
                          JSON::make_object(), JSON{0}, &allowed);
  });
  rejects([] {
    mcp_make_notification(CURRENT, true, "notifications/cancelled",
                          parse_json(R"({"requestId":1})"), JSON{0});
  });
  rejects([] {
    mcp_make_notification(CURRENT, true, "notifications/roots/list_changed",
                          JSON::make_object(), std::nullopt);
  });
}

TEST(conformance_notification_shapes) {
  auto opts{parameters()};
  opts.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"info"});
  opts.at("_meta").assign("progressToken", JSON{0});
  const auto context{mcp_validate_request_parameters(opts)};
  for (const auto version : REVISIONS) {
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

TEST(conformance_result_writer) {
  constexpr auto *FIXTURE{
      R"({"resources":[{"name":"test","uri":"https://example.com/a"}],"nextCursor":"","_meta":{"example.org/value":"preserved"}})"};
  const auto page{parse_json(FIXTURE)};
  // Parse a separate snapshot so the comparison detects source mutation.
  const auto saved{parse_json(FIXTURE)};
  for (const auto version : REVISIONS) {
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
    EXPECT_TRUE(parsed.at("result").is_object());
  }
  std::ostringstream stream;
  rejects([&] {
    mcp_write_result(stream, CURRENT, "resources/list", JSON{0}, page,
                     std::nullopt);
  });
  rejects([&] {
    mcp_write_result(stream, CURRENT, "tools/call", JSON{0}, page,
                     MCPCachePolicy{});
  });
}

TEST(conformance_builder_results) {
  const JSON identifier{0};
  const MCPImplementation server{.name = "server",
                                 .version = "1",
                                 .title = "Title",
                                 .description = "Description",
                                 .website_url = "https://example.com"};
  for (const auto version : REVISIONS) {
    EXPECT_TRUE(
        mcp_make_empty_result(version, identifier).at("result").is_object());
    EXPECT_TRUE(mcp_make_tool_success(version, identifier, JSON::make_object())
                    .at("result")
                    .is_object());
    EXPECT_TRUE(
        mcp_make_tool_success(version, identifier, JSON::make_object(),
                              parse_json(R"([{"type":"text","text":"hello"}])"))
            .at("result")
            .is_object());
    EXPECT_TRUE(mcp_make_tool_error(version, identifier, "failure")
                    .at("result")
                    .is_object());
    EXPECT_TRUE(mcp_make_tool_descriptor(
                    version, "tool", "description",
                    parse_json(R"({"type":"object","properties":{}})"))
                    .is_object());
    EXPECT_TRUE(mcp_make_resource(version, "https://example.com", "resource",
                                  "text/plain", "", 10, 0.5)
                    .is_object());
    EXPECT_TRUE(
        mcp_make_resource_template(version, "https://example.com/{name}",
                                   "template", "description", "text/plain")
            .is_object());
    EXPECT_TRUE(
        (mcp_supports_resource_link_content(version)
             ? mcp_make_resource_link(version, "https://example.com", "link")
             : mcp_make_text_block("link"))
            .is_object());
    EXPECT_TRUE(mcp_make_tools_list_result(version, JSON::make_array(), "", {})
                    .is_object());
    EXPECT_TRUE(
        mcp_make_resources_list_result(version, JSON::make_array(), "", {})
            .is_object());
    EXPECT_TRUE(mcp_make_resource_templates_list_result(
                    version, JSON::make_array(), "", {})
                    .is_object());
    EXPECT_TRUE(
        mcp_make_prompts_list_result(version, JSON::make_array(), "", {})
            .is_object());
    EXPECT_TRUE(
        mcp_make_resources_read_result(
            version,
            parse_json(R"([{"uri":"https://example.com","text":"hello"}])"), {})
            .is_object());
    EXPECT_TRUE(
        mcp_make_prompts_get_result(
            version, "description",
            parse_json(
                R"([{"role":"user","content":{"type":"text","text":"hello"}}])"))
            .is_object());
    EXPECT_TRUE(mcp_make_completion_result(
                    version, parse_json(R"(["one","two"] )"), 3, true)
                    .is_object());
    EXPECT_TRUE(mcp_serialize_client_capabilities(version, {}).is_object());
    const auto error{mcp_make_error(version, &identifier,
                                    JSONRPC_CODE_INVALID_PARAMS, "bad")};
    EXPECT_TRUE(error.is_object());
    EXPECT_TRUE(
        mcp_make_error_resource_not_found(version, identifier).is_object());
    if (mcp_uses_initialization_handshake(version)) {
      auto init{parse_json(
          R"({"jsonrpc":"2.0","id":0,"method":"initialize","params":{"capabilities":{},"clientInfo":{"name":"client","version":"1"}}})")};
      init.at("params").assign("protocolVersion",
                               JSON{mcp_protocol_version_string(version)});
      EXPECT_TRUE(mcp_make_initialize_result(init, {}, server, WIRES)
                      .at("result")
                      .is_object());
    }
  }
  EXPECT_TRUE(mcp_make_server_discover_result(CURRENT, identifier, {}, server,
                                              WIRES, "instructions", {})
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_subscription_close_result(CURRENT, identifier)
                  .at("result")
                  .is_object());
  EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "resources/read",
                                             identifier, std::nullopt, "state",
                                             {})
                  .at("result")
                  .is_object());
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
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, "state"));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, "different"));
  EXPECT_FALSE(mcp_validate_continuation(MCPProtocolVersion::V_2025_11_25,
                                         value, prior, "state"));
  auto bad{value};
  bad.at("inputResponses")
      .at("roots")
      .at("roots")
      .at(0)
      .assign("uri", JSON{"https://example.com"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("form").assign("action", JSON{"unknown"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.at("inputResponses").at("sample").erase("model");
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.assign("inputResponses", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, "state"));
  bad = value;
  bad.assign("requestState", JSON{1});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, bad, prior, std::nullopt));
  EXPECT_FALSE(
      mcp_validate_continuation(CURRENT, parameters(), prior, std::nullopt));
}

TEST(continuation_empty_expected_state) {
  auto value{parameters()};
  value.assign("inputResponses", JSON::make_object());
  const auto prior{JSON::make_object()};
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, ""));
  value.assign("requestState", JSON{""});
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, ""));
  value.assign("requestState", JSON{"different"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, ""));
}

TEST(tool_schema_content_is_opaque) {
  const auto schema{parse_json(
      R"({"type":"object","$defs":{"value":{"type":"string"}},"properties":{"name":{"$ref":"#/$defs/value"}},"examples":[{"x-mcp-header":"Not an annotation","properties":{"type":"invalid"}}]})")};
  for (const auto version : REVISIONS) {
    EXPECT_EQ(
        mcp_make_tool_descriptor(version, "tool", "", schema).at("inputSchema"),
        schema);
    for (const auto *const text :
         {"false", "null", "[]", "{}", R"({"type":"array"})"}) {
      rejects([&] {
        mcp_make_tool_descriptor(version, "tool", "", parse_json(text));
      });
    }
  }
}

TEST(conformance_encoded_header_boundaries) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  for (const auto *const header :
       {"=?base64?!!!!?=", "=?base64?/w==?=", " café", "café", "cafe\n"}) {
    EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28",
                                             "tools/call", header, value, WIRES)
                    .has_value());
  }
  for (const auto *const text :
       {"=?base64?YWJj?=", " padded ", "café", "a\tb"}) {
    value.at("params").assign("name", JSON{text});
    const auto encoded{mcp_encode_header_value(text)};
    EXPECT_FALSE(mcp_validate_request_headers(
                     CURRENT, "2026-07-28", "tools/call", encoded, value, WIRES)
                     .has_value());
  }
  rejects([] { mcp_encode_header_value(std::string_view{"\xff", 1}); });
}

TEST(review_elicitation_numbers_and_mode) {
  auto value{parameters()};
  auto prior{parse_json(
      R"({"form":{"method":"elicitation/create","params":{"mode":"form","message":"value","requestedSchema":{"type":"object","properties":{}}}}})")};
  value.assign(
      "inputResponses",
      parse_json(
          R"({"form":{"action":"accept","content":{"value":1.25,"precise":1.0000000000000000001}}})"));
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  prior.at("form").at("params").assign("mode", JSON{"url"});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  value.at("inputResponses").at("form").erase("content");
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  prior.at("form").at("params").assign("mode", JSON{"form"});
  for (const auto *const action : {"decline", "cancel"}) {
    value.at("inputResponses").at("form").assign("action", JSON{action});
    EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
    value.at("inputResponses")
        .at("form")
        .assign("content", JSON::make_object());
    EXPECT_FALSE(
        mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
    value.at("inputResponses").at("form").erase("content");
  }
  MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  for (const auto *const tokens : {"100.0", "0", "-1"}) {
    auto inputs{parse_json(
        R"({"sample":{"method":"sampling/createMessage","params":{"messages":[],"maxTokens":0}}})")};
    inputs.at("sample").at("params").assign("maxTokens", parse_json(tokens));
    EXPECT_TRUE(mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                               inputs, std::nullopt,
                                               capabilities)
                    .at("result")
                    .defines("inputRequests"));
  }
}

TEST(review_descriptor_and_content) {
  constexpr std::array<JSON::StringView, 2> ROLES{{"user", "assistant"}};
  const auto icons{parse_json(
      R"([{"src":"https://example.com/icon.png","mimeType":"image/png","sizes":["32x32"],"theme":"dark"}])")};
  const MCPDescriptorPresentation presentation{.title = "Title",
                                               .icons = &icons};
  const MCPResourceAnnotations annotations{.audience = ROLES,
                                           .priority = 0.5,
                                           .last_modified =
                                               "2026-07-28T00:00:00Z"};
  for (const auto version : REVISIONS) {
    const auto resource{mcp_make_resource(
        version, "file:///a", "a", "text/plain", "description", std::nullopt,
        std::nullopt, presentation, annotations)};
    EXPECT_EQ(resource.defines("title"),
              version != MCPProtocolVersion::V_2025_03_26);
    EXPECT_EQ(resource.defines("icons"),
              mcp_protocol_version_at_least(version,
                                            MCPProtocolVersion::V_2025_11_25));
    EXPECT_EQ(resource.at("annotations").defines("lastModified"),
              version != MCPProtocolVersion::V_2025_03_26);
    EXPECT_TRUE(resource.is_object());
    EXPECT_TRUE(mcp_make_resource_template(
                    version, "file:///{name}", "template", "description",
                    "text/plain", presentation, annotations)
                    .is_object());
    EXPECT_TRUE(mcp_make_tool_descriptor(version, "tool", "description",
                                         parse_json(R"({"type":"object"})"),
                                         std::nullopt, {}, presentation)
                    .is_object());
    EXPECT_TRUE(
        mcp_serialize_resource_annotations(version, annotations).is_object());
    EXPECT_TRUE(mcp_make_image_block(version, "YWJj", "image/png", annotations)
                    .is_object());
    EXPECT_TRUE(mcp_make_audio_block(version, "YWJj", "audio/wav", annotations)
                    .is_object());
    const auto blob{mcp_make_resource_blob_content(
        "file:///a", "application/octet-stream", "YWJj")};
    EXPECT_TRUE(blob.is_object());
    EXPECT_TRUE(
        mcp_make_embedded_resource(version, blob, annotations).is_object());
    EXPECT_TRUE(
        mcp_make_embedded_resource(
            version,
            mcp_make_resource_text_content("file:///a", "text/plain", "hello"),
            annotations)
            .is_object());
    EXPECT_TRUE(
        mcp_serialize_server_capabilities(
            version,
            mcp_parse_server_capabilities(
                version,
                parse_json(
                    R"({"resources":{"subscribe":true,"listChanged":false,"custom":{}},"logging":{},"completions":{}})")))
            .is_object());
  }
  const std::array<JSON::StringView, 1> invalid{{"system"}};
  rejects([&] {
    mcp_serialize_resource_annotations(
        CURRENT,
        {.audience = invalid, .priority = std::nullopt, .last_modified = {}});
  });
  rejects([&] {
    mcp_make_embedded_resource(CURRENT, parse_json(R"({"uri":"file:///a"})"));
  });
  const auto bad_icons{parse_json(R"([{"src":true}])")};
  rejects([&] {
    mcp_make_tool_descriptor(CURRENT, "tool", "",
                             parse_json(R"({"type":"object"})"), std::nullopt,
                             {}, {.title = {}, .icons = &bad_icons});
  });
  rejects([&] {
    mcp_parse_server_capabilities(
        CURRENT,
        parse_json(
            R"({"resources":{"subscribe":false,"listChanged":"invalid"}})"));
  });
  rejects([&] {
    mcp_serialize_server_capabilities(CURRENT, {.extensions = JSON{false}});
  });
}

TEST(review_result_writer_full_parity) {
  const std::array<std::pair<JSON::StringView, JSON::StringView>, 6> cases{
      {{"tools/list", R"({"tools":[],"nextCursor":""})"},
       {"resources/list", R"({"resources":[],"nextCursor":""})"},
       {"resources/templates/list",
        R"({"resourceTemplates":[],"nextCursor":""})"},
       {"prompts/list", R"({"prompts":[],"nextCursor":""})"},
       {"resources/read",
        R"({"contents":[{"uri":"file:///a","text":"hello","blob":"YWJj"}]})"},
       {"server/discover",
        R"({"supportedVersions":["2026-07-28"],"capabilities":{},"serverInfo":{"name":"server","version":"1"}})"}}};
  const JSON identifier{"escaped\"\\\n"};
  const MCPImplementation server{.name = "server", .version = "1"};
  for (const auto version : REVISIONS) {
    for (const auto &entry : cases) {
      if (entry.first == "server/discover" &&
          !mcp_supports_server_discover(version)) {
        continue;
      }
      for (const auto decorated : {false, true}) {
        auto source{parse_json(entry.second)};
        source.assign("_meta",
                      parse_json(R"({"org.example/custom":{"value":true}})"));
        if (decorated && version == CURRENT) {
          source = mcp_decorate_result(version, std::move(source), server);
          source = mcp_decorate_cacheable_result(
              version, std::move(source),
              {.ttl_ms = 5, .scope = MCPCacheScope::Public}, entry.first);
        }
        const auto before{source};
        const auto copied{jsonrpc_make_success(
            identifier,
            mcp_decorate_cacheable_result(
                version, mcp_decorate_result(version, source, server),
                {.ttl_ms = 123, .scope = MCPCacheScope::Private},
                entry.first))};
        std::ostringstream stream;
        mcp_write_result(
            stream, version, entry.first, identifier, source,
            version == CURRENT
                ? std::optional<MCPCachePolicy>{{.ttl_ms = 123,
                                                 .scope =
                                                     MCPCacheScope::Private}}
                : std::nullopt,
            server);
        EXPECT_EQ(parse_json(stream.str()), copied);
        EXPECT_EQ(source, before);
      }
    }
  }
  for (const auto *const text :
       {R"({"resources":false})", R"({"resources":[],"nextCursor":false})",
        R"({"resources":[],"_meta":false})",
        R"({"resources":[],"resultType":"input_required"})"}) {
    std::ostringstream stream;
    rejects([&] {
      mcp_write_result(stream, CURRENT, "resources/list", identifier,
                       parse_json(text), MCPCachePolicy{});
    });
    EXPECT_TRUE(stream.str().empty());
  }
}

TEST(review_disputed_valid_inputs) {
  auto value{parameters()};
  for (const auto *const text : {"", " , ", "vendor=value,,"}) {
    value.at("_meta").assign("tracestate", JSON{text});
    EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  }
  value.at("_meta").erase("tracestate");
  value.at("_meta").assign("baggage", JSON{"key=value;flag"});
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           parse_json(R"({"extensions":{"org.example/":{}}})"));
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(conformance_notification_opt_in) {
  rejects([] {
    mcp_make_notification(CURRENT, true, "notifications/message",
                          parse_json(R"({"level":"info","data":"hello"})"),
                          std::nullopt);
  });
  rejects([] {
    mcp_make_notification(CURRENT, true, "notifications/progress",
                          parse_json(R"({"progressToken":0,"progress":1})"),
                          std::nullopt);
  });
  const auto cancelled{mcp_make_notification(
      CURRENT, true, "notifications/cancelled",
      parse_json(R"({"requestId":"listen"})"), JSON{"listen"})};
  EXPECT_FALSE(cancelled.defines("id"));
  EXPECT_TRUE(cancelled.is_object());
  const auto initialized{mcp_make_notification(
      MCPProtocolVersion::V_2025_11_25, false, "notifications/initialized",
      JSON::make_object(), std::nullopt)};
  EXPECT_FALSE(initialized.defines("id"));
  rejects([] {
    mcp_make_notification(CURRENT, false, "notifications/initialized",
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
  EXPECT_TRUE(source.is_object());
  for (const auto *const text :
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
      mcp_validate_request_headers(CURRENT, headers, value, WIRES).has_value());
  headers.emplace_back("Mcp-Method", "tools/list");
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, headers, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32020});
  const auto legacy{MCPProtocolVersion::V_2025_11_25};
  headers.front().second = "2025-11-25";
  EXPECT_FALSE(
      mcp_validate_request_headers(legacy, headers, value, WIRES).has_value());
}

TEST(conformance_stateless_revision_and_missing_capabilities) {
  auto value{request()};
  value.at("params").at("_meta").assign(
      "io.modelcontextprotocol/protocolVersion", JSON{"2025-11-25"});
  EXPECT_FALSE(mcp_validate_request_meta(value).second.has_value());
  value = request();
  value.at("params").at("_meta").erase(
      "io.modelcontextprotocol/clientCapabilities");
  EXPECT_EQ(mcp_validate_request_headers(CURRENT, std::nullopt, std::nullopt,
                                         std::nullopt, value, WIRES)
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(conformance_result_writer_metadata_rejections) {
  std::ostringstream stream;
  const auto bad{parse_json(
      R"({"resources":[],"_meta":{"io.modelcontextprotocol/serverInfo":{"name":"missing-version"}}})")};
  rejects([&] {
    mcp_write_result(stream, CURRENT, "resources/list", JSON{0}, bad,
                     MCPCachePolicy{});
  });
  EXPECT_TRUE(stream.str().empty());
  const auto discovery{
      parse_json(R"({"supportedVersions":["2025-11-25"],"capabilities":{}})")};
  rejects([&] {
    mcp_write_result(stream, CURRENT, "server/discover", JSON{0}, discovery,
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
  const std::array<Entry, 35> entries{{
      {.method = "completion/complete",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "elicitation/create",
       .client_request = 0,
       .server_request = 6,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "initialize",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "logging/setLevel",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/cancelled",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 15,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/elicitation/complete",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 4,
       .input_request = 0},
      {.method = "notifications/initialized",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/message",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/progress",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/prompts/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/resources/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/resources/updated",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "notifications/roots/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 7,
       .server_notification = 0,
       .input_request = 0},
      {.method = "notifications/subscriptions/acknowledged",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 8,
       .input_request = 0},
      {.method = "notifications/tasks/status",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 4,
       .server_notification = 4,
       .input_request = 0},
      {.method = "notifications/tools/list_changed",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 15,
       .input_request = 0},
      {.method = "ping",
       .client_request = 7,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "prompts/get",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "prompts/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/read",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/subscribe",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/templates/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "resources/unsubscribe",
       .client_request = 7,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "roots/list",
       .client_request = 0,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "sampling/createMessage",
       .client_request = 0,
       .server_request = 7,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 8},
      {.method = "server/discover",
       .client_request = 8,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "subscriptions/listen",
       .client_request = 8,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/cancel",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/get",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/list",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tasks/result",
       .client_request = 4,
       .server_request = 4,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tools/call",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "tools/list",
       .client_request = 15,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
      {.method = "extension/unknown",
       .client_request = 0,
       .server_request = 0,
       .client_notification = 0,
       .server_notification = 0,
       .input_request = 0},
  }};
  for (std::size_t index = 0; index < std::size(REVISIONS); ++index) {
    struct Revision {
      MCPProtocolVersion version;
      unsigned bit;
    };
    constexpr std::array<Revision, 4> DATED{
        {{.version = MCPProtocolVersion::V_2025_03_26, .bit = 1},
         {.version = MCPProtocolVersion::V_2025_06_18, .bit = 2},
         {.version = MCPProtocolVersion::V_2025_11_25, .bit = 4},
         {.version = MCPProtocolVersion::V_2026_07_28, .bit = 8}}};
    static_assert(DATED.size() == REVISIONS.size());
    const auto version{DATED[index].version};
    const auto bit{DATED[index].bit};
    EXPECT_EQ(mcp_supports_implementation_website_url(version),
              mcp_protocol_version_at_least(version,
                                            MCPProtocolVersion::V_2025_11_25));
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
  for (const auto *const method :
       {"tools/call", "resources/read", "prompts/get", "resources/subscribe",
        "resources/unsubscribe"}) {
    EXPECT_TRUE(mcp_is_named_request_method(method));
  }
  for (const auto *const method :
       {"tools/list", "resources/list", "server/discover",
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
  EXPECT_EQ(mcp_make_input_required_result(CURRENT, "tools/call", JSON{0},
                                           inputs, std::nullopt, capabilities)
                .at("result")
                .at("inputRequests"),
            inputs);
  const auto prior{
      parse_json(R"({"sample":{"method":"sampling/createMessage"}})")};
  auto value{parameters()};
  value.assign(
      "inputResponses",
      parse_json(
          R"({"sample":{"role":"assistant","model":"model","content":[{"type":"text","text":"answer"},{"type":"tool_use","id":"call-1","name":"tool","input":{}}]}})"));
  EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  value.at("inputResponses")
      .at("sample")
      .at("content")
      .at(1)
      .assign("input", JSON{false});
  EXPECT_FALSE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
}

TEST(conformance_nested_descriptor_metadata) {
  const std::array<std::pair<JSON::StringView, JSON::StringView>, 5> cases{{
      {"resources/list", R"({"resources":[{"name":"r","uri":"file:///r"}]})"},
      {"resources/templates/list",
       R"({"resourceTemplates":[{"name":"r","uriTemplate":"file:///{name}"}]})"},
      {"resources/read",
       R"({"contents":[{"uri":"file:///r","text":"value"}]})"},
      {"prompts/list", R"({"prompts":[{"name":"p"}]})"},
      {"tools/list",
       R"({"tools":[{"name":"t","inputSchema":{"type":"object","properties":{"_meta":{"const":false}}}}]})"},
  }};
  const std::array<JSON::StringView, 5> keys{
      {"resources", "resourceTemplates", "contents", "prompts", "tools"}};
  for (std::size_t index = 0; index < cases.size(); ++index) {
    const auto &[method, fixture]{cases[index]};
    for (const auto *const text :
         {"false", "null", "[]", R"({"invalid key":true})",
          R"({"traceparent":false})", R"({"traceparent":"bad"})",
          R"({"tracestate":[]})", R"({"baggage":null})"}) {
      auto source{parse_json(fixture)};
      const auto meta{parse_json(text)};
      source.at(keys[index]).at(0).assign("_meta", meta);
      rejects([&] { list_result(method, CURRENT, source.at(keys[index])); });
      std::ostringstream stream;
      rejects([&] {
        mcp_write_result(stream, CURRENT, method, JSON{1}, source,
                         MCPCachePolicy{});
      });
      EXPECT_TRUE(stream.str().empty());
      if (meta.is_object()) {
        for (const auto version : REVISIONS) {
          if (version != CURRENT) {
            EXPECT_EQ(list_result(method, version, source.at(keys[index]))
                          .at(keys[index]),
                      source.at(keys[index]));
            std::ostringstream legacy;
            mcp_write_result(legacy, version, method, JSON{1}, source,
                             std::nullopt);
            EXPECT_EQ(parse_json(legacy.str()).at("result").at(keys[index]),
                      source.at(keys[index]));
          }
        }
      }
    }
    for (
        const auto *const text :
        {"{}", R"({"org.example/data":{"_meta":false}})",
         R"({"traceparent":"00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01","tracestate":"vendor=value","baggage":"key=value"})"}) {
      auto source{parse_json(fixture)};
      source.at(keys[index]).at(0).assign("_meta", parse_json(text));
      const auto built{list_result(method, CURRENT, source.at(keys[index]))};
      std::ostringstream stream;
      mcp_write_result(stream, CURRENT, method, JSON{1}, source,
                       MCPCachePolicy{});
      EXPECT_EQ(parse_json(stream.str()).at("result"), built);
    }
  }
  const auto embedded{parse_json(
      R"([{"type":"resource","resource":{"uri":"file:///r","text":"value","_meta":{"invalid key":true}}}])")};
  rejects([&] {
    mcp_make_tool_success(CURRENT, JSON{1}, JSON::make_object(), embedded);
  });
  rejects([&] {
    mcp_make_prompts_get_result(
        CURRENT, "",
        parse_json(
            R"([{"role":"user","content":{"type":"text","text":"value","_meta":false}}])"));
  });
  const auto business{
      parse_json(R"({"_meta":false,"nested":{"_meta":{"invalid key":true}}})")};
  EXPECT_EQ(mcp_make_tool_success(CURRENT, JSON{1}, business)
                .at("result")
                .at("structuredContent"),
            business);
}

TEST(conformance_nested_input_metadata) {
  MCPClientCapabilities capabilities;
  capabilities.roots = true;
  capabilities.elicitation_form = true;
  capabilities.sampling = true;
  const std::array<JSON::StringView, 3> cases{{
      R"({"r":{"method":"roots/list","params":{}}})",
      R"({"r":{"method":"elicitation/create","params":{"message":"choose","requestedSchema":{"type":"object","properties":{}}}}})",
      R"({"r":{"method":"sampling/createMessage","params":{"maxTokens":1,"messages":[{"role":"user","content":{"type":"text","text":"hello"}}]}}})",
  }};
  for (const auto fixture : cases) {
    for (const auto *const text :
         {"false", "null", "[]", R"({"invalid key":true})",
          R"({"traceparent":"bad"})"}) {
      auto request{parse_json(fixture)};
      request.at("r").at("params").assign("_meta", parse_json(text));
      rejects([&] {
        mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, request,
                                       std::nullopt, capabilities);
      });
    }
    for (const auto *const text : {"{}", R"({"org.example/value":true})"}) {
      auto request{parse_json(fixture)};
      request.at("r").at("params").assign("_meta", parse_json(text));
      const auto result{mcp_make_input_required_result(
          CURRENT, "tools/call", JSON{1}, request, std::nullopt, capabilities)};
      EXPECT_TRUE(result.at("result").is_object());
    }
  }
  auto sampling{parse_json(cases[2])};
  auto &message{sampling.at("r").at("params").at("messages").at(0)};
  message.assign("_meta", JSON{false});
  rejects([&] {
    mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, sampling,
                                   std::nullopt, capabilities);
  });
  message.erase("_meta");
  message.at("content").assign("_meta", parse_json(R"({"invalid key":true})"));
  rejects([&] {
    mcp_make_input_required_result(CURRENT, "tools/call", JSON{1}, sampling,
                                   std::nullopt, capabilities);
  });
  message.at("content") = parse_json(
      R"({"type":"tool_use","id":"call","name":"tool","input":{"_meta":false},"_meta":{"org.example/value":true}})");
  message.assign("role", JSON{"assistant"});
  capabilities.sampling_tools = true;
  sampling.at("r")
      .at("params")
      .at("messages")
      .push_back(parse_json(
          R"({"role":"user","content":{"type":"tool_result","toolUseId":"call","content":[{"type":"text","text":"done"}]}})"));
  const auto result{mcp_make_input_required_result(
      CURRENT, "tools/call", JSON{1}, sampling, std::nullopt, capabilities)};
  EXPECT_TRUE(result.at("result").is_object());
}

TEST(conformance_prompt_argument_metadata) {
  for (const auto *const text :
       {"false", "null", "[]", R"({"invalid key":true})",
        R"({"traceparent":"bad"})"}) {
    auto source{
        parse_json(R"({"prompts":[{"name":"p","arguments":[{"name":"a"}]}]})")};
    const auto meta{parse_json(text)};
    source.at("prompts").at(0).at("arguments").at(0).assign("_meta", meta);
    rejects([&] {
      mcp_make_prompts_list_result(CURRENT, source.at("prompts"), std::nullopt,
                                   {});
    });
    std::ostringstream stream;
    rejects([&] {
      mcp_write_result(stream, CURRENT, "prompts/list", JSON{1}, source,
                       MCPCachePolicy{});
    });
    EXPECT_TRUE(stream.str().empty());
    if (meta.is_object()) {
      for (const auto version : REVISIONS) {
        if (version == CURRENT) {
          continue;
        }
        EXPECT_EQ(mcp_make_prompts_list_result(version, source.at("prompts"),
                                               std::nullopt, {})
                      .at("prompts"),
                  source.at("prompts"));
        std::ostringstream legacy;
        mcp_write_result(legacy, version, "prompts/list", JSON{1}, source,
                         std::nullopt);
        EXPECT_EQ(parse_json(legacy.str()).at("result").at("prompts"),
                  source.at("prompts"));
      }
    }
  }
  const auto source{parse_json(
      R"({"prompts":[{"name":"p","arguments":[{"name":"a","_meta":{"org.example/value":{"_meta":false}}}]}]})")};
  const auto result{mcp_make_prompts_list_result(CURRENT, source.at("prompts"),
                                                 std::nullopt, {})};
  std::ostringstream stream;
  mcp_write_result(stream, CURRENT, "prompts/list", JSON{1}, source,
                   MCPCachePolicy{});
  EXPECT_EQ(parse_json(stream.str()).at("result"), result);
  EXPECT_TRUE(result.is_object());
}

TEST(conformance_sampling_tool_history) {
  MCPClientCapabilities capabilities;
  capabilities.sampling = true;
  capabilities.sampling_tools = true;
  const auto call{parse_json(
      R"({"type":"tool_use","id":"call","name":"tool","input":{"_meta":false}})")};
  const auto answer{parse_json(
      R"({"type":"tool_result","toolUseId":"call","content":[{"type":"text","text":"done"}]})")};
  const auto valid{parse_json(
      R"([{"role":"assistant","content":[{"type":"tool_use","id":"a","name":"tool","input":{}},{"type":"tool_use","id":"b","name":"tool","input":{}}]},{"role":"user","content":[{"type":"tool_result","toolUseId":"b","content":[]},{"type":"tool_result","toolUseId":"a","content":[]}]},{"role":"assistant","content":{"type":"text","text":"done"}}])")};
  const auto result{sampling_result(valid, capabilities)};
  EXPECT_TRUE(result.at("result").is_object());
  capabilities.sampling_tools = false;
  rejects([&] { sampling_result(valid, capabilities); });
  capabilities.sampling_tools = true;
  for (
      const auto *const text :
      {R"([{"role":"user","content":{}}])",
       R"([{"role":"assistant","content":{}}])",
       R"([{"role":"assistant","content":{}},{"role":"user","content":[{}, {"type":"text","text":"mixed"}]}])",
       R"([{"role":"assistant","content":{}},{"role":"user","content":[{}, {}]}])"}) {
    auto messages{parse_json(text)};
    messages.at(0).assign("content", call);
    if (messages.size() > 1) {
      auto &content{messages.at(1).at("content")};
      if (content.is_array()) {
        content.at(0) = answer;
        if (content.at(1).empty()) {
          content.at(1) = answer;
        }
      } else {
        content = answer;
      }
    }
    rejects([&] { sampling_result(messages, capabilities); });
  }
  auto incomplete{valid};
  incomplete.at(1).at("content").at(1).assign("toolUseId", JSON{"unknown"});
  rejects([&] { sampling_result(incomplete, capabilities); });
  auto duplicate{valid};
  duplicate.at(0).at("content").at(1).assign("id", JSON{"a"});
  duplicate.at(1).assign(
      "content",
      parse_json(R"([{ "type":"tool_result","toolUseId":"a","content":[]}])"));
  rejects([&] { sampling_result(duplicate, capabilities); });
  // Every block is otherwise valid: only the intervening text message violates
  // the immediate tool-result requirement.
  const auto interrupted{parse_json(
      R"([{"role":"assistant","content":{"type":"tool_use","id":"call","name":"tool","input":{}}},{"role":"assistant","content":{"type":"text","text":"interrupted"}},{"role":"user","content":{"type":"tool_result","toolUseId":"call","content":[]}}])")};
  rejects([&] { sampling_result(interrupted, capabilities); });
  auto orphan{JSON::make_array()};
  auto message{JSON::make_object()};
  message.assign("role", JSON{"user"});
  message.assign("content", answer);
  orphan.push_back(std::move(message));
  rejects([&] { sampling_result(orphan, capabilities); });
}

TEST(conformance_continuation_nested_metadata) {
  const auto prior{parse_json(
      R"({"roots":{"method":"roots/list"},"sample":{"method":"sampling/createMessage"}})")};
  for (const auto *const text :
       {"false", "null", "[]", R"({"invalid key":true})",
        R"({"traceparent":"bad"})"}) {
    auto value{parameters()};
    value.assign(
        "inputResponses",
        parse_json(
            R"({"roots":{"roots":[{"uri":"file:///r"}]},"sample":{"role":"assistant","model":"model","content":{"type":"text","text":"hello"}}})"));
    auto &root{value.at("inputResponses").at("roots").at("roots").at(0)};
    root.assign("_meta", parse_json(text));
    EXPECT_FALSE(
        mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
    root.erase("_meta");
    auto &sample{value.at("inputResponses").at("sample")};
    sample.assign("_meta", parse_json(text));
    EXPECT_FALSE(
        mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
    sample.erase("_meta");
    sample.at("content").assign("_meta", parse_json(text));
    EXPECT_FALSE(
        mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
    sample.at("content").assign(
        "_meta", parse_json(R"({"org.example/value":{"_meta":false}})"));
    root.assign("_meta", JSON::make_object());
    EXPECT_TRUE(mcp_validate_continuation(CURRENT, value, prior, std::nullopt));
  }
}

TEST(conformance_reserved_error_contracts) {
  const JSON identifier{1};
  for (std::int64_t code = -32099; code <= -32020; ++code) {
    if (code != MCP_CODE_HEADER_MISMATCH &&
        code != MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY &&
        code != MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION) {
      rejects([&] { mcp_make_error(CURRENT, &identifier, code, "undefined"); });
    }
    for (const auto version : REVISIONS) {
      if (version != CURRENT) {
        EXPECT_EQ(mcp_make_error(version, &identifier, code, "legacy")
                      .at("error")
                      .at("code"),
                  JSON{code});
      }
    }
  }
  for (const auto code :
       {-32100, -32019, -32600, -32601, -32602, -32603, -32700, 1}) {
    EXPECT_EQ(
        mcp_make_error(CURRENT, &identifier, code, "application or JSON-RPC")
            .at("error")
            .at("code"),
        JSON{code});
  }
  for (const auto code : {MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION,
                          MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY}) {
    rejects([&] { mcp_make_error(CURRENT, &identifier, code, "missing"); });
    for (const auto *const text : {"null", "false", "[]", "{}"}) {
      rejects([&] {
        mcp_make_error(CURRENT, &identifier, code, "bad", parse_json(text));
      });
    }
  }
  for (const auto *const text : {R"({"requested":false,"supported":[]})",
                                 R"({"requested":"x","supported":false})",
                                 R"({"requested":"x","supported":[1]})"}) {
    rejects([&] {
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION, "bad",
                     parse_json(text));
    });
  }
  for (const auto *const text :
       {R"({"requiredCapabilities":false})",
        R"({"requiredCapabilities":{"roots":false}})",
        R"({"requiredCapabilities":{"extensions":{"invalid key":{}}}})"}) {
    rejects([&] {
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, "bad",
                     parse_json(text));
    });
  }
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_UNSUPPORTED_PROTOCOL_VERSION, "Unsupported",
                     parse_json(R"({"requested":"future","supported":[]})"))
          .is_object());
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier,
                     MCP_CODE_MISSING_REQUIRED_CLIENT_CAPABILITY, "Missing",
                     parse_json(R"({"requiredCapabilities":{"roots":{}}})"))
          .is_object());
  EXPECT_TRUE(
      mcp_make_error(CURRENT, &identifier, MCP_CODE_HEADER_MISMATCH, "Mismatch")
          .is_object());
}
