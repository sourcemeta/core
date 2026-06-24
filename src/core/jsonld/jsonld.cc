#include <sourcemeta/core/jsonld.h>

namespace sourcemeta::core {

auto jsonld_expand(const JSON &, const JSON::StringView, const JSONLDResolver &,
                   const JSONLDVersion) -> JSON {
  // TODO: Implement the JSON-LD 1.1 expansion algorithm
  return JSON::make_array();
}

auto jsonld_expand(const JSON &input, const JSON &,
                   const JSON::StringView base_iri,
                   const JSONLDResolver &resolver, const JSONLDVersion version)
    -> JSON {
  return jsonld_expand(input, base_iri, resolver, version);
}

} // namespace sourcemeta::core
