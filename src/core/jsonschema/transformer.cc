#include <sourcemeta/core/jsonschema.h>

#include <cassert>   // assert
#include <set>       // std::set
#include <sstream>   // std::ostringstream
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move, std::pair

namespace {

auto vocabularies_to_set(const std::map<std::string, bool> &vocabularies)
    -> std::set<std::string> {
  std::set<std::string> result;
  for (const auto &pair : vocabularies) {
    result.insert(pair.first);
  }

  return result;
}

} // namespace

namespace sourcemeta::core {

SchemaTransformRule::SchemaTransformRule(std::string &&name,
                                         std::string &&message)
    : name_{std::move(name)}, message_{std::move(message)} {}

auto SchemaTransformRule::operator==(const SchemaTransformRule &other) const
    -> bool {
  return this->name() == other.name();
}

auto SchemaTransformRule::name() const -> const std::string & {
  return this->name_;
}

auto SchemaTransformRule::message() const -> const std::string & {
  return this->message_;
}

auto SchemaTransformRule::apply(
    JSON &schema, const Pointer &pointer, const SchemaResolver &resolver,
    const SchemaFrame &frame,
    const std::optional<std::string> &default_dialect) const -> bool {
  const std::optional<std::string> effective_dialect{
      dialect(schema, default_dialect)};
  if (!effective_dialect.has_value()) {
    throw SchemaError("Could not determine the schema dialect");
  }

  // TODO: Stop converting to set. This hurts performance
  const auto current_vocabularies{
      vocabularies_to_set(vocabularies(schema, resolver, default_dialect))};
  if (!this->condition(schema, effective_dialect.value(), current_vocabularies,
                       pointer, frame)) {
    return false;
  }

  this->transform(schema);

  // The condition must always be false after applying the
  // transformation in order to avoid infinite loops
  if (this->condition(schema, effective_dialect.value(), current_vocabularies,
                      pointer, frame)) {
    std::ostringstream error;
    error << "Rule condition holds after application: " << this->name();
    throw std::runtime_error(error.str());
  }

  return true;
}

auto SchemaTransformRule::check(
    const JSON &schema, const Pointer &pointer, const SchemaResolver &resolver,
    const SchemaFrame &frame,
    const std::optional<std::string> &default_dialect) const -> bool {
  const std::optional<std::string> effective_dialect{
      dialect(schema, default_dialect)};
  if (!effective_dialect.has_value()) {
    throw SchemaError("Could not determine the schema dialect");
  }

  return this->condition(
      schema, effective_dialect.value(),
      vocabularies_to_set(vocabularies(schema, resolver, default_dialect)),
      pointer, frame);
}

auto SchemaTransformer::check(
    const JSON &schema, const SchemaWalker &walker,
    const SchemaResolver &resolver,
    const SchemaTransformer::CheckCallback &callback,
    const std::optional<std::string> &default_dialect) const -> bool {
  SchemaFrame frame{SchemaFrame::Mode::Locations};
  frame.analyse(schema, walker, resolver, default_dialect);

  bool result{true};
  for (const auto &entry : frame.locations()) {
    if (entry.second.type != SchemaFrame::LocationType::Resource &&
        entry.second.type != SchemaFrame::LocationType::Subschema) {
      continue;
    }

    const auto &current{get(schema, entry.second.pointer)};
    for (const auto &[name, rule] : this->rules) {
      if (rule->check(current, entry.second.pointer, resolver, frame,
                      entry.second.dialect)) {
        result = false;
        callback(entry.second.pointer, name, rule->message());
      }
    }
  }

  return result;
}

auto SchemaTransformer::apply(
    JSON &schema, const SchemaWalker &walker, const SchemaResolver &resolver,
    const std::optional<std::string> &default_dialect) const -> bool {
  // There is no point in applying an empty bundle
  assert(!this->rules.empty());
  std::set<std::pair<Pointer, JSON::String>> processed_rules;

  while (true) {
    SchemaFrame frame{SchemaFrame::Mode::Locations};
    frame.analyse(schema, walker, resolver, default_dialect);

    bool applied{false};
    for (const auto &entry : frame.locations()) {
      if (entry.second.type != SchemaFrame::LocationType::Resource &&
          entry.second.type != SchemaFrame::LocationType::Subschema) {
        continue;
      }

      auto &current{get(schema, entry.second.pointer)};
      for (const auto &[name, rule] : this->rules) {
        applied = rule->apply(current, entry.second.pointer, resolver, frame,
                              entry.second.dialect) ||
                  applied;
        if (!applied) {
          continue;
        }

        if (processed_rules.contains({entry.second.pointer, name})) {
          // TODO: Throw a better custom error that also highlights the schema
          // location
          std::ostringstream error;
          error << "Rules must only be processed once: " << name;
          throw std::runtime_error(error.str());
        }

        processed_rules.emplace(entry.second.pointer, name);
        goto alterschema_start_again;
      }
    }

  alterschema_start_again:
    if (!applied) {
      break;
    }
  }

  return true;
}

auto SchemaTransformer::remove(const std::string &name) -> bool {
  return this->rules.erase(name) > 0;
}

} // namespace sourcemeta::core
