class ThenFalse final : public SchemaTransformRule {
public:
  ThenFalse()
      : SchemaTransformRule{
            "then_false",
            "`Setting the `then` keyword to the false schema is a convoluted "
            "way of negating the `if` subschema, which you can more cleanly "
            "represent using the `not` keyword"} {};

  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &, const SchemaFrame::Location &,
            const SchemaWalker &, const SchemaResolver &) const
      -> SchemaTransformRule::Result override {
    ONLY_CONTINUE_IF(
        contains_any(vocabularies,
                     {"https://json-schema.org/draft/2020-12/vocab/applicator",
                      "https://json-schema.org/draft/2019-09/vocab/applicator",
                      "http://json-schema.org/draft-07/schema#"}) &&
        schema.is_object() && schema.defines("if") && schema.defines("then") &&
        is_schema(schema.at("then")) && schema.at("then").is_boolean() &&
        !schema.at("then").to_boolean() && is_schema(schema.at("if")) &&
        !(schema.at("if").is_boolean() && schema.at("if").to_boolean()) &&
        (!schema.defines("else") ||
         (schema.at("else").is_boolean() && schema.at("else").to_boolean())) &&
        (!schema.at("if").is_object() ||
         std::all_of(schema.at("if").as_object().begin(),
                     schema.at("if").as_object().end(),
                     [&schema](const auto &entry) {
                       return !schema.defines(entry.first);
                     })));

    if (schema.defines("else")) {
      return APPLIES_TO_KEYWORDS("then", "if", "else");
    } else {
      return APPLIES_TO_KEYWORDS("then", "if");
    }
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    auto if_schema = schema.at("if");
    schema.erase("if");
    schema.erase("then");
    schema.erase("else");
    schema.assign("not", std::move(if_schema));
  }
};
