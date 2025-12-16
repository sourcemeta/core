class DuplicateExamples final : public SchemaTransformRule {
public:
  DuplicateExamples()
      : SchemaTransformRule{
            "duplicate_examples",
            "Setting duplicate values in `examples` is redundant"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    ONLY_CONTINUE_IF(vocabularies.contains_any(
                         {Vocabularies::Known::JSON_Schema_2020_12_Meta_Data,
                          Vocabularies::Known::JSON_Schema_2019_09_Meta_Data,
                          Vocabularies::Known::JSON_Schema_Draft_7,
                          Vocabularies::Known::JSON_Schema_Draft_6}) &&
                     schema.is_object() && schema.defines("examples") &&
                     schema.at("examples").is_array() &&
                     !schema.at("examples").unique());
    return APPLIES_TO_KEYWORDS("examples");
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    auto collection = schema.at("examples");
    std::sort(collection.as_array().begin(), collection.as_array().end());
    auto last =
        std::unique(collection.as_array().begin(), collection.as_array().end());
    collection.erase(last, collection.as_array().end());
    schema.at("examples").into(std::move(collection));
  }
};
