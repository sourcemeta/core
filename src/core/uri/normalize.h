#ifndef SOURCEMETA_CORE_URI_NORMALIZE_H_
#define SOURCEMETA_CORE_URI_NORMALIZE_H_

#include <cstddef> // std::size_t
#include <string>  // std::string

namespace sourcemeta::core {

// Remove "." and ".." segments from a URI path per RFC 3986 Section 5.2.4
// (Remove Dot Segments). Operates in place.
inline auto normalize_path(std::string &path) -> void {
  std::string input{std::move(path)};
  std::string output;
  output.reserve(input.size());

  while (!input.empty()) {
    if (input.starts_with("../")) {
      input.erase(0, 3);
    } else if (input.starts_with("./")) {
      input.erase(0, 2);
    } else if (input.starts_with("/./")) {
      input.replace(0, 3, 1, '/');
    } else if (input == "/.") {
      input = "/";
    } else if (input.starts_with("/../")) {
      input.replace(0, 4, 1, '/');
      const auto last_slash{output.rfind('/')};
      if (last_slash == std::string::npos) {
        output.clear();
      } else {
        output.erase(last_slash);
      }
    } else if (input == "/..") {
      input = "/";
      const auto last_slash{output.rfind('/')};
      if (last_slash == std::string::npos) {
        output.clear();
      } else {
        output.erase(last_slash);
      }
    } else if (input == "." || input == "..") {
      input.clear();
    } else {
      const std::size_t next_slash{input.starts_with('/') ? input.find('/', 1)
                                                          : input.find('/')};
      if (next_slash == std::string::npos) {
        output.append(input);
        input.clear();
      } else {
        output.append(input, 0, next_slash);
        input.erase(0, next_slash);
      }
    }
  }

  path = std::move(output);
}

} // namespace sourcemeta::core

#endif
