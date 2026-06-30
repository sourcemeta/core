#include <sourcemeta/core/http.h>

#include "helpers.h"

#include <chrono>      // std::chrono::seconds
#include <optional>    // std::optional, std::nullopt
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_cache_control_max_age(const std::string_view cache_control) noexcept
    -> std::optional<std::chrono::seconds> {
  // RFC 9111 §1.2.2: a delta-seconds value larger than the cache can
  // represent, or any subsequent overflow, must be treated as 2^31
  constexpr std::chrono::seconds::rep overflow_seconds{2147483648};
  std::optional<std::chrono::seconds> result;
  bool found{false};
  http_for_each_list_entry(
      cache_control, [&](const std::string_view directive) -> void {
        if (found) {
          return;
        }

        const auto separator{directive.find('=')};
        if (separator == std::string_view::npos ||
            !http_iequals_ascii(http_subview(directive, 0, separator),
                                "max-age")) {
          return;
        }

        found = true;
        const auto digits{http_subview(directive, separator + 1,
                                       directive.size() - separator - 1)};
        if (digits.empty()) {
          return;
        }

        std::chrono::seconds::rep seconds{0};
        for (const char character : digits) {
          if (character < '0' || character > '9') {
            return;
          }

          seconds = (seconds * 10) + (character - '0');
          if (seconds > overflow_seconds) {
            seconds = overflow_seconds;
          }
        }

        result = std::chrono::seconds{seconds};
      });

  return result;
}

} // namespace sourcemeta::core
