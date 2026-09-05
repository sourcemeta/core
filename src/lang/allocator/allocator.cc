#include <sourcemeta/core/allocator.h>

namespace sourcemeta::core {

auto allocator_name() -> std::string_view {
#ifdef SOURCEMETA_CORE_ALLOCATOR_MIMALLOC
  return "mimalloc";
#else
  return "system";
#endif
}

} // namespace sourcemeta::core
