#include <sourcemeta/core/mcp_error.h>

#include <sourcemeta/core/json.h>

#include <sourcemeta/core/test.h>

#include <cstdint> // std::int64_t

TEST(code_resource_not_found) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_RESOURCE_NOT_FOUND,
            static_cast<std::int64_t>(-32002));
}

TEST(code_url_elicitation_required) {
  EXPECT_EQ(sourcemeta::core::MCP_CODE_URL_ELICITATION_REQUIRED,
            static_cast<std::int64_t>(-32042));
}

TEST(error_resource_not_found_with_integer_id) {
  const auto identifier{sourcemeta::core::JSON{3}};
  const auto envelope{
      sourcemeta::core::mcp_make_error_resource_not_found(identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": 3,
    "error": {
      "code": -32002,
      "message": "Resource not found"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}

TEST(error_resource_not_found_with_string_id) {
  const auto identifier{sourcemeta::core::JSON{"req-7"}};
  const auto envelope{
      sourcemeta::core::mcp_make_error_resource_not_found(identifier)};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "jsonrpc": "2.0",
    "id": "req-7",
    "error": {
      "code": -32002,
      "message": "Resource not found"
    }
  })JSON")};
  EXPECT_EQ(envelope, expected);
}
