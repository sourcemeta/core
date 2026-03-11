#ifndef SOURCEMETA_CORE_YAML_ROUNDTRIP_H_
#define SOURCEMETA_CORE_YAML_ROUNDTRIP_H_

#ifndef SOURCEMETA_CORE_YAML_EXPORT
#include <sourcemeta/core/yaml_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <cstdint>  // std::uint8_t, std::size_t
#include <map>      // std::map
#include <optional> // std::optional
#include <string>   // std::string
#include <vector>   // std::vector

namespace sourcemeta::core {

/// @ingroup yaml
enum class YAMLScalarStyle : std::uint8_t {
  Plain,
  SingleQuoted,
  DoubleQuoted,
  Literal,
  Folded
};

/// @ingroup yaml
enum class YAMLCollectionStyle : std::uint8_t { Block, Flow };

/// @ingroup yaml
enum class YAMLChomping : std::uint8_t { Clip, Strip, Keep };

/// @ingroup yaml
struct SOURCEMETA_CORE_YAML_EXPORT YAMLNodeStyle {
  std::optional<YAMLScalarStyle> scalar;
  std::optional<YAMLCollectionStyle> collection;
  std::optional<YAMLChomping> chomping;
  std::optional<std::string> block_content;
  std::optional<std::string> anchor;
  std::vector<std::string> comments_before;
  std::optional<std::string> comment_inline;
  std::optional<std::string> comment_on_indicator;
};

/// @ingroup yaml
/// Holds per-node metadata collected during YAML parsing to reproduce the
/// original formatting
class SOURCEMETA_CORE_YAML_EXPORT YAMLRoundTrip {
public:
  auto clear() -> void;
  std::map<Pointer, YAMLNodeStyle> styles;
  std::map<Pointer, std::string> aliases;
  std::map<Pointer, YAMLScalarStyle> key_styles;
  bool explicit_document_start{false};
  bool explicit_document_end{false};
  std::optional<std::string> document_start_comment;
  std::optional<std::string> document_end_comment;
  std::vector<std::string> leading_comments;
  std::vector<std::string> post_start_comments;
  std::vector<std::string> pre_end_comments;
  std::vector<std::string> trailing_comments;
  std::size_t indent_width{2};
};

} // namespace sourcemeta::core

#endif
