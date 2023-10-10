#ifndef SOURCEMETA_JSONTOOLKIT_URI_H_
#define SOURCEMETA_JSONTOOLKIT_URI_H_

#if defined(__EMSCRIPTEN__) || defined(__Unikraft__)
#define SOURCEMETA_JSONTOOLKIT_URI_EXPORT
#else
#include "uri_export.h"
#endif

#include <sourcemeta/jsontoolkit/uri_error.h>

#include <memory>      // std::unique_ptr
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::jsontoolkit {

class SOURCEMETA_JSONTOOLKIT_URI_EXPORT URI {
public:
  URI(std::string input);
  ~URI();

  auto is_absolute() const noexcept -> bool;
  auto host() const -> std::optional<std::string_view>;
  auto recompose() const -> std::string;

  // We return a string for performance reasons, which the
  // user can always parse back into a URI object if they wish.
  auto resolve_from(const URI &base) const -> std::string;

private:
  // We need to keep the string as the URI structure just
  // points to fragments of it.
  // We keep this as const as this class is immutable
  const std::string data;

  // Use PIMPL idiom to hide `urlparser`
  struct Internal;
  const std::unique_ptr<Internal> internal;
};

} // namespace sourcemeta::jsontoolkit

#endif
