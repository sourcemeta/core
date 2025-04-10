class DropNonBooleanKeywords_Draft7 final : public SchemaTransformRule {
public:
  DropNonBooleanKeywords_Draft7()
      : SchemaTransformRule{
            "drop_non_boolean_keywords_draft7",
            "Keywords that don't apply to booleans will never match if the "
            "instance is guaranteed to be a boolean"} {};

  [[nodiscard]] auto condition(
      const sourcemeta::core::JSON &schema, const std::string &,
      const std::set<std::string> &vocabularies,
      const sourcemeta::core::Pointer &, const sourcemeta::core::SchemaFrame &,
      const sourcemeta::core::SchemaFrame::Location &) const -> bool override {
    return vocabularies.contains("http://json-schema.org/draft-07/schema#") &&
           schema.is_object() && schema.defines("type") &&
           schema.at("type").is_string() &&
           schema.at("type").to_string() == "boolean" &&
           schema.defines_any(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

  auto transform(JSON &schema) const -> void override {
    schema.erase_keys(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

private:
  const std::set<std::string> BLACKLIST{"multipleOf",
                                        "maximum",
                                        "exclusiveMaximum",
                                        "minimum",
                                        "exclusiveMinimum",
                                        "maxLength",
                                        "minLength",
                                        "pattern",
                                        "contentEncoding",
                                        "contentMediaType",
                                        "additionalItems",
                                        "items",
                                        "contains",
                                        "maxItems",
                                        "minItems",
                                        "uniqueItems",
                                        "maxProperties",
                                        "minProperties",
                                        "required",
                                        "properties",
                                        "patternProperties",
                                        "additionalProperties",
                                        "propertyNames",
                                        "dependencies",
                                        "format"};
};
