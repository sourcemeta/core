class ThenFalse final : public SchemaTransformRule {
public:
  ThenFalse()
      : SchemaTransformRule{
            "then_false",
            "`if: S, then: false` is logically equivalent to `not: S`"} {};

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
           schema.defines("then") && schema.at("then").is_boolean() &&
           !schema.at("then").to_boolean();
  }

  auto transform(JSON &schema) const -> void override {
    const auto if_schema = schema.at("if");
    schema.erase("if");
    schema.erase("then");
    schema.assign("not", if_schema);
  }
};
