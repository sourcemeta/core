#ifndef SOURCEMETA_CORE_JSONSCHEMA_VOCABULARIES_H_
#define SOURCEMETA_CORE_JSONSCHEMA_VOCABULARIES_H_

#include <sourcemeta/core/json.h>
#ifndef SOURCEMETA_CORE_JSONSCHEMA_EXPORT
#include <sourcemeta/core/jsonschema_export.h>
#endif

#include <cstdint>       // std::uint32_t, std::size_t
#include <optional>      // std::optional
#include <stdexcept>     // std::out_of_range
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::pair
#include <vector>        // std::vector

namespace sourcemeta::core {

/// @ingroup jsonschema
/// Vocabulary enumeration for known JSON Schema vocabularies.
/// Each vocabulary is represented as a bitflag for efficient storage and
/// lookup.
enum class KnownVocabulary : uint32_t {
  // Pre-vocabulary dialects (treated as vocabularies)
  Draft00 = 1u << 0,
  Draft00Hyper = 1u << 1,
  Draft01 = 1u << 2,
  Draft01Hyper = 1u << 3,
  Draft02 = 1u << 4,
  Draft02Hyper = 1u << 5,
  Draft03 = 1u << 6,
  Draft03Hyper = 1u << 7,
  Draft04 = 1u << 8,
  Draft04Hyper = 1u << 9,
  Draft06 = 1u << 10,
  Draft06Hyper = 1u << 11,
  Draft07 = 1u << 12,
  Draft07Hyper = 1u << 13,
  // 2019-09 vocabularies
  Draft201909Core = 1u << 14,
  Draft201909Applicator = 1u << 15,
  Draft201909Validation = 1u << 16,
  Draft201909MetaData = 1u << 17,
  Draft201909Format = 1u << 18,
  Draft201909Content = 1u << 19,
  Draft201909HyperSchema = 1u << 20,
  // 2020-12 vocabularies
  Draft202012Core = 1u << 21,
  Draft202012Applicator = 1u << 22,
  Draft202012Unevaluated = 1u << 23,
  Draft202012Validation = 1u << 24,
  Draft202012MetaData = 1u << 25,
  Draft202012FormatAnnotation = 1u << 26,
  Draft202012FormatAssertion = 1u << 27,
  Draft202012Content = 1u << 28
};

/// @ingroup jsonschema
/// Optimized vocabulary set using bitflags for known vocabularies
/// and a fallback unordered_map for custom vocabularies.
///
/// This struct provides significant performance improvements over a plain
/// unordered_map by using bitwise operations for the 28 known JSON Schema
/// vocabularies. Lookups for known vocabularies are O(1) with minimal overhead,
/// while unknown/custom vocabularies fall back to the hash map.
///
/// Memory footprint:
/// - Known vocabularies: 8 bytes (2x uint32_t bitflags)
/// - Custom vocabularies: ~100+ bytes (unordered_map overhead)
///
/// Performance characteristics:
/// - Known vocabulary lookup: O(1) bitwise AND operation
/// - Custom vocabulary lookup: O(1) hash lookup (fallback)
/// - Insert: O(1) for both known and custom vocabularies
/// - Merge: O(n) where n is the number of custom vocabularies
struct SOURCEMETA_CORE_JSONSCHEMA_EXPORT Vocabularies {
  using key_type = JSON::String;
  /// Bitflags for enabled known vocabularies
  uint32_t enabled_known = 0;

  /// Bitflags for disabled known vocabularies (for explicit false values)
  uint32_t disabled_known = 0;

  /// Fallback storage for custom/unknown vocabularies
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
  std::unordered_map<JSON::String, bool> custom;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

  /// Default constructor
  Vocabularies() = default;

  /// Construct from initializer list (for backward compatibility)
  Vocabularies(std::initializer_list<std::pair<JSON::String, bool>> init) {
    for (const auto &entry : init) {
      insert(entry);
    }
  }

  /// Check if a vocabulary is enabled
  /// @param uri The vocabulary URI to check
  /// @return true if the vocabulary is enabled, false otherwise
  [[nodiscard]] auto contains(std::string_view uri) const -> bool;

  /// Insert a vocabulary with its enabled/disabled status
  /// @param entry A pair of vocabulary URI and its boolean status
  auto insert(const std::pair<JSON::String, bool> &entry) -> void;

  /// Get vocabulary status by URI
  /// @param uri The vocabulary URI to look up
  /// @return std::optional<bool> containing the status if found, std::nullopt
  /// otherwise
  [[nodiscard]] auto find(std::string_view uri) const -> std::optional<bool>;

  /// Merge another Vocabularies into this one.
  /// Only adds vocabularies that don't already exist in this instance.
  /// Matches std::unordered_map::merge semantics.
  /// @param other The Vocabularies instance to merge from
  auto merge(const Vocabularies &other) -> void;

  /// Get all active (enabled or disabled) vocabularies as key-value pairs
  /// @return A vector of pairs containing vocabulary URIs and their status
  [[nodiscard]] auto all_vocabularies() const
      -> std::vector<std::pair<std::string, bool>>;

  /// Get the number of vocabularies (enabled + disabled + custom)
  /// @return The total count of vocabularies
  [[nodiscard]] auto size() const -> std::size_t {
    // Count set bits in both enabled and disabled
    std::size_t count = 0;
    uint32_t combined = enabled_known | disabled_known;
    while (combined) {
      count += combined & 1;
      combined >>= 1;
    }
    return count + custom.size();
  }

  /// Access vocabulary status by URI (throws if not found)
  /// @param uri The vocabulary URI to look up
  /// @return The boolean status of the vocabulary
  /// @throws std::out_of_range if vocabulary not found
  [[nodiscard]] auto at(std::string_view uri) const -> bool {
    const auto status = find(uri);
    if (!status.has_value()) {
      throw std::out_of_range("Vocabulary not found: " + std::string(uri));
    }
    return status.value();
  }

  /// Check if there are no vocabularies
  /// @return true if no vocabularies are set
  [[nodiscard]] auto empty() const -> bool {
    return enabled_known == 0 && disabled_known == 0 && custom.empty();
  }
};

} // namespace sourcemeta::core

#endif
