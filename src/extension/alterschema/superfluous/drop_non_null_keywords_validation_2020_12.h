class DropNonNullKeywordsValidation_2020_12 final : public SchemaTransformRule {
public:
  DropNonNullKeywordsValidation_2020_12()
      : SchemaTransformRule{
            "drop_non_null_keywords_validation_2020_12",
            "Keywords that don't apply to null values will never match if the "
            "instance is guaranteed to be null"} {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const std::set<std::string> &vocabularies,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return vocabularies.contains(
               "https://json-schema.org/draft/2020-12/vocab/validation") &&
           schema.is_object() &&
           ((schema.defines("type") && schema.at("type").is_string() &&
             schema.at("type").to_string() == "null") ||
            (schema.defines("enum") && schema.at("enum").is_array() &&
             every_item_is_null(schema.at("enum").as_array()))) &&
           vocabularies.contains(
               "https://json-schema.org/draft/2020-12/vocab/validation") &&
           schema.defines_any(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

  auto transform(JSON &schema) const -> void override {
    schema.erase_keys(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

private:
  const std::set<std::string> BLACKLIST{
      "minLength",         "maxLength",     "pattern",          "maximum",
      "exclusiveMinimum",  "multipleOf",    "exclusiveMaximum", "minimum",
      "dependentRequired", "minProperties", "maxProperties",    "required",
      "minItems",          "maxItems",      "minContains",      "maxContains",
      "uniqueItems"};
};
