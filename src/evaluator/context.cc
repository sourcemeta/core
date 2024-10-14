#include <sourcemeta/jsontoolkit/evaluator_context.h>
#include <sourcemeta/jsontoolkit/evaluator_error.h>

#include <cassert> // assert

namespace sourcemeta::jsontoolkit {

auto EvaluationContext::prepare(const JSON &instance) -> void {
  // Do a full reset for the next run
  assert(this->evaluate_path_.empty());
  assert(this->instance_location_.empty());
  assert(this->frame_sizes.empty());
  assert(this->resources_.empty());
  this->instances_.clear();
  this->instances_.emplace_back(instance);
  this->labels.clear();
  this->property_as_instance = false;
  this->annotation_blacklist.clear();
  this->annotations_.clear();
}

auto EvaluationContext::push_without_traverse(
    const Pointer &relative_schema_location,
    const Pointer &relative_instance_location,
    const std::string &schema_resource, const bool dynamic) -> void {
  // Guard against infinite recursion in a cheap manner, as
  // infinite recursion will manifest itself through huge
  // ever-growing evaluate paths
  constexpr auto EVALUATE_PATH_LIMIT{300};
  if (this->evaluate_path_.size() > EVALUATE_PATH_LIMIT) [[unlikely]] {
    throw sourcemeta::jsontoolkit::SchemaEvaluationError(
        "The evaluation path depth limit was reached "
        "likely due to infinite recursion");
  }

  this->frame_sizes.emplace_back(relative_schema_location.size(),
                                 relative_instance_location.size());
  this->evaluate_path_.push_back(relative_schema_location);
  this->instance_location_.push_back(relative_instance_location);

  if (dynamic) {
    // Note that we are potentially repeatedly pushing back the
    // same schema resource over and over again. However, the
    // logic for making sure this list is "pure" takes a lot of
    // computation power. Being silly seems faster.
    this->resources_.push_back(schema_resource);
  }
}

auto EvaluationContext::push(const Pointer &relative_schema_location,
                             const Pointer &relative_instance_location,
                             const std::string &schema_resource,
                             const bool dynamic) -> void {
  this->push_without_traverse(relative_schema_location,
                              relative_instance_location, schema_resource,
                              dynamic);
  if (!relative_instance_location.empty()) {
    assert(!this->instances_.empty());
    this->instances_.emplace_back(
        get(this->instances_.back().get(), relative_instance_location));
  }
}

auto EvaluationContext::push(const Pointer &relative_schema_location,
                             const Pointer &relative_instance_location,
                             const std::string &schema_resource,
                             const bool dynamic,
                             std::reference_wrapper<const JSON> &&new_instance)
    -> void {
  this->push_without_traverse(relative_schema_location,
                              relative_instance_location, schema_resource,
                              dynamic);
  assert(!relative_instance_location.empty());
  this->instances_.emplace_back(new_instance);
}

auto EvaluationContext::pop(const bool dynamic) -> void {
  assert(!this->frame_sizes.empty());
  const auto &sizes{this->frame_sizes.back()};
  this->evaluate_path_.pop_back(sizes.first);
  this->instance_location_.pop_back(sizes.second);
  if (sizes.second > 0) {
    this->instances_.pop_back();
  }

  this->frame_sizes.pop_back();

  // TODO: Do schema resource management using hashes to avoid
  // expensive string comparisons
  if (dynamic) {
    assert(!this->resources_.empty());
    this->resources_.pop_back();
  }
}

// TODO: At least currently, we only need to mask if a schema
// makes use of `unevaluatedProperties` or `unevaluatedItems`
// Detect if a schema does need this so if not, we avoid
// an unnecessary copy
auto EvaluationContext::mask() -> void {
  this->annotation_blacklist.push_back(this->evaluate_path_);
}

auto EvaluationContext::annotate(const WeakPointer &current_instance_location,
                                 const JSON &value)
    -> std::pair<std::reference_wrapper<const JSON>, bool> {
  const auto result{this->annotations_.insert({current_instance_location, {}})
                        .first->second.insert({this->evaluate_path(), {}})
                        .first->second.insert(value)};
  return {*(result.first), result.second};
}

auto EvaluationContext::defines_any_annotation(
    const std::string &expected_keyword) const -> bool {
  const auto instance_location_result{
      this->annotations_.find(this->instance_location_)};
  if (instance_location_result == this->annotations_.end()) {
    return false;
  }

  for (const auto &[schema_location, schema_annotations] :
       instance_location_result->second) {
    assert(!schema_location.empty());
    const auto &keyword{schema_location.back()};

    if (keyword.is_property() && expected_keyword == keyword.to_property() &&
        !schema_annotations.empty() &&
        schema_location.initial().starts_with(this->evaluate_path_)) {
      bool blacklisted = false;
      for (const auto &masked : this->annotation_blacklist) {
        if (schema_location.starts_with(masked) &&
            !this->evaluate_path_.starts_with(masked)) {
          blacklisted = true;
          break;
        }
      }

      if (!blacklisted) {
        return true;
      }
    }
  }

  return false;
}

auto EvaluationContext::defines_sibling_annotation(
    const std::vector<std::string> &keywords, const JSON &value) const -> bool {
  if (keywords.empty()) {
    return false;
  }

  const auto instance_location_result{
      this->annotations_.find(this->instance_location_)};
  if (instance_location_result == this->annotations_.end()) {
    return false;
  }

  for (const auto &[schema_location, schema_annotations] :
       instance_location_result->second) {
    assert(!schema_location.empty());
    const auto &keyword{schema_location.back()};

    if (keyword.is_property() &&
        std::find(keywords.cbegin(), keywords.cend(), keyword.to_property()) !=
            keywords.cend() &&
        schema_annotations.contains(value) &&
        schema_location.initial().starts_with(this->evaluate_path_.initial())) {
      bool blacklisted = false;
      for (const auto &masked : this->annotation_blacklist) {
        if (schema_location.starts_with(masked) &&
            !this->evaluate_path_.starts_with(masked)) {
          blacklisted = true;
          break;
        }
      }

      if (!blacklisted) {
        return true;
      }
    }
  }

  return false;
}

auto EvaluationContext::largest_annotation_index(
    const std::string &expected_keyword) const -> std::uint64_t {
  // TODO: We should be taking masks into account
  std::uint64_t result{0};

  const auto instance_location_result{
      this->annotations_.find(this->instance_location_)};
  if (instance_location_result == this->annotations_.end()) {
    return result;
  }

  for (const auto &[schema_location, schema_annotations] :
       instance_location_result->second) {
    assert(!schema_location.empty());
    const auto &keyword{schema_location.back()};
    if (!keyword.is_property() || expected_keyword != keyword.to_property()) {
      continue;
    }

    for (const auto &annotation : schema_annotations) {
      if (annotation.is_integer() && annotation.is_positive()) {
        result = std::max(
            result, static_cast<std::uint64_t>(annotation.to_integer()) + 1);
      }
    }
  }

  return result;
}

auto EvaluationContext::enter(const WeakPointer::Token::Property &property)
    -> void {
  this->instance_location_.push_back(property);
  this->instances_.emplace_back(this->instances_.back().get().at(property));
}

auto EvaluationContext::enter(const WeakPointer::Token::Index &index) -> void {
  this->instance_location_.push_back(index);
  this->instances_.emplace_back(this->instances_.back().get().at(index));
}

auto EvaluationContext::leave() -> void {
  this->instance_location_.pop_back();
  this->instances_.pop_back();
}

auto EvaluationContext::instances() const noexcept
    -> const std::vector<std::reference_wrapper<const JSON>> & {
  return this->instances_;
}

auto EvaluationContext::hash(const std::string &base,
                             const std::string &fragment) const noexcept
    -> std::size_t {
  // TODO: Avoid these string copies
  std::ostringstream result;
  result << base;
  result << '#';
  result << fragment;
  return this->hasher_(result.str());
}

auto EvaluationContext::resources() const noexcept
    -> const std::vector<std::string> & {
  return this->resources_;
}

auto EvaluationContext::evaluate_path() const noexcept -> const WeakPointer & {
  return this->evaluate_path_;
}

auto EvaluationContext::instance_location() const noexcept
    -> const WeakPointer & {
  return this->instance_location_;
}

auto EvaluationContext::target_type(const TargetType type) noexcept -> void {
  this->property_as_instance = (type == TargetType::Key);
}

auto EvaluationContext::resolve_target() -> const JSON & {
  if (this->property_as_instance) [[unlikely]] {
    // In this case, we still need to return a string in order
    // to cope with non-string keywords inside `propertyNames`
    // that need to fail validation. But then, the actual string
    // we return doesn't matter, so we can always return a dummy one.
    static const JSON empty_string{""};
    return empty_string;
  }

  return this->instances_.back().get();
}

auto EvaluationContext::resolve_string_target()
    -> std::optional<std::reference_wrapper<const JSON::String>> {
  if (this->property_as_instance) [[unlikely]] {
    assert(!this->instance_location().empty());
    assert(this->instance_location().back().is_property());
    return this->instance_location().back().to_property();
  } else {
    const auto &result{this->instances_.back().get()};
    if (!result.is_string()) {
      return std::nullopt;
    }

    return result.to_string();
  }
}

auto EvaluationContext::mark(const std::size_t id,
                             const SchemaCompilerTemplate &children) -> void {
  this->labels.try_emplace(id, children);
}

auto EvaluationContext::jump(const std::size_t id) const noexcept
    -> const SchemaCompilerTemplate & {
  assert(this->labels.contains(id));
  return this->labels.at(id).get();
}

auto EvaluationContext::find_dynamic_anchor(const std::string &anchor) const
    -> std::optional<std::size_t> {
  for (const auto &resource : this->resources()) {
    const auto label{this->hash(resource, anchor)};
    if (this->labels.contains(label)) {
      return label;
    }
  }

  return std::nullopt;
}

} // namespace sourcemeta::jsontoolkit
