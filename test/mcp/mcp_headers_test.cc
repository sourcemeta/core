#include <sourcemeta/core/mcp.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include "mcp_helpers.h"

#include <array>    // std::array
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <utility>  // std::pair
#include <vector>   // std::vector

TEST(request_meta_validation_valid_minimum) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_EQ(meta->protocol_version,
            sourcemeta::core::MCPProtocolVersion::V_2026_07_28);
  EXPECT_EQ(meta->client_capabilities,
            sourcemeta::core::parse_json(R"JSON({})JSON"));
  EXPECT_FALSE(meta->client_info.has_value());
  EXPECT_FALSE(meta->log_level.has_value());
  EXPECT_EQ(meta->progress_token, nullptr);
}

// Integer-valued numbers are integers regardless of their JSON representation
// https://json-schema.org/understanding-json-schema/reference/numeric#integer
TEST(request_meta_validation_accepts_integral_real_identifier) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 42.0, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  EXPECT_TRUE(envelope.at("id").is_real());
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_meta_validation_accepts_integral_exponent_identifier) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 4.2e1, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  EXPECT_EQ(envelope.at("id"), sourcemeta::core::JSON{42});
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_meta_validation_accepts_integral_decimal_identifier) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 18446744073709551617, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  EXPECT_TRUE(envelope.at("id").is_decimal());
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_meta_validation_rejects_fractional_decimal_identifier) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 42.00000000000000000001, "method": "tools/list",
    "params": { "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    } }
  })JSON")};
  EXPECT_TRUE(envelope.at("id").is_decimal());
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidEnvelope);
}

TEST(request_meta_validation_valid_with_client_info) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": { "name": "ExampleClient", "version": "1.0.0" }
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_EQ(meta->client_info->name, "ExampleClient");
  EXPECT_EQ(meta->client_info->version, "1.0.0");
  EXPECT_TRUE(meta->client_info->title.empty());
  EXPECT_TRUE(meta->client_info->description.empty());
  EXPECT_TRUE(meta->client_info->website_url.empty());
  EXPECT_EQ(meta->client_info->icons, nullptr);
}

// A method name is not metadata, so an empty one is left to the dispatcher
TEST(request_meta_validation_accepts_an_empty_method) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(request_meta_validation_valid_with_log_level_and_progress_token) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": { "roots": {} },
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0",
          "title": "Example",
          "description": "An example",
          "websiteUrl": "https://example.com"
        },
        "io.modelcontextprotocol/logLevel": "warning",
        "progressToken": 7
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_EQ(meta->client_capabilities,
            sourcemeta::core::parse_json(R"JSON({ "roots": {} })JSON"));
  EXPECT_EQ(meta->client_info->name, "ExampleClient");
  EXPECT_EQ(meta->client_info->version, "1.0.0");
  EXPECT_EQ(meta->client_info->title, "Example");
  EXPECT_EQ(meta->client_info->description, "An example");
  EXPECT_EQ(meta->client_info->website_url, "https://example.com");
  EXPECT_EQ(meta->log_level.value(), "warning");
  EXPECT_EQ(*meta->progress_token, sourcemeta::core::JSON{7});
}

TEST(request_meta_validation_missing_params) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 1, "method": "tools/list" })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MissingParams);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_params_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": [ 1, 2 ]
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::ParamsNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_meta) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": { "name": "test" }
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
    "params": { "_meta": "invalid" }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::MetaNotObject);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": { "io.modelcontextprotocol/clientCapabilities": {} }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_protocol_version_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": 2026,
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::ProtocolVersionNotString);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_unsupported_protocol_version) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "1900-01-01",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_capabilities) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28"
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientCapabilities);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_client_capabilities_not_object) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": "not-an-object"
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": null
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": [ "ExampleClient", "1.0.0" ]
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": "ExampleClient/1.0.0"
      }
    }
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

TEST(request_meta_validation_empty_client_info) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {}
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_missing_client_info_name) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": { "version": "1.0.0" }
      }
    }
  })JSON")};
  const auto [status,
              meta]{sourcemeta::core::mcp_validate_request_meta(envelope)};
  EXPECT_EQ(status,
            sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName);
  EXPECT_FALSE(meta.has_value());
}

TEST(request_meta_validation_client_info_name_not_string) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": { "name": 42, "version": "1.0.0" }
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": { "name": "ExampleClient" }
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": { "name": "ExampleClient", "version": false }
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0",
          "title": 123
        }
      }
    }
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
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {},
        "io.modelcontextprotocol/clientInfo": {
          "name": "ExampleClient",
          "version": "1.0.0",
          "description": [ "an", "mcp", "client" ]
        }
      }
    }
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
      R"JSON({
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
      })JSON")};
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

// A malformed request is rejected with invalid params, and the status says
// which field was at fault
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(make_error_request_meta_invalid_envelope) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::InvalidEnvelope, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_error_request_meta_invalid_meta_key) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_invalid_client_capabilities) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::InvalidClientCapabilities, "",
          SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_website_url_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::ClientInfoWebsiteUrlNotString,
          "", SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_invalid_client_info) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::InvalidClientInfo, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_invalid_log_level) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::InvalidLogLevel, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_invalid_progress_token) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::InvalidProgressToken,
                "", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: malformed metadata field"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_params) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::MissingParams, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing params object"
    }
  })JSON"));
}

TEST(make_error_request_meta_params_not_object) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::ParamsNotObject, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: params must be an object"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_meta) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::MissingMeta, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing _meta object"
    }
  })JSON"));
}

TEST(make_error_request_meta_meta_not_object) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::MetaNotObject, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: _meta must be an object"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_protocol_version) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::MissingProtocolVersion,
                "", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing protocolVersion in _meta"
    }
  })JSON"));
}

