#include <sourcemeta/core/jsonschema_vocabularies.h>

#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::pair
#include <vector>   // std::vector

namespace {
// Map vocabulary URI to its corresponding enum flag
auto uri_to_known_vocabulary(std::string_view uri)
    -> std::optional<sourcemeta::core::KnownVocabulary> {
  using sourcemeta::core::KnownVocabulary;

  // Pre-vocabulary dialects
  if (uri == "http://json-schema.org/draft-00/schema#")
    return KnownVocabulary::Draft00;
  if (uri == "http://json-schema.org/draft-00/hyper-schema#")
    return KnownVocabulary::Draft00Hyper;
  if (uri == "http://json-schema.org/draft-01/schema#")
    return KnownVocabulary::Draft01;
  if (uri == "http://json-schema.org/draft-01/hyper-schema#")
    return KnownVocabulary::Draft01Hyper;
  if (uri == "http://json-schema.org/draft-02/schema#")
    return KnownVocabulary::Draft02;
  if (uri == "http://json-schema.org/draft-02/hyper-schema#")
    return KnownVocabulary::Draft02Hyper;
  if (uri == "http://json-schema.org/draft-03/schema#")
    return KnownVocabulary::Draft03;
  if (uri == "http://json-schema.org/draft-03/hyper-schema#")
    return KnownVocabulary::Draft03Hyper;
  if (uri == "http://json-schema.org/draft-04/schema#")
    return KnownVocabulary::Draft04;
  if (uri == "http://json-schema.org/draft-04/hyper-schema#")
    return KnownVocabulary::Draft04Hyper;
  if (uri == "http://json-schema.org/draft-06/schema#")
    return KnownVocabulary::Draft06;
  if (uri == "http://json-schema.org/draft-06/hyper-schema#")
    return KnownVocabulary::Draft06Hyper;
  if (uri == "http://json-schema.org/draft-07/schema#")
    return KnownVocabulary::Draft07;
  if (uri == "http://json-schema.org/draft-07/hyper-schema#")
    return KnownVocabulary::Draft07Hyper;

  // 2019-09 vocabularies
  if (uri == "https://json-schema.org/draft/2019-09/vocab/core")
    return KnownVocabulary::Draft201909Core;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/applicator")
    return KnownVocabulary::Draft201909Applicator;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/validation")
    return KnownVocabulary::Draft201909Validation;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/meta-data")
    return KnownVocabulary::Draft201909MetaData;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/format")
    return KnownVocabulary::Draft201909Format;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/content")
    return KnownVocabulary::Draft201909Content;
  if (uri == "https://json-schema.org/draft/2019-09/vocab/hyper-schema")
    return KnownVocabulary::Draft201909HyperSchema;

  // 2020-12 vocabularies
  if (uri == "https://json-schema.org/draft/2020-12/vocab/core")
    return KnownVocabulary::Draft202012Core;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/applicator")
    return KnownVocabulary::Draft202012Applicator;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/unevaluated")
    return KnownVocabulary::Draft202012Unevaluated;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/validation")
    return KnownVocabulary::Draft202012Validation;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/meta-data")
    return KnownVocabulary::Draft202012MetaData;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/format-annotation")
    return KnownVocabulary::Draft202012FormatAnnotation;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/format-assertion")
    return KnownVocabulary::Draft202012FormatAssertion;
  if (uri == "https://json-schema.org/draft/2020-12/vocab/content")
    return KnownVocabulary::Draft202012Content;

  return std::nullopt;
}
} // anonymous namespace

// Vocabularies struct implementations
auto sourcemeta::core::Vocabularies::contains(std::string_view uri) const
    -> bool {
  const auto maybe_known = uri_to_known_vocabulary(uri);
  if (maybe_known.has_value()) {
    const auto flag = static_cast<uint32_t>(maybe_known.value());
    // Presence is recorded when either enabled or disabled
    return ((enabled_known | disabled_known) & flag) != 0;
  }
  // Fallback to custom vocabularies
  const auto it = custom.find(std::string{uri});
  return it != custom.end() && it->second;
}

auto sourcemeta::core::Vocabularies::insert(
    const std::pair<JSON::String, bool> &entry) -> void {
  const auto maybe_known = uri_to_known_vocabulary(entry.first);
  if (maybe_known.has_value()) {
    const auto flag = static_cast<uint32_t>(maybe_known.value());
    if (entry.second) {
      enabled_known |= flag;
      disabled_known &= ~flag; // Clear from disabled
    } else {
      disabled_known |= flag;
      enabled_known &= ~flag; // Clear from enabled
    }
  } else {
    custom.insert(entry);
  }
}

auto sourcemeta::core::Vocabularies::find(std::string_view uri) const
    -> std::optional<bool> {
  const auto maybe_known = uri_to_known_vocabulary(uri);
  if (maybe_known.has_value()) {
    const auto flag = static_cast<uint32_t>(maybe_known.value());
    if ((enabled_known & flag) != 0)
      return true;
    if ((disabled_known & flag) != 0)
      return false;
    return std::nullopt;
  }
  // Fallback to custom vocabularies
  const auto it = custom.find(std::string{uri});
  if (it != custom.end()) {
    return it->second;
  }
  return std::nullopt;
}

