#include <sourcemeta/core/jsonpath.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include "parser.h"

#include <algorithm>  // std::max, std::min
#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <cstdint>    // std::int64_t
#include <functional> // std::cref
#include <optional>   // std::optional
#include <string>     // std::to_string
#include <variant> // std::get, std::get_if, std::holds_alternative, std::monostate, std::variant
#include <vector> // std::vector

namespace sourcemeta::core {

namespace {

// One step of the current traversal location. The full pointer is only
// materialized when a result node fires, so the walk itself performs raw
// stores instead of token constructions
struct LocationFrame {
  const JSON::String *property;
  JSON::Object::hash_type hash;
  std::size_t index;
};

class LocationStack {
public:
  LocationStack() { this->storage_.resize(this->capacity_); }

  auto push_property(const JSON::String &property,
                     const JSON::Object::hash_type hash) -> void {
    if (this->depth_ == this->capacity_) {
      this->grow();
    }

    auto &frame{*(this->storage_.data() + this->depth_)};
    frame.property = &property;
    frame.hash = hash;
    this->depth_ += 1;
  }

  auto push_index(const std::size_t index) -> void {
    if (this->depth_ == this->capacity_) {
      this->grow();
    }

    auto &frame{*(this->storage_.data() + this->depth_)};
    frame.property = nullptr;
    frame.index = index;
    this->depth_ += 1;
  }

  auto pop() -> void { this->depth_ -= 1; }

  auto materialize(WeakPointer &location) const -> void {
    location.pop_back(location.size());
    const auto *frame{this->storage_.data()};
    const auto *const frames_end{frame + this->depth_};
    for (; frame != frames_end; ++frame) {
      if (frame->property == nullptr) {
        location.emplace_back(frame->index);
      } else {
        location.emplace_back(std::cref(*frame->property), frame->hash);
      }
    }
  }

private:
  auto grow() -> void {
    this->capacity_ *= 2;
    this->storage_.resize(this->capacity_);
  }