TEST(make_error_request_meta_protocol_version_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::ProtocolVersionNotString, "",
          SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: protocolVersion must be a string"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_client_capabilities) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::MissingClientCapabilities, "",
          SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing clientCapabilities in _meta"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_capabilities_not_object) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::ClientCapabilitiesNotObject,
          "", SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientCapabilities must be an object"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_not_object) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::ClientInfoNotObject, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientInfo must be an object"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_client_info_name) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoName,
                "", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing name in clientInfo"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_name_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::ClientInfoNameNotString,
                "", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientInfo name must be a string"
    }
  })JSON"));
}

TEST(make_error_request_meta_missing_client_info_version) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::MissingClientInfoVersion, "",
          SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing version in clientInfo"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_version_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::ClientInfoVersionNotString,
          "", SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientInfo version must be a string"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_title_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::ClientInfoTitleNotString, "",
          SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientInfo title must be a string"
    }
  })JSON"));
}

TEST(make_error_request_meta_client_info_description_not_string) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
                sourcemeta::core::MCPRequestMetaStatus::
                    ClientInfoDescriptionNotString,
                "", SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32602,
      "message": "Invalid params: clientInfo description must be a string"
    }
  })JSON"));
}

// An unsupported version reports the revisions the server does offer
// https://modelcontextprotocol.io/specification/2026-07-28/schema#unsupportedprotocolversionerror
TEST(make_error_request_meta_unsupported_protocol_version) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion,
          "2024-01-01", SUPPORTED),
      sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": {
        "supported": [ "2026-07-28" ],
        "requested": "2024-01-01"
      }
    }
  })JSON"));
}

TEST(make_error_request_meta_omits_an_absent_identifier) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_make_error_request_meta(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, nullptr,
                sourcemeta::core::MCPRequestMetaStatus::MissingMeta, "",
                SUPPORTED),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32602,
      "message": "Invalid params: missing _meta object"
    }
  })JSON"));
}

TEST(make_error_request_meta_rejects_a_valid_status) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::Valid, "", SUPPORTED),
      "Cannot construct an error for valid metadata");
}

// Stateless metadata errors only exist in the revision that defines the
// metadata
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(make_error_request_meta_rejects_2025_11_25) {
  const sourcemeta::core::JSON identifier{1};
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_error_request_meta(
          sourcemeta::core::MCPProtocolVersion::V_2025_11_25, &identifier,
          sourcemeta::core::MCPRequestMetaStatus::MissingMeta, "", SUPPORTED),
      "Stateless metadata errors require MCP 2026-07-28");
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
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"42.0"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  const auto headers{
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments)};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.front().second,
            std::to_string(arguments.at("value").as_integer()));
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{headers.front().first, headers.front().second}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_integer_accepts_exponent) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"4.2e1"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  const auto headers{
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments)};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.front().second,
            std::to_string(arguments.at("value").as_integer()));
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{headers.front().first, headers.front().second}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_integer_accepts_maximum) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"9007199254740991.0"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  const auto headers{
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments)};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.front().second,
            std::to_string(arguments.at("value").as_integer()));
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{headers.front().first, headers.front().second}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_integer_accepts_minimum) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"-9007199254740991.0"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  const auto headers{
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments)};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.front().second,
            std::to_string(arguments.at("value").as_integer()));
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{headers.front().first, headers.front().second}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_integer_accepts_zero) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"0.0"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  const auto headers{
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments)};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.front().second,
            std::to_string(arguments.at("value").as_integer()));
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{headers.front().first, headers.front().second}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_integer_rejects_fraction) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"42.00000000000000000001"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_maximum_fraction) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"9007199254740991.1"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_minimum_fraction) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"-9007199254740991.1"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_above_maximum) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"9007199254740992"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_below_minimum) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"-9007199254740992"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_huge_exponent) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"1e100"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(parameter_integer_rejects_tiny_exponent) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};

  const auto *const text{"1e-100"};

  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::parse_json(text));
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      "Invalid mirrored argument type or unsafe integer");
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, {})
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid mirrored argument" }
  })JSON"));
}

TEST(legacy_headers_ignore_the_method_and_name_on_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   std::nullopt, "ignored", "ignored", envelope, SUPPORTED)
                   .has_value());
}

TEST(legacy_headers_ignore_the_method_and_name_on_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   std::nullopt, "ignored", "ignored", envelope, SUPPORTED)
                   .has_value());
}

TEST(legacy_headers_ignore_the_method_and_name_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, "ignored", "ignored", envelope, SUPPORTED)
                   .has_value());
}

TEST(legacy_headers_reject_boolean_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": false
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(legacy_headers_reject_array_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": []
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(legacy_headers_reject_null_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": null
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(legacy_headers_reject_non_object_metadata) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": { "_meta": true }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(request_name_from_body_of_a_tool_call) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({
        "method": "tools/call",
        "params": { "name": "weather" }
      })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(envelope).value(),
            "weather");
}

TEST(request_name_from_body_of_a_prompt_get) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({
        "method": "prompts/get",
        "params": { "name": "review" }
      })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(envelope).value(),
            "review");
}

// A resource is named by its URI rather than by a name
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#standard-request-headers
TEST(request_name_from_body_of_a_resource_read) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({
        "method": "resources/read",
        "params": { "uri": "file:///a" }
      })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(envelope).value(),
            "file:///a");
}

TEST(request_name_from_body_of_a_resource_read_ignores_a_name) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "method": "resources/read",
    "params": { "uri": "file:///a", "name": "ignored" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(envelope).value(),
            "file:///a");
}

TEST(request_name_from_body_without_parameters) {
  const auto envelope{
      sourcemeta::core::parse_json(R"JSON({ "method": "tools/call" })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_request_name_from_body(envelope).has_value());
}

TEST(request_name_from_body_without_a_name) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "method": "tools/call", "params": {} })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_request_name_from_body(envelope).has_value());
}

TEST(request_name_from_body_with_a_non_string_name) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "method": "tools/call", "params": { "name": 1 } })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_request_name_from_body(envelope).has_value());
}

