class EmptyAllOfBranches final : public SchemaTransformRule {
public:
  EmptyAllOfBranches()
      : SchemaTransformRule{"empty_allof_branches",
                            "Avoid empty branches `allOf` that do not add any "
                            "further constraints"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (contains_any(vocabularies,
                     {"https://json-schema.org/draft/2020-12/vocab/applicator",
                      "https://json-schema.org/draft/2019-09/vocab/applicator",
                      "http://json-schema.org/draft-07/schema#",
                      "http://json-schema.org/draft-06/schema#",
                      "http://json-schema.org/draft-04/schema#"}) &&
        schema.is_object() && schema.defines("allOf") &&
        schema.at("allOf").is_array() &&
        std::any_of(
            schema.at("allOf").as_array().cbegin(),
            schema.at("allOf").as_array().cend(),
            [this](const auto &branch) { return this->is_empty(branch); })) {
      return true;
    } else {
      return false;
    }
  }

  auto transform(JSON &schema) const -> void override {
    auto &all_of{schema.at("allOf")};
    for (auto iterator = all_of.as_array().begin();
         iterator != all_of.as_array().end();) {
      if (this->is_empty(*iterator)) {
        iterator = all_of.erase(iterator);
      } else {
        ++iterator;
      }
    }

    if (all_of.empty()) {
      schema.erase("allOf");
    }
  }

  auto is_empty(const JSON &schema) const -> bool {
    return (schema.is_boolean() && schema.to_boolean()) ||
           (schema.is_object() && schema.empty());
  }
};
