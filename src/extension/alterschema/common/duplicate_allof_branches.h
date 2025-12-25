class DuplicateAllOfBranches final : public SchemaTransformRule {
public:
  DuplicateAllOfBranches()
      : SchemaTransformRule{
            "duplicate_allof_branches",
            "Setting duplicate subschemas in `allOf` is redundant, as it "
            "produces "
            "unnecessary additional validation that is guaranteed to not "
            "affect the validation result"} {};

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
                         {Vocabularies::Known::JSON_Schema_2020_12_Applicator,
                          Vocabularies::Known::JSON_Schema_2019_09_Applicator,
                          Vocabularies::Known::JSON_Schema_Draft_7,
                          Vocabularies::Known::JSON_Schema_Draft_6,
                          Vocabularies::Known::JSON_Schema_Draft_4}) &&
                     schema.is_object() && schema.defines("allOf") &&
                     schema.at("allOf").is_array() &&
                     !schema.at("allOf").unique());
    // TODO: Highlight which specific entries in `allOf` are duplicated
    return APPLIES_TO_KEYWORDS("allOf");
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Build a mapping from old indices to new indices
    // We need to track where each original entry ends up after sort+unique
    this->index_mapping_.clear();
    const auto &original{schema.at("allOf")};
    const auto original_size{original.size()};

    // Create pairs of (value, original_index) to track positions
    std::vector<std::pair<JSON, std::size_t>> indexed_values;
    indexed_values.reserve(original_size);
    for (std::size_t index = 0; index < original_size; ++index) {
      indexed_values.emplace_back(original.at(index), index);
    }

    // Sort by value (maintaining original index information)
    std::ranges::sort(indexed_values, [](const auto &left, const auto &right) {
      return left.first < right.first;
    });

    // Remove duplicates and build index mapping
    std::size_t new_index{0};
    for (std::size_t index = 0; index < indexed_values.size(); ++index) {
      const auto original_index{indexed_values[index].second};
      if (index == 0 ||
          indexed_values[index].first != indexed_values[index - 1].first) {
        // This is a unique entry, it stays at new_index
        this->index_mapping_[original_index] = new_index;
        ++new_index;
      } else {
        // This is a duplicate, point to the same new_index as the previous
        this->index_mapping_[original_index] = new_index - 1;
      }
    }

    // Now perform the actual transformation
    auto collection = schema.at("allOf");
    std::sort(collection.as_array().begin(), collection.as_array().end());
    auto last =
        std::unique(collection.as_array().begin(), collection.as_array().end());
    collection.erase(last, collection.as_array().end());
    schema.at("allOf").into(std::move(collection));
  }

  [[nodiscard]] auto rereference(const std::string &, const Pointer &,
                                 const Pointer &target,
                                 const Pointer &current) const
      -> Pointer override {
    const auto allof_prefix{current.concat({"allOf"})};
    const auto relative{target.resolve_from(allof_prefix)};
    const auto old_index{relative.at(0).to_index()};
    const auto new_index{this->index_mapping_.at(old_index)};
    const Pointer old_prefix{allof_prefix.concat({old_index})};
    const Pointer new_prefix{allof_prefix.concat({new_index})};
    return target.rebase(old_prefix, new_prefix);
  }

private:
  mutable std::map<std::size_t, std::size_t> index_mapping_;
};
