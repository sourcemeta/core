class EnumValidationKeywordsDefault final : public SchemaTransformRule {
public:
  EnumValidationKeywordsDefault()
      : SchemaTransformRule{
            "enum_validation_keywords_default",
            "Setting validation keywords alongside `enum` is considered an "
            "anti-pattern, as the enumeration choices already imply their "
            "respective constraints"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &walker,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/validation",
                       "https://json-schema.org/draft/2019-09/vocab/validation",
                       "http://json-schema.org/draft-07/schema#",
                       "http://json-schema.org/draft-06/schema#",
                       "http://json-schema.org/draft-04/schema#",
                       "http://json-schema.org/draft-03/schema#",
                       "http://json-schema.org/draft-02/schema#",
                       "http://json-schema.org/draft-02/hyper-schema#",
                       "http://json-schema.org/draft-01/schema#",
                       "http://json-schema.org/draft-01/hyper-schema#",
                       "http://json-schema.org/draft-00/schema#",
                       "http://json-schema.org/draft-00/hyper-schema#"}) ||
        !schema.is_object() || !schema.defines("enum") ||
        !schema.at("enum").is_array() || schema.defines("$ref")) {
      return false;
    }

    std::set<sourcemeta::core::JSON::Type> enum_types;
    for (const auto &value : schema.at("enum").as_array()) {
      if (value.is_null()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Null);
      } else if (value.is_boolean()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Boolean);
      } else if (value.is_integer()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Integer);
      } else if (value.is_real()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Real);
      } else if (value.is_string()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::String);
      } else if (value.is_array()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Array);
      } else if (value.is_object()) {
        enum_types.emplace(sourcemeta::core::JSON::Type::Object);
      }
    }

    if (enum_types.empty()) {
      return false;
    }

    // Build blacklist of keywords that don't apply to any enum value types
    this->blacklist.clear();
    for (const auto &entry : schema.as_object()) {
      if (entry.first == "enum" || entry.first == "$schema" ||
          entry.first == "$id" || entry.first == "$ref") {
        continue;
      }

      const auto metadata = walker(entry.first, vocabularies);

      // If keyword applies to any type, we can't determine type applicability
      if (metadata.instances.empty()) {
        continue;
      }

      // If none of the types that the keyword applies to match the enum types,
      // then this keyword is redundant and can be removed
      if (std::ranges::none_of(metadata.instances,
                               [&enum_types](const auto keyword_type) {
                                 return enum_types.contains(keyword_type);
                               })) {
        this->blacklist.emplace_back(entry.first);
      }
    }

    return !this->blacklist.empty();
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase_keys(this->blacklist.cbegin(), this->blacklist.cend());
  }

private:
  mutable std::vector<sourcemeta::core::JSON::String> blacklist;
};
