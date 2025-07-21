class DraftRefSiblings final : public SchemaTransformRule {
public:
  DraftRefSiblings()
      : SchemaTransformRule{
            "draft_ref_siblings",
            "The official dialect URI of Draft 7 and older must not contain "
            "other keywords as only $ref is evaluated"} {}

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

    for (const auto &entry : schema.as_object()) {
      const auto &key = entry.first;

      if (key == "$ref" || key == "$schema" || key == "title" ||
          key == "description" || key == "examples" || key == "$comment" ||
          key == "default" || key == "properties") {
        continue;
      }

      return true;
    }

    return false;
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.clear_except({"$ref", "$schema", "title", "description", "examples",
                         "$comment", "default", "properties"});
  }
};
