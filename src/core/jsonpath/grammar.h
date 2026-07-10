#ifndef SOURCEMETA_CORE_JSONPATH_GRAMMAR_H_
#define SOURCEMETA_CORE_JSONPATH_GRAMMAR_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/regex.h>

#include <cstdint>  // std::int64_t, std::uint8_t
#include <memory>   // std::unique_ptr
#include <optional> // std::optional
#include <variant>  // std::variant
#include <vector>   // std::vector

namespace sourcemeta::core {

struct JSONPathSegment;
struct FilterFunctionCall;

// RFC 9535 Section 2.3.1: "A name selector '<name>' selects at most one
// object member value". The decoded member name is owned here rather than
// viewed from the query source, as string literal escapes decode at parse
// time and result locations need a durable string to reference. The hash is
// precomputed once so evaluation resolves members without hashing
struct SelectorName {
  JSON::String name;
  JSON::Object::hash_type hash;
};

struct SelectorWildcard {};

// RFC 9535 Section 2.3.3: "A non-negative index-selector applies to arrays
// only" and "a negative index-selector counts from the array end"
struct SelectorIndex {
  std::int64_t index;
};

// RFC 9535 Section 2.3.4.2.2: the default start and end depend on the sign
// of the step, so their absence must be preserved
struct SelectorSlice {
  std::optional<std::int64_t> start;
  std::optional<std::int64_t> end;
  std::int64_t step;
};

// RFC 9535 Section 2.3.5.1: a filter query embedded in a logical expression,
// evaluated relative to the candidate node or to the query root. Whether the
// query is singular is determined at parse time, as comparisons and value
// typed function arguments only admit singular queries
struct FilterQuery {
  bool relative;
  std::vector<JSONPathSegment> segments;
  bool singular;
};

enum class FilterFunctionName : std::uint8_t {
  Length,
  Count,
  Match,
  Search,
  Value
};

enum class FilterComparisonOperator : std::uint8_t {
  Equal,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual
};

// A comparable or function argument per RFC 9535 Sections 2.3.5.1 and 2.4.3:
// a literal, a query, or a nested function call
using FilterOperand =
    std::variant<JSON, FilterQuery, std::unique_ptr<FilterFunctionCall>>;

struct FilterFunctionCall {
  FilterFunctionName function;
  std::vector<FilterOperand> arguments;
  // A string literal pattern of the match and search functions compiles at
  // parse time, and stays empty when the pattern is not a valid RFC 9485
  // expression, which evaluates as a false outcome rather than an error
  std::optional<Regex> compiled;
};

struct FilterComparison {
  FilterOperand left;
  FilterComparisonOperator operation;
  FilterOperand right;
};

struct FilterTest {
  bool negated;
  std::variant<FilterQuery, std::unique_ptr<FilterFunctionCall>> subject;
};

struct FilterExpression;

struct FilterConjunction {
  std::vector<FilterExpression> children;
};

struct FilterDisjunction {
  std::vector<FilterExpression> children;
};

struct FilterNegation {
  std::unique_ptr<FilterExpression> child;
};

struct FilterExpression {
  std::variant<FilterComparison, FilterTest, FilterConjunction,
               FilterDisjunction, FilterNegation>
      value;
};

struct SelectorFilter {
  FilterExpression expression;
};

using Selector = std::variant<SelectorName, SelectorWildcard, SelectorIndex,
                              SelectorSlice, SelectorFilter>;

// For fast internal dispatching. It must stay in sync with the variant above
enum class SelectorKind : std::uint8_t {
  Name = 0,
  Wildcard,
  Index,
  Slice,
  Filter
};

struct JSONPathSegment {
  bool descendant;
  std::vector<Selector> selectors;
};

struct JSONPathQuery {
  std::vector<JSONPathSegment> segments;
};

} // namespace sourcemeta::core

#endif
