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
constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED_VERSIONS{
    {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};

using namespace sourcemeta::core;

constexpr std::array<JSON::StringView, 4> WIRES{
    {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
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

auto request(const JSON::StringView method = "tools/list") -> JSON {
  auto result{jsonrpc_make_notification(method, parameters())};
  result.assign("id", JSON{0});
  return result;
}

} // namespace

TEST(protocol_era_predicates_requires_request_meta) {
  auto version_2025_03_26{sourcemeta::core::MCPProtocolVersion::V_2025_03_26};
  auto version_2025_06_18{sourcemeta::core::MCPProtocolVersion::V_2025_06_18};
  auto version_2025_11_25{sourcemeta::core::MCPProtocolVersion::V_2025_11_25};
  auto version_2026_07_28{sourcemeta::core::MCPProtocolVersion::V_2026_07_28};
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_03_26));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_06_18));
  EXPECT_FALSE(sourcemeta::core::mcp_requires_request_meta(version_2025_11_25));
  EXPECT_TRUE(sourcemeta::core::mcp_requires_request_meta(version_2026_07_28));
}

TEST(request_meta_validation_valid_minimum) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0"
        }
      }
    }
  })JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->protocol_version,
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
  EXPECT_TRUE(meta->client_capabilities.is_object());
  EXPECT_TRUE(meta->client_info.has_value());
  EXPECT_EQ(meta->client_info->name, "ExampleClient");
  EXPECT_EQ(meta->client_info->version, "1.0.0");
  EXPECT_TRUE(meta->client_info->title.empty());
  EXPECT_TRUE(meta->client_info->description.empty());
  EXPECT_TRUE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
               sourcemeta::core::MCPRequestMetaStatus::Valid));

  const auto client_info{
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)};
  EXPECT_TRUE(client_info.has_value());
  EXPECT_EQ(client_info->name, "ExampleClient");
  EXPECT_EQ(client_info->version, "1.0.0");
  EXPECT_FALSE(meta->log_level.has_value());
  auto empty_method{envelope};
  empty_method.assign("method", sourcemeta::core::JSON{""});
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(empty_method).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_meta_validation_valid_with_client_info_and_log_level) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/call",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {
        "roots": {
          "listChanged": true
        }
      },
      "io.modelcontextprotocol/clientInfo": {
        "name": "client-one",
        "version": "1.2.3",
        "title": "Client Title",
        "description": "Client Description"
      },
      "io.modelcontextprotocol/logLevel": "debug",
      "customExtension": 42
    },
    "name": "compute"
  }
})JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_TRUE(meta->client_info.has_value());
  EXPECT_EQ(meta->client_info->name, "client-one");
  EXPECT_EQ(meta->client_info->version, "1.2.3");
  EXPECT_EQ(meta->client_info->title, "Client Title");
  EXPECT_EQ(meta->client_info->description, "Client Description");
  EXPECT_TRUE(meta->log_level.has_value());
  EXPECT_EQ(meta->log_level.value(), "debug");
  EXPECT_TRUE(meta->meta_object.defines("customExtension"));
}

TEST(request_meta_validation_missing_params) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list"
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MissingParams);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_params_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list", "params": [ 1, 2 ]
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::ParamsNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_meta) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list", "params": { "name": "test" }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MissingMeta);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_meta_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/list",
  "params": {
    "_meta": "invalid"
  }
})JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MetaNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "io.modelcontextprotocol/clientCapabilities": {} } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_protocol_version_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": 2026,
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ProtocolVersionNotString);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_unsupported_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "1900-01-01",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_capabilities) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28"
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientCapabilities);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_client_capabilities_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": "not-an-object"
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(
      status,
      sourcemeta::core::MCPRequestMetaStatus::ClientCapabilitiesNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_info) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_TRUE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
               sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
  EXPECT_EQ(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second
          .transform([](const auto &value) { return value.protocol_version; })
          .value(),
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
  EXPECT_NE(&sourcemeta::core::mcp_validate_request_meta(envelope)
                 .second->client_capabilities,
            nullptr);
}

