#ifndef SOURCEMETA_CORE_URI_NORMALIZE_H_
#define SOURCEMETA_CORE_URI_NORMALIZE_H_

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

// Remove "." and ".." segments from a URI path per RFC 3986 Section 5.2.4
// (Remove Dot Segments). Operates in place.
inline auto normalize_path(std::string &path) -> void {
  const std::string buffer{std::move(path)};
  std::string_view input{buffer};
  std::string output;
  output.reserve(buffer.size());

  while (!input.empty()) {
    if (input.starts_with("../")) {
      input.remove_prefix(3);
    } else if (input.starts_with("./") || input.starts_with("/./")) {
      input.remove_prefix(2);
    } else if (input == "/.") {
      output.push_back('/');
      break;
    } else if (input.starts_with("/../")) {
      input.remove_prefix(3);
      const auto last_slash{output.rfind('/')};
      if (last_slash == std::string::npos) {
        output.clear();
      } else {
        output.resize(last_slash);
      }
    } else if (input == "/..") {
      const auto last_slash{output.rfind('/')};
      if (last_slash == std::string::npos) {
        output.clear();
      } else {
        output.resize(last_slash);
      }
      output.push_back('/');
      break;
    } else if (input == "." || input == "..") {
      break;
    } else {
      const std::size_t next_slash{input.starts_with('/') ? input.find('/', 1)
                                                          : input.find('/')};
      if (next_slash == std::string_view::npos) {
        output.append(input);
        break;
      }
      output.append(input.substr(0, next_slash));
      input.remove_prefix(next_slash);
    }
  }

  path = std::move(output);
}

} // namespace sourcemeta::core

#endif
