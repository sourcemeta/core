#include <sourcemeta/core/text.h>

#include <cctype> // std::toupper

namespace sourcemeta::core {

auto to_title_case(std::string &value) -> void {
  bool capitalize_next{true};
  for (auto &character : value) {
    if (character == '_' || character == '-') {
      character = ' ';
      capitalize_next = true;
    } else if (capitalize_next) {
      character = static_cast<char>(
          std::toupper(static_cast<unsigned char>(character)));
      capitalize_next = false;
    }
  }
}

} // namespace sourcemeta::core
