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
      enum_types.emplace(value.type());
    }

    if (enum_types.empty()) {
      return false;
    }

    this->blacklist.clear();
    for (const auto &entry : schema.as_object()) {
      const auto metadata = walker(entry.first, vocabularies);
      if (metadata.type == sourcemeta::core::SchemaKeywordType::Other ||
          metadata.type == sourcemeta::core::SchemaKeywordType::Reference) {
        continue;
      }

      // If keyword applies to any type, we can't determine type applicability
      if (metadata.instances.empty()) {
        continue;
      }

      // If none of the types that the keyword applies to match the enum types,
      // then this keyword is redundant and can be removed
      if (std::none_of(metadata.instances.cbegin(), metadata.instances.cend(),
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
