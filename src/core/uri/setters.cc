#include <sourcemeta/core/uri.h>

#include "escaping.h"
#include "normalize.h"

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {

auto apply_leading_slash_transform(std::optional<std::string> parsed_path,
                                   const bool needs_leading_slash)
    -> std::optional<std::string> {
  if (!parsed_path.has_value()) {
    return parsed_path;
  }

  const auto &path_value = parsed_path.value();

  if (needs_leading_slash) {
    if (path_value.empty() || !path_value.starts_with("/")) {
      return "/" + path_value;
    }
  }

  return parsed_path;
}

auto normalize_fragment(const std::string_view input) -> std::string {
  if (input.empty()) {
    return "";
  }

  // Strip leading '#' and store raw value
  return std::string{input.starts_with('#') ? input.substr(1) : input};
}

} // namespace

namespace sourcemeta::core {

auto URI::path(const std::string &path) -> URI & {
  if (path.empty()) {
    this->path_ = std::nullopt;
    return *this;
  }

  if (path.starts_with(".") && this->is_absolute()) {
    throw URIError{"You cannot set a relative path to an absolute URI"};
  }

  // Parse the path string to extract its normalized value
  const auto parsed_path = URI{path}.path_;

  // Determine if this URI needs a leading slash
  // (URIs with scheme/authority need leading slash, except URNs/tags/mailto)
  const auto needs_leading_slash =
      (!this->is_urn() && !this->is_tag() && !this->is_mailto() &&
       this->scheme_.has_value()) ||
      this->port_.has_value() || this->host_.has_value();

  this->path_ = apply_leading_slash_transform(parsed_path, needs_leading_slash);
  return *this;
}

auto URI::path(std::string &&path) -> URI & {
  if (path.empty()) {
    this->path_ = std::nullopt;
    return *this;
  }

  if (path.starts_with(".") && this->is_absolute()) {
    throw URIError{"You cannot set a relative path to an absolute URI"};
  }

  // Parse the path string to extract its normalized value
  const auto parsed_path = URI{path}.path_;

  // Determine if this URI needs a leading slash
  // (URIs with scheme/authority need leading slash, except URNs/tags/mailto)
  const auto needs_leading_slash =
      (!this->is_urn() && !this->is_tag() && !this->is_mailto() &&
       this->scheme_.has_value()) ||
      this->port_.has_value() || this->host_.has_value();

  this->path_ = apply_leading_slash_transform(parsed_path, needs_leading_slash);
  return *this;
}

auto URI::append_path(const std::string &path) -> URI & {
  if (path.empty()) {
    return *this;
  }

  const URI reference{path};
  if (reference.scheme_.has_value() || reference.userinfo_.has_value() ||
      reference.host_.has_value() || reference.port_.has_value()) {
    throw URIError{
        "append_path argument must be a path, not a URI with scheme or "
        "authority"};
  }

  if (reference.query_.has_value() || reference.fragment_.has_value()) {
    throw URIError{
        "append_path argument must be a path, not a URI reference with a query "
        "or fragment"};
  }

  if (!reference.path_.has_value()) {
    return *this;
  }

  const auto &reference_path = reference.path_.value();

  const auto needs_leading_slash =
      (!this->is_urn() && !this->is_tag() && !this->is_mailto() &&
       this->scheme_.has_value()) ||
      this->port_.has_value() || this->host_.has_value();

  std::string merged_path;
  if (this->path_.has_value()) {
    const auto &current_path = this->path_.value();
    const auto current_ends_with_slash = current_path.ends_with('/');
    const auto reference_starts_with_slash = reference_path.starts_with('/');

    if (current_ends_with_slash && reference_starts_with_slash) {
      merged_path = current_path;
      merged_path.append(reference_path, 1);
    } else if (!current_ends_with_slash && !reference_starts_with_slash) {
      merged_path = current_path;
      merged_path += '/';
      merged_path += reference_path;
    } else {
      merged_path = current_path;
      merged_path += reference_path;
    }
  } else if (needs_leading_slash && !reference_path.starts_with('/')) {
    merged_path = "/";
    merged_path += reference_path;
  } else {
    merged_path = reference_path;
  }

  normalize_path(merged_path);
  if (merged_path.empty()) {
    this->path_ = std::nullopt;
  } else {
    this->path_ = std::move(merged_path);
  }
  return *this;
}

auto URI::extension(std::string &&extension) -> URI & {
  if (!this->path_.has_value()) {
    return *this;
  }

  auto &path = this->path_.value();
  if (path.empty() || path.ends_with('/')) {
    return *this;
  }

  // Find the last dot after the last slash (if any)
  const auto last_slash_pos = path.find_last_of('/');
  const auto last_dot_pos = path.find_last_of('.');

  // Only consider the dot if it's after the last slash (or there's no slash)
  const auto has_extension =
      last_dot_pos != std::string::npos &&
      (last_slash_pos == std::string::npos || last_dot_pos > last_slash_pos);

  // Remove existing extension if present
  if (has_extension) {
    path.erase(last_dot_pos);
  }

  // Add new extension if not empty
  if (!extension.empty()) {
    // Strip leading dot from extension if present
    if (extension.starts_with('.')) {
      path += extension;
    } else {
      path += '.';
      path += extension;
    }
  }

  return *this;
}

auto URI::fragment(const std::string_view fragment) -> URI & {
  this->fragment_ = normalize_fragment(std::string{fragment});
  return *this;
}

auto URI::query(const std::string_view query) -> URI & {
  if (query.empty()) {
    this->query_ = std::nullopt;
    return *this;
  }

  std::string value{query.starts_with('?') ? query.substr(1) : query};
  uri_unescape_unreserved_inplace(value);
  this->query_ = std::move(value);
  return *this;
}

auto URI::userinfo(const std::string_view userinfo) -> URI & {
  if (userinfo.empty()) {
    this->userinfo_ = std::nullopt;
    return *this;
  }

  std::string value{userinfo};
  uri_unescape_unreserved_inplace(value);
  this->userinfo_ = std::move(value);
  return *this;
}

} // namespace sourcemeta::core
