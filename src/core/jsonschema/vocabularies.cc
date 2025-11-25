#include <sourcemeta/core/jsonschema_vocabularies.h>

#include <cassert>  // assert
#include <optional> // std::optional
#include <string>   // std::string
#include <utility>  // std::pair
#include <vector>   // std::vector

namespace {
// Map vocabulary URI to its corresponding enum flag
// Ordered from most recent/common to oldest for faster short-circuiting
// TODO: Use a static unordered map for quickly checking this instead of
// a chain of string comparisons
auto uri_to_known_vocabulary(std::string_view uri)
    -> std::optional<sourcemeta::core::Vocabularies::Known> {
  using sourcemeta::core::Vocabularies;

  // 2020-12 vocabularies (most recent/common)
  if (uri == "https://json-schema.org/draft/2020-12/vocab/core") {
    return Vocabularies::Known::JSON_Schema_2020_12_Core;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/applicator") {
    return Vocabularies::Known::JSON_Schema_2020_12_Applicator;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/unevaluated") {
    return Vocabularies::Known::JSON_Schema_2020_12_Unevaluated;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/validation") {
    return Vocabularies::Known::JSON_Schema_2020_12_Validation;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/meta-data") {
    return Vocabularies::Known::JSON_Schema_2020_12_Meta_Data;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/format-annotation") {
    return Vocabularies::Known::JSON_Schema_2020_12_Format_Annotation;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/format-assertion") {
    return Vocabularies::Known::JSON_Schema_2020_12_Format_Assertion;
  }
  if (uri == "https://json-schema.org/draft/2020-12/vocab/content") {
    return Vocabularies::Known::JSON_Schema_2020_12_Content;
  }

  // 2019-09 vocabularies
  if (uri == "https://json-schema.org/draft/2019-09/vocab/core") {
    return Vocabularies::Known::JSON_Schema_2019_09_Core;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/applicator") {
    return Vocabularies::Known::JSON_Schema_2019_09_Applicator;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/validation") {
    return Vocabularies::Known::JSON_Schema_2019_09_Validation;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/meta-data") {
    return Vocabularies::Known::JSON_Schema_2019_09_Meta_Data;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/format") {
    return Vocabularies::Known::JSON_Schema_2019_09_Format;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/content") {
    return Vocabularies::Known::JSON_Schema_2019_09_Content;
  }
  if (uri == "https://json-schema.org/draft/2019-09/vocab/hyper-schema") {
    return Vocabularies::Known::JSON_Schema_2019_09_Hyper_Schema;
  }

  // Pre-vocabulary dialects (least common, checked last)
  if (uri == "http://json-schema.org/draft-07/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_7;
  }
  if (uri == "http://json-schema.org/draft-07/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_7_Hyper;
  }
  if (uri == "http://json-schema.org/draft-06/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_6;
  }
  if (uri == "http://json-schema.org/draft-06/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_6_Hyper;
  }
  if (uri == "http://json-schema.org/draft-04/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_4;
  }
  if (uri == "http://json-schema.org/draft-04/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_4_Hyper;
  }
  if (uri == "http://json-schema.org/draft-03/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_3;
  }
  if (uri == "http://json-schema.org/draft-03/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_3_Hyper;
  }
  if (uri == "http://json-schema.org/draft-02/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_2;
  }
  if (uri == "http://json-schema.org/draft-02/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_2_Hyper;
  }
  if (uri == "http://json-schema.org/draft-01/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_1;
  }
  if (uri == "http://json-schema.org/draft-01/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_1_Hyper;
  }
  if (uri == "http://json-schema.org/draft-00/schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_0;
  }
  if (uri == "http://json-schema.org/draft-00/hyper-schema#") {
    return Vocabularies::Known::JSON_Schema_Draft_0_Hyper;
  }

  return std::nullopt;
}
} // anonymous namespace

sourcemeta::core::Vocabularies::Vocabularies(
    std::initializer_list<std::pair<JSON::String, bool>> init) {
  for (const auto &entry : init) {
    this->insert(entry.first, entry.second);
  }
}

sourcemeta::core::Vocabularies::Vocabularies(
    std::initializer_list<std::pair<Known, bool>> init) {
  for (const auto &entry : init) {
    this->insert(entry.first, entry.second);
  }
}

auto sourcemeta::core::Vocabularies::contains(
    const JSON::String &uri) const noexcept -> bool {
  const auto maybe_known = uri_to_known_vocabulary(uri);
  if (maybe_known.has_value()) {
    return this->contains(maybe_known.value());
  }
  const auto iterator = this->custom.find(uri);
  return iterator != this->custom.end();
}

auto sourcemeta::core::Vocabularies::contains(Known vocabulary) const noexcept
    -> bool {
  const auto index = static_cast<std::size_t>(vocabulary);
  // Use [] operator instead of test() to avoid exceptions in noexcept function
  return this->required_known[index] || this->optional_known[index];
}

auto sourcemeta::core::Vocabularies::insert(const JSON::String &uri,
                                            bool required) noexcept -> void {
  const auto maybe_known = uri_to_known_vocabulary(uri);
  if (maybe_known.has_value()) {
    this->insert(maybe_known.value(), required);
  } else {
    this->custom.insert({uri, required});
  }
}

auto sourcemeta::core::Vocabularies::insert(Known vocabulary,
                                            bool required) noexcept -> void {
  const auto index = static_cast<std::size_t>(vocabulary);
  if (required) {
    this->required_known[index] = true;
    this->optional_known[index] = false;
  } else {
    this->optional_known[index] = true;
    this->required_known[index] = false;
  }
  // Verify invariant: vocabulary cannot be both required and optional
  assert((this->required_known & this->optional_known).none());
}

auto sourcemeta::core::Vocabularies::get(const JSON::String &uri) const noexcept
    -> std::optional<bool> {
  const auto maybe_known = uri_to_known_vocabulary(uri);
  if (maybe_known.has_value()) {
    return this->get(maybe_known.value());
  }
  const auto iterator = this->custom.find(uri);
  if (iterator != this->custom.end()) {
    return iterator->second;
  }
  return std::nullopt;
}

auto sourcemeta::core::Vocabularies::get(Known vocabulary) const noexcept
    -> std::optional<bool> {
  const auto index = static_cast<std::size_t>(vocabulary);
  // Use [] operator instead of test() to avoid exceptions in noexcept function
  assert(!this->required_known[index] || !this->optional_known[index]);
  if (this->required_known[index]) {
    return true;
  }
  if (this->optional_known[index]) {
    return false;
  }
  return std::nullopt;
}

auto sourcemeta::core::Vocabularies::size() const noexcept -> std::size_t {
  return (this->required_known | this->optional_known).count() +
         this->custom.size();
}

auto sourcemeta::core::Vocabularies::empty() const noexcept -> bool {
  return this->required_known.none() && this->optional_known.none() &&
         this->custom.empty();
}