TEST(request_name_from_body_of_a_non_object) {
  EXPECT_FALSE(sourcemeta::core::mcp_request_name_from_body(
                   sourcemeta::core::parse_json(R"JSON([])JSON"))
                   .has_value());
}

TEST(metadata_borrows_request_values) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  const auto parsed{sourcemeta::core::mcp_validate_request_parameters(value)};
  EXPECT_EQ(parsed.first, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_EQ(&parsed.second->meta_object, &value.at("_meta"));
  EXPECT_EQ(
      &parsed.second->client_capabilities,
      &value.at("_meta").at("io.modelcontextprotocol/clientCapabilities"));
}

TEST(request_parameters_rejects_null) {
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(
                   sourcemeta::core::JSON{nullptr})
                   .second.has_value());
}

TEST(request_meta_rejects_null) {
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_meta(
                   sourcemeta::core::JSON{nullptr})
                   .second.has_value());
}

TEST(request_parameters_rejects_boolean) {
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(
                   sourcemeta::core::JSON{true})
                   .second.has_value());
}

TEST(request_meta_rejects_boolean) {
  EXPECT_FALSE(
      sourcemeta::core::mcp_validate_request_meta(sourcemeta::core::JSON{true})
          .second.has_value());
}

TEST(request_parameters_rejects_array) {
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(
                   sourcemeta::core::JSON::make_array())
                   .second.has_value());
}

TEST(request_meta_rejects_array) {
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_meta(
                   sourcemeta::core::JSON::make_array())
                   .second.has_value());
}

TEST(request_metadata_rejects_roots_boolean) {
  const auto capabilities{
      sourcemeta::core::parse_json(R"JSON({ "roots": true })JSON")};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(client_capabilities_rejects_roots_boolean) {
  const auto capabilities{
      sourcemeta::core::parse_json(R"JSON({ "roots": true })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid client capabilities");
}

TEST(request_metadata_rejects_sampling_tools_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "sampling": { "tools": false } })JSON")};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(client_capabilities_rejects_sampling_tools_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "sampling": { "tools": false } })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid client capabilities");
}

TEST(request_metadata_rejects_elicitation_url_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "elicitation": { "url": true } })JSON")};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(client_capabilities_rejects_elicitation_url_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "elicitation": { "url": true } })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid client capabilities");
}

TEST(request_metadata_rejects_unqualified_extension) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "extensions": { "bad": {} } })JSON")};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(client_capabilities_rejects_unqualified_extension) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "extensions": { "bad": {} } })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid client capabilities");
}

TEST(request_metadata_rejects_experimental_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "experimental": { "extension": false } })JSON")};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientCapabilities",
                           capabilities);
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(client_capabilities_rejects_experimental_boolean) {
  const auto capabilities{sourcemeta::core::parse_json(
      R"JSON({ "experimental": { "extension": false } })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_parse_client_capabilities(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, capabilities),
      "Invalid client capabilities");
}

TEST(request_client_info_rejects_website_url_boolean) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientInfo",
                           sourcemeta::core::parse_json(
                               R"JSON({
            "name": "client",
            "version": "1",
            "websiteUrl": false
          })JSON"));
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(request_client_info_rejects_icon_src_number) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/clientInfo",
                           sourcemeta::core::parse_json(
                               R"JSON({
            "name": "client",
            "version": "1",
            "icons": [ { "src": 5 } ]
          })JSON"));
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(request_metadata_accepts_integral_real_progress_token) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "progressToken": 7.0
    }
  })JSON")};
  const auto [status, meta]{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->progress_token, &parameters.at("_meta").at("progressToken"));
}

TEST(request_metadata_accepts_integral_decimal_progress_token) {
  const auto parameters{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "progressToken": 18446744073709551617
    }
  })JSON")};
  const auto [status, meta]{
      sourcemeta::core::mcp_validate_request_parameters(parameters)};
  EXPECT_EQ(status, sourcemeta::core::MCPRequestMetaStatus::Valid);
  EXPECT_TRUE(meta.has_value());
  EXPECT_EQ(meta->progress_token, &parameters.at("_meta").at("progressToken"));
}

TEST(request_metadata_rejects_fractional_progress_token) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("progressToken", sourcemeta::core::JSON{1.5});
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(value).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidProgressToken);
}

TEST(request_metadata_rejects_unknown_log_level) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("io.modelcontextprotocol/logLevel",
                           sourcemeta::core::JSON{"verbose"});
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(value).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidLogLevel);
}

TEST(request_metadata_rejects_boolean_traceparent) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("traceparent", sourcemeta::core::JSON{false});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(request_metadata_rejects_boolean_tracestate) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("tracestate", sourcemeta::core::JSON{false});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(request_metadata_rejects_boolean_baggage) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("baggage", sourcemeta::core::JSON{false});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

TEST(request_metadata_accepts_tracing_whitespace_and_escapes) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign(
      "traceparent",
      sourcemeta::core::JSON{
          "00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign(
      "tracestate",
      sourcemeta::core::JSON{"\tvendor=state,1tenant@system=value\t"});
  value.at("_meta").assign(
      "baggage",
      sourcemeta::core::JSON{
          "\tuser=alice;property=value,region=us%20east,path=%2f%2F\t"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_parameters(value)
                  .second.has_value());
}

// A tracestate that does not parse must not decide the outcome for a
// traceparent that does, so one arriving alongside a valid traceparent is left
// alone rather than costing the trace it was meant to carry
// https://www.w3.org/TR/trace-context/#tracestate-header
TEST(a_malformed_tracestate_does_not_discard_a_valid_traceparent) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign(
      "traceparent",
      sourcemeta::core::JSON{
          "00-0123456789abcdef0123456789abcdef-0123456789abcdef-01"});
  value.at("_meta").assign("tracestate", sourcemeta::core::JSON{"UPPER=value"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_parameters(value)
                  .second.has_value());
}

// A malformed traceparent ends the walk on its own terms, and the grammar
// forbids reading the tracestate once it has failed
// https://www.w3.org/TR/trace-context/#tracestate-header
TEST(a_malformed_traceparent_is_still_rejected_with_a_good_tracestate) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("traceparent", sourcemeta::core::JSON{"nope"});
  value.at("_meta").assign("tracestate",
                           sourcemeta::core::JSON{"vendor=state"});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

// A malformed tracing value is refused through the MCP path too, which is the
// policy this module owns rather than the grammar the http module owns
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(request_metadata_rejects_a_malformed_baggage) {
  auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {}
    }
  })JSON")};
  value.at("_meta").assign("baggage", sourcemeta::core::JSON{"a=%ZZ"});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_parameters(value)
                   .second.has_value());
}

