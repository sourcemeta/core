#ifndef SOURCEMETA_CORE_JSON_PACKED_H_
#define SOURCEMETA_CORE_JSON_PACKED_H_

#ifndef SOURCEMETA_CORE_JSON_EXPORT
#include <sourcemeta/core/json_export.h>
#endif

#include <sourcemeta/core/json_value.h>

#include <memory>      // std::unique_ptr
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup json
/// An experimental read-only JSON document backed by the parser's token tape.
/// It owns its input and reuses storage across parses. For example:
///
/// ```cpp
/// #include <sourcemeta/core/json.h>
/// #include <cassert>
/// sourcemeta::core::JSONPacked document;
/// document.parse(R"({"version": 1})");
/// assert(document.contains_root_key("version"));
/// ```
class SOURCEMETA_CORE_JSON_EXPORT JSONPacked {
public:
  JSONPacked();
  ~JSONPacked();
  JSONPacked(const JSONPacked &) = delete;
  auto operator=(const JSONPacked &) -> JSONPacked & = delete;

  /// Replace the current document, taking ownership of the input string.
  /// Invalid JSON throws JSONParseError and leaves this object empty.
  /// Error positions may differ from parse_json().
  auto parse(std::string input) -> void;

  /// Discard the document while retaining storage for reuse.
  auto reset() noexcept -> void;

  /// Test for a decoded property name in a top-level object.
  /// Returns false for an empty document or a root of any other type.
  [[nodiscard]] auto contains_root_key(std::string_view key) const -> bool;

  /// Build an independent, mutable JSON value. Throws std::logic_error if
  /// this object contains no document.
  [[nodiscard]] auto materialize() const -> JSON;

private:
  struct Storage;
  std::unique_ptr<Storage> storage_;
};

} // namespace sourcemeta::core

#endif