  std::size_t depth_{0};
  std::size_t capacity_{64};
  std::vector<LocationFrame> storage_;
};

struct EvaluationState {
  const JSON *root;
  const JSONPath::Callback *callback;
  LocationStack frames;
  WeakPointer location;
};

auto filter_matches(const JSONPath::FilterExpression &expression,
                    const JSON &candidate, const JSON &root) -> bool;

// The underlying containers are contiguous, so iterating through raw
// pointers instead of iterators keeps the hot traversal loops free of the
// unoptimized iterator call chains of debug builds
inline auto first_member(const JSON::Object &object)
    -> const JSON::Object::Entry * {
  return object.empty() ? nullptr : &*object.cbegin();
}

inline auto first_element(const JSON::Array &array) -> const JSON * {
  return array.size() == 0 ? nullptr : &*array.cbegin();
}

// RFC 9535 Section 2.3.4.2.2: slice expression bounds against a concrete
// array length, where an omitted start or end defaults according to the
// sign of the step and a zero step selects nothing
inline auto slice_bounds(const JSONPath::SelectorSlice &slice,
                         const std::int64_t size, std::int64_t &lower,
                         std::int64_t &upper) -> bool {
  if (slice.step == 0) {
    return false;
  }

  const auto start{slice.start.has_value() ? slice.start.value()
                                           : (slice.step > 0 ? 0 : size - 1)};
  const auto end{slice.end.has_value() ? slice.end.value()
                                       : (slice.step > 0 ? size : -size - 1)};
  const auto normalized_start{start >= 0 ? start : size + start};
  const auto normalized_end{end >= 0 ? end : size + end};
  if (slice.step > 0) {
    lower = std::min(std::max(normalized_start, std::int64_t{0}), size);
    upper = std::min(std::max(normalized_end, std::int64_t{0}), size);
  } else {
    upper = std::min(std::max(normalized_start, std::int64_t{-1}), size - 1);
    lower = std::min(std::max(normalized_end, std::int64_t{-1}), size - 1);
  }

  return true;
}

// Walk the nodes selected by a filter query without tracking locations. The
// visitor returns whether to continue, and the traversal reports whether it
// ran to completion without the visitor stopping it
template <typename Visitor>
auto filter_query_visit(const std::vector<JSONPath::Segment> &segments,
                        const std::size_t cursor, const JSON &current,
                        const JSON &root, const Visitor &visitor) -> bool {
  if (cursor == segments.size()) {
    return visitor(current);
  }

  const auto &segment{*(segments.data() + cursor)};
  const auto type{current.type()};
  const auto *selector_cursor{segment.selectors.data()};
  const auto *const selectors_end{selector_cursor + segment.selectors.size()};
  for (; selector_cursor != selectors_end; ++selector_cursor) {
    const auto &selector{*selector_cursor};
    switch (static_cast<JSONPath::SelectorKind>(selector.index())) {
      case JSONPath::SelectorKind::Name: {
        const auto *entry{std::get_if<JSONPath::SelectorName>(&selector)};
        if (current.is_object()) {
          const auto *child{current.try_at(entry->name, entry->hash)};
          if (child != nullptr && !filter_query_visit(segments, cursor + 1,
                                                      *child, root, visitor)) {
            return false;
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Wildcard:
        if (type == JSON::Type::Object) {
          for (const auto &member : current.as_object()) {
            if (!filter_query_visit(segments, cursor + 1, member.second, root,
                                    visitor)) {
              return false;
            }
          }
        } else if (current.is_array()) {
          for (const auto &element : current.as_array()) {
            if (!filter_query_visit(segments, cursor + 1, element, root,
                                    visitor)) {
              return false;
            }
          }
        }

        break;
      case JSONPath::SelectorKind::Index: {
        const auto *entry{std::get_if<JSONPath::SelectorIndex>(&selector)};
        if (current.is_array()) {
          const auto size{static_cast<std::int64_t>(current.as_array().size())};
          const auto index{entry->index < 0 ? entry->index + size
                                            : entry->index};
          if (index >= 0 && index < size &&
              !filter_query_visit(segments, cursor + 1,
                                  current.at(static_cast<std::size_t>(index)),
                                  root, visitor)) {
            return false;
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Slice: {
        const auto *entry{std::get_if<JSONPath::SelectorSlice>(&selector)};
        if (current.is_array()) {
          const auto size{static_cast<std::int64_t>(current.as_array().size())};
          std::int64_t lower{0};
          std::int64_t upper{0};
          if (!slice_bounds(*entry, size, lower, upper)) {
            break;
          }

          if (entry->step > 0) {
            for (auto index{lower}; index < upper; index += entry->step) {
              if (!filter_query_visit(
                      segments, cursor + 1,
                      current.at(static_cast<std::size_t>(index)), root,
                      visitor)) {
                return false;
              }
            }
          } else {
            for (auto index{upper}; index > lower; index += entry->step) {
              if (!filter_query_visit(
                      segments, cursor + 1,
                      current.at(static_cast<std::size_t>(index)), root,
                      visitor)) {
                return false;
              }
            }
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Filter: {
        const auto *entry{std::get_if<JSONPath::SelectorFilter>(&selector)};
        if (current.is_object()) {
          for (const auto &member : current.as_object()) {
            if (filter_matches(entry->expression, member.second, root) &&
                !filter_query_visit(segments, cursor + 1, member.second, root,
                                    visitor)) {
              return false;
            }
          }
        } else if (current.is_array()) {
          for (const auto &element : current.as_array()) {
            if (filter_matches(entry->expression, element, root) &&
                !filter_query_visit(segments, cursor + 1, element, root,
                                    visitor)) {
              return false;
            }
          }
        }

        break;
      }
    }
  }

  if (segment.descendant) {
    if (current.is_object()) {
      for (const auto &member : current.as_object()) {
        if (!filter_query_visit(segments, cursor, member.second, root,
                                visitor)) {
          return false;
        }
      }
    } else if (current.is_array()) {
      for (const auto &element : current.as_array()) {
        if (!filter_query_visit(segments, cursor, element, root, visitor)) {
          return false;
        }
      }
    }
  }

  return true;
}

// RFC 9535 Section 2.3.5.1: a singular query "produces a nodelist containing
// at most one node", so it resolves directly without traversal
inline auto resolve_singular(const JSONPath::FilterQuery &query,
                             const JSON &candidate, const JSON &root)
    -> const JSON * {
  const auto *current{query.relative ? &candidate : &root};
  const auto *segment{query.segments.data()};
  const auto *const segments_end{segment + query.segments.size()};
  for (; segment != segments_end; ++segment) {
    assert(!segment->descendant);
    assert(segment->selectors.size() == 1);
    const auto *selector{segment->selectors.data()};
    if (selector->index() == 0) {
      const auto *entry{std::get_if<JSONPath::SelectorName>(selector)};
      current = current->is_object() ? current->try_at(entry->name, entry->hash)
                                     : nullptr;
    } else {
      const auto *entry{std::get_if<JSONPath::SelectorIndex>(selector)};
      if (current->is_array()) {
        const auto &array{current->as_array()};
        const auto size{static_cast<std::int64_t>(array.size())};
        const auto index{entry->index < 0 ? entry->index + size : entry->index};
        current = index >= 0 && index < size
                      ? first_element(array) + static_cast<std::size_t>(index)
                      : nullptr;
      } else {
        current = nullptr;
      }
    }

    if (current == nullptr) {
      return nullptr;
    }
  }

  return current;
}

auto evaluate_value_function(const JSONPath::FilterFunctionCall &call,
                             const JSON &candidate, const JSON &root,
                             std::optional<JSON> &storage) -> const JSON *;

// The value of a comparable during evaluation is a borrowed pointer, or no
// pointer for the special result Nothing. Computed function results live in
// the caller provided storage
inline auto evaluate_operand(const JSONPath::FilterOperand &operand,
                             const JSON &candidate, const JSON &root,
                             std::optional<JSON> &storage) -> const JSON * {
  switch (operand.value.index()) {
    case 0:
      return std::get_if<JSON>(&operand.value);
    case 1:
      return resolve_singular(
          *std::get_if<JSONPath::FilterQuery>(&operand.value), candidate, root);
    default:
      return evaluate_value_function(
          *std::get_if<JSONPath::FilterFunctionCall>(&operand.value), candidate,
          root, storage);
  }
}

inline auto evaluate_value_function(const JSONPath::FilterFunctionCall &call,
                                    const JSON &candidate, const JSON &root,
                                    std::optional<JSON> &storage)
    -> const JSON * {
  switch (call.function) {
    // RFC 9535 Section 2.4.4: the length function counts string characters,
    // array elements, or object members, and is nothing for anything else
    case JSONPath::FilterFunctionName::Length: {
      std::optional<JSON> argument_storage;
      const auto *target{evaluate_operand(call.arguments.front(), candidate,
                                          root, argument_storage)};
      if (target == nullptr) {
        return nullptr;
      }

      if (target->is_string()) {
        storage.emplace(static_cast<std::int64_t>(
            utf8_codepoint_count(target->to_string())));
        return &storage.value();
      }

      if (target->is_array()) {
        storage.emplace(static_cast<std::int64_t>(target->as_array().size()));
        return &storage.value();
      }

      if (target->is_object()) {
        storage.emplace(static_cast<std::int64_t>(target->as_object().size()));
        return &storage.value();
      }

      return nullptr;
    }
    // RFC 9535 Section 2.4.5: the count function yields the number of nodes
    case JSONPath::FilterFunctionName::Count: {
      const auto &query{
          std::get<JSONPath::FilterQuery>(call.arguments.front().value)};
      std::int64_t count{0};
      filter_query_visit(query.segments, 0, query.relative ? candidate : root,
                         root, [&count](const JSON &) -> bool {
                           count += 1;
                           return true;
                         });
      storage.emplace(count);
      return &storage.value();
    }
    // RFC 9535 Section 2.4.8: the value function yields the value of a
    // single node and nothing otherwise
    case JSONPath::FilterFunctionName::Value: {
      const auto &query{
          std::get<JSONPath::FilterQuery>(call.arguments.front().value)};
      const JSON *single{nullptr};
      std::size_t count{0};
      filter_query_visit(query.segments, 0, query.relative ? candidate : root,
                         root, [&single, &count](const JSON &node) -> bool {
                           single = &node;
                           count += 1;
                           return count < 2;
                         });
      return count == 1 ? single : nullptr;
    }
    default:
      assert(false);
      return nullptr;
  }
}

// RFC 9535 Sections 2.4.6 and 2.4.7: the match function considers the whole
// input while the search function considers any substring, and any argument
// mismatch yields a false outcome rather than an error
inline auto evaluate_logical_function(const JSONPath::FilterFunctionCall &call,
                                      const JSON &candidate, const JSON &root)
    -> bool {
  std::optional<JSON> input_storage;
  const auto *subject{
      evaluate_operand(call.arguments.front(), candidate, root, input_storage)};
  if (subject == nullptr || !subject->is_string()) {
    return false;
  }

  if (std::holds_alternative<JSON>(call.arguments.back().value) &&
      std::get<JSON>(call.arguments.back().value).is_string()) {
    return call.compiled.has_value() &&
           matches(call.compiled.value(), subject->to_string());
  }

  std::optional<JSON> pattern_storage;
  const auto *expression{evaluate_operand(call.arguments.back(), candidate,
                                          root, pattern_storage)};
  if (expression == nullptr || !expression->is_string()) {
    return false;
  }

  const auto compiled{
      to_regex(expression->to_string(),
               call.function == JSONPath::FilterFunctionName::Match
                   ? RegexDialect::IRegexp
                   : RegexDialect::IRegexpSearch)};
  return compiled.has_value() &&
         matches(compiled.value(), subject->to_string());
}

// RFC 9535 Section 2.3.5.2.2: equality is deep JSON equality where numbers
// compare mathematically, and an absent value equals only an absent value
inline auto filter_equals(const JSON *left, const JSON *right) -> bool {
  if (left == nullptr || right == nullptr) {
    return left == nullptr && right == nullptr;
  }

  return *left == *right;
}

// RFC 9535 Section 2.3.5.2.2: ordering applies only when both sides are
// numbers or both sides are strings
inline auto filter_less(const JSON *left, const JSON *right) -> bool {
  if (left == nullptr || right == nullptr) {
    return false;
  }

  // RFC 9535 Section 2.3.5.2.2: ordering applies only when both sides are
  // numbers or both sides are strings. The value comparison itself orders
  // numbers of different representations by exact value
  if ((left->is_number() && right->is_number()) ||
      (left->is_string() && right->is_string())) {
    return *left < *right;
  }

  return false;
}

inline auto filter_compare(const JSONPath::FilterComparison &comparison,
                           const JSON &candidate, const JSON &root) -> bool {
  std::optional<JSON> left_storage;
  std::optional<JSON> right_storage;
  const auto *left{
      evaluate_operand(comparison.left, candidate, root, left_storage)};
  const auto *right{
      evaluate_operand(comparison.right, candidate, root, right_storage)};
  switch (comparison.operation) {
    case JSONPath::FilterComparisonOperator::Equal:
      return filter_equals(left, right);
    case JSONPath::FilterComparisonOperator::NotEqual:
      return !filter_equals(left, right);
    case JSONPath::FilterComparisonOperator::Less:
      return filter_less(left, right);
    case JSONPath::FilterComparisonOperator::LessEqual:
      return filter_less(left, right) || filter_equals(left, right);
    case JSONPath::FilterComparisonOperator::Greater:
      return filter_less(right, left);
    case JSONPath::FilterComparisonOperator::GreaterEqual:
      return filter_less(right, left) || filter_equals(left, right);
  }

  assert(false);
  return false;
}

inline auto filter_matches(const JSONPath::FilterExpression &expression,
                           const JSON &candidate, const JSON &root) -> bool {
  switch (
      static_cast<JSONPath::FilterExpressionKind>(expression.value.index())) {
    case JSONPath::FilterExpressionKind::Comparison:
      return filter_compare(
          *std::get_if<JSONPath::FilterComparison>(&expression.value),
          candidate, root);
    case JSONPath::FilterExpressionKind::Test: {
      const auto &test{*std::get_if<JSONPath::FilterTest>(&expression.value)};
      bool result{false};
      if (test.subject.index() == 0) {
        const auto &query{*std::get_if<JSONPath::FilterQuery>(&test.subject)};
        result = !filter_query_visit(
            query.segments, 0, query.relative ? candidate : root, root,
            [](const JSON &) -> bool { return false; });
      } else {
        result = evaluate_logical_function(
            *std::get_if<JSONPath::FilterFunctionCall>(&test.subject),
            candidate, root);
      }

      return test.negated ? !result : result;
    }
    case JSONPath::FilterExpressionKind::Conjunction: {
      const auto &conjunction{
          *std::get_if<JSONPath::FilterConjunction>(&expression.value)};
      const auto *child{conjunction.children.data()};
      const auto *const children_end{child + conjunction.children.size()};
      for (; child != children_end; ++child) {
        if (!filter_matches(*child, candidate, root)) {
          return false;
        }
      }

      return true;
    }
    case JSONPath::FilterExpressionKind::Disjunction: {
      const auto &disjunction{
          *std::get_if<JSONPath::FilterDisjunction>(&expression.value)};
      const auto *child{disjunction.children.data()};
      const auto *const children_end{child + disjunction.children.size()};
      for (; child != children_end; ++child) {
        if (filter_matches(*child, candidate, root)) {
          return true;
        }
      }

      return false;
    }
    case JSONPath::FilterExpressionKind::Negation:
      return !filter_matches(
          std::get_if<JSONPath::FilterNegation>(&expression.value)
              ->children.front(),
          candidate, root);
  }

  assert(false);
  return false;
}

auto evaluate_segments(const std::vector<JSONPath::Segment> &segments,
                       const std::size_t cursor, const JSON &current,
                       EvaluationState &state) -> void;

inline auto evaluate_selectors(const std::vector<JSONPath::Segment> &segments,
                               const std::size_t cursor, const JSON &current,
                               EvaluationState &state) -> void {
  const auto &segment{*(segments.data() + cursor)};
  const auto type{current.type()};
  const auto *selector{segment.selectors.data()};
  const auto *const selectors_end{selector + segment.selectors.size()};
  for (; selector != selectors_end; ++selector) {
    switch (static_cast<JSONPath::SelectorKind>(selector->index())) {
      case JSONPath::SelectorKind::Name: {
        const auto *entry{std::get_if<JSONPath::SelectorName>(selector)};
        if (type == JSON::Type::Object) {
          const auto *child{current.try_at(entry->name, entry->hash)};
          if (child != nullptr) {
            state.frames.push_property(entry->name, entry->hash);
            evaluate_segments(segments, cursor + 1, *child, state);
            state.frames.pop();
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Wildcard:
        if (type == JSON::Type::Object) {
          const auto &object{current.as_object()};
          const auto *member{first_member(object)};
          const auto *const members_end{member + object.size()};
          for (; member != members_end; ++member) {
            state.frames.push_property(member->first, member->hash);
            evaluate_segments(segments, cursor + 1, member->second, state);
            state.frames.pop();
          }
        } else if (type == JSON::Type::Array) {
          const auto &array{current.as_array()};
          const auto *element{first_element(array)};
          const auto *const elements_end{element + array.size()};
          for (std::size_t index{0}; element != elements_end;
               ++element, ++index) {
            state.frames.push_index(index);
            evaluate_segments(segments, cursor + 1, *element, state);
            state.frames.pop();
          }
        }

        break;
      case JSONPath::SelectorKind::Index: {
        const auto *entry{std::get_if<JSONPath::SelectorIndex>(selector)};
        if (type == JSON::Type::Array) {
          const auto &array{current.as_array()};
          const auto size{static_cast<std::int64_t>(array.size())};
          const auto index{entry->index < 0 ? entry->index + size
                                            : entry->index};
          if (index >= 0 && index < size) {
            state.frames.push_index(static_cast<std::size_t>(index));
            evaluate_segments(
                segments, cursor + 1,
                *(first_element(array) + static_cast<std::size_t>(index)),
                state);
            state.frames.pop();
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Slice: {
        const auto *entry{std::get_if<JSONPath::SelectorSlice>(selector)};
        if (type == JSON::Type::Array) {
          const auto &array{current.as_array()};
          const auto size{static_cast<std::int64_t>(array.size())};
          std::int64_t lower{0};
          std::int64_t upper{0};
          if (!slice_bounds(*entry, size, lower, upper)) {
            break;
          }

          const auto *const base{first_element(array)};
          if (entry->step > 0) {
            for (auto index{lower}; index < upper; index += entry->step) {
              state.frames.push_index(static_cast<std::size_t>(index));
              evaluate_segments(segments, cursor + 1,
                                *(base + static_cast<std::size_t>(index)),
                                state);
              state.frames.pop();
            }
          } else {
            for (auto index{upper}; index > lower; index += entry->step) {
              state.frames.push_index(static_cast<std::size_t>(index));
              evaluate_segments(segments, cursor + 1,
                                *(base + static_cast<std::size_t>(index)),
                                state);
              state.frames.pop();
            }
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Filter: {
        const auto *entry{std::get_if<JSONPath::SelectorFilter>(selector)};
        if (type == JSON::Type::Object) {
          const auto &object{current.as_object()};
          const auto *member{first_member(object)};
          const auto *const members_end{member + object.size()};
          for (; member != members_end; ++member) {
            if (filter_matches(entry->expression, member->second,
                               *state.root)) {
              state.frames.push_property(member->first, member->hash);
              evaluate_segments(segments, cursor + 1, member->second, state);
              state.frames.pop();
            }
          }
        } else if (type == JSON::Type::Array) {
          const auto &array{current.as_array()};
          const auto *element{first_element(array)};
          const auto *const elements_end{element + array.size()};
          for (std::size_t index{0}; element != elements_end;
               ++element, ++index) {
            if (filter_matches(entry->expression, *element, *state.root)) {
              state.frames.push_index(index);
              evaluate_segments(segments, cursor + 1, *element, state);
              state.frames.pop();
            }
          }
        }

        break;
      }
    }
  }
}

// RFC 9535 Section 2.5.2.2: a descendant segment visits the input node and
// every descendant in depth-first document order, applying its selectors at
// each visited node
inline auto evaluate_descendant(const std::vector<JSONPath::Segment> &segments,
                                const std::size_t cursor, const JSON &current,
                                EvaluationState &state) -> void {
  evaluate_selectors(segments, cursor, current, state);
  const auto type{current.type()};
  if (type == JSON::Type::Object) {
    const auto &object{current.as_object()};
    const auto *member{first_member(object)};
    const auto *const members_end{member + object.size()};
    for (; member != members_end; ++member) {
      state.frames.push_property(member->first, member->hash);
      evaluate_descendant(segments, cursor, member->second, state);
      state.frames.pop();
    }
  } else if (type == JSON::Type::Array) {
    const auto &array{current.as_array()};
    const auto *element{first_element(array)};
    const auto *const elements_end{element + array.size()};
    for (std::size_t index{0}; element != elements_end; ++element, ++index) {
      state.frames.push_index(index);
      evaluate_descendant(segments, cursor, *element, state);
      state.frames.pop();
    }
  }
}

inline auto evaluate_segments(const std::vector<JSONPath::Segment> &segments,
                              const std::size_t cursor, const JSON &current,
                              EvaluationState &state) -> void {
  if (cursor == segments.size()) {
    state.frames.materialize(state.location);
    (*state.callback)(current, state.location);
    return;
  }

  if (segments[cursor].descendant) {
    evaluate_descendant(segments, cursor, current, state);
  } else {
    evaluate_selectors(segments, cursor, current, state);
  }
}

} // namespace

JSONPath::JSONPath(const JSON::StringView expression)
    : query_{parse_jsonpath(expression)} {}

auto JSONPath::evaluate(const JSON &document, const Callback &callback) const
    -> void {
  EvaluationState state{.root = &document,
                        .callback = &callback,
                        .frames = LocationStack{},
                        .location = WeakPointer{}};
  state.location.reserve(32);
  evaluate_segments(this->query_.segments, 0, document, state);
}

auto JSONPath::normalize(const WeakPointer &location) -> JSON::String {
  JSON::String result{"$"};
  for (const auto &token : location) {
    if (token.is_property()) {
      result += "['";
      for (const char character : token.to_property()) {
        // RFC 9535 Section 2.7: "normal-escapable" covers the apostrophe,
        // the backslash, and the control characters with a two character
        // JSON escape, while "normal-hexchar" covers the remaining control
        // characters in lowercase hexadecimal form
        switch (character) {
          case '\x08':
            result += "\\b";
            break;
          case '\x0C':
            result += "\\f";
            break;
          case '\n':
            result += "\\n";
            break;
          case '\r':
            result += "\\r";
            break;
          case '\t':
            result += "\\t";
            break;
          case '\'':
            result += "\\'";
            break;
          case '\\':
            result += "\\\\";
            break;
          default:
            if (static_cast<unsigned char>(character) < 0x20) {
              result += "\\u00";
              result += bytes_to_hex(JSON::StringView{&character, 1});
            } else {
              result += character;
            }
        }
      }

      result += "']";
    } else {
      result += '[';
      result += std::to_string(token.to_index());
      result += ']';
    }
  }

  return result;
}

} // namespace sourcemeta::core
