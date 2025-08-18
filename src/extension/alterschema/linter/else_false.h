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
           schema.defines("else") && is_schema(schema.at("else")) &&
           schema.at("else").is_boolean() && !schema.at("else").to_boolean() &&
           is_schema(schema.at("if")) &&
           (!schema.defines("then") || (schema.at("then").is_boolean() &&
                                        schema.at("then").to_boolean())) &&
           (!schema.at("if").is_object() || [&schema](const JSON &if_schema) {
             for (const auto &entry : if_schema.as_object()) {
               if (schema.defines(entry.first)) {
                 return false;
               }
             }
             return true;
           }(schema.at("if")));
  }

  auto transform(JSON &schema) const -> void override {
    auto if_schema = schema.at("if");
    schema.erase("if");
    schema.erase("else");
    schema.erase("then");

    if (if_schema.is_object()) {
      for (auto &entry : if_schema.as_object()) {
        schema.assign(entry.first, entry.second);
      }
    } else if (if_schema.is_boolean() && !if_schema.to_boolean()) {
      auto metadata = JSON::make_object();
      for (const auto &entry : schema.as_object()) {
        if (entry.first.starts_with("$")) {
          metadata.assign(entry.first, entry.second);
        }
      }
      schema = std::move(metadata);
      schema.assign("not", JSON::make_object());
    }
  }
};
