class UnknownKeywordsPrefix final : public SchemaTransformRule {
public:
  UnknownKeywordsPrefix()
      : SchemaTransformRule{"unknown_keywords_prefix",
                            "Prefix unknown keywords with \"x-\""} {};

  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &, const SchemaFrame::Location &,
            const SchemaWalker &walker, const SchemaResolver &) const
      -> SchemaTransformRule::Result override {
    if (!is_schema(schema) || !schema.is_object()) {
      return false;
    }

    // Only apply to JSON Schema dialects
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/validation",
                       "https://json-schema.org/draft/2019-09/vocab/validation",
                       "http://json-schema.org/draft-07/schema#",
                       "http://json-schema.org/draft-06/schema#",
                       "http://json-schema.org/draft-04/schema#",
                       "http://json-schema.org/draft-03/schema#",
                       "http://json-schema.org/draft-02/schema#",
                       "http://json-schema.org/draft-01/schema#",
                       "http://json-schema.org/draft-00/schema#"})) {
      return false;
    }

    this->unknown_keywords.clear();
    for (const auto &entry : schema.as_object()) {
      const auto &keyword = entry.first;

      // Skip keywords that are already properly prefixed with x-
      if (keyword.starts_with("x-") || keyword.starts_with("X-")) {
        continue;
      }

      const auto metadata{walker(keyword, vocabularies)};

      // If the walker identifies this as an unknown keyword (type Unknown),
      // and it's not already prefixed, it needs prefixing
      if (metadata.type == SchemaKeywordType::Unknown) {
        this->unknown_keywords.emplace_back(keyword);
      }
    }
    // add tests for multiple same name keywords
    if (this->unknown_keywords.empty()) {
      return false;
    }

    std::ostringstream message;
    message << "Prefix unknown keyword(s) with \"x-\":\n";
    for (const auto &keyword : this->unknown_keywords) {
      message << "- " << keyword << "\n";
    }

    return message.str();
  }

  auto transform(JSON &schema) const -> void override {
    for (const auto &keyword : this->unknown_keywords) {
      if (schema.defines(keyword)) {
        auto value = schema.at(keyword);
        schema.erase(keyword);
        schema.assign("x-" + keyword, std::move(value)); // rename
      }
    }
  }

private:
  mutable std::vector<JSON::String> unknown_keywords;
};
