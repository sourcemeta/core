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
        vocabularies.contains_any(
            {Vocabularies::Known::JSON_Schema_2020_12_Applicator,
             Vocabularies::Known::JSON_Schema_2019_09_Applicator,
             Vocabularies::Known::JSON_Schema_Draft_7}) &&
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
                     })) &&
        std::all_of(schema.as_object().begin(), schema.as_object().end(),
                    [](const auto &entry) {
                      return entry.first == "$schema" || entry.first == "if" ||
                             entry.first == "then" || entry.first == "else";
                    }));

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
