#ifndef SOURCEMETA_CORE_JSONPATH_H_
#define SOURCEMETA_CORE_JSONPATH_H_

#ifndef SOURCEMETA_CORE_JSONPATH_EXPORT
#include <sourcemeta/core/jsonpath_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/jsonpath_error.h>
// NOLINTEND(misc-include-cleaner)

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <functional> // std::function
#include <memory>     // std::unique_ptr
#include <vector>     // std::vector

/// @defgroup jsonpath JSONPath
/// @brief A strict RFC 9535 JSONPath implementation.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/jsonpath.h>
/// ```

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

// The opaque compiled representation of a query
struct JSONPathQuery;

/// @ingroup jsonpath
/// A parsed RFC 9535 JSONPath query that can be evaluated many times.
class SOURCEMETA_CORE_JSONPATH_EXPORT JSONPath {
public:
  /// A single query result, pairing a value with its location
  struct Node {
    /// The matched value within the queried document
    const JSON *value;
    /// The location of the matched value within the queried document
    WeakPointer location;
  };

  /// The callback invoked for every query result node
  using Callback =
      std::function<void(const JSON &value, const WeakPointer &location)>;

  /// Parse a query expression, throwing on invalid input. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpath.h>
  ///
  /// const sourcemeta::core::JSONPath path{"$.store.book[0].title"};
  /// ```
  explicit JSONPath(const JSON::StringView expression);

  /// Evaluate the query against a document, invoking the callback once per
  /// result node. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpath.h>
  /// #include <cassert>
  ///
  /// const sourcemeta::core::JSON document{
  ///   sourcemeta::core::parse_json("{ \"foo\": [ 1, 2 ] }")};
  /// const sourcemeta::core::JSONPath path{"$.foo[0]"};
  /// path.evaluate(document, [](const auto &value, const auto &location) {
  ///   assert(value.is_integer());
  ///   assert(location.size() == 2);
  /// });
  /// ```
  auto evaluate(const JSON &document, const Callback &callback) const -> void;

  /// Evaluate the query against a document, materializing the result
  /// nodelist. For example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpath.h>
  /// #include <cassert>
  ///
  /// const sourcemeta::core::JSON document{
  ///   sourcemeta::core::parse_json("{ \"foo\": [ 1, 2 ] }")};
  /// const sourcemeta::core::JSONPath path{"$.foo[0]"};
  /// const auto nodes{path.evaluate(document)};
  /// assert(nodes.size() == 1);
  /// assert(nodes.front().value->is_integer());
  /// ```
  [[nodiscard]] auto evaluate(const JSON &document) const -> std::vector<Node>;

  /// Serialize a location into the RFC 9535 normalized path form. For
  /// example:
  ///
  /// ```cpp
  /// #include <sourcemeta/core/jsonpath.h>
  /// #include <cassert>
  ///
  /// const sourcemeta::core::JSON document{
  ///   sourcemeta::core::parse_json("{ \"foo\": [ 1, 2 ] }")};
  /// const sourcemeta::core::JSONPath path{"$.foo[0]"};
  /// const auto nodes{path.evaluate(document)};
  /// assert(sourcemeta::core::JSONPath::normalize(
  ///   nodes.front().location) == "$['foo'][0]");
  /// ```
  [[nodiscard]] static auto normalize(const WeakPointer &location)
      -> JSON::String;

  /// Destruct a query
  ~JSONPath();

  /// Move construct a query
  JSONPath(JSONPath &&other) noexcept;

  /// Move assign a query
  auto operator=(JSONPath &&other) noexcept -> JSONPath &;

  // To avoid mistakes
  JSONPath(const JSONPath &) = delete;
  auto operator=(const JSONPath &) -> JSONPath & = delete;

private:
  std::unique_ptr<JSONPathQuery> internal_;
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

} // namespace sourcemeta::core

#endif
