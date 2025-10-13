#include <uriparser/Uri.h>

#include <sourcemeta/core/uri.h>

#include <cassert>   // assert
#include <cstdint>   // std::uint64_t
#include <optional>  // std::optional
#include <sstream>   // std::ostringstream
#include <stdexcept> // std::length_error, std::runtime_error
#include <string>    // std::stoul, std::string

namespace {

auto uri_normalize(UriUriA *uri) -> void {
  if (uriNormalizeSyntaxA(uri) != URI_SUCCESS) {
    throw sourcemeta::core::URIError{"Could not normalize URI"};
  }
}

auto uri_text_range(const UriTextRangeA *const range)
    -> std::optional<std::string_view> {
  if (range->afterLast == nullptr) {
    return std::nullopt;
  }

  return std::string_view{range->first,
                          static_cast<std::string_view::size_type>(
                              range->afterLast - range->first)};
}

auto uri_parse_and_normalize(const std::string &data, UriUriA *uri) -> void {
  const char *error_position = nullptr;
  switch (uriParseSingleUriA(uri, data.c_str(), &error_position)) {
    case URI_ERROR_SYNTAX:
      throw sourcemeta::core::URIParseError{
          static_cast<std::uint64_t>(error_position - data.c_str() + 1)};
    case URI_ERROR_MALLOC:
      throw std::runtime_error("URI malloc error");
    case URI_ERROR_OUTPUT_TOO_LARGE:
      throw std::length_error("URI output too large");
    case URI_SUCCESS:
      break;
    default:
      throw sourcemeta::core::URIError{"Unknown URI error"};
  }

  uri_normalize(uri);
}

} // namespace

namespace sourcemeta::core {

auto URI::parse(const std::string &input) -> void {
  // Clear all components
  this->path_ = std::nullopt;
  this->scheme_ = std::nullopt;
  this->userinfo_ = std::nullopt;
  this->host_ = std::nullopt;
  this->port_ = std::nullopt;
  this->fragment_ = std::nullopt;
  this->query_ = std::nullopt;
  this->is_dot_reference_ = (input == "." || input == "./");

  // Parse using uriparser locally
  UriUriA uri;
  uri_parse_and_normalize(input, &uri);

  // Scheme
  const auto scheme{uri_text_range(&uri.scheme)};
  if (scheme.has_value()) {
    this->scheme_ = scheme.value();
  }

  // Userinfo
  const auto userinfo{uri_text_range(&uri.userInfo)};
  if (userinfo.has_value()) {
    this->userinfo_ = userinfo.value();
  }

  // Host
  const auto host{uri_text_range(&uri.hostText)};
  if (host.has_value()) {
    this->host_ = host.value();
  }

  // Port
  const auto port_text{uri_text_range(&uri.portText)};
  if (!port_text.has_value()) {
    this->port_ = std::nullopt;
  } else {
    this->port_ = std::stoul(std::string{port_text.value()});
  }

  // Query
  const auto query{uri_text_range(&uri.query)};
  if (query.has_value()) {
    this->query_ = query.value();
  }

  // Fragment
  const auto fragment{uri_text_range(&uri.fragment)};
  if (fragment.has_value()) {
    this->fragment_ = fragment.value();
  }

  // Special case: /.. normalizes to /
  if (input == "/..") {
    this->path_ = "/";
    uriFreeUriMembersA(&uri);
    return;
  }

  const UriPathSegmentA *segment{uri.pathHead};
  if (segment != nullptr) {
    std::ostringstream path;
    // URNs and tags have a single path segment by definition
    if (this->is_urn() || this->is_tag()) {
      const auto part{uri_text_range(&segment->text)};
      assert(part.has_value());
      path << part.value();
    } else {
      bool first{true};
      while (segment) {
        const auto part{uri_text_range(&segment->text)};
        assert(part.has_value());
        const auto value{part.value()};

        if (first) {
          path << value;
        } else {
          path << "/" << value;
        }

        segment = segment->next;
        first = false;
      }
    }
    std::string path_value{path.str()};

    // Add leading "/" if appropriate during parsing
    // This replicates the logic from the original path() accessor
    const bool has_scheme_needing_slash = !this->is_urn() && !this->is_tag() &&
                                          !this->is_mailto() &&
                                          this->scheme_.has_value();
    const bool has_authority{this->port_.has_value() ||
                             this->host_.has_value()};
    const bool had_leading_slash_in_original =
        input.starts_with('/') && input.size() > 1;

    const bool needs_leading_slash = has_scheme_needing_slash ||
                                     has_authority ||
                                     had_leading_slash_in_original;

    if (needs_leading_slash) {
      if (path_value.empty()) {
        // Empty path with authority or scheme becomes "/"
        path_value = "/";
      } else {
        path_value = "/" + path_value;
      }
    }

    this->path_ = path_value;
  } else {
    // No path segments from uriparser, but we might still need to set a path
    // For URIs like "https://example.com/" or just "/"
    const bool has_scheme_needing_slash = !this->is_urn() && !this->is_tag() &&
                                          !this->is_mailto() &&
                                          this->scheme_.has_value();
    const bool has_authority{this->port_.has_value() ||
                             this->host_.has_value()};
    const bool had_trailing_slash = input.ends_with('/');
    const bool is_just_slash = input == "/";

    if (((has_scheme_needing_slash || has_authority) && had_trailing_slash) ||
        is_just_slash) {
      this->path_ = "/";
    }
  }

  // Free uriparser memory
  uriFreeUriMembersA(&uri);
}

} // namespace sourcemeta::core