// The format checks apply to what this module builds, not to what a peer
// sends, so a received timestamp that is not ISO 8601 does not cost the
// message. Both reference implementations type this field as a plain string
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#general-fields
TEST(a_received_icon_is_not_held_to_its_formats) {
  const auto value{sourcemeta::core::parse_json(R"JSON({
    "_meta": {
      "io.modelcontextprotocol/protocolVersion": "2026-07-28",
      "io.modelcontextprotocol/clientCapabilities": {},
      "io.modelcontextprotocol/clientInfo": {
        "name": "c",
        "version": "1",
        "icons": [
          {
            "src": "/relative",
            "mimeType": "not a media type",
            "sizes": [ "big" ]
          }
        ]
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_parameters(value).first,
            sourcemeta::core::MCPRequestMetaStatus::Valid);
}

TEST(legacy_result_allows_unreserved_traceparent) {
  const auto legacy{sourcemeta::core::parse_json(
      R"JSON({ "_meta": { "traceparent": false } })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_decorate_result(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26, legacy),
            legacy);
}

TEST(modern_result_rejects_boolean_traceparent) {
  const auto legacy{sourcemeta::core::parse_json(
      R"JSON({ "_meta": { "traceparent": false } })JSON")};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_decorate_result(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28, legacy),
      "Invalid MCP 2026-07-28 metadata");
}

// Every POST must carry the protocol version header
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#protocol-version-header
TEST(validate_request_headers_rejects_a_missing_protocol_version_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                std::nullopt, "tools/list", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32020,
      "message": "Missing or malformed MCP-Protocol-Version"
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_request_without_metadata) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {}
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing or malformed protocol metadata"
    }
  })JSON"));
}

// The header value must match the version carried in the body, and a
// disagreement is reported before the server decides whether it supports the
// version at all
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#protocol-version-header
TEST(validate_request_headers_rejects_a_version_disagreement) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "unknown",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "MCP-Protocol-Version",
        "headerValue": "2026-07-28",
        "bodyValue": "unknown"
      }
    }
  })JSON"));
}

// An unknown version is reported with the versions the server does support
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#protocol-version-header
TEST(validate_request_headers_rejects_an_unknown_version) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "unknown",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "unknown",
                "tools/list", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": {
        "supported": [
          "2025-03-26",
          "2025-06-18",
          "2025-11-25",
          "2026-07-28"
        ],
        "requested": "unknown"
      }
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_version_the_server_withholds) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32022,
      "message": "Unsupported protocol version",
      "data": {
        "supported": [ "2025-11-25" ],
        "requested": "2026-07-28"
      }
    }
  })JSON"));
}

// Only a tool call, a resource read and a prompt get name a target, so the
// name header is unexpected anywhere else
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#standard-request-headers
TEST(validate_request_headers_rejects_an_unexpected_name_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", "unexpected", envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32020,
      "message": "Header mismatch: unexpected header provided",
      "data": { "header": "Mcp-Name", "headerValue": "unexpected" }
    }
  })JSON"));
}

// A legacy error response has no way to omit the identifier, so an unreadable
// one is reported as null
// https://modelcontextprotocol.io/specification/2025-06-18/schema#jsonrpcerror
TEST(validate_request_headers_reports_a_null_identifier_on_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": true,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(make_parameter_headers_mirrors_every_annotated_type) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 3> descriptors{
      {{.name = "Label",
        .path = {"nested", "text"},
        .type = sourcemeta::core::JSON::Type::String},
       {.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer},
       {.name = "Enabled",
        .path = {"enabled"},
        .type = sourcemeta::core::JSON::Type::Boolean}}};
  const auto arguments{sourcemeta::core::parse_json(R"JSON({
    "nested": { "text": " café " },
    "number": 42,
    "enabled": false
  })JSON")};
  const std::vector<std::pair<std::string, std::string>> expected{
      {"Mcp-Param-Label", "=?base64?IGNhZsOpIA==?="},
      {"Mcp-Param-Count", "42"},
      {"Mcp-Param-Enabled", "false"}};
  EXPECT_EQ(
      sourcemeta::core::mcp_make_parameter_headers(descriptors, arguments),
      expected);
}

TEST(validate_parameter_headers_accepts_mirrored_values) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 2> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer},
       {.name = "Enabled",
        .path = {"enabled"},
        .type = sourcemeta::core::JSON::Type::Boolean}}};
  const auto arguments{sourcemeta::core::parse_json(
      R"JSON({ "number": 42, "enabled": false })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"Mcp-Param-Count", "42"}, {"Mcp-Param-Enabled", "false"}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, headers)
                   .has_value());
}

// An integer parameter is compared numerically rather than as a string
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#server-validation
TEST(validate_parameter_headers_compares_integers_numerically) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  const auto arguments{
      sourcemeta::core::parse_json(R"JSON({ "number": 42 })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"Mcp-Param-Count", "42.0"}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, headers)
                   .has_value());
}

// Field names are compared case insensitively, so the same parameter cannot
// arrive twice under different spellings
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#case-sensitivity
TEST(validate_parameter_headers_rejects_a_duplicate_parameter) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  const auto arguments{
      sourcemeta::core::parse_json(R"JSON({ "number": 42 })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"Mcp-Param-Count", "42"}, {"mcp-param-count", "42"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors, arguments, headers)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32020,
      "message": "Duplicate parameter header"
    }
  })JSON"));
}

