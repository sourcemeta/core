#ifndef SOURCEMETA_CORE_BUILD_ADAPTER_FILESYSTEM_JSON_H_
#define SOURCEMETA_CORE_BUILD_ADAPTER_FILESYSTEM_JSON_H_

#ifndef SOURCEMETA_CORE_BUILD_EXPORT
#include <sourcemeta/core/build_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/build_types.h>
// NOLINTEND(misc-include-cleaner)

#include <sourcemeta/core/json.h>

#include <filesystem> // std::filesystem
#include <optional>   // std::optional
#include <string>     // std::string

namespace sourcemeta::core {

/// @ingroup build
class SOURCEMETA_CORE_BUILD_EXPORT BuildAdapterFilesystemJSON {
public:
  using node_type = std::filesystem::path;
  using mark_type = std::filesystem::file_time_type;
  using dependencies_type = JSON::Array;
  using dependency_type = typename dependencies_type::value_type;

  BuildAdapterFilesystemJSON() = default;
  BuildAdapterFilesystemJSON(std::string dependency_extension);

  [[nodiscard]] auto dependencies_path(const node_type &path) const
      -> node_type;
  [[nodiscard]] auto read_dependencies(const node_type &path) const
      -> std::optional<dependencies_type>;
  auto
  write_dependencies(const node_type &path,
                     const BuildDependencies<node_type> &dependencies) const
      -> void;
  [[nodiscard]] auto dependency_to_node(const dependency_type &dep) const
      -> node_type;
  [[nodiscard]] auto mark(const node_type &path) const
      -> std::optional<mark_type>;
  [[nodiscard]] auto is_newer_than(const mark_type left,
                                   const mark_type right) const -> bool;

private:
// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif
  std::string extension{".deps"};
#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif
};

} // namespace sourcemeta::core

#endif // SOURCEMETA_CORE_OPTIONS_H_