TEST(request_meta_validation_client_info_null) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": null
      } }
    })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_client_info_array) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": [ "ExampleClient", "1.0.0" ]
      } }
    })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_client_info_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": "ExampleClient/1.0.0"
      } }
    })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_missing_client_info_name) {
  // Empty object
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {}
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                  sourcemeta::core::MCPRequestMetaStatus::Valid));
    EXPECT_FALSE(
        sourcemeta::core::mcp_validate_request_meta(envelope)
            .second
            .transform([](const auto &value) { return value.client_info; })
            .value_or(std::nullopt)
            .has_value());
  }
  // Version present, name missing
  {
    const auto envelope{sourcemeta::core::parse_json(R"JSON({
      "jsonrpc": "2.0", "id": 1, "method": "tools/list",
      "params": { "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "version": "1.0.0"
        }
      } }
    })JSON")};
    const auto [status,
                meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
    EXPECT_EQ(status,
              sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
    EXPECT_FALSE(meta.has_value());
    EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                  sourcemeta::core::MCPRequestMetaStatus::Valid));
    EXPECT_FALSE(
        sourcemeta::core::mcp_validate_request_meta(envelope)
            .second
            .transform([](const auto &value) { return value.client_info; })
            .value_or(std::nullopt)
            .has_value());
  }
}

TEST(request_meta_validation_client_info_name_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": 42,
        "version": "1.0.0"
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoNameNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_missing_client_info_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient"
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoVersion);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_client_info_version_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": false
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoVersionNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_client_info_title_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": "1.0.0",
        "title": 123
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ClientInfoTitleNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_validation_client_info_description_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "ExampleClient",
        "version": "1.0.0",
        "description": [ "an", "mcp", "client" ]
      }
    } }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(
      status,
      sourcemeta::core::MCPRequestMetaStatus::ClientInfoDescriptionNotString);
  EXPECT_FALSE(meta.has_value());
  EXPECT_FALSE((sourcemeta::core::mcp_validate_request_meta(envelope).first ==
                sourcemeta::core::MCPRequestMetaStatus::Valid));
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(envelope)
          .second.transform([](const auto &value) { return value.client_info; })
          .value_or(std::nullopt)
          .has_value());
}

TEST(request_meta_view_borrows_source) {
  const auto request{sourcemeta::core::parse_json(
      R"({
  "jsonrpc": "2.0",
  "id": 1,
  "method": "tools/list",
  "params": {
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/logLevel": "info"
    }
  }
})")};
  const auto [status,
              parsed]{sourcemeta::core::mcp_validate_request_meta(request)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(parsed.has_value());
  EXPECT_EQ(&parsed->client_capabilities,
            &request.at("params").at("_meta").at(
                "io.modelcontextprotocol/clientCapabilities"));
  EXPECT_EQ(&parsed->meta_object, &request.at("params").at("_meta"));
  EXPECT_EQ(parsed->log_level, "info");
}

TEST(error_request_meta_generation) {
  const auto identifier{sourcemeta::core::JSON{1}};
  const auto error{sourcemeta::core::mcp_make_error_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::MCPRequestMetaStatus::MissingMeta, "",
      SUPPORTED_VERSIONS)};
  EXPECT_EQ(error.at("error").at("code").to_integer(),
            sourcemeta::core::JSONRPC_CODE_INVALID_PARAMS);
  EXPECT_EQ(error.at("id").to_integer(), 1);

  const auto unsupported_error{sourcemeta::core::mcp_make_error_request_meta(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, identifier,
      sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion,
      "2024-01-01",
      std::array<sourcemeta::core::JSON::StringView, 1>{{"2026-07-28"}})};
  EXPECT_EQ(unsupported_error.at("error").at("code").to_integer(), -32022);
  EXPECT_EQ(
      unsupported_error.at("error").at("data").at("requested").to_string(),
      "2024-01-01");
  EXPECT_EQ(unsupported_error.at("error")
                .at("data")
                .at("supported")
                .at(0)
                .to_string(),
            "2026-07-28");
}

TEST(request_meta_validation_empty_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};

  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
}

TEST(request_meta_validation_accepts_params_with_jsonrpc_property) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "custom-param-value",
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  const auto [status, meta]{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->protocol_version,
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
}

TEST(parameter_integer_accepts_decimal) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"42.0"};

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

TEST(parameter_integer_accepts_exponent) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"4.2e1"};

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

TEST(parameter_integer_accepts_maximum) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"9007199254740991.0"};

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