// A null value is not mirrored, and the server must not expect the header
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#server-behavior-for-custom-headers
TEST(make_parameter_headers_omits_a_null_value) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  EXPECT_TRUE(sourcemeta::core::mcp_make_parameter_headers(
                  descriptors,
                  sourcemeta::core::parse_json(R"JSON({ "number": null })JSON"))
                  .empty());
}

TEST(make_parameter_headers_omits_an_absent_value) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  EXPECT_TRUE(sourcemeta::core::mcp_make_parameter_headers(
                  descriptors, sourcemeta::core::parse_json(R"JSON({})JSON"))
                  .empty());
}

// Integer values must stay within the safe range for JavaScript
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#schema-extension
TEST(make_parameter_headers_rejects_an_unsafe_integer) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(
          descriptors, sourcemeta::core::parse_json(
                           R"JSON({ "number": 9007199254740992 })JSON")),
      "Invalid mirrored argument type or unsafe integer");
}

TEST(make_parameter_headers_rejects_a_fractional_integer) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_parameter_headers(
                         descriptors, sourcemeta::core::parse_json(
                                          R"JSON({ "number": 1.5 })JSON")),
                     "Invalid mirrored argument type or unsafe integer");
}

TEST(make_parameter_headers_rejects_non_object_arguments) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Count",
        .path = {"number"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(
          descriptors, sourcemeta::core::parse_json(R"JSON([])JSON")),
      "Tool arguments must be an object");
}

TEST(parameter_header_builder_rejects_empty_path) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_parameter_headers(
                         descriptors, sourcemeta::core::JSON::make_object()),
                     "Parameter header descriptors must name a property path");
}

TEST(parameter_header_validator_rejects_empty_path) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_validate_parameter_headers(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON{0}, descriptors,
                         sourcemeta::core::JSON::make_object(), {}),
                     "Parameter header descriptors must name a property path");
}

TEST(parameter_header_builder_rejects_real_type) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::Real}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(
          descriptors, sourcemeta::core::JSON::make_object()),
      "Parameter headers only mirror strings, integers and booleans");
}

TEST(parameter_header_validator_rejects_real_type) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::Real}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_validate_parameter_headers(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{0}, descriptors,
          sourcemeta::core::JSON::make_object(), {}),
      "Parameter headers only mirror strings, integers and booleans");
}

TEST(parameter_header_builder_rejects_injected_name) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "bad\r\nInjected",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_make_parameter_headers(
                         descriptors, sourcemeta::core::JSON::make_object()),
                     "Parameter header names must be non-empty HTTP tokens");
}

TEST(parameter_header_validator_rejects_injected_name) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "bad\r\nInjected",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(sourcemeta::core::mcp_validate_parameter_headers(
                         sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                         sourcemeta::core::JSON{0}, descriptors,
                         sourcemeta::core::JSON::make_object(), {}),
                     "Parameter header names must be non-empty HTTP tokens");
}

TEST(parameter_header_builder_rejects_duplicate_names) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 2> descriptors{
      {{.name = "Value",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::String},
       {.name = "value",
        .path = {"y"},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_make_parameter_headers(
          descriptors, sourcemeta::core::JSON::make_object()),
      "Parameter header names must be case-insensitively unique");
}

TEST(parameter_header_validator_rejects_duplicate_names) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 2> descriptors{
      {{.name = "Value",
        .path = {"x"},
        .type = sourcemeta::core::JSON::Type::String},
       {.name = "value",
        .path = {"y"},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_validate_parameter_headers(
          sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
          sourcemeta::core::JSON{0}, descriptors,
          sourcemeta::core::JSON::make_object(), {}),
      "Parameter header names must be case-insensitively unique");
}

TEST(name_header_rejects_bad_base64) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  value.at("params").assign("name", sourcemeta::core::JSON{"café"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  "2026-07-28", "tools/call", "=?base64?!!!!?=", value,
                  SUPPORTED)
                  .has_value());
}

TEST(name_header_rejects_invalid_utf8) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  value.at("params").assign("name", sourcemeta::core::JSON{"café"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  "2026-07-28", "tools/call", "=?base64?/w==?=", value,
                  SUPPORTED)
                  .has_value());
}

TEST(name_header_rejects_leading_space) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  value.at("params").assign("name", sourcemeta::core::JSON{"café"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  "2026-07-28", "tools/call", " café", value, SUPPORTED)
                  .has_value());
}

TEST(name_header_rejects_non_ascii) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  value.at("params").assign("name", sourcemeta::core::JSON{"café"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  "2026-07-28", "tools/call", "café", value, SUPPORTED)
                  .has_value());
}

TEST(name_header_rejects_newline) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  value.at("params").assign("name", sourcemeta::core::JSON{"café"});
  EXPECT_TRUE(sourcemeta::core::mcp_validate_request_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  "2026-07-28", "tools/call", "cafe\n", value, SUPPORTED)
                  .has_value());
}

TEST(encoded_name_header_roundtrip_base64_marker) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto *text{"=?base64?YWJj?="};
  value.at("params").assign("name", sourcemeta::core::JSON{text});
  const auto encoded{sourcemeta::core::mcp_encode_header_value(text)};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", encoded, value, SUPPORTED)
                   .has_value());
}

// A space between the visible characters is printable ASCII, so a value
// carrying one travels literally. Only a leading or trailing space forces the
// encoded form
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#value-encoding
TEST(an_interior_space_travels_literally) {
  EXPECT_EQ(sourcemeta::core::mcp_encode_header_value("get weather"),
            "get weather");
}

TEST(a_leading_space_is_encoded) {
  EXPECT_EQ(sourcemeta::core::mcp_encode_header_value(" weather"),
            "=?base64?IHdlYXRoZXI=?=");
}

TEST(a_trailing_space_is_encoded) {
  EXPECT_EQ(sourcemeta::core::mcp_encode_header_value("weather "),
            "=?base64?d2VhdGhlciA=?=");
}

