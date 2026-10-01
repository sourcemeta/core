#ifndef SOURCEMETA_CORE_MCP_HELPERS_H_
#define SOURCEMETA_CORE_MCP_HELPERS_H_

#include <sourcemeta/core/json.h>

#include <string_view>

namespace sourcemeta::core {

using namespace std::string_view_literals;

inline constexpr auto MCP_HASH_ANNOTATIONS{
    sourcemeta::core::JSON::Object::hash("annotations"sv)};
inline constexpr auto MCP_HASH_ARGUMENTS{
    sourcemeta::core::JSON::Object::hash("arguments"sv)};
inline constexpr auto MCP_HASH_BODY_VALUE{
    sourcemeta::core::JSON::Object::hash("bodyValue"sv)};
inline constexpr auto MCP_HASH_CACHE_SCOPE{
    sourcemeta::core::JSON::Object::hash("cacheScope"sv)};
inline constexpr auto MCP_HASH_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash("capabilities"sv)};
inline constexpr auto MCP_HASH_COMPLETIONS{
    sourcemeta::core::JSON::Object::hash("completions"sv)};
inline constexpr auto MCP_HASH_CONTENT{
    sourcemeta::core::JSON::Object::hash("content"sv)};
inline constexpr auto MCP_HASH_CONTENTS{
    sourcemeta::core::JSON::Object::hash("contents"sv)};
inline constexpr auto MCP_HASH_CONTEXT{
    sourcemeta::core::JSON::Object::hash("context"sv)};
inline constexpr auto MCP_HASH_DESCRIPTION{
    sourcemeta::core::JSON::Object::hash("description"sv)};
inline constexpr auto MCP_HASH_DESTRUCTIVE_HINT{
    sourcemeta::core::JSON::Object::hash("destructiveHint"sv)};
inline constexpr auto MCP_HASH_ELICITATION{
    sourcemeta::core::JSON::Object::hash("elicitation"sv)};
inline constexpr auto MCP_HASH_EXPERIMENTAL{
    sourcemeta::core::JSON::Object::hash("experimental"sv)};
inline constexpr auto MCP_HASH_EXTENSIONS{
    sourcemeta::core::JSON::Object::hash("extensions"sv)};
inline constexpr auto MCP_HASH_FORM{
    sourcemeta::core::JSON::Object::hash("form"sv)};
inline constexpr auto MCP_HASH_HEADER{
    sourcemeta::core::JSON::Object::hash("header"sv)};
inline constexpr auto MCP_HASH_HEADER_VALUE{
    sourcemeta::core::JSON::Object::hash("headerValue"sv)};
inline constexpr auto MCP_HASH_IDEMPOTENT_HINT{
    sourcemeta::core::JSON::Object::hash("idempotentHint"sv)};
inline constexpr auto MCP_HASH_INPUT_REQUESTS{
    sourcemeta::core::JSON::Object::hash("inputRequests"sv)};
inline constexpr auto MCP_HASH_INPUT_RESPONSES{
    sourcemeta::core::JSON::Object::hash("inputResponses"sv)};
inline constexpr auto MCP_HASH_INPUT_SCHEMA{
    sourcemeta::core::JSON::Object::hash("inputSchema"sv)};
inline constexpr auto MCP_HASH_INSTRUCTIONS{
    sourcemeta::core::JSON::Object::hash("instructions"sv)};
inline constexpr auto MCP_HASH_IS_ERROR{
    sourcemeta::core::JSON::Object::hash("isError"sv)};
inline constexpr auto MCP_HASH_JSONRPC{
    sourcemeta::core::JSON::Object::hash("jsonrpc"sv)};
inline constexpr auto MCP_HASH_LIST_CHANGED{
    sourcemeta::core::JSON::Object::hash("listChanged"sv)};
inline constexpr auto MCP_HASH_LOGGING{
    sourcemeta::core::JSON::Object::hash("logging"sv)};
inline constexpr auto MCP_HASH_META{
    sourcemeta::core::JSON::Object::hash("_meta"sv)};