TEST(parameter_integer_accepts_minimum) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"-9007199254740991.0"};

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

TEST(parameter_integer_accepts_zero) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"0.0"};

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

TEST(parameter_integer_rejects_fraction) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"42.00000000000000000001"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_maximum_fraction) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"9007199254740991.1"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_minimum_fraction) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"-9007199254740991.1"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_above_maximum) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"9007199254740992"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_below_minimum) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"-9007199254740992"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_huge_exponent) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"1e100"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(parameter_integer_rejects_tiny_exponent) {
  using namespace sourcemeta::core;
  constexpr auto VERSION{MCPProtocolVersion::V_2026_07_28};
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"value"}, .type = JSON::Type::Integer}}};

  const auto *const text{"1e-100"};

  auto arguments{JSON::make_object()};
  arguments.assign("value", parse_json(text));
  try {
    mcp_make_parameter_headers(descriptors, arguments);
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  EXPECT_EQ(mcp_validate_parameter_headers(VERSION, JSON{0}, descriptors,
                                           arguments, {})
                ->at("error")
                .at("code"),
            JSON{-32602});
}

TEST(legacy_parameter_structure_and_borrowed_name) {
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

TEST(metadata_borrows_request_values) {
  auto value{parameters()};
  const auto parsed{mcp_validate_request_parameters(value)};
  EXPECT_EQ(parsed.first, MCPRequestMetaStatus::Valid);
  EXPECT_EQ(&parsed.second->meta_object, &value.at("_meta"));
  EXPECT_EQ(
      &parsed.second->client_capabilities,
      &value.at("_meta").at("io.modelcontextprotocol/clientCapabilities"));
}

TEST(request_parameters_rejects_null) {
  EXPECT_FALSE(
      mcp_validate_request_parameters(JSON{nullptr}).second.has_value());
}

TEST(request_meta_rejects_null) {
  EXPECT_FALSE(mcp_validate_request_meta(JSON{nullptr}).second.has_value());
}

TEST(request_parameters_rejects_boolean) {
  EXPECT_FALSE(mcp_validate_request_parameters(JSON{true}).second.has_value());
}

TEST(request_meta_rejects_boolean) {
  EXPECT_FALSE(mcp_validate_request_meta(JSON{true}).second.has_value());
}

TEST(request_parameters_rejects_array) {
  EXPECT_FALSE(
      mcp_validate_request_parameters(JSON::make_array()).second.has_value());
}

TEST(request_meta_rejects_array) {
  EXPECT_FALSE(
      mcp_validate_request_meta(JSON::make_array()).second.has_value());
}

TEST(request_metadata_rejects_roots_boolean) {
  const auto capabilities{parse_json(R"({"roots":true})")};
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(client_capabilities_rejects_roots_boolean) {
  const auto capabilities{parse_json(R"({"roots":true})")};
  try {
    mcp_parse_client_capabilities(CURRENT, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(request_metadata_rejects_sampling_tools_boolean) {
  const auto capabilities{parse_json(R"({"sampling":{"tools":false}})")};
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(client_capabilities_rejects_sampling_tools_boolean) {
  const auto capabilities{parse_json(R"({"sampling":{"tools":false}})")};
  try {
    mcp_parse_client_capabilities(CURRENT, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(request_metadata_rejects_elicitation_url_boolean) {
  const auto capabilities{parse_json(R"({"elicitation":{"url":true}})")};
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(client_capabilities_rejects_elicitation_url_boolean) {
  const auto capabilities{parse_json(R"({"elicitation":{"url":true}})")};
  try {
    mcp_parse_client_capabilities(CURRENT, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(request_metadata_rejects_unqualified_extension) {
  const auto capabilities{parse_json(R"({"extensions":{"bad":{}}})")};
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(client_capabilities_rejects_unqualified_extension) {
  const auto capabilities{parse_json(R"({"extensions":{"bad":{}}})")};
  try {
    mcp_parse_client_capabilities(CURRENT, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(request_metadata_rejects_experimental_boolean) {
  const auto capabilities{
      parse_json(R"({"experimental":{"extension":false}})")};
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(client_capabilities_rejects_experimental_boolean) {
  const auto capabilities{
      parse_json(R"({"experimental":{"extension":false}})")};
  try {
    mcp_parse_client_capabilities(CURRENT, capabilities);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(request_client_info_rejects_website_url_boolean) {
  auto value{parameters()};
  value.at("_meta").assign(
      "io.modelcontextprotocol/clientInfo",
      parse_json(R"({"name":"client","version":"1","websiteUrl":false})"));
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(request_client_info_rejects_icon_src_number) {
  auto value{parameters()};
  value.at("_meta").assign(
      "io.modelcontextprotocol/clientInfo",
      parse_json(R"({"name":"client","version":"1","icons":[{"src":5}]})"));
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(request_metadata_rejects_fractional_progress_token) {
  auto value{parameters()};
  value.at("_meta").assign("progressToken", JSON{1.5});
  EXPECT_EQ(mcp_validate_request_parameters(value).first,
            MCPRequestMetaStatus::InvalidProgressToken);
}

TEST(request_metadata_rejects_unknown_log_level) {
  auto value{parameters()};
  value.at("_meta").assign("io.modelcontextprotocol/logLevel", JSON{"verbose"});
  EXPECT_EQ(mcp_validate_request_parameters(value).first,
            MCPRequestMetaStatus::InvalidLogLevel);
}

TEST(request_metadata_rejects_boolean_traceparent) {
  auto value{parameters()};
  value.at("_meta").assign("traceparent", JSON{false});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(request_metadata_rejects_boolean_tracestate) {
  auto value{parameters()};
  value.at("_meta").assign("tracestate", JSON{false});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(request_metadata_rejects_boolean_baggage) {
  auto value{parameters()};
  value.at("_meta").assign("baggage", JSON{false});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(traceparent_rejects_zero_trace) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-00000000000000000000000000000000-0123456789abcdef-01"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(traceparent_rejects_zero_parent) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0000000000000000-01"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(traceparent_rejects_reserved_version) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"ff-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(traceparent_rejects_uppercase_hex) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789ABCDEF0123456789abcdef-0123456789abcdef-01"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(traceparent_rejects_extra_fields) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01-extra"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(request_metadata_accepts_tracing_whitespace_and_escapes) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate",
                           JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      JSON{"\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  EXPECT_TRUE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(tracestate_rejects_duplicate_key) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate",
                           JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      JSON{"\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  value.at("_meta").assign("tracestate", JSON{"a=b,a=c"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(tracestate_rejects_uppercase_key) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate",
                           JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      JSON{"\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  value.at("_meta").assign("tracestate", JSON{"UPPER=value"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(tracestate_rejects_extra_equals) {
  auto value{parameters()};
  value.at("_meta").assign(
      "traceparent",
      JSON{"00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate",
                           JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      JSON{"\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  value.at("_meta").assign("tracestate", JSON{"a=value=wrong"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(baggage_rejects_invalid_escape) {
  auto value{parameters()};
  value.at("_meta").assign("baggage", JSON{"a=%ZZ"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(baggage_rejects_bare_percent) {
  auto value{parameters()};
  value.at("_meta").assign("baggage", JSON{"a=%"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(baggage_rejects_short_escape) {
  auto value{parameters()};
  value.at("_meta").assign("baggage", JSON{"a=%2"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(baggage_rejects_newline) {
  auto value{parameters()};
  value.at("_meta").assign("baggage", JSON{"a=value\n"});
  EXPECT_FALSE(mcp_validate_request_parameters(value).second.has_value());
}

TEST(legacy_result_allows_unreserved_traceparent) {
  const auto legacy{parse_json(R"({"_meta":{"traceparent":false}})")};
  EXPECT_EQ(mcp_decorate_result(MCPProtocolVersion::V_2025_03_26, legacy),
            legacy);
}

TEST(modern_result_rejects_boolean_traceparent) {
  const auto legacy{parse_json(R"({"_meta":{"traceparent":false}})")};
  try {
    mcp_decorate_result(CURRENT, legacy);
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(header_error_ordering) {
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

TEST(parameter_header_roundtrip) {
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
  try {
    mcp_make_parameter_headers(descriptors,
                               parse_json(R"({"number":9007199254740992})"));
    FAIL();
  } catch (const std::invalid_argument &) {
    // Invalid input is expected to be rejected
  }
  try {
    mcp_make_parameter_headers(descriptors, parse_json(R"({"number":1.5})"));
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_builder_rejects_empty_path) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {}, .type = JSON::Type::String}}};
  try {
    mcp_make_parameter_headers(descriptors, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_validator_rejects_empty_path) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {}, .type = JSON::Type::String}}};
  try {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors,
                                   JSON::make_object(), {});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_builder_rejects_real_type) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"x"}, .type = JSON::Type::Real}}};
  try {
    mcp_make_parameter_headers(descriptors, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_validator_rejects_real_type) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "Value", .path = {"x"}, .type = JSON::Type::Real}}};
  try {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors,
                                   JSON::make_object(), {});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_builder_rejects_injected_name) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "bad\r\nInjected", .path = {"x"}, .type = JSON::Type::String}}};
  try {
    mcp_make_parameter_headers(descriptors, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_validator_rejects_injected_name) {
  const std::array<MCPHeaderParameter, 1> descriptors{
      {{.name = "bad\r\nInjected", .path = {"x"}, .type = JSON::Type::String}}};
  try {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors,
                                   JSON::make_object(), {});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_builder_rejects_duplicate_names) {
  const std::array<MCPHeaderParameter, 2> descriptors{
      {{.name = "Value", .path = {"x"}, .type = JSON::Type::String},
       {.name = "value", .path = {"y"}, .type = JSON::Type::String}}};
  try {
    mcp_make_parameter_headers(descriptors, JSON::make_object());
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(parameter_header_validator_rejects_duplicate_names) {
  const std::array<MCPHeaderParameter, 2> descriptors{
      {{.name = "Value", .path = {"x"}, .type = JSON::Type::String},
       {.name = "value", .path = {"y"}, .type = JSON::Type::String}}};
  try {
    mcp_validate_parameter_headers(CURRENT, JSON{0}, descriptors,
                                   JSON::make_object(), {});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(name_header_rejects_bad_base64) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                           "=?base64?!!!!?=", value, WIRES)
                  .has_value());
}

TEST(name_header_rejects_invalid_utf8) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                           "=?base64?/w==?=", value, WIRES)
                  .has_value());
}

TEST(name_header_rejects_leading_space) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                           " café", value, WIRES)
                  .has_value());
}

TEST(name_header_rejects_non_ascii) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                           "café", value, WIRES)
                  .has_value());
}

TEST(name_header_rejects_newline) {
  auto value{request("tools/call")};
  value.at("params").assign("name", JSON{"café"});
  EXPECT_TRUE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                           "cafe\n", value, WIRES)
                  .has_value());
}

TEST(encoded_name_header_roundtrip_base64_marker) {
  auto value{request("tools/call")};
  const auto *text{"=?base64?YWJj?="};
  value.at("params").assign("name", JSON{text});
  const auto encoded{mcp_encode_header_value(text)};
  EXPECT_FALSE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                            encoded, value, WIRES)
                   .has_value());
}

TEST(encoded_name_header_roundtrip_padded) {
  auto value{request("tools/call")};
  const auto *text{" padded "};
  value.at("params").assign("name", JSON{text});
  const auto encoded{mcp_encode_header_value(text)};
  EXPECT_FALSE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                            encoded, value, WIRES)
                   .has_value());
}

TEST(encoded_name_header_roundtrip_non_ascii) {
  auto value{request("tools/call")};
  const auto *text{"café"};
  value.at("params").assign("name", JSON{text});
  const auto encoded{mcp_encode_header_value(text)};
  EXPECT_FALSE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                            encoded, value, WIRES)
                   .has_value());
}

TEST(encoded_name_header_roundtrip_tab) {
  auto value{request("tools/call")};
  const auto *text{"a\tb"};
  value.at("params").assign("name", JSON{text});
  const auto encoded{mcp_encode_header_value(text)};
  EXPECT_FALSE(mcp_validate_request_headers(CURRENT, "2026-07-28", "tools/call",
                                            encoded, value, WIRES)
                   .has_value());
}

TEST(header_encoder_rejects_invalid_utf8) {
  try {
    mcp_encode_header_value(std::string_view{"\xff", 1});
  } catch (const std::invalid_argument &) {
    return;
  }
  FAIL();
}

TEST(borrowed_header_collection) {
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

TEST(stateless_revision_and_missing_capabilities) {
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