// A horizontal tab is a control character, which the specification sends to
// the encoded form even though the same passage lists it among what a field
// value may carry. Encoding is the safe side of that disagreement
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#value-encoding
TEST(a_horizontal_tab_is_encoded) {
  EXPECT_EQ(sourcemeta::core::mcp_encode_header_value("a\tb"),
            "=?base64?YQli?=");
}

TEST(a_tilde_travels_literally) {
  EXPECT_EQ(sourcemeta::core::mcp_encode_header_value("a~b"), "a~b");
}

TEST(encoded_name_header_roundtrip_padded) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto *text{" padded "};
  value.at("params").assign("name", sourcemeta::core::JSON{text});
  const auto encoded{sourcemeta::core::mcp_encode_header_value(text)};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", encoded, value, SUPPORTED)
                   .has_value());
}

TEST(encoded_name_header_roundtrip_non_ascii) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto *text{"café"};
  value.at("params").assign("name", sourcemeta::core::JSON{text});
  const auto encoded{sourcemeta::core::mcp_encode_header_value(text)};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", encoded, value, SUPPORTED)
                   .has_value());
}

TEST(encoded_name_header_roundtrip_tab) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  auto value{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const auto *text{"a\tb"};
  value.at("params").assign("name", sourcemeta::core::JSON{text});
  const auto encoded{sourcemeta::core::mcp_encode_header_value(text)};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", encoded, value, SUPPORTED)
                   .has_value());
}

TEST(header_encoder_rejects_invalid_utf8) {
  EXPECT_MCP_REJECTS(
      sourcemeta::core::mcp_encode_header_value(std::string_view{"\xff", 1}),
      "MCP header values must be UTF-8");
}

TEST(validate_request_headers_matches_case_insensitively) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"mcp-protocol-version", "2026-07-28"},
              {"MCP-METHOD", "tools/list"}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28, headers,
                   envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_rejects_a_duplicate_routing_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"mcp-protocol-version", "2026-07-28"},
              {"MCP-METHOD", "tools/list"},
              {"Mcp-Method", "tools/list"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, headers,
                envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": { "code": -32020, "message": "Duplicate routing header" }
  })JSON"));
}

TEST(validate_request_headers_ignores_routing_headers_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"mcp-protocol-version", "2025-11-25"},
              {"MCP-METHOD", "tools/list"},
              {"Mcp-Method", "tools/list"}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25, headers,
                   envelope, SUPPORTED)
                   .has_value());
}

TEST(request_meta_validation_rejects_a_legacy_revision) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2025-11-25",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::UnsupportedProtocolVersion);
}

// A request that omits a required metadata field is malformed and the server
// must reject it with invalid params
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(validate_request_headers_rejects_missing_client_capabilities) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28"
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                std::nullopt, std::nullopt, std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 0,
    "error": {
      "code": -32602,
      "message": "Invalid params: missing clientCapabilities in _meta"
    }
  })JSON"));
}

// Every request mirrors its method, and a tool call, a resource read and a
// prompt get also mirror the target they name
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#standard-request-headers
// A notification carries no stateless metadata, so it needs no parameters
// either, and the schema marks them optional on one
// https://modelcontextprotocol.io/specification/2026-07-28/schema#jsonrpcnotification
TEST(validate_request_headers_accepts_a_notification_without_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "notifications/cancelled"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "notifications/cancelled", std::nullopt,
                   envelope, SUPPORTED)
                   .has_value());
}

// A request carries the stateless metadata inside its parameters, so leaving
// them out is still a rejection
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#_meta
TEST(validate_request_headers_rejects_a_request_without_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list"
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(validate_request_headers_accepts_server_discover) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "server/discover",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "server/discover", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_tools_list) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/list", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_tools_call) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "tools/call", "calculator", envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_resources_read) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/read",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "uri": "file:///a"
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "resources/read", "file:///a", envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_prompts_get) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "prompts/get",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "review"
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "prompts/get", "review", envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_subscriptions_listen) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "subscriptions/listen",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   "2026-07-28", "subscriptions/listen", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_rejects_a_missing_method_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", std::nullopt, "calculator", envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": {
      "code": -32020,
      "message": "Missing or malformed Mcp-Method"
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_missing_name_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/call", std::nullopt, envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Name",
        "headerValue": "",
        "bodyValue": "calculator"
      }
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_method_header_disagreement) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", "calculator", envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Method",
        "headerValue": "tools/list",
        "bodyValue": "tools/call"
      }
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_name_header_disagreement) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/call", "other", envelope, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Name",
        "headerValue": "other",
        "bodyValue": "calculator"
      }
    }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_uri_header_disagreement) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "method": "resources/read",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "uri": "file:///a"
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "resources/read", "file:///b", envelope,
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 42,
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Name",
        "headerValue": "file:///b",
        "bodyValue": "file:///a"
      }
    }
  })JSON"));
}

// An envelope that is not a JSON-RPC request is rejected before any header is
// considered
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#requests
TEST(validate_request_headers_rejects_a_non_object_envelope) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", std::nullopt,
                sourcemeta::core::JSON{42}, SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(validate_request_headers_rejects_an_envelope_without_a_method) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/list", std::nullopt,
                sourcemeta::core::parse_json(
                    R"JSON({
                      "jsonrpc": "2.0",
                      "id": 1,
                      "params": {}
                    })JSON"),
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_named_request_without_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/call", "calculator",
                sourcemeta::core::parse_json(
                    R"JSON({
                "jsonrpc": "2.0",
                "id": 1,
                "method": "tools/call"
              })JSON"),
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(validate_request_headers_rejects_array_parameters) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/call", "calculator",
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": [ 1, 2, 3 ]
  })JSON"),
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

TEST(validate_request_headers_rejects_a_non_string_name) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2026-07-28", "tools/call", "calculator",
                sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": 42
    }
  })JSON"),
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": { "code": -32602, "message": "Invalid params" }
  })JSON"));
}

