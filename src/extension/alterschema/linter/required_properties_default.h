class RequiredPropertiesDefault final : public SchemaTransformRule {
public:
  RequiredPropertiesDefault()
      : SchemaTransformRule{"required_properties_default",
                            "Properties not listed in `required` add "
                            "unnecessary constraints to `properties`"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/validation",
                "https://json-schema.org/draft/2019-09/vocab/validation",
                "http://json-schema.org/draft-07/schema#",
                "http://json-schema.org/draft-06/schema#",
                "http://json-schema.org/draft-04/schema#"}) &&
           schema.is_object() && schema.defines("required") &&
           schema.at("required").is_array() && !schema.at("required").empty() &&
           schema.defines("properties") &&
           schema.at("properties").is_object() &&
           !schema.at("properties").empty() && has_extra_properties(schema);
  }

  auto transform(JSON &schema) const -> void override {
    const auto &required_array = schema.at("required").as_array();
    std::unordered_set<std::string> required_names;
    for (const auto &item : required_array) {
      if (item.is_string()) {
        required_names.insert(item.to_string());
      }
    }

    std::vector<JSON::String> properties_to_remove;
    for (const auto &entry : schema.at("properties").as_object()) {
      if (required_names.find(entry.first) == required_names.end()) {
        properties_to_remove.emplace_back(entry.first);
      }
    }

    for (const auto &property_name : properties_to_remove) {
      schema.at("properties").erase(property_name);
    }
  }

private:
  auto has_extra_properties(const sourcemeta::core::JSON &schema) const
      -> bool {
    const auto &required_array = schema.at("required").as_array();
    const auto &properties_obj = schema.at("properties").as_object();

    std::unordered_set<std::string> required_names;
    for (const auto &item : required_array) {
      if (item.is_string()) {
        required_names.insert(item.to_string());
      }
    }

    for (const auto &entry : properties_obj) {
      if (required_names.find(entry.first) == required_names.end()) {
        return true;
      }
    }

    return false;
  }
};
