#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <optional> // std::nullopt
#include <utility>  // std::move

namespace sourcemeta::core {

// Run the expansion algorithm on a top-level input document and normalise the
// result into the expanded document form (JSON-LD 1.1 API Section 5.1).
auto run_expansion(ExpansionState &state, ActiveContext &active_context,
                   const JSON &input) -> JSON {
  auto expanded{
      expand(state, active_context, std::nullopt, input, empty_weak_pointer)};

  // A top-level map containing only @graph is replaced by its value.
  if (expanded.is_object() && expanded.object_size() == 1 &&
      expanded.defines(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH)) {
    expanded = expanded.at(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH);
  }

  if (expanded.is_object()) {
    if (expanded.empty() || (expanded.object_size() == 1 &&
                             expanded.defines(KEYWORD_ID, KEYWORD_ID_HASH))) {
      return JSON::make_array();
    }
    auto result{JSON::make_array()};
    result.push_back(std::move(expanded));
    return result;
  }

  if (!expanded.is_array()) {
    return JSON::make_array();
  }

  return expanded;
}

// Set up the shared expansion state and initial active context from the public
// entry-point arguments.
auto initialise_expansion(const JSONLDResolver &resolver,
                          const JSON::StringView base_iri,
                          const JSONLDVersion version, ExpansionState &state,
                          ActiveContext &active_context) -> void {
  state.resolver = &resolver;
  state.processing_1_0 = version == JSONLDVersion::V1_0;
  if (!base_iri.empty()) {
    active_context.base = JSON::String{base_iri};
    state.document_base = JSON::String{base_iri};
  }
}

} // namespace sourcemeta::core

namespace sourcemeta::core {

auto jsonld_expand(const JSON &input, const JSON::StringView base_iri,
                   const JSONLDResolver &resolver, const JSONLDVersion version)
    -> JSON {
  ExpansionState state;
  ActiveContext active_context;
  initialise_expansion(resolver, base_iri, version, state, active_context);
  return run_expansion(state, active_context, input);
}

auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView base_iri,
                   const JSONLDResolver &resolver, const JSONLDVersion version)
    -> JSON {
  ExpansionState state;
  ActiveContext active_context;
  initialise_expansion(resolver, base_iri, version, state, active_context);
  const auto &context{
      expand_context.is_object() &&
              expand_context.defines(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
          ? expand_context.at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
          : expand_context};
  // The external expansion context is not part of the input document, so its
  // errors carry an empty pointer.
  process_context(state, active_context, context, empty_weak_pointer);
  return run_expansion(state, active_context, input);
}

} // namespace sourcemeta::core
