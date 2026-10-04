#ifndef SOURCEMETA_CORE_MCP_LOGGING_H_
#define SOURCEMETA_CORE_MCP_LOGGING_H_

#include <array>
#include <cstdint>
#include <optional>
#include <sourcemeta/core/json.h>
#include <utility>

namespace sourcemeta::core {

/// @ingroup mcp
/// Syslog severity levels, ordered from least to most severe. A request's
/// logging threshold admits its own level and all more severe levels.
/// @see
/// https://modelcontextprotocol.io/specification/2026-07-28/server/utilities/logging
enum class MCPLogLevel : std::uint8_t {
  Debug,
  Info,
  Notice,
  Warning,
  Error,
  Critical,
  Alert,
  Emergency
};

/// @ingroup mcp
/// Convert a logging severity to its canonical wire string.
constexpr auto mcp_log_level_string(const MCPLogLevel level) noexcept
    -> JSON::StringView {
  switch (level) {
    case MCPLogLevel::Debug:
      return "debug";
    case MCPLogLevel::Info:
      return "info";
    case MCPLogLevel::Notice:
      return "notice";
    case MCPLogLevel::Warning:
      return "warning";
    case MCPLogLevel::Error:
      return "error";
    case MCPLogLevel::Critical:
      return "critical";
    case MCPLogLevel::Alert:
      return "alert";
    case MCPLogLevel::Emergency:
      return "emergency";
  }
  std::unreachable();
}

/// @ingroup mcp
/// Parse an exact, case-sensitive logging severity without allocating.
constexpr auto mcp_resolve_log_level(const JSON::StringView value) noexcept
    -> std::optional<MCPLogLevel> {
  constexpr std::array<MCPLogLevel, 8> LEVELS{
      {MCPLogLevel::Debug, MCPLogLevel::Info, MCPLogLevel::Notice,
       MCPLogLevel::Warning, MCPLogLevel::Error, MCPLogLevel::Critical,
       MCPLogLevel::Alert, MCPLogLevel::Emergency}};
  for (const auto level : LEVELS) {
    if (mcp_log_level_string(level) == value) {
      return level;
    }
  }
  return std::nullopt;
}

/// @ingroup mcp
/// Whether a notification meets an explicitly selected logging threshold.
constexpr auto mcp_log_level_enabled(const MCPLogLevel level,
                                     const MCPLogLevel threshold) noexcept
    -> bool {
  return level >= threshold;
}

} // namespace sourcemeta::core
#endif
