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
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/applicator",
                       "https://json-schema.org/draft/2019-09/vocab/applicator",
                       "http://json-schema.org/draft-07/schema#"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("if") ||
        !schema.defines("then") || !is_schema(schema.at("then")) ||
        !schema.at("then").is_boolean() || schema.at("then").to_boolean() ||
        !is_schema(schema.at("if"))) {
      return false;
    }

    if (schema.defines("else") &&
        (!schema.at("else").is_boolean() || !schema.at("else").to_boolean())) {
      return false;
    }

    if (schema.at("if").is_object()) {
      for (const auto &entry : schema.at("if").as_object()) {
        if (schema.defines(entry.first)) {
          return false;
        }
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto if_schema = schema.at("if");
    schema.erase("if");
    schema.erase("then");
    if (schema.defines("else")) {
      schema.erase("else");
    }
    schema.assign("not", std::move(if_schema));
  }
};
