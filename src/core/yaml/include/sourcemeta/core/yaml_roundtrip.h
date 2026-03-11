#ifndef SOURCEMETA_CORE_YAML_ROUNDTRIP_H_
#define SOURCEMETA_CORE_YAML_ROUNDTRIP_H_

#ifndef SOURCEMETA_CORE_YAML_EXPORT
#include <sourcemeta/core/yaml_export.h>
#endif

namespace sourcemeta::core {

/// @ingroup yaml
/// Holds per-node metadata collected during YAML parsing to reproduce the
/// original formatting
class SOURCEMETA_CORE_YAML_EXPORT YAMLRoundTrip {
public:
  auto clear() -> void;
};

} // namespace sourcemeta::core

#endif
