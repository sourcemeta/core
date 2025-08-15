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
    const auto &object = schema.as_object();
    for (const auto &entry : object) {
      const auto &keyword = entry.first;

      if (keyword.starts_with("x-")) {
        continue;
      }

      const auto metadata = walker(keyword, vocabularies);
      if (metadata.type == SchemaKeywordType::Unknown) {
        this->unknown_keywords.emplace_back(keyword);
      }
    }
    if (this->unknown_keywords.empty()) {
      return false;
    }

    std::ostringstream message;
    message << "Prefix unknown keyword(s) with \"x-\":\n";
    std::ranges::for_each(this->unknown_keywords,
                          [&message](const auto &keyword) {
                            message << "- " << keyword << "\n";
                          });

    return message.str();
  }

  auto transform(JSON &schema) const -> void override {
    std::ranges::for_each(this->unknown_keywords,
                          [&schema](const auto &keyword) {
                            if (schema.defines(keyword)) {
                              auto value = schema.at(keyword);
                              schema.erase(keyword);

                              std::string prefixed_name = "x-" + keyword;
                              while (schema.defines(prefixed_name)) {
                                prefixed_name.insert(0, "x-");
                              }

                              schema.assign(prefixed_name, std::move(value));
                            }
                          });
  }

private:
  mutable std::vector<JSON::String> unknown_keywords;
};