auto sourcemeta::core::Vocabularies::merge(const Vocabularies &other) -> void {
  // Merge bitflags - only add entries that don't exist in this
  // Match unordered_map::merge semantics: insert only if key doesn't exist
  const uint32_t already_set = enabled_known | disabled_known;

  // Add enabled vocabularies from other that aren't already set
  const uint32_t to_enable = other.enabled_known & ~already_set;
  enabled_known |= to_enable;

  // Add disabled vocabularies from other that aren't already set
  const uint32_t to_disable = other.disabled_known & ~already_set;
  disabled_known |= to_disable;

  // Merge custom vocabularies
  for (const auto &entry : other.custom) {
    custom.insert(entry);
  }
}

auto sourcemeta::core::Vocabularies::all_vocabularies() const
    -> std::vector<std::pair<std::string, bool>> {
  std::vector<std::pair<std::string, bool>> result;
  result.reserve(32); // Reserve for typical case

  // Helper to check and add known vocabulary
  auto check_and_add = [&](KnownVocabulary vocab, std::string_view uri) {
    const auto flag = static_cast<uint32_t>(vocab);
    if ((enabled_known & flag) != 0) {
      result.emplace_back(std::string{uri}, true);
    } else if ((disabled_known & flag) != 0) {
      result.emplace_back(std::string{uri}, false);
    }
  };

  // Check all known vocabularies
  // Pre-vocabulary dialects
  check_and_add(KnownVocabulary::Draft00,
                "http://json-schema.org/draft-00/schema#");
  check_and_add(KnownVocabulary::Draft00Hyper,
                "http://json-schema.org/draft-00/hyper-schema#");
  check_and_add(KnownVocabulary::Draft01,
                "http://json-schema.org/draft-01/schema#");
  check_and_add(KnownVocabulary::Draft01Hyper,
                "http://json-schema.org/draft-01/hyper-schema#");
  check_and_add(KnownVocabulary::Draft02,
                "http://json-schema.org/draft-02/schema#");
  check_and_add(KnownVocabulary::Draft02Hyper,
                "http://json-schema.org/draft-02/hyper-schema#");
  check_and_add(KnownVocabulary::Draft03,
                "http://json-schema.org/draft-03/schema#");
  check_and_add(KnownVocabulary::Draft03Hyper,
                "http://json-schema.org/draft-03/hyper-schema#");
  check_and_add(KnownVocabulary::Draft04,
                "http://json-schema.org/draft-04/schema#");
  check_and_add(KnownVocabulary::Draft04Hyper,
                "http://json-schema.org/draft-04/hyper-schema#");
  check_and_add(KnownVocabulary::Draft06,
                "http://json-schema.org/draft-06/schema#");
  check_and_add(KnownVocabulary::Draft06Hyper,
                "http://json-schema.org/draft-06/hyper-schema#");
  check_and_add(KnownVocabulary::Draft07,
                "http://json-schema.org/draft-07/schema#");
  check_and_add(KnownVocabulary::Draft07Hyper,
                "http://json-schema.org/draft-07/hyper-schema#");

  // 2019-09 vocabularies
  check_and_add(KnownVocabulary::Draft201909Core,
                "https://json-schema.org/draft/2019-09/vocab/core");
  check_and_add(KnownVocabulary::Draft201909Applicator,
                "https://json-schema.org/draft/2019-09/vocab/applicator");
  check_and_add(KnownVocabulary::Draft201909Validation,
                "https://json-schema.org/draft/2019-09/vocab/validation");
  check_and_add(KnownVocabulary::Draft201909MetaData,
                "https://json-schema.org/draft/2019-09/vocab/meta-data");
  check_and_add(KnownVocabulary::Draft201909Format,
                "https://json-schema.org/draft/2019-09/vocab/format");
  check_and_add(KnownVocabulary::Draft201909Content,
                "https://json-schema.org/draft/2019-09/vocab/content");
  check_and_add(KnownVocabulary::Draft201909HyperSchema,
                "https://json-schema.org/draft/2019-09/vocab/hyper-schema");

  // 2020-12 vocabularies
  check_and_add(KnownVocabulary::Draft202012Core,
                "https://json-schema.org/draft/2020-12/vocab/core");
  check_and_add(KnownVocabulary::Draft202012Applicator,
                "https://json-schema.org/draft/2020-12/vocab/applicator");
  check_and_add(KnownVocabulary::Draft202012Unevaluated,
                "https://json-schema.org/draft/2020-12/vocab/unevaluated");
  check_and_add(KnownVocabulary::Draft202012Validation,
                "https://json-schema.org/draft/2020-12/vocab/validation");
  check_and_add(KnownVocabulary::Draft202012MetaData,
                "https://json-schema.org/draft/2020-12/vocab/meta-data");
  check_and_add(
      KnownVocabulary::Draft202012FormatAnnotation,
      "https://json-schema.org/draft/2020-12/vocab/format-annotation");
  check_and_add(KnownVocabulary::Draft202012FormatAssertion,
                "https://json-schema.org/draft/2020-12/vocab/format-assertion");
  check_and_add(KnownVocabulary::Draft202012Content,
                "https://json-schema.org/draft/2020-12/vocab/content");

  // Add custom vocabularies
  result.reserve(result.size() + custom.size());
  for (const auto &entry : custom) {
    result.emplace_back(entry);
  }

  return result;
}
