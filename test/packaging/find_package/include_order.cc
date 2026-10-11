// The system library wraps the platform headers so that they do not reach the
// rest of this surface. Including it ahead of the modules built on top of it is
// what proves it, as a platform header defines macros that collide with
// ordinary identifiers, such as the request methods of the HTTP library and the
// router of the URI Template one
// clang-format off
#include <sourcemeta/core/system.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/uritemplate.h>
// clang-format on

auto core_include_order() -> sourcemeta::core::HTTPMethod {
  return sourcemeta::core::HTTPMethod::DELETE;
}
