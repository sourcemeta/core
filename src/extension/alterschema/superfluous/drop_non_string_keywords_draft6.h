class DropNonStringKeywords_Draft6 final : public SchemaTransformRule {
public:
  DropNonStringKeywords_Draft6()
      : SchemaTransformRule{
            "drop_non_string_keywords_draft6",
            "Keywords that don't apply to strings will never match if the "
            "instance is guaranteed to be a string"} {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const std::set<std::string> &vocabularies,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> bool override {
    return vocabularies.contains("http://json-schema.org/draft-06/schema#") &&
           schema.is_object() && schema.defines("type") &&
           schema.at("type").is_string() &&
           schema.at("type").to_string() == "string" &&
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
                                        "dependencies"};
};
