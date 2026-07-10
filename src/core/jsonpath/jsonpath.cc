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

struct EvaluationState {
  const JSON *root;
  const JSONPath::Callback *callback;
  WeakPointer location;
};

// The value of a comparable during evaluation: absent, borrowed from the
// document or the compiled query, or computed by a function
using FilterValue = std::variant<std::monostate, const JSON *, JSON>;

auto filter_value_pointer(const FilterValue &value) -> const JSON * {
  switch (value.index()) {
    case 0:
      return nullptr;
    case 1:
      return *std::get_if<const JSON *>(&value);
    default:
      return std::get_if<JSON>(&value);
  }
}

auto filter_matches(const JSONPath::FilterExpression &expression,
                    const JSON &candidate, const JSON &root) -> bool;

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

  const auto &segment{segments[cursor]};
  for (const auto &selector : segment.selectors) {
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
        if (current.is_object()) {
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
  for (const auto &segment : query.segments) {
    assert(!segment.descendant);
    assert(segment.selectors.size() == 1);
    const auto &selector{segment.selectors.front()};
    if (std::holds_alternative<JSONPath::SelectorName>(selector)) {
      const auto *entry{std::get_if<JSONPath::SelectorName>(&selector)};
      current = current->is_object() ? current->try_at(entry->name, entry->hash)
                                     : nullptr;
    } else {
      const auto *entry{std::get_if<JSONPath::SelectorIndex>(&selector)};
      if (current->is_array()) {
        const auto size{static_cast<std::int64_t>(current->as_array().size())};
        const auto index{entry->index < 0 ? entry->index + size : entry->index};
        current = index >= 0 && index < size
                      ? &current->at(static_cast<std::size_t>(index))
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
                             const JSON &candidate, const JSON &root)
    -> FilterValue;

inline auto evaluate_operand(const JSONPath::FilterOperand &operand,
                             const JSON &candidate, const JSON &root)
    -> FilterValue {
  if (std::holds_alternative<JSON>(operand.value)) {
    return FilterValue{&std::get<JSON>(operand.value)};
  }

  if (std::holds_alternative<JSONPath::FilterQuery>(operand.value)) {
    const auto *result{resolve_singular(
        std::get<JSONPath::FilterQuery>(operand.value), candidate, root)};
    return result == nullptr ? FilterValue{} : FilterValue{result};
  }

  return evaluate_value_function(
      std::get<JSONPath::FilterFunctionCall>(operand.value), candidate, root);
}

inline auto evaluate_value_function(const JSONPath::FilterFunctionCall &call,
                                    const JSON &candidate, const JSON &root)
    -> FilterValue {
  switch (call.function) {
    // RFC 9535 Section 2.4.4: the length function counts string characters,
    // array elements, or object members, and is nothing for anything else
    case JSONPath::FilterFunctionName::Length: {
      const auto value{
          evaluate_operand(call.arguments.front(), candidate, root)};
      const auto *target{filter_value_pointer(value)};
      if (target == nullptr) {
        return FilterValue{};
      }

      if (target->is_string()) {
        return FilterValue{JSON{static_cast<std::int64_t>(
            utf8_codepoint_count(target->to_string()))}};
      }

      if (target->is_array()) {
        return FilterValue{
            JSON{static_cast<std::int64_t>(target->as_array().size())}};
      }

      if (target->is_object()) {
        return FilterValue{
            JSON{static_cast<std::int64_t>(target->as_object().size())}};
      }

      return FilterValue{};
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
      return FilterValue{JSON{count}};
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
      return count == 1 ? FilterValue{single} : FilterValue{};
    }
    default:
      assert(false);
      return FilterValue{};
  }
}

// RFC 9535 Sections 2.4.6 and 2.4.7: the match function considers the whole
// input while the search function considers any substring, and any argument
// mismatch yields a false outcome rather than an error
inline auto evaluate_logical_function(const JSONPath::FilterFunctionCall &call,
                                      const JSON &candidate, const JSON &root)
    -> bool {
  const auto input{evaluate_operand(call.arguments.front(), candidate, root)};
  const auto *subject{filter_value_pointer(input)};
  if (subject == nullptr || !subject->is_string()) {
    return false;
  }

  if (std::holds_alternative<JSON>(call.arguments.back().value) &&
      std::get<JSON>(call.arguments.back().value).is_string()) {
    return call.compiled.has_value() &&
           matches(call.compiled.value(), subject->to_string());
  }

  const auto pattern{evaluate_operand(call.arguments.back(), candidate, root)};
  const auto *expression{filter_value_pointer(pattern)};
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
  const auto left{evaluate_operand(comparison.left, candidate, root)};
  const auto right{evaluate_operand(comparison.right, candidate, root)};
  const auto *left_value{filter_value_pointer(left)};
  const auto *right_value{filter_value_pointer(right)};
  switch (comparison.operation) {
    case JSONPath::FilterComparisonOperator::Equal:
      return filter_equals(left_value, right_value);
    case JSONPath::FilterComparisonOperator::NotEqual:
      return !filter_equals(left_value, right_value);
    case JSONPath::FilterComparisonOperator::Less:
      return filter_less(left_value, right_value);
    case JSONPath::FilterComparisonOperator::LessEqual:
      return filter_less(left_value, right_value) ||
             filter_equals(left_value, right_value);
    case JSONPath::FilterComparisonOperator::Greater:
      return filter_less(right_value, left_value);
    case JSONPath::FilterComparisonOperator::GreaterEqual:
      return filter_less(right_value, left_value) ||
             filter_equals(left_value, right_value);
  }

  assert(false);
  return false;
}

inline auto filter_matches(const JSONPath::FilterExpression &expression,
                           const JSON &candidate, const JSON &root) -> bool {
  if (std::holds_alternative<JSONPath::FilterComparison>(expression.value)) {
    return filter_compare(
        std::get<JSONPath::FilterComparison>(expression.value), candidate,
        root);
  }

  if (std::holds_alternative<JSONPath::FilterTest>(expression.value)) {
    const auto &test{std::get<JSONPath::FilterTest>(expression.value)};
    bool result{false};
    if (std::holds_alternative<JSONPath::FilterQuery>(test.subject)) {
      const auto &query{std::get<JSONPath::FilterQuery>(test.subject)};
      result = !filter_query_visit(query.segments, 0,
                                   query.relative ? candidate : root, root,
                                   [](const JSON &) -> bool { return false; });
    } else {
      result = evaluate_logical_function(
          std::get<JSONPath::FilterFunctionCall>(test.subject), candidate,
          root);
    }

    return test.negated ? !result : result;
  }

  if (std::holds_alternative<JSONPath::FilterConjunction>(expression.value)) {
    const auto &conjunction{
        std::get<JSONPath::FilterConjunction>(expression.value)};
    for (const auto &child : conjunction.children) {
      if (!filter_matches(child, candidate, root)) {
        return false;
      }
    }

    return true;
  }

  if (std::holds_alternative<JSONPath::FilterDisjunction>(expression.value)) {
    const auto &disjunction{
        std::get<JSONPath::FilterDisjunction>(expression.value)};
    for (const auto &child : disjunction.children) {
      if (filter_matches(child, candidate, root)) {
        return true;
      }
    }

    return false;
  }

  return !filter_matches(
      std::get<JSONPath::FilterNegation>(expression.value).children.front(),
      candidate, root);
}

auto evaluate_segments(const std::vector<JSONPath::Segment> &segments,
                       const std::size_t cursor, const JSON &current,
                       EvaluationState &state) -> void;

inline auto evaluate_selectors(const std::vector<JSONPath::Segment> &segments,
                               const std::size_t cursor, const JSON &current,
                               EvaluationState &state) -> void {
  for (const auto &selector : segments[cursor].selectors) {
    switch (static_cast<JSONPath::SelectorKind>(selector.index())) {
      case JSONPath::SelectorKind::Name: {
        const auto *entry{std::get_if<JSONPath::SelectorName>(&selector)};
        if (current.is_object()) {
          const auto *child{current.try_at(entry->name, entry->hash)};
          if (child != nullptr) {
            state.location.emplace_back(std::cref(entry->name), entry->hash);
            evaluate_segments(segments, cursor + 1, *child, state);
            state.location.pop_back();
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Wildcard:
        if (current.is_object()) {
          for (const auto &member : current.as_object()) {
            state.location.emplace_back(std::cref(member.first), member.hash);
            evaluate_segments(segments, cursor + 1, member.second, state);
            state.location.pop_back();
          }
        } else if (current.is_array()) {
          const auto size{current.as_array().size()};
          for (std::size_t index = 0; index < size; ++index) {
            state.location.emplace_back(index);
            evaluate_segments(segments, cursor + 1, current.at(index), state);
            state.location.pop_back();
          }
        }

        break;
      case JSONPath::SelectorKind::Index: {
        const auto *entry{std::get_if<JSONPath::SelectorIndex>(&selector)};
        if (current.is_array()) {
          const auto size{static_cast<std::int64_t>(current.as_array().size())};
          const auto index{entry->index < 0 ? entry->index + size
                                            : entry->index};
          if (index >= 0 && index < size) {
            state.location.emplace_back(static_cast<std::size_t>(index));
            evaluate_segments(segments, cursor + 1,
                              current.at(static_cast<std::size_t>(index)),
                              state);
            state.location.pop_back();
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
              state.location.emplace_back(static_cast<std::size_t>(index));
              evaluate_segments(segments, cursor + 1,
                                current.at(static_cast<std::size_t>(index)),
                                state);
              state.location.pop_back();
            }
          } else {
            for (auto index{upper}; index > lower; index += entry->step) {
              state.location.emplace_back(static_cast<std::size_t>(index));
              evaluate_segments(segments, cursor + 1,
                                current.at(static_cast<std::size_t>(index)),
                                state);
              state.location.pop_back();
            }
          }
        }

        break;
      }
      case JSONPath::SelectorKind::Filter: {
        const auto *entry{std::get_if<JSONPath::SelectorFilter>(&selector)};
        if (current.is_object()) {
          for (const auto &member : current.as_object()) {
            if (filter_matches(entry->expression, member.second, *state.root)) {
              state.location.emplace_back(std::cref(member.first), member.hash);
              evaluate_segments(segments, cursor + 1, member.second, state);
              state.location.pop_back();
            }
          }
        } else if (current.is_array()) {
          const auto size{current.as_array().size()};
          for (std::size_t index = 0; index < size; ++index) {
            if (filter_matches(entry->expression, current.at(index),
                               *state.root)) {
              state.location.emplace_back(index);
              evaluate_segments(segments, cursor + 1, current.at(index), state);
              state.location.pop_back();
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
  if (current.is_object()) {
    for (const auto &member : current.as_object()) {
      state.location.emplace_back(std::cref(member.first), member.hash);
      evaluate_descendant(segments, cursor, member.second, state);
      state.location.pop_back();
    }
  } else if (current.is_array()) {
    const auto size{current.as_array().size()};
    for (std::size_t index = 0; index < size; ++index) {
      state.location.emplace_back(index);
      evaluate_descendant(segments, cursor, current.at(index), state);
      state.location.pop_back();
    }
  }
}

inline auto evaluate_segments(const std::vector<JSONPath::Segment> &segments,
                              const std::size_t cursor, const JSON &current,
                              EvaluationState &state) -> void {
  if (cursor == segments.size()) {
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
  EvaluationState state{
      .root = &document, .callback = &callback, .location = WeakPointer{}};
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
