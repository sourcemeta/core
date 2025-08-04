class DefinitionsToDefs final : public SchemaTransformRule {
public:
  DefinitionsToDefs()
      : SchemaTransformRule{
            "definitions_to_defs",
            "`definitions` is superseded by `$defs` starting in draft-2019-09. "
            "Migrate to `$defs` for forward compatibility."} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return contains_any(vocabularies,
                        {"https://json-schema.org/draft/2020-12/vocab/core",
                         "https://json-schema.org/draft/2019-09/vocab/core"}) &&
           schema.is_object() && schema.defines("definitions") &&
           !schema.defines("$defs");
  }

  auto transform(JSON &schema) const -> void override {
    schema.rename("definitions", "$defs");
  }

  [[nodiscard]] auto rereference(const std::string &reference, const Pointer &,
                                 const Pointer &, const Pointer &) const
      -> Pointer override {
    // Update any $ref references from #/definitions/ to #/$defs/
    if (reference.find("#/definitions/") == 0) {
      std::string updated_ref = reference;
      updated_ref.replace(0, 14, "#/$defs/");
      // Extract just the fragment part (skip the #)
      std::string fragment = updated_ref.substr(1);
      return to_pointer(fragment);
    }

    if (reference.find("#") == 0) {
      std::string fragment = reference.substr(1);
      return to_pointer(fragment);
    }

    // If it's not a fragment reference, return empty pointer
    return Pointer{};
  }
};
