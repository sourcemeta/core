class UnknownKeywordsPrefix final : public SchemaTransformRule {
public:
  UnknownKeywordsPrefix()
      : SchemaTransformRule{
            "unknown_keywords_prefix",
            "Future versions of JSON Schema will refuse to evaluate unknown "
            "keywords that don't have an x- prefix"} {};

  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &, const SchemaFrame::Location &,
            const SchemaWalker &walker, const SchemaResolver &) const
      -> SchemaTransformRule::Result override {
    if (!is_schema(schema) || !schema.is_object()) {
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
    for (const auto &keyword : this->unknown_keywords) {
      message << "- " << keyword << "\n";
    }

    return message.str();
  }

  auto transform(JSON &schema) const -> void override {
    for (const auto &keyword : this->unknown_keywords) {
      if (schema.defines(keyword)) {
        std::string prefixed_name = "x-" + keyword;
        while (schema.defines(prefixed_name)) {
          prefixed_name.insert(0, "x-");
        }
        schema.rename(keyword, std::move(prefixed_name));
      }
    }
  }

private:
  mutable std::vector<JSON::String> unknown_keywords;
};
