#ifndef SOURCEMETA_CORE_URLPATTERN_H_
#define SOURCEMETA_CORE_URLPATTERN_H_

#ifndef SOURCEMETA_CORE_URLPATTERN_EXPORT
#include <sourcemeta/core/urlpattern_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/urlpattern_component.h>
#include <sourcemeta/core/urlpattern_error.h>
#include <sourcemeta/core/urlpattern_part.h>
// NOLINTEND(misc-include-cleaner)

#include <sourcemeta/core/json.h>

#include <compare>     // std::strong_ordering
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup urlpattern URL Pattern
/// @brief A WHATWG URL Pattern implementation.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/urlpattern.h>
/// ```

namespace sourcemeta::core {

/// @ingroup urlpattern
struct URLPatternResult {
  std::optional<URLPatternComponentResult> protocol;
  std::optional<URLPatternComponentResult> username;
  std::optional<URLPatternComponentResult> password;
  std::optional<URLPatternComponentResult> hostname;
  std::optional<URLPatternComponentResult> port;
  std::optional<URLPatternComponentResult> pathname;
  std::optional<URLPatternComponentResult> search;
  std::optional<URLPatternComponentResult> hash;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternInput {
  std::string protocol;
  std::string username;
  std::string password;
  std::string hostname;
  std::string port;
  std::string pathname;
  std::string search;
  std::string hash;

  [[nodiscard]] static auto parse(const JSON &input)
      -> std::optional<URLPatternInput>;
  [[nodiscard]] static auto parse(const std::string_view input)
      -> std::optional<URLPatternInput>;
};

/// @ingroup urlpattern
/// See https://urlpattern.spec.whatwg.org/#url-pattern-struct
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPattern {
  URLPatternProtocol protocol;
  URLPatternUsername username;
  URLPatternPassword password;
  URLPatternHostname hostname;
  URLPatternPort port;
  URLPatternPathname pathname;
  URLPatternSearch search;
  URLPatternHash hash;

  auto operator==(const URLPattern &other) const -> bool = default;
  auto operator<=>(const URLPattern &other) const -> std::strong_ordering;

  [[nodiscard]] static auto parse(const std::string_view input) -> URLPattern;
  [[nodiscard]] static auto parse(const JSON &input)
      -> std::optional<URLPattern>;

  [[nodiscard]] auto match(const URLPatternInput &input) const
      -> URLPatternResult;
};

} // namespace sourcemeta::core

#endif
