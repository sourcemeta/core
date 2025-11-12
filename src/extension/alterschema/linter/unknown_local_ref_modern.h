class UnknownLocalRefModern final : public SchemaTransformRule {
public:
  UnknownLocalRefModern()
      : SchemaTransformRule{
            "unknown_local_ref",
            "Local references that point to unknown locations are invalid"} {};

  // TODO: On Draft 7 and older, if we remove the $ref we need to remove ALL
  // siblings?
  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &frame, const SchemaFrame::Location &location,
            const SchemaWalker &, const SchemaResolver &) const
      -> SchemaTransformRule::Result override {
    ONLY_CONTINUE_IF(vocabularies.contains(
                         "https://json-schema.org/draft/2019-09/vocab/core") ||
                     vocabularies.contains(
                         "https://json-schema.org/draft/2020-12/vocab/core"));
    ONLY_CONTINUE_IF(schema.is_object() && schema.defines("$ref") &&
                     schema.at("$ref").is_string());

    // Find the keyword location entry
    const auto absolute_ref_pointer{location.pointer.concat({"$ref"})};
    const auto reference_entry{frame.references().find(
        {SchemaReferenceType::Static, absolute_ref_pointer})};
    ONLY_CONTINUE_IF(reference_entry != frame.references().end());

    // If the keyword has no fragment, continue
    const auto &reference_fragment{reference_entry->second.fragment};
    ONLY_CONTINUE_IF(reference_fragment.has_value());

    // Only continue if the reference target does not exist
    const auto target_location{frame.locations().find(
        {SchemaReferenceType::Static, reference_entry->second.destination})};
    ONLY_CONTINUE_IF(target_location == frame.locations().end());

    // If there is a base beyond the fragment, the base must exist.
    // Otherwise it is likely an external reference?
    const auto &reference_base{reference_entry->second.base};
    if (reference_base.has_value()) {
      const auto base_location{frame.locations().find(
          {SchemaReferenceType::Static, reference_base.value()})};
      ONLY_CONTINUE_IF(base_location != frame.locations().end());
    }

    return APPLIES_TO_KEYWORDS("$ref");
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    schema.erase("$ref");
  }
};
