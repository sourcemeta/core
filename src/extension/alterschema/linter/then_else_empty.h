class ThenElseEmpty final : public SchemaTransformRule {
public:
  ThenElseEmpty()
      : SchemaTransformRule{
            "then_else_empty",
            "`then` or `else` with an empty schema does not restrict "
            "validation and is likely ineffective"} {};

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
           ((schema.defines("then") && schema.at("then").is_object() &&
             schema.at("then").empty()) ||
            (schema.defines("else") && schema.at("else").is_object() &&
             schema.at("else").empty()));
  }

  auto transform(JSON &schema) const -> void override {
    if (schema.defines("then") && schema.at("then").is_object() &&
        schema.at("then").empty()) {
      schema.erase("then");
    }
    if (schema.defines("else") && schema.at("else").is_object() &&
        schema.at("else").empty()) {
      schema.erase("else");
    }
  }
};
