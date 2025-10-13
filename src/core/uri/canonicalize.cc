#include <sourcemeta/core/uri.h>

#include <cctype>   // std::tolower
#include <cstdint>  // std::uint32_t
#include <optional> // std::optional
#include <string>   // std::string

namespace {

auto to_lowercase(std::string_view input) -> std::string {
  std::string result;
  result.reserve(input.size());
  for (const auto character : input) {
    result +=
        static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
  }
  return result;
}

} // namespace

namespace sourcemeta::core {

auto URI::canonicalize() -> URI & {
  // Lowercase scheme (schemes are case-insensitive per RFC 3986)
  if (this->scheme_.has_value()) {
    this->scheme_ = to_lowercase(this->scheme_.value());
  }

  // Lowercase host (hostnames are case-insensitive per RFC 3986)
  if (this->host_.has_value()) {
    this->host_ = to_lowercase(this->host_.value());
  }

  // Canonicalize path by removing "." and ".." segments
  if (this->path_.has_value() && !this->path_.value().empty()) {
    const auto &path = this->path_.value();

    // Relative paths starting with ".." should be preserved as-is
    if (path.starts_with("..")) {
      // Keep the path unchanged
    } else {
      std::string canonical_path;
      const auto had_leading_slash = path.starts_with("/");
      const auto had_trailing_slash = path.ends_with('/') && path != "/";

      // Minimum position for write (0 for relative, 1 for absolute with leading
      // /)
      const std::string::size_type minimum_position = had_leading_slash ? 1 : 0;
      std::string::size_type write_position = 0;

      if (had_leading_slash) {
        canonical_path = path;
        write_position = 1;
      } else {
        canonical_path = path;
      }

      std::string::size_type read_position = had_leading_slash ? 1 : 0;
      std::string::size_type segment_start = read_position;

      // Skip leading "." for relative paths like "./foo"
      if (!had_leading_slash && read_position < path.size() &&
          path[read_position] == '.') {
        if (read_position + 1 < path.size() && path[read_position + 1] == '/') {
          read_position += 2;
          segment_start = read_position;
        }
      }

      while (read_position <= path.size()) {
        if (read_position == path.size() || path[read_position] == '/') {
          const auto segment_length = read_position - segment_start;

          if (segment_length == 2 && path[segment_start] == '.' &&
              path[segment_start + 1] == '.') {
            // ".." segment - remove previous segment if possible
            if (write_position > minimum_position) {
              // Backtrack to previous segment
              if (write_position > 0 &&
                  canonical_path[write_position - 1] == '/') {
                --write_position;
              }
              while (write_position > minimum_position &&
                     canonical_path[write_position - 1] != '/') {
                --write_position;
              }
            }
          } else if (segment_length == 1 && path[segment_start] == '.') {
            // "." segment - skip it
          } else if (segment_length > 0) {
            // Regular segment - copy it
            if (write_position > minimum_position) {
              canonical_path[write_position++] = '/';
            }
            for (std::string::size_type i = segment_start; i < read_position;
                 ++i) {
              canonical_path[write_position++] = path[i];
            }
          }

          ++read_position;
          segment_start = read_position;
        } else {
          ++read_position;
        }
      }

      // Resize to actual length
      canonical_path.resize(write_position);

      // Handle trailing slash
      if (had_trailing_slash && !canonical_path.empty() &&
          canonical_path != "/") {
        canonical_path += '/';
      }

      // Empty relative path becomes nullopt
      if (canonical_path.empty() && !had_leading_slash) {
        this->path_ = std::nullopt;
      } else {
        this->path_ = std::move(canonical_path);
      }
    }
  }

  // Remove empty fragment (empty fragments are optional per RFC 3986)
  if (this->fragment_.has_value() && this->fragment_.value().empty()) {
    this->fragment_ = std::nullopt;
  }

  // Remove default ports (80 for http, 443 for https)
  if (this->port_.has_value() && this->scheme_.has_value()) {
    const auto port_value = this->port_.value();
    const auto scheme_value = this->scheme_.value();

    if ((scheme_value == "http" && port_value == 80) ||
        (scheme_value == "https" && port_value == 443)) {
      this->port_ = std::nullopt;
    }
  }

  return *this;
}

auto URI::canonicalize(const std::string &input) -> std::string {
  return URI{input}.canonicalize().recompose();
}

} // namespace sourcemeta::core
