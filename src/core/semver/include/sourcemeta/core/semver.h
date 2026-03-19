#ifndef SOURCEMETA_CORE_SEMVER_H_
#define SOURCEMETA_CORE_SEMVER_H_

#ifndef SOURCEMETA_CORE_SEMVER_EXPORT
#include <sourcemeta/core/semver_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/semver_error.h>
// NOLINTEND(misc-include-cleaner)

#include <sourcemeta/core/preprocessor.h>

#include <cstdint>     // std::uint64_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

/// @defgroup semver SemVer
/// @brief An implementation of the Semantic Versioning 2.0.0 specification.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/semver.h>
/// ```

namespace sourcemeta::core {

/// @ingroup semver
/// A non-owning view over a parsed SemVer 2.0.0 version string.
/// The input string must outlive this object.
class SOURCEMETA_CORE_SEMVER_EXPORT SemVer {
public:
  SemVer(std::string_view input);

  [[nodiscard]] static auto from(std::string_view input) noexcept
      -> std::optional<SemVer>;

  [[nodiscard]] SOURCEMETA_FORCEINLINE inline auto major() const noexcept
      -> std::uint64_t {
    return this->major_;
  }

  [[nodiscard]] SOURCEMETA_FORCEINLINE inline auto minor() const noexcept
      -> std::uint64_t {
    return this->minor_;
  }

  [[nodiscard]] SOURCEMETA_FORCEINLINE inline auto patch() const noexcept
      -> std::uint64_t {
    return this->patch_;
  }

  [[nodiscard]] SOURCEMETA_FORCEINLINE inline auto pre_release() const noexcept
      -> std::string_view {
    return this->pre_release_;
  }

  [[nodiscard]] SOURCEMETA_FORCEINLINE inline auto build() const noexcept
      -> std::string_view {
    return this->build_;
  }

  // Build metadata is included in equality because equality is not
  // precedence. The spec says build metadata must be ignored when
  // determining version *precedence*, but two versions with different build
  // metadata are distinct versions.
  // See https://semver.org/spec/v2.0.0.html#spec-item-10
  SOURCEMETA_FORCEINLINE inline auto
  operator==(const SemVer &other) const noexcept -> bool {
    return this->major_ == other.major_ && this->minor_ == other.minor_ &&
           this->patch_ == other.patch_ &&
           this->pre_release_ == other.pre_release_ &&
           this->build_ == other.build_;
  }

  auto operator<(const SemVer &other) const noexcept -> bool;

  SOURCEMETA_FORCEINLINE inline auto
  operator!=(const SemVer &other) const noexcept -> bool {
    return !(*this == other);
  }

  SOURCEMETA_FORCEINLINE inline auto
  operator>(const SemVer &other) const noexcept -> bool {
    return other < *this;
  }

  SOURCEMETA_FORCEINLINE inline auto
  operator<=(const SemVer &other) const noexcept -> bool {
    return !(other < *this);
  }

  SOURCEMETA_FORCEINLINE inline auto
  operator>=(const SemVer &other) const noexcept -> bool {
    return !(*this < other);
  }

  [[nodiscard]] auto to_string() const -> std::string;

private:
  SemVer() = default;
  std::uint64_t major_{0};
  std::uint64_t minor_{0};
  std::uint64_t patch_{0};
  std::string_view pre_release_;
  std::string_view build_;
};

} // namespace sourcemeta::core

#endif
