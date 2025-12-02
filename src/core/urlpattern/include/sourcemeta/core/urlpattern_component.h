#ifndef SOURCEMETA_CORE_URLPATTERN_COMPONENT_H_
#define SOURCEMETA_CORE_URLPATTERN_COMPONENT_H_

#ifndef SOURCEMETA_CORE_URLPATTERN_EXPORT
#include <sourcemeta/core/urlpattern_export.h>
#endif

#include <sourcemeta/core/urlpattern_part.h>

#include <compare>       // std::strong_ordering
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace sourcemeta::core {

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternComponentResult {
  [[nodiscard]] auto size() const noexcept -> std::size_t;
  [[nodiscard]] auto at(const std::size_t index) const noexcept
      -> std::string_view;
  [[nodiscard]] auto at(const std::string_view name) const noexcept
      -> std::optional<std::string_view>;

  auto insert(const std::string_view value) -> void;
  auto insert(const std::string_view name, const std::string_view value)
      -> void;
  auto insert(const std::string_view name, const std::size_t index) -> void;

private:
  std::vector<std::string_view> positions;
  std::unordered_map<std::string, std::size_t> names;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternProtocol {
  URLPatternProtocol() : value{URLPatternPartAsterisk{}} {}
  URLPatternProtocol(const char *input);
  URLPatternProtocol(const URLPatternProtocol &) = default;
  URLPatternProtocol(URLPatternProtocol &&) noexcept = default;
  auto operator=(const URLPatternProtocol &) -> URLPatternProtocol & = default;
  auto operator=(URLPatternProtocol &&) noexcept
      -> URLPatternProtocol & = default;

  auto operator==(const URLPatternProtocol &other) const -> bool = default;
  auto operator<=>(const URLPatternProtocol &other) const
      -> std::strong_ordering;

  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternUsername {
  URLPatternUsername() : value{URLPatternPartAsterisk{}} {}
  URLPatternUsername(const char *input);
  URLPatternUsername(const URLPatternUsername &) = default;
  URLPatternUsername(URLPatternUsername &&) noexcept = default;
  auto operator=(const URLPatternUsername &) -> URLPatternUsername & = default;
  auto operator=(URLPatternUsername &&) noexcept
      -> URLPatternUsername & = default;

  auto operator==(const URLPatternUsername &other) const -> bool = default;
  auto operator<=>(const URLPatternUsername &other) const
      -> std::strong_ordering;

  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPassword {
  URLPatternPassword() : value{URLPatternPartAsterisk{}} {}
  URLPatternPassword(const char *input);
  URLPatternPassword(const URLPatternPassword &) = default;
  URLPatternPassword(URLPatternPassword &&) noexcept = default;
  auto operator=(const URLPatternPassword &) -> URLPatternPassword & = default;
  auto operator=(URLPatternPassword &&) noexcept
      -> URLPatternPassword & = default;

  auto operator==(const URLPatternPassword &other) const -> bool = default;
  auto operator<=>(const URLPatternPassword &other) const
      -> std::strong_ordering;

  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternHostname {
  URLPatternHostname() : value{{URLPatternPartAsterisk{}}} {}
  URLPatternHostname(const char *input);
  URLPatternHostname(const URLPatternHostname &) = default;
  URLPatternHostname(URLPatternHostname &&) noexcept = default;
  auto operator=(const URLPatternHostname &) -> URLPatternHostname & = default;
  auto operator=(URLPatternHostname &&) noexcept
      -> URLPatternHostname & = default;

  auto operator==(const URLPatternHostname &other) const -> bool = default;
  auto operator<=>(const URLPatternHostname &other) const
      -> std::strong_ordering;

  std::vector<URLPatternPart> value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPort {
  URLPatternPort() : value{URLPatternPartAsterisk{}} {}
  URLPatternPort(const char *input);
  URLPatternPort(const URLPatternPort &) = default;
  URLPatternPort(URLPatternPort &&) noexcept = default;
  auto operator=(const URLPatternPort &) -> URLPatternPort & = default;
  auto operator=(URLPatternPort &&) noexcept -> URLPatternPort & = default;

  auto operator==(const URLPatternPort &other) const -> bool = default;
  auto operator<=>(const URLPatternPort &other) const -> std::strong_ordering;

  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPathname {
  URLPatternPathname() : value{{URLPatternPartAsterisk{}}} {}
  URLPatternPathname(const char *input);
  URLPatternPathname(const URLPatternPathname &) = default;
  URLPatternPathname(URLPatternPathname &&) noexcept = default;
  auto operator=(const URLPatternPathname &) -> URLPatternPathname & = default;
  auto operator=(URLPatternPathname &&) noexcept
      -> URLPatternPathname & = default;

  auto operator==(const URLPatternPathname &other) const -> bool = default;
  auto operator<=>(const URLPatternPathname &other) const
      -> std::strong_ordering;

  std::vector<URLPatternPart> value;
  // TODO: Find a way to get rid of this
  bool is_bare_pattern{false};
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternSearch {
  URLPatternSearch() : value{URLPatternPartAsterisk{}} {}
  URLPatternSearch(const char *input);
  URLPatternSearch(const URLPatternSearch &) = default;
  URLPatternSearch(URLPatternSearch &&) noexcept = default;
  auto operator=(const URLPatternSearch &) -> URLPatternSearch & = default;
  auto operator=(URLPatternSearch &&) noexcept -> URLPatternSearch & = default;

  auto operator==(const URLPatternSearch &other) const -> bool = default;
  auto operator<=>(const URLPatternSearch &other) const -> std::strong_ordering;

  // URL Pattern treats search queries as opaque strings
  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

/// @ingroup urlpattern
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternHash {
  URLPatternHash() : value{URLPatternPartAsterisk{}} {}
  URLPatternHash(const char *input);
  URLPatternHash(const URLPatternHash &) = default;
  URLPatternHash(URLPatternHash &&) noexcept = default;
  auto operator=(const URLPatternHash &) -> URLPatternHash & = default;
  auto operator=(URLPatternHash &&) noexcept -> URLPatternHash & = default;

  auto operator==(const URLPatternHash &other) const -> bool = default;
  auto operator<=>(const URLPatternHash &other) const -> std::strong_ordering;

  URLPatternPart value;
  [[nodiscard]] auto match(const std::string_view input) const
      -> std::optional<URLPatternComponentResult>;
};

} // namespace sourcemeta::core

#endif