inline constexpr auto MCP_HASH_META_CLIENT_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/clientCapabilities"sv)};
inline constexpr auto MCP_HASH_META_CLIENT_INFO{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/clientInfo"sv)};
inline constexpr auto MCP_HASH_META_LOG_LEVEL{
    sourcemeta::core::JSON::Object::hash("io.modelcontextprotocol/logLevel"sv)};
inline constexpr auto MCP_HASH_META_PROTOCOL_VERSION{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/protocolVersion"sv)};
inline constexpr auto MCP_HASH_META_SERVER_INFO{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/serverInfo"sv)};
inline constexpr auto MCP_HASH_META_SUBSCRIPTION_ID{
    sourcemeta::core::JSON::Object::hash(
        "io.modelcontextprotocol/subscriptionId"sv)};
inline constexpr auto MCP_HASH_METHOD{
    sourcemeta::core::JSON::Object::hash("method"sv)};
inline constexpr auto MCP_HASH_MIME_TYPE{
    sourcemeta::core::JSON::Object::hash("mimeType"sv)};
inline constexpr auto MCP_HASH_NAME{
    sourcemeta::core::JSON::Object::hash("name"sv)};
inline constexpr auto MCP_HASH_NEXT_CURSOR{
    sourcemeta::core::JSON::Object::hash("nextCursor"sv)};
inline constexpr auto MCP_HASH_NOTIFICATIONS{
    sourcemeta::core::JSON::Object::hash("notifications"sv)};
inline constexpr auto MCP_HASH_OPEN_WORLD_HINT{
    sourcemeta::core::JSON::Object::hash("openWorldHint"sv)};
inline constexpr auto MCP_HASH_OUTPUT_SCHEMA{
    sourcemeta::core::JSON::Object::hash("outputSchema"sv)};
inline constexpr auto MCP_HASH_PARAMS{
    sourcemeta::core::JSON::Object::hash("params"sv)};
inline constexpr auto MCP_HASH_PRIORITY{
    sourcemeta::core::JSON::Object::hash("priority"sv)};
inline constexpr auto MCP_HASH_PROMPTS{
    sourcemeta::core::JSON::Object::hash("prompts"sv)};
inline constexpr auto MCP_HASH_PROTOCOL_VERSION{
    sourcemeta::core::JSON::Object::hash("protocolVersion"sv)};
inline constexpr auto MCP_HASH_READ_ONLY_HINT{
    sourcemeta::core::JSON::Object::hash("readOnlyHint"sv)};
inline constexpr auto MCP_HASH_REQUEST_STATE{
    sourcemeta::core::JSON::Object::hash("requestState"sv)};
inline constexpr auto MCP_HASH_REQUESTED{
    sourcemeta::core::JSON::Object::hash("requested"sv)};
inline constexpr auto MCP_HASH_REQUIRED_CAPABILITIES{
    sourcemeta::core::JSON::Object::hash("requiredCapabilities"sv)};
inline constexpr auto MCP_HASH_RESOURCE_TEMPLATES{
    sourcemeta::core::JSON::Object::hash("resourceTemplates"sv)};
inline constexpr auto MCP_HASH_RESOURCES{
    sourcemeta::core::JSON::Object::hash("resources"sv)};
inline constexpr auto MCP_HASH_RESULT{
    sourcemeta::core::JSON::Object::hash("result"sv)};
inline constexpr auto MCP_HASH_RESULT_TYPE{
    sourcemeta::core::JSON::Object::hash("resultType"sv)};
inline constexpr auto MCP_HASH_ROOTS{
    sourcemeta::core::JSON::Object::hash("roots"sv)};
inline constexpr auto MCP_HASH_SAMPLING{
    sourcemeta::core::JSON::Object::hash("sampling"sv)};
inline constexpr auto MCP_HASH_SCOPE{
    sourcemeta::core::JSON::Object::hash("scope"sv)};
inline constexpr auto MCP_HASH_SERVER_INFO{
    sourcemeta::core::JSON::Object::hash("serverInfo"sv)};
inline constexpr auto MCP_HASH_SIZE{
    sourcemeta::core::JSON::Object::hash("size"sv)};
inline constexpr auto MCP_HASH_STRUCTURED_CONTENT{
    sourcemeta::core::JSON::Object::hash("structuredContent"sv)};
inline constexpr auto MCP_HASH_SUBSCRIBE{
    sourcemeta::core::JSON::Object::hash("subscribe"sv)};
inline constexpr auto MCP_HASH_SUBSCRIPTIONS{
    sourcemeta::core::JSON::Object::hash("subscriptions"sv)};
inline constexpr auto MCP_HASH_SUPPORTED{
    sourcemeta::core::JSON::Object::hash("supported"sv)};
inline constexpr auto MCP_HASH_SUPPORTED_VERSIONS{
    sourcemeta::core::JSON::Object::hash("supportedVersions"sv)};
inline constexpr auto MCP_HASH_TEXT{
    sourcemeta::core::JSON::Object::hash("text"sv)};
inline constexpr auto MCP_HASH_TITLE{
    sourcemeta::core::JSON::Object::hash("title"sv)};
inline constexpr auto MCP_HASH_TOOLS{
    sourcemeta::core::JSON::Object::hash("tools"sv)};
inline constexpr auto MCP_HASH_TTL_MS{
    sourcemeta::core::JSON::Object::hash("ttlMs"sv)};
inline constexpr auto MCP_HASH_TYPE{
    sourcemeta::core::JSON::Object::hash("type"sv)};
inline constexpr auto MCP_HASH_URI{
    sourcemeta::core::JSON::Object::hash("uri"sv)};
inline constexpr auto MCP_HASH_URI_TEMPLATE{
    sourcemeta::core::JSON::Object::hash("uriTemplate"sv)};
inline constexpr auto MCP_HASH_URL{
    sourcemeta::core::JSON::Object::hash("url"sv)};
inline constexpr auto MCP_HASH_VERSION{
    sourcemeta::core::JSON::Object::hash("version"sv)};
inline constexpr auto MCP_HASH_WEBSITE_URL{
    sourcemeta::core::JSON::Object::hash("websiteUrl"sv)};

} // namespace sourcemeta::core

#endif
