class UnnecessaryAllOfWrapperDraft final : public SchemaTransformRule {
public:
  UnnecessaryAllOfWrapperDraft()
      : SchemaTransformRule{"unnecessary_allof_wrapper_draft",
                            "Wrapping any keyword other than `$ref` in `allOf` "
                            "is unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!contains_any(vocabularies,
                      {"http://json-schema.org/draft-07/schema#",
                       "http://json-schema.org/draft-06/schema#",
                       "http://json-schema.org/draft-04/schema#"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    for (const auto &entry : schema.at("allOf").as_array()) {
      if (entry.is_object()) {
        for (const auto &subentry : entry.as_object()) {
          if (subentry.first != "$ref" && !schema.defines(subentry.first)) {
            return true;
          }
        }
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    for (auto &entry : schema.at("allOf").as_array()) {
      if (entry.is_object()) {
        std::vector<JSON::String> blacklist;
        for (auto &subentry : entry.as_object()) {
          if (subentry.first != "$ref" && !schema.defines(subentry.first)) {
            blacklist.push_back(subentry.first);
          }
        }

        for (auto &property : blacklist) {
          schema.try_assign_before(property, entry.at(property), "allOf");
          entry.erase(property);
        }
      }
    }

    // TODO: Do we really need this?
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

  // TODO: This is duplicated in `empty_allof_branches`
  auto is_empty(const JSON &schema) const -> bool {
    return (schema.is_boolean() && schema.to_boolean()) ||
           (schema.is_object() && schema.empty());
  }
};
