class DraftRefSiblings final : public SchemaTransformRule {
public:
  DraftRefSiblings()
      : SchemaTransformRule{"draft_ref_siblings",
                            "In Draft 7 and older dialects, keywords sibling "
                            "to $ref are never evaluated"} {}

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
                      {"http://json-schema.org/draft-07/schema#",
                       "http://json-schema.org/draft-06/schema#",
                       "http://json-schema.org/draft-04/schema#",
                       "http://json-schema.org/draft-03/schema#",
                       "http://json-schema.org/draft-02/schema#",
                       "http://json-schema.org/draft-01/schema#",
                       "http://json-schema.org/draft-00/schema#"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("$ref")) {
      return false;
    }

    // Determine which keywords to preserve
    std::set<std::string> preserve_keywords{"$ref"};

    // If at top-level (has $schema), also preserve schema identifiers
    if (schema.defines("$schema")) {
      preserve_keywords.insert("$schema");

      // Use walker to check if 'id' is a valid keyword for this dialect
      if (schema.defines("id")) {
        const auto id_metadata = walker("id", vocabularies);
        // If the walker recognizes 'id' as a valid keyword, preserve it
        if (id_metadata.vocabulary.has_value()) {
          preserve_keywords.insert("id");
        }
      }

      // Use walker to check if '$id' is a valid keyword for this dialect
      if (schema.defines("$id")) {
        const auto id_metadata = walker("$id", vocabularies);
        // If the walker recognizes '$id' as a valid keyword, preserve it
        if (id_metadata.vocabulary.has_value()) {
          preserve_keywords.insert("$id");
        }
      }
    }

    // Check if we have any siblings that should be removed
    for (const auto &entry : schema.as_object()) {
      const auto &key = entry.first;
      if (preserve_keywords.find(key) == preserve_keywords.end()) {
        return true;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    std::set<std::string> preserve_keywords{"$ref"};

    // If at top-level (has $schema), also preserve schema identifiers
    if (schema.defines("$schema")) {
      preserve_keywords.insert("$schema");

      // Preserve the appropriate identifier based on what's present
      if (schema.defines("id")) {
        preserve_keywords.insert("id");
      }
      if (schema.defines("$id")) {
        preserve_keywords.insert("$id");
      }
    }

    schema.clear_except(preserve_keywords.cbegin(), preserve_keywords.cend());
  }
};
