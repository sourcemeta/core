class ElseFalse final : public SchemaTransformRule {
public:
  ElseFalse()
      : SchemaTransformRule{"else_false",
                            "`if: S, else: false` collapses to just `S`"} {};

  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &, const SchemaFrame::Location &,
            const SchemaWalker &, const SchemaResolver &) const
      -> SchemaTransformRule::Result override {
    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/applicator",
                "https://json-schema.org/draft/2019-09/vocab/applicator",
                "http://json-schema.org/draft-07/schema#"}) &&
           schema.is_object() && schema.defines("if") &&
           schema.defines("else") && schema.at("else").is_boolean() &&
           !schema.at("else").to_boolean();
  }

  auto transform(JSON &schema) const -> void override {
    const auto if_schema = schema.at("if");
    schema.erase("if");
    schema.erase("else");
    if (if_schema.is_object()) {
      for (const auto &entry : if_schema.as_object()) {
        if (!schema.defines(entry.first)) {
          schema.assign(entry.first, entry.second);
        }
      }
    } else if (if_schema.is_boolean()) {
      if (if_schema.to_boolean()) {
      } else {
      }
    }
  }
};
