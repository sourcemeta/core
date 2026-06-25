#ifndef SOURCEMETA_CORE_JSONLD_ALGORITHMS_H_
#define SOURCEMETA_CORE_JSONLD_ALGORITHMS_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

#include <functional> // std::less
#include <map>        // std::map
#include <memory>     // std::shared_ptr
#include <optional>   // std::optional
#include <vector>     // std::vector

namespace sourcemeta::core {

struct TermDefinition {
  std::optional<JSON::String> iri;
  std::optional<JSON::String> type_mapping;
  std::vector<JSON::String> container;
  std::optional<JSON::String> language;
  bool has_language{false};
  std::optional<JSON::String> direction;
  bool has_direction{false};
  std::optional<JSON> context;
  std::optional<JSON::String> context_base;
  std::optional<JSON::String> index;
  bool reverse{false};
  bool prefix{false};
  bool is_protected{false};
};

struct ActiveContext {
  std::map<JSON::String, TermDefinition, std::less<>> terms;
  std::optional<JSON::String> base;
  std::optional<JSON::String> vocabulary;
  std::optional<JSON::String> default_language;
  std::optional<JSON::String> default_direction;
  std::shared_ptr<ActiveContext> previous;
};

// The mutable state shared by the expansion algorithms for the duration of a
// single top-level expansion.
struct ExpansionState {
  // Used to load remote contexts. The chain detects recursive inclusion.
  const JSONLDResolver *resolver{nullptr};
  std::vector<JSON::String> remote_context_chain;
  std::optional<JSON::String> document_base;
  // When a scoped context is processed after the fact, remote references in it
  // resolve against the URL of the document that defined the term.
  std::optional<JSON::String> context_base_override;
  // Protected-term state for the context currently being processed.
  bool context_protected{false};
  bool protected_override{false};
  bool processing_1_0{false};

  // Remote context references resolve against the URL of the document that
  // contains them: the current remote context if any, otherwise the input
  // document. This is distinct from the active context base, which @base
  // mutates.
  [[nodiscard]] auto context_resolution_base() const
      -> std::optional<JSON::String> {
    if (!this->remote_context_chain.empty()) {
      return this->remote_context_chain.back();
    }
    if (this->context_base_override.has_value()) {
      return this->context_base_override;
    }
    return this->document_base;
  }
};

// Tracks, while a context is being processed, which terms have been fully
// defined (true) versus are still being defined (false, used to detect cycles).
using DefinedTerms = std::map<JSON::String, bool>;

// IRI Expansion (JSON-LD 1.1 API Section 5.2)
auto expand_iri(ExpansionState &state, ActiveContext &active_context,
                const JSON::String &value, const bool document_relative,
                const bool vocabulary, const JSON *const local_context,
                DefinedTerms *const defined, const WeakPointer &context_pointer)
    -> std::optional<JSON::String>;

// Create Term Definition (JSON-LD 1.1 API Section 5.1.1)
auto create_term_definition(ExpansionState &state,
                            ActiveContext &active_context,
                            const JSON &local_context, const JSON::String &term,
                            DefinedTerms &defined,
                            const WeakPointer &context_pointer) -> void;

// Context Processing (JSON-LD 1.1 API Section 5.1)
auto process_context(ExpansionState &state, ActiveContext &active_context,
                     const JSON &local_context, const WeakPointer &pointer,
                     const bool propagate = true) -> void;

// Value Expansion (JSON-LD 1.1 API Section 5.3.3)
auto expand_value(ExpansionState &state, ActiveContext &active_context,
                  const std::optional<JSON::String> &active_property,
                  const JSON &value) -> JSON;

// Expansion (JSON-LD 1.1 API Section 5.1.2)
auto expand(ExpansionState &state, ActiveContext &active_context,
            const std::optional<JSON::String> &active_property,
            const JSON &element, const WeakPointer &pointer) -> JSON;

} // namespace sourcemeta::core

#endif
