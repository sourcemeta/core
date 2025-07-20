#include <uriparser/Uri.h>

#include <sourcemeta/core/uri_escape.h>

#include <istream>       // std::istream
#include <ostream>       // std::ostream
#include <string>        // std::string
#include <unordered_set> // std::unordered_set

namespace sourcemeta::core {

auto uri_escape(const char character, std::ostream &output,
                const URIEscapeMode mode) -> void {
  // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
  // See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
  std::unordered_set<char> unreserved{
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '-', '.', '_', '~'};

  // sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" /
  // "=" See https://www.rfc-editor.org/rfc/rfc3986#appendix-A
  std::unordered_set<char> sub_delims{'!', '$', '&', '\'', '(', ')',
                                      '*', '+', ',', ';',  '='};

  if (unreserved.contains(character)) {
    output << character;
    return;
  }

  if (mode == URIEscapeMode::SkipSubDelims) {
    if (sub_delims.contains(character)) {
      output << character;
      return;
    }
  }
  output << '%' << std::hex << std::uppercase
         << +(static_cast<unsigned char>(character));
  return;
}

auto uri_escape(std::istream &input, std::ostream &output,
                const URIEscapeMode mode) -> void {
  char character = 0;
  while (input.get(character)) {
    uri_escape(character, output, mode);
  }
}

auto uri_unescape(std::istream &input, std::ostream &output) -> void {
  std::istream_iterator<char> iterator(input);
  std::istream_iterator<char> end;
  auto plus_1 = std::ranges::next(iterator, 1, end);
  auto plus_2 = std::ranges::next(plus_1, 1, end);
  const int hex_base = 16;

  while (iterator != end) {
    if (*iterator == '%' && plus_1 != end && plus_2 != end &&
        std::isxdigit(*(plus_1)) && std::isxdigit(*(plus_2))) {
      std::string hex{*plus_1, *plus_2};
      char decoded_char = static_cast<char>(std::stoi(hex, nullptr, hex_base));
      output << decoded_char;

      iterator = std::ranges::next(plus_2, 1, end);
      plus_1 = std::ranges::next(iterator, 1, end);
      plus_2 = std::ranges::next(plus_1, 1, end);
    } else {
      output << *iterator;
      iterator = plus_1;
      plus_1 = plus_2;
      plus_2 = std::ranges::next(plus_1, 1, end);
    }
  }
}

} // namespace sourcemeta::core
