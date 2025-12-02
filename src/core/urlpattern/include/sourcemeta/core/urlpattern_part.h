#ifndef SOURCEMETA_CORE_URLPATTERN_PART_H_
#define SOURCEMETA_CORE_URLPATTERN_PART_H_

#ifndef SOURCEMETA_CORE_URLPATTERN_EXPORT
#include <sourcemeta/core/urlpattern_export.h>
#endif

#include <sourcemeta/core/regex.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <variant>     // std::variant
#include <vector>      // std::vector

namespace sourcemeta::core {

// See https://urlpattern.spec.whatwg.org/#tokens

/// @ingroup urlpattern
/// The token represents a string of the form "(<regular expression>)". The
/// regular expression is required to consist of only ASCII code points.
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartRegExp {
  Regex value;
  std::string original_pattern;
  auto operator==(const URLPatternPartRegExp &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "(<regular expression>)?".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartRegExpOptional {
  Regex value;
  std::string original_pattern;
  auto operator==(const URLPatternPartRegExpOptional &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "(<regular expression>)+".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartRegExpMultiple {
  Regex value;
  std::string original_pattern;
  auto operator==(const URLPatternPartRegExpMultiple &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "(<regular expression>)*".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartRegExpAsterisk {
  Regex value;
  std::string original_pattern;
  auto operator==(const URLPatternPartRegExpAsterisk &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form ":<name>". The name value is
/// restricted to code points that are consistent with JavaScript identifiers.
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartName {
  std::string value;
  auto operator==(const URLPatternPartName &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form ":<name>(<regular expression>)".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartNameRegExp {
  std::string value;
  Regex modifier;
  std::string original_pattern;
  auto operator==(const URLPatternPartNameRegExp &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form ":<name>?".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartNameOptional {
  std::string value;
  auto operator==(const URLPatternPartNameOptional &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form ":<name>+".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartNameMultiple {
  std::string value;
  auto operator==(const URLPatternPartNameMultiple &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form ":<name>*".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartNameAsterisk {
  std::string value;
  auto operator==(const URLPatternPartNameAsterisk &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a valid pattern code point without any special
/// syntactical meaning.
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartChar {
  std::string value;
  auto operator==(const URLPatternPartChar &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a U+002A (*) code point that is a wildcard matching
/// group
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartAsterisk {
  auto operator==(const URLPatternPartAsterisk &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "*?".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartAsteriskOptional {
  auto operator==(const URLPatternPartAsteriskOptional &) const
      -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "*+".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartAsteriskMultiple {
  auto operator==(const URLPatternPartAsteriskMultiple &) const
      -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "**".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartAsteriskAsterisk {
  auto operator==(const URLPatternPartAsteriskAsterisk &) const
      -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
using URLPatternPartNonGroup = std::vector<std::variant<
    URLPatternPartRegExp, URLPatternPartRegExpOptional,
    URLPatternPartRegExpMultiple, URLPatternPartRegExpAsterisk,
    URLPatternPartName, URLPatternPartNameRegExp, URLPatternPartNameOptional,
    URLPatternPartNameMultiple, URLPatternPartNameAsterisk, URLPatternPartChar,
    URLPatternPartAsterisk, URLPatternPartAsteriskOptional,
    URLPatternPartAsteriskMultiple, URLPatternPartAsteriskAsterisk>>;

/// @ingroup urlpattern
/// The token represents a string of the form "{<any>}".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartGroup {
  URLPatternPartNonGroup value;
  // Whether the group content starts with a segment delimiter (e.g. {/:bar})
  bool has_inner_segment_prefix = false;
  // Whether the group content ends with a segment delimiter (e.g. {bar/})
  bool has_inner_segment_suffix = false;
  auto operator==(const URLPatternPartGroup &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "{<any>}?".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartGroupOptional {
  URLPatternPartNonGroup value;
  // Whether the group content starts with a segment delimiter (e.g. {/:bar}?)
  bool has_inner_segment_prefix = false;
  // Whether the group content ends with a segment delimiter (e.g. {bar/}?)
  bool has_inner_segment_suffix = false;
  auto operator==(const URLPatternPartGroupOptional &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "{<any>}+".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartGroupMultiple {
  URLPatternPartNonGroup value;
  // Whether the group content starts with a segment delimiter (e.g. {/:bar}+)
  bool has_inner_segment_prefix = false;
  // Whether the group content ends with a segment delimiter (e.g. {bar/}+)
  bool has_inner_segment_suffix = false;
  auto operator==(const URLPatternPartGroupMultiple &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// The token represents a string of the form "{<any>}*".
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartGroupAsterisk {
  URLPatternPartNonGroup value;
  // Whether the group content starts with a segment delimiter (e.g. {/:bar}*)
  bool has_inner_segment_prefix = false;
  // Whether the group content ends with a segment delimiter (e.g. {bar/}*)
  bool has_inner_segment_suffix = false;
  auto operator==(const URLPatternPartGroupAsterisk &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
/// This token represents a single segment that consist of multiple tokens, such
/// as "foo-:bar{baz}+"
struct SOURCEMETA_CORE_URLPATTERN_EXPORT URLPatternPartComplexSegment {
  std::vector<std::variant<
      URLPatternPartRegExp, URLPatternPartRegExpOptional,
      URLPatternPartRegExpMultiple, URLPatternPartRegExpAsterisk,
      URLPatternPartName, URLPatternPartNameRegExp, URLPatternPartNameOptional,
      URLPatternPartNameMultiple, URLPatternPartNameAsterisk,
      URLPatternPartChar, URLPatternPartAsterisk,
      URLPatternPartAsteriskOptional, URLPatternPartAsteriskMultiple,
      URLPatternPartAsteriskAsterisk, URLPatternPartGroup,
      URLPatternPartGroupOptional, URLPatternPartGroupMultiple,
      URLPatternPartGroupAsterisk>>
      value;
  auto operator==(const URLPatternPartComplexSegment &) const -> bool = default;
  [[nodiscard]] auto matches(const std::string_view segment) const noexcept
      -> bool;
};

/// @ingroup urlpattern
using URLPatternPart =
    std::variant<URLPatternPartRegExp, URLPatternPartRegExpOptional,
                 URLPatternPartRegExpMultiple, URLPatternPartRegExpAsterisk,
                 URLPatternPartName, URLPatternPartNameRegExp,
                 URLPatternPartNameOptional, URLPatternPartNameMultiple,
                 URLPatternPartNameAsterisk, URLPatternPartChar,
                 URLPatternPartAsterisk, URLPatternPartAsteriskOptional,
                 URLPatternPartAsteriskMultiple, URLPatternPartAsteriskAsterisk,
                 URLPatternPartGroup, URLPatternPartGroupOptional,
                 URLPatternPartGroupMultiple, URLPatternPartGroupAsterisk,
                 URLPatternPartComplexSegment>;

} // namespace sourcemeta::core

#endif