// An identifier that cannot be read is omitted from the error response
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#error-responses
TEST(validate_request_headers_omits_a_null_identifier) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "other", sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": null,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON"),
      SUPPORTED)};
  EXPECT_EQ(error.value(), sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

TEST(validate_request_headers_omits_an_unreadable_identifier) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "other", sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": true,
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON"),
      SUPPORTED)};
  EXPECT_EQ(error.value(), sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": { "code": -32600, "message": "Invalid Request" }
  })JSON"));
}

// A notification has no identifier to echo
// https://modelcontextprotocol.io/specification/2026-07-28/basic/index#notifications
TEST(validate_request_headers_omits_the_identifier_of_a_notification) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto error{sourcemeta::core::mcp_validate_request_headers(
      sourcemeta::core::MCPProtocolVersion::V_2026_07_28, "2026-07-28",
      "tools/call", "other", sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "method": "tools/call",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      },
      "name": "calculator"
    }
  })JSON"),
      SUPPORTED)};
  EXPECT_EQ(error.value(), sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "error": {
      "code": -32020,
      "message": "Header mismatch",
      "data": {
        "header": "Mcp-Name",
        "headerValue": "other",
        "bodyValue": "calculator"
      }
    }
  })JSON"));
}

// Routing headers are undefined before 2026-07-28, so a legacy request needs
// none of them and a disagreement is not an error
// https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http#request-metadata
TEST(validate_request_headers_without_any_header_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_method_disagreement_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   std::nullopt, "tools/list", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_name_disagreement_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   std::nullopt, std::nullopt, "other", envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_an_empty_method_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_rejects_a_malformed_version_header_2025_03_26) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "invalid-protocol", std::nullopt, std::nullopt, envelope,
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32600,
      "message": "Unsupported protocol version"
    }
  })JSON"));
}

TEST(validate_request_headers_without_any_header_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_method_disagreement_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   std::nullopt, "tools/list", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_name_disagreement_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   std::nullopt, std::nullopt, "other", envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_an_empty_method_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_rejects_a_malformed_version_header_2025_06_18) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_06_18,
                "invalid-protocol", std::nullopt, std::nullopt, envelope,
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32600,
      "message": "Unsupported protocol version"
    }
  })JSON"));
}

TEST(validate_request_headers_without_any_header_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_method_disagreement_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, "tools/list", std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_ignores_a_name_disagreement_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, std::nullopt, "other", envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_an_empty_method_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, std::nullopt, std::nullopt, envelope,
                   SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_rejects_a_malformed_version_header_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "tools/call",
    "params": { "name": "calculator" }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                "invalid-protocol", std::nullopt, std::nullopt, envelope,
                SUPPORTED)
                .value(),
            sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
      "code": -32600,
      "message": "Unsupported protocol version"
    }
  })JSON"));
}

// A legacy resource subscription names its target with a URI
// https://modelcontextprotocol.io/specification/2025-11-25/server/resources#subscriptions
TEST(validate_request_headers_accepts_resources_subscribe_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/subscribe",
    "params": { "uri": "memo://channel" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, "resources/subscribe", "memo://channel",
                   envelope, SUPPORTED)
                   .has_value());
}

TEST(validate_request_headers_accepts_resources_unsubscribe_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 4> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 1,
    "method": "resources/unsubscribe",
    "params": { "uri": "memo://channel" }
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_request_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2025_11_25,
                   std::nullopt, "resources/unsubscribe", "memo://channel",
                   envelope, SUPPORTED)
                   .has_value());
}

// MCP writes a metadata key as an optional reverse-DNS prefix, a solidus and a
// name, so a key that opens with the separator names no prefix at all and one
// that carries a second separator names more than the grammar allows
TEST(request_meta_validation_rejects_a_key_opening_with_a_separator) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "/name": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

TEST(request_meta_validation_rejects_a_key_with_two_separators) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "example.com/a/b": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

// Each label of the prefix stands between dots, so a dot with no label before
// it, a label opening with something other than a letter, a character the
// grammar does not admit, and a prefix closing on a dot all name no prefix
TEST(request_meta_validation_rejects_a_prefix_opening_with_a_dot) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { ".example/name": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

TEST(request_meta_validation_rejects_a_prefix_label_opening_with_a_digit) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "1example/name": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

TEST(request_meta_validation_rejects_a_prefix_carrying_an_underscore) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "exam_ple/name": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

TEST(request_meta_validation_rejects_a_prefix_closing_on_a_dot) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "example./name": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

TEST(request_meta_validation_rejects_an_empty_metadata_key) {
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 1, "method": "tools/list",
    "params": { "_meta": { "": true } }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_meta(envelope).first,
            sourcemeta::core::MCPRequestMetaStatus::InvalidMetaKey);
}

// A value that cannot travel in a header as it stands is carried base64
// encoded between the markers, so what the comparison sees is what the
// encoding spells out rather than the encoded form itself
TEST(parameter_string_accepts_an_encoded_value) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::String}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::JSON{"hello"});
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{"Mcp-Param-Value", "=?base64?aGVsbG8=?="}}};
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, views)
                   .has_value());
}

TEST(parameter_string_rejects_an_encoded_value_that_is_not_base64) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::String}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::JSON{"hello"});
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{"Mcp-Param-Value", "=?base64?!!!!?="}}};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_parameter_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  sourcemeta::core::JSON{0}, descriptors, arguments, views)
                  .has_value());
}

// A header value carrying a byte no header may hold says nothing the
// comparison can read, encoded or not
TEST(parameter_string_rejects_a_value_with_a_forbidden_byte) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::String}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::JSON{"hello"});
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{"Mcp-Param-Value", "hel\x01lo"}}};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_parameter_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  sourcemeta::core::JSON{0}, descriptors, arguments, views)
                  .has_value());
}

// A resource operation names what it acts on through its uri, so one whose
// parameters carry no such name leaves the request unnamed rather than
// falling back on a name meant for other operations
TEST(request_name_from_body_of_a_resource_read_without_a_uri) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({
        "method": "resources/read",
        "params": { "name": "not-a-uri" }
      })JSON")};
  EXPECT_FALSE(
      sourcemeta::core::mcp_request_name_from_body(envelope).has_value());
}

