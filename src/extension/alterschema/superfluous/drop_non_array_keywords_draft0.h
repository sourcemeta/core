class DropNonArrayKeywords_Draft0 final : public SchemaTransformRule {
public:
  DropNonArrayKeywords_Draft0()
      : SchemaTransformRule{
            "drop_non_array_keywords_draft0",
            "Keywords that don't apply to arrays will never match if the "
            "instance is guaranteed to be an array"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const -> bool override {
    return vocabularies.contains(
               "http://json-schema.org/draft-00/hyper-schema#") &&
           schema.is_object() && schema.defines("type") &&
           schema.at("type").is_string() &&
           schema.at("type").to_string() == "array" &&
           schema.defines_any(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

  auto transform(JSON &schema) const -> void override {
    schema.erase_keys(this->BLACKLIST.cbegin(), this->BLACKLIST.cend());
  }

private:
  const std::set<std::string> BLACKLIST{
      "maxDecimal",      "maximum",   "maximumCanEqual", "minimum",
      "minimumCanEqual", "maxLength", "minLength",       "pattern",
      "requires",        "optional",  "properties",      "additionalProperties",
      "contentEncoding", "format"};
};
