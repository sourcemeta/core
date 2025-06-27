class ThenElseEmpty final : public SchemaTransformRule {
public:
  ThenElseEmpty()
      : SchemaTransformRule{"then_else_empty",
                            "then or else is an empty schema ({} or true) "
                            "and therefore adds no restriction"} {}

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {

    const auto empty_object = [](const sourcemeta::core::JSON &s) {
      return s.is_object() && s.empty();
    };

    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/applicator",
                "https://json-schema.org/draft/2019-09/vocab/applicator",
                "http://json-schema.org/draft-07/schema#"}) &&
           schema.is_object() && schema.defines("if") &&
           ((schema.defines("then") && empty_object(schema.at("then"))) ||
            (schema.defines("else") && empty_object(schema.at("else"))));
  }

  auto transform(JSON &schema) const -> void override {
    const auto empty_object = [](const JSON &s) {
      return s.is_object() && s.empty();
    };

    if (schema.defines("then") && empty_object(schema.at("then"))) {
      schema.erase("then");
    }
    if (schema.defines("else") && empty_object(schema.at("else"))) {
      schema.erase("else");
    }
  }
};