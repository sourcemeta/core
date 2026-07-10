#ifndef SOURCEMETA_CORE_JSONPATH_GRAMMAR_H_
#define SOURCEMETA_CORE_JSONPATH_GRAMMAR_H_

#include <sourcemeta/core/jsonpath.h>

namespace sourcemeta::core {

using JSONPathQuery = JSONPath::Query;
using JSONPathSegment = JSONPath::Segment;
using Selector = JSONPath::Selector;
using SelectorKind = JSONPath::SelectorKind;
using SelectorName = JSONPath::SelectorName;
using SelectorWildcard = JSONPath::SelectorWildcard;
using SelectorIndex = JSONPath::SelectorIndex;
using SelectorSlice = JSONPath::SelectorSlice;
using SelectorFilter = JSONPath::SelectorFilter;
using FilterQuery = JSONPath::FilterQuery;
using FilterOperand = JSONPath::FilterOperand;
using FilterFunctionName = JSONPath::FilterFunctionName;
using FilterFunctionCall = JSONPath::FilterFunctionCall;
using FilterComparisonOperator = JSONPath::FilterComparisonOperator;
using FilterComparison = JSONPath::FilterComparison;
using FilterTest = JSONPath::FilterTest;
using FilterExpression = JSONPath::FilterExpression;
using FilterConjunction = JSONPath::FilterConjunction;
using FilterDisjunction = JSONPath::FilterDisjunction;
using FilterNegation = JSONPath::FilterNegation;

} // namespace sourcemeta::core

#endif
