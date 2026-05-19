#include <sourcemeta/core/uri.h>

#include "escaping.h"
#include "grammar.h"
#include "normalize.h"

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

using sourcemeta::core::uri_hex_to_int;
using sourcemeta::core::uri_is_pchar;
using sourcemeta::core::uri_is_unreserved;
using sourcemeta::core::URI_PERCENT;
using sourcemeta::core::URI_SLASH;

auto is_hex_uppercase(const char character) -> bool {
  return (character >= '0' && character <= '9') ||
         (character >= 'A' && character <= 'F');
}

auto validate_pchar_string(const std::string_view value) -> bool {
  std::size_t index{0};
  while (index < value.size()) {
    const auto character{value[index]};
    if (character == URI_PERCENT) {
      if (index + 3 > value.size()) {
        return false;
      }
      const auto high{value[index + 1]};
      const auto low{value[index + 2]};
      if (!is_hex_uppercase(high) || !is_hex_uppercase(low)) {
        return false;
      }
      const auto decoded{
          static_cast<char>((uri_hex_to_int(high) << 4) | uri_hex_to_int(low))};
      if (uri_is_unreserved(decoded)) {
        return false;
      }
      index += 3;
    } else if (character != URI_PERCENT && uri_is_pchar(character)) {
      ++index;
    } else {
      return false;
    }
  }
  return true;
}

auto validate_and_collect(const std::string_view path, std::string &output)
    -> bool {
  output.clear();
  if (path.empty()) {
    return true;
  }
  if (path.front() != URI_SLASH) {
    return false;
  }

  output.reserve(path.size());
  output.push_back(URI_SLASH);

  std::size_t segment_start{1};
  for (std::size_t index{1}; index <= path.size(); ++index) {
    if (index < path.size() && path[index] != URI_SLASH) {
      continue;
    }

    const auto length{index - segment_start};
    if (length == 0) {
      if (index < path.size()) {
        return false;
      }
      break;
    }

    const auto segment{path.substr(segment_start, length)};
    if (segment != "." && segment != "..") {
      if (!validate_pchar_string(segment)) {
        return false;
      }
    }
    if (output.size() > 1) {
      output.push_back(URI_SLASH);
    }
    output.append(segment);
    segment_start = index + 1;
  }

  if (path.size() > 1 && path.back() == URI_SLASH && output.size() > 1 &&
      output.back() != URI_SLASH) {
    output.push_back(URI_SLASH);
  }

  return true;
}

auto canonicalize_path(const std::string_view input, std::string &output)
    -> bool {
  if (!validate_and_collect(input, output)) {
    return false;
  }
  sourcemeta::core::normalize_path(output);
  return true;
}

} // namespace

namespace sourcemeta::core {

auto URI::strip_path_prefix(const std::string_view path,
                            const std::string_view prefix)
    -> std::optional<std::string> {
  std::string path_canonical;
  std::string prefix_canonical;
  if (!canonicalize_path(path, path_canonical) ||
      !canonicalize_path(prefix, prefix_canonical)) {
    return std::nullopt;
  }

  std::size_t suffix_start{0};
  if (!prefix_canonical.empty()) {
    if (!path_canonical.starts_with(prefix_canonical)) {
      return std::nullopt;
    }
    if (!prefix_canonical.ends_with(URI_SLASH) &&
        path_canonical.size() > prefix_canonical.size() &&
        path_canonical[prefix_canonical.size()] != URI_SLASH) {
      return std::nullopt;
    }
    suffix_start = prefix_canonical.size();
  }
  if (suffix_start < path_canonical.size() &&
      path_canonical[suffix_start] == URI_SLASH) {
    ++suffix_start;
  }

  path_canonical.erase(0, suffix_start);
  return path_canonical;
}

auto URI::rebase_path(const std::string_view path,
                      const std::string_view old_prefix,
                      const std::string_view new_prefix)
    -> std::optional<std::string> {
  const auto suffix{URI::strip_path_prefix(path, old_prefix)};
  if (!suffix.has_value()) {
    return std::nullopt;
  }
  const bool needs_separator{!suffix.value().empty() && !new_prefix.empty() &&
                             !new_prefix.ends_with(URI_SLASH)};
  std::string result;
  result.reserve(new_prefix.size() + (needs_separator ? 1 : 0) +
                 suffix.value().size());
  result.append(new_prefix);
  if (needs_separator) {
    result.push_back(URI_SLASH);
  }
  result.append(suffix.value());
  return result;
}

} // namespace sourcemeta::core
