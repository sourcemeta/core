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
    ONLY_CONTINUE_IF(schema.is_object());
    this->unknown_keywords.clear();
    std::vector<Pointer> locations;
    for (const auto &entry : schema.as_object()) {
      if (entry.first.starts_with("x-")) {
        continue;
      }

      const auto metadata = walker(entry.first, vocabularies);
      if (metadata.type == SchemaKeywordType::Unknown) {
        this->unknown_keywords.emplace_back(entry.first);
        locations.push_back(Pointer{entry.first});
      }
    }

    ONLY_CONTINUE_IF(!locations.empty());
    return APPLIES_TO_POINTERS(std::move(locations));
  }

  auto transform(JSON &schema) const -> void override {
    for (const auto &keyword : this->unknown_keywords) {
      assert(schema.defines(keyword));
      std::string prefixed_name = "x-" + keyword;
      while (schema.defines(prefixed_name)) {
        prefixed_name.insert(0, "x-");
      }
      schema.rename(keyword, std::move(prefixed_name));
    }
  }

private:
  mutable std::vector<JSON::String> unknown_keywords;
};