// An operation that is not one of the resource ones is named by whichever of
// the two it carries, so one carrying only a uri is named by that
TEST(request_name_from_body_of_a_subscription_listen_with_a_uri) {
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({
        "method": "subscriptions/listen",
        "params": { "uri": "file:///a" }
      })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_request_name_from_body(envelope).value(),
            "file:///a");
}

// A protocol version travels as a header value, so one carrying a byte no
// header may hold names no revision at all
TEST(legacy_headers_reject_a_protocol_version_with_a_forbidden_byte) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "2025-\x01-26", "ignored", "ignored", envelope, SUPPORTED)
                .value()
                .at("error")
                .at("message")
                .to_string(),
            "Invalid protocol version header");
}

// A revision the server knows is still the wrong one to name once a session
// has settled on another
TEST(legacy_headers_reject_a_protocol_version_other_than_the_session_one) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 3> SUPPORTED{
      {"2025-03-26", "2025-06-18", "2025-11-25"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                "2025-06-18", "ignored", "ignored", envelope, SUPPORTED)
                .value()
                .at("error")
                .at("message")
                .to_string(),
            "Protocol version disagrees with session");
}

// The name a request routes on travels in its own header, which a duplicate
// of any routing header turns down before the request is read
TEST(validate_request_headers_reject_a_duplicate_name_header) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 1> SUPPORTED{
      {"2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 0, "method": "tools/call",
    "params": {
      "name": "weather",
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2026-07-28",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"mcp-protocol-version", "2026-07-28"},
              {"Mcp-Method", "tools/call"},
              {"Mcp-Name", "weather"},
              {"mcp-name", "weather"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28, headers,
                envelope, SUPPORTED)
                .value()
                .at("error")
                .at("message")
                .to_string(),
            "Duplicate routing header");
}

// Before the routing headers existed, naming the protocol twice was a
// malformed request rather than a routing conflict
TEST(
    validate_request_headers_reject_a_duplicate_protocol_header_on_2025_11_25) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> SUPPORTED{
      {"2025-11-25", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(
      R"JSON({ "jsonrpc": "2.0", "id": 0, "method": "tools/list" })JSON")};
  const std::vector<std::pair<sourcemeta::core::JSON::StringView,
                              sourcemeta::core::JSON::StringView>>
      headers{{"mcp-protocol-version", "2025-11-25"},
              {"MCP-Protocol-Version", "2025-11-25"}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2025_11_25, headers,
                envelope, SUPPORTED)
                .value()
                .at("error")
                .at("message")
                .to_string(),
            "Duplicate protocol header");
}

// A revision the deployment lists as supported is still the wrong one to name
// once this session speaks another, which is settled after the header and the
// body have been held to each other
TEST(validate_request_headers_reject_a_version_the_session_does_not_speak) {
  constexpr std::array<sourcemeta::core::JSON::StringView, 2> SUPPORTED{
      {"2024-01-01", "2026-07-28"}};
  const auto envelope{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0", "id": 0, "method": "tools/list",
    "params": {
      "_meta": {
        "io.modelcontextprotocol/protocolVersion": "2024-01-01",
        "io.modelcontextprotocol/clientCapabilities": {}
      }
    }
  })JSON")};
  EXPECT_EQ(sourcemeta::core::mcp_validate_request_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                "2024-01-01", "tools/list", std::nullopt, envelope, SUPPORTED)
                .value()
                .at("error")
                .at("code"),
            sourcemeta::core::JSON{-32020});
}

// The arguments a call mirrors into headers are the object the call was made
// with, so anything else mirrors nothing
TEST(parameter_headers_reject_arguments_that_are_not_an_object) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::String}}};
  EXPECT_EQ(sourcemeta::core::mcp_validate_parameter_headers(
                sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                sourcemeta::core::JSON{0}, descriptors,
                sourcemeta::core::JSON{42}, {})
                .value()
                .at("error")
                .at("message")
                .to_string(),
            "Invalid tool arguments");
}

// A nested parameter is reached by walking the path it names, which stops at
// the first step that holds no further members, leaving the parameter absent
TEST(parameter_headers_ignore_a_path_through_a_non_object) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"a", "b"},
        .type = sourcemeta::core::JSON::Type::String}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("a", sourcemeta::core::JSON{42});
  EXPECT_FALSE(sourcemeta::core::mcp_validate_parameter_headers(
                   sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                   sourcemeta::core::JSON{0}, descriptors, arguments, {})
                   .has_value());
}

// A header claiming a parameter the call never passed mirrors an argument
// that is not there
TEST(parameter_headers_reject_a_header_for_an_absent_parameter) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"a", "b"},
        .type = sourcemeta::core::JSON::Type::String}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("a", sourcemeta::core::JSON{42});
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{"Mcp-Param-Value", "anything"}}};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_parameter_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  sourcemeta::core::JSON{0}, descriptors, arguments, views)
                  .has_value());
}

// An integer parameter is mirrored as the number it is, so a header that
// spells no number at all matches nothing
TEST(parameter_integer_rejects_a_header_that_is_not_a_number) {
  const std::array<sourcemeta::core::MCPHeaderParameter, 1> descriptors{
      {{.name = "Value",
        .path = {"value"},
        .type = sourcemeta::core::JSON::Type::Integer}}};
  auto arguments{sourcemeta::core::JSON::make_object()};
  arguments.assign("value", sourcemeta::core::JSON{42});
  const std::array<std::pair<sourcemeta::core::JSON::StringView,
                             sourcemeta::core::JSON::StringView>,
                   1>
      views{{{"Mcp-Param-Value", "forty two"}}};
  EXPECT_TRUE(sourcemeta::core::mcp_validate_parameter_headers(
                  sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
                  sourcemeta::core::JSON{0}, descriptors, arguments, views)
                  .has_value());
}
