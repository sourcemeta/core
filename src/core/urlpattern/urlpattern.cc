#include <sourcemeta/core/urlpattern.h>

#include <sourcemeta/core/punycode.h>

#include <cctype>   // std::tolower
#include <cstdint>  // std::uint8_t
#include <cstdlib>  // std::stoul
#include <optional> // std::optional
#include <vector>   // std::vector

namespace sourcemeta::core {

namespace {

auto hex_digit_value(const char character) -> int {
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }
  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }
  return -1;
}

auto is_uppercase_hex(const char character) -> bool {
  return (character >= '0' && character <= '9') ||
         (character >= 'A' && character <= 'F');
}

auto strip_tab_newline(const std::string &input) -> std::string {
  std::string result;
  result.reserve(input.size());
  for (const auto character : input) {
    if (character != '\t' && character != '\n' && character != '\r') {
      result += character;
    }
  }
  return result;
}

auto is_ascii(const std::string &input) -> bool {
  for (const auto character : input) {
    if (static_cast<unsigned char>(character) > 127) {
      return false;
    }
  }
  return true;
}

auto to_lowercase(const std::string &input) -> std::string {
  std::string result;
  result.reserve(input.size());
  for (const auto character : input) {
    result +=
        static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
  }
  return result;
}

auto hostname_to_ascii(const std::string &input) -> std::string {
  std::string result;
  std::string label;

  const auto process_label = [&result, &label]() {
    if (label.empty()) {
      return;
    }

    if (!result.empty()) {
      result += '.';
    }

    if (is_ascii(label)) {
      result += to_lowercase(label);
    } else {
      result += "xn--";
      try {
        result += utf8_to_punycode(label);
      } catch (const PunycodeError &) {
        result += label;
      }
    }
    label.clear();
  };

  for (const auto character : input) {
    if (character == '.') {
      process_label();
    } else {
      label += character;
    }
  }
  process_label();

  return result;
}

auto has_pattern_syntax(const std::string &input) -> bool {
  bool after_backslash = false;
  for (const auto character : input) {
    if (after_backslash) {
      after_backslash = false;
      continue;
    }
    if (character == '\\') {
      after_backslash = true;
      continue;
    }
    if (character == ':' || character == '*' || character == '{' ||
        character == '}' || character == '(' || character == ')' ||
        character == '?') {
      return true;
    }
  }
  return false;
}

auto canonicalize_hostname_pattern(const std::string &input) -> std::string {
  auto stripped = strip_tab_newline(input);

  std::string truncated;
  for (const auto character : stripped) {
    if (character == '#' || character == '/') {
      break;
    }
    truncated += character;
  }

  if (!has_pattern_syntax(truncated)) {
    return hostname_to_ascii(truncated);
  }

  stripped = truncated;

  std::string result;
  bool in_name = false;
  bool after_backslash = false;
  for (const auto character : stripped) {
    if (character == '#' || character == '/') {
      break;
    }

    if (after_backslash) {
      result += static_cast<char>(
          std::tolower(static_cast<unsigned char>(character)));
      after_backslash = false;
      continue;
    }

    if (character == '\\') {
      result += character;
      after_backslash = true;
      continue;
    }

    if (character == ':') {
      result += character;
      in_name = true;
      continue;
    }

    if (in_name) {
      const auto byte = static_cast<unsigned char>(character);
      if (std::isalnum(byte) || character == '_' || character == '$' ||
          byte >= 0x80) {
        result += character;
        continue;
      }
      in_name = false;
    }

    if (character == '*' || character == '{' || character == '}' ||
        character == '(' || character == ')' || character == '?') {
      result += character;
    } else {
      result += static_cast<char>(
          std::tolower(static_cast<unsigned char>(character)));
    }
  }
  return result;
}

auto canonicalize_port(const std::string &input) -> std::optional<std::string> {
  auto stripped = strip_tab_newline(input);
  std::string result;
  for (const auto character : stripped) {
    if (character >= '0' && character <= '9') {
      result += character;
    } else {
      break;
    }
  }
  // If the stripped input starts with a non-digit (excluding case of empty
  // stripped input becoming empty result), the port is invalid
  if (!stripped.empty() && result.empty()) {
    return std::nullopt;
  }
  return result;
}

auto is_valid_port_pattern(const std::string &input) -> bool {
  if (input.empty()) {
    return true;
  }

  bool is_literal = true;
  for (const auto character : input) {
    if (character >= '0' && character <= '9') {
      continue;
    }
    is_literal = false;
    break;
  }

  if (is_literal && !input.empty()) {
    try {
      const auto port_value = std::stoul(input);
      if (port_value > 65535) {
        return false;
      }
    } catch (...) {
      return false;
    }
  }

  return true;
}

auto percent_decode(const std::string &input) -> std::string {
  std::string result;
  result.reserve(input.size());

  for (std::string::size_type index = 0; index < input.size(); index += 1) {
    if (input[index] == '%' && index + 2 < input.size()) {
      // Only decode uppercase percent-encoding (canonical form)
      if (is_uppercase_hex(input[index + 1]) &&
          is_uppercase_hex(input[index + 2])) {
        const auto high = hex_digit_value(input[index + 1]);
        const auto low = hex_digit_value(input[index + 2]);
        const auto byte = static_cast<std::uint8_t>((high << 4) | low);
        result += static_cast<char>(byte);
        index += 2;
        continue;
      }
    }
    result += input[index];
  }

  return result;
}

auto normalize_pathname_input(const std::string &pathname) -> std::string {
  if (pathname.empty()) {
    return pathname;
  }

  const auto starts_with_slash = pathname[0] == '/';

  // Only normalize absolute paths (starting with /)
  // Bare inputs like "./foo" should not be normalized
  if (!starts_with_slash) {
    return pathname;
  }

  // Only normalize . and .. segments for absolute input pathnames
  std::vector<std::string_view> segments;
  std::string::size_type position{1};
  const auto ends_with_slash =
      pathname.size() > 1 && pathname[pathname.size() - 1] == '/';

  while (position < pathname.size()) {
    auto segment_end = position;
    while (segment_end < pathname.size() && pathname[segment_end] != '/') {
      segment_end += 1;
    }

    const std::string_view segment{pathname.data() + position,
                                   segment_end - position};

    if (segment == "..") {
      if (!segments.empty()) {
        segments.pop_back();
      }
    } else if (segment != "." && !segment.empty()) {
      segments.emplace_back(segment);
    }

    position = segment_end + 1;
  }

  std::string result;
  result += '/';
  for (std::size_t index = 0; index < segments.size(); index += 1) {
    result += segments[index];
    if (index + 1 < segments.size()) {
      result += '/';
    }
  }

  if (ends_with_slash && (result.empty() || result[result.size() - 1] != '/')) {
    result += '/';
  }

  return result;
}

auto normalize_pathname_pattern(const std::string &pathname) -> std::string {
  if (pathname.empty()) {
    return pathname;
  }

  const auto starts_with_slash = pathname[0] == '/';

  // Only normalize absolute patterns (starting with /)
  // Bare patterns like "./foo" or "../foo" should not be normalized
  if (!starts_with_slash) {
    return pathname;
  }

  // Normalize .. segments while preserving empty segments (from //)
  std::vector<std::string_view> segments;
  std::string::size_type position{1};
  const auto ends_with_slash =
      pathname.size() > 1 && pathname[pathname.size() - 1] == '/';

  while (position < pathname.size()) {
    auto segment_end = position;
    while (segment_end < pathname.size() && pathname[segment_end] != '/') {
      segment_end += 1;
    }

    const std::string_view segment{pathname.data() + position,
                                   segment_end - position};

    if (segment == "..") {
      if (!segments.empty() && segments.back() != "..") {
        segments.pop_back();
      } else {
        segments.emplace_back(segment);
      }
    } else if (segment == ".") {
      // Skip single dot segments
    } else {
      // Keep all other segments including empty ones
      segments.emplace_back(segment);
    }

    position = segment_end + 1;
  }

  std::string result;
  result += '/';
  for (std::size_t index = 0; index < segments.size(); index += 1) {
    result += segments[index];
    if (index + 1 < segments.size()) {
      result += '/';
    }
  }

  if (ends_with_slash && (result.empty() || result[result.size() - 1] != '/')) {
    result += '/';
  }

  return result;
}
} // namespace

auto URLPatternInput::parse(const JSON &input)
    -> std::optional<URLPatternInput> {
  if (!input.is_object()) {
    return std::nullopt;
  }

  URLPatternInput result;

  if (input.defines("protocol")) {
    const auto &value{input.at("protocol")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    const auto protocol_str = value.to_string();
    if (!is_ascii(protocol_str)) {
      return std::nullopt;
    }
    result.protocol = protocol_str;
  }

  if (input.defines("username")) {
    const auto &value{input.at("username")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    result.username = value.to_string();
  }

  if (input.defines("password")) {
    const auto &value{input.at("password")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    result.password = value.to_string();
  }

  if (input.defines("hostname")) {
    const auto &value{input.at("hostname")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    result.hostname = hostname_to_ascii(value.to_string());
  }

  if (input.defines("port")) {
    const auto &value{input.at("port")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    const auto canonicalized = canonicalize_port(value.to_string());
    if (!canonicalized.has_value()) {
      return std::nullopt;
    }
    result.port = canonicalized.value();
  }

  if (input.defines("pathname")) {
    const auto &value{input.at("pathname")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    const auto decoded = percent_decode(value.to_string());
    result.pathname = normalize_pathname_input(decoded);
  }

  if (input.defines("search")) {
    const auto &value{input.at("search")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    result.search = value.to_string();
  }

  if (input.defines("hash")) {
    const auto &value{input.at("hash")};
    if (!value.is_string()) {
      return std::nullopt;
    }
    result.hash = value.to_string();
  }

  return result;
}

auto URLPatternInput::parse(const std::string_view input)
    -> std::optional<URLPatternInput> {
  if (input.empty()) {
    return std::nullopt;
  }

  URLPatternInput result;
  const std::string_view remaining{input};
  std::string::size_type position{0};

  const auto protocol_end = remaining.find("://");
  const auto colon_pos = remaining.find(':');

  if (protocol_end == std::string_view::npos) {
    if (colon_pos == std::string_view::npos) {
      return std::nullopt;
    }

    const auto protocol_str = remaining.substr(0, colon_pos);
    if (protocol_str.empty() || !is_ascii(std::string{protocol_str})) {
      return std::nullopt;
    }
    result.protocol = std::string{protocol_str};

    const auto path_str = remaining.substr(colon_pos + 1);
    if (!path_str.empty()) {
      const auto decoded = percent_decode(std::string{path_str});
      result.pathname = normalize_pathname_input(decoded);
    }

    return result;
  }

  const auto protocol_str = remaining.substr(0, protocol_end);
  if (protocol_str.empty() || !is_ascii(std::string{protocol_str})) {
    return std::nullopt;
  }
  result.protocol = std::string{protocol_str};
  position = protocol_end + 3;

  if (position >= remaining.size()) {
    return result;
  }

  const auto path_start = remaining.find('/', position);
  const auto query_start = remaining.find('?', position);
  const auto hash_start = remaining.find('#', position);

  auto authority_end = path_start;
  if (query_start != std::string_view::npos &&
      (authority_end == std::string_view::npos ||
       query_start < authority_end)) {
    authority_end = query_start;
  }
  if (hash_start != std::string_view::npos &&
      (authority_end == std::string_view::npos || hash_start < authority_end)) {
    authority_end = hash_start;
  }

  if (authority_end == std::string_view::npos) {
    authority_end = remaining.size();
  }

  const auto authority = remaining.substr(position, authority_end - position);
  if (!authority.empty()) {
    auto userinfo_end = authority.find('@');
    std::string::size_type host_start = 0;

    if (userinfo_end != std::string_view::npos) {
      const auto userinfo = authority.substr(0, userinfo_end);
      const auto password_sep = userinfo.find(':');

      if (password_sep != std::string_view::npos) {
        result.username = std::string{userinfo.substr(0, password_sep)};
        result.password = std::string{userinfo.substr(password_sep + 1)};
      } else {
        result.username = std::string{userinfo};
      }

      host_start = userinfo_end + 1;
    }

    const auto host_part = authority.substr(host_start);
    if (!host_part.empty()) {
      auto port_sep = host_part.rfind(':');

      if (host_part[0] == '[') {
        const auto bracket_end = host_part.find(']');
        if (bracket_end != std::string_view::npos) {
          if (port_sep != std::string_view::npos && port_sep < bracket_end) {
            port_sep = std::string_view::npos;
          }
        }
      }

      if (port_sep != std::string_view::npos) {
        const auto hostname_str = host_part.substr(0, port_sep);
        result.hostname = hostname_to_ascii(std::string{hostname_str});
        const auto port_str = host_part.substr(port_sep + 1);
        const auto canonicalized = canonicalize_port(std::string{port_str});
        if (canonicalized.has_value()) {
          result.port = canonicalized.value();
        }
      } else {
        result.hostname = hostname_to_ascii(std::string{host_part});
      }
    }
  }

  position = authority_end;

  if (path_start != std::string_view::npos && path_start == position) {
    auto path_end = query_start;
    if (hash_start != std::string_view::npos &&
        (path_end == std::string_view::npos || hash_start < path_end)) {
      path_end = hash_start;
    }
    if (path_end == std::string_view::npos) {
      path_end = remaining.size();
    }

    const auto path_str = remaining.substr(path_start, path_end - path_start);
    const auto decoded = percent_decode(std::string{path_str});
    result.pathname = normalize_pathname_input(decoded);

    position = path_end;
  }

  if (query_start != std::string_view::npos && query_start == position) {
    auto query_end = hash_start;
    if (query_end == std::string_view::npos) {
      query_end = remaining.size();
    }

    result.search =
        std::string{remaining.substr(query_start + 1, query_end - query_start - 1)};

    position = query_end;
  }

  if (hash_start != std::string_view::npos && hash_start == position) {
    result.hash = std::string{remaining.substr(hash_start + 1)};
  }

  return result;
}

auto URLPattern::parse(const std::string_view input) -> URLPattern {
  URLPattern pattern;

  if (input.empty()) {
    return pattern;
  }

  const std::string_view remaining{input};
  std::string::size_type position{0};

  if (remaining[0] == '/') {
    pattern.pathname = URLPatternPathname{std::string{remaining}.c_str()};
    return pattern;
  }

  const auto protocol_end = remaining.find("://");
  if (protocol_end != std::string_view::npos) {
    const auto protocol_str = remaining.substr(0, protocol_end);
    if (!protocol_str.empty()) {
      pattern.protocol = URLPatternProtocol{std::string{protocol_str}.c_str()};
    }
    position = protocol_end + 3;
  }

  if (position >= remaining.size()) {
    return pattern;
  }

  const auto path_start = remaining.find('/', position);
  const auto query_start = remaining.find('?', position);
  const auto hash_start = remaining.find('#', position);

  auto authority_end = path_start;
  if (query_start != std::string_view::npos &&
      (authority_end == std::string_view::npos ||
       query_start < authority_end)) {
    authority_end = query_start;
  }
  if (hash_start != std::string_view::npos &&
      (authority_end == std::string_view::npos || hash_start < authority_end)) {
    authority_end = hash_start;
  }

  if (authority_end == std::string_view::npos) {
    authority_end = remaining.size();
  }

  const auto authority = remaining.substr(position, authority_end - position);
  if (!authority.empty()) {
    auto userinfo_end = authority.find('@');
    std::string::size_type host_start = 0;

    if (userinfo_end != std::string_view::npos) {
      const auto userinfo = authority.substr(0, userinfo_end);
      const auto password_sep = userinfo.find(':');

      if (password_sep != std::string_view::npos) {
        const auto username_str = userinfo.substr(0, password_sep);
        if (!username_str.empty()) {
          pattern.username =
              URLPatternUsername{std::string{username_str}.c_str()};
        }
        const auto password_str = userinfo.substr(password_sep + 1);
        if (!password_str.empty()) {
          pattern.password =
              URLPatternPassword{std::string{password_str}.c_str()};
        }
      } else if (!userinfo.empty()) {
        pattern.username = URLPatternUsername{std::string{userinfo}.c_str()};
      }

      host_start = userinfo_end + 1;
    }

    const auto host_part = authority.substr(host_start);
    if (!host_part.empty()) {
      const auto port_sep = host_part.rfind(':');

      if (port_sep != std::string_view::npos) {
        const auto hostname_str = host_part.substr(0, port_sep);
        if (!hostname_str.empty()) {
          pattern.hostname =
              URLPatternHostname{std::string{hostname_str}.c_str()};
        }
        const auto port_str = host_part.substr(port_sep + 1);
        if (!port_str.empty()) {
          pattern.port = URLPatternPort{std::string{port_str}.c_str()};
        }
      } else {
        pattern.hostname = URLPatternHostname{std::string{host_part}.c_str()};
      }
    }
  }

  position = authority_end;

  if (path_start != std::string_view::npos && path_start == position) {
    auto path_end = query_start;
    if (hash_start != std::string_view::npos &&
        (path_end == std::string_view::npos || hash_start < path_end)) {
      path_end = hash_start;
    }
    if (path_end == std::string_view::npos) {
      path_end = remaining.size();
    }

    const auto path_str = remaining.substr(path_start, path_end - path_start);
    if (!path_str.empty()) {
      pattern.pathname = URLPatternPathname{std::string{path_str}.c_str()};
    }

    position = path_end;
  }

  if (query_start != std::string_view::npos && query_start == position) {
    auto query_end = hash_start;
    if (query_end == std::string_view::npos) {
      query_end = remaining.size();
    }

    const auto query_str =
        remaining.substr(query_start + 1, query_end - query_start - 1);
    if (!query_str.empty()) {
      pattern.search = URLPatternSearch{std::string{query_str}.c_str()};
    }

    position = query_end;
  }

  if (hash_start != std::string_view::npos && hash_start == position) {
    const auto hash_str = remaining.substr(hash_start + 1);
    if (!hash_str.empty()) {
      pattern.hash = URLPatternHash{std::string{hash_str}.c_str()};
    }
  }

  return pattern;
}

auto URLPattern::parse(const JSON &input) -> std::optional<URLPattern> {
  if (!input.is_object()) {
    return std::nullopt;
  }

  URLPattern pattern;

  try {
    if (input.defines("protocol")) {
      const auto &value{input.at("protocol")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      pattern.protocol = URLPatternProtocol{value.to_string().c_str()};
    }

    if (input.defines("username")) {
      const auto &value{input.at("username")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto decoded = percent_decode(value.to_string());
      pattern.username = URLPatternUsername{decoded.c_str()};
    }

    if (input.defines("password")) {
      const auto &value{input.at("password")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto decoded = percent_decode(value.to_string());
      pattern.password = URLPatternPassword{decoded.c_str()};
    }

    if (input.defines("hostname")) {
      const auto &value{input.at("hostname")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto canonicalized =
          canonicalize_hostname_pattern(value.to_string());
      pattern.hostname = URLPatternHostname{canonicalized.c_str()};
    }

    if (input.defines("port")) {
      const auto &value{input.at("port")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto port_str = value.to_string();
      if (!is_valid_port_pattern(port_str)) {
        return std::nullopt;
      }
      pattern.port = URLPatternPort{port_str.c_str()};
    }

    if (input.defines("pathname")) {
      const auto &value{input.at("pathname")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto decoded = percent_decode(value.to_string());
      const auto normalized = normalize_pathname_pattern(decoded);
      pattern.pathname = URLPatternPathname{normalized.c_str()};
    }

    if (input.defines("search")) {
      const auto &value{input.at("search")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto decoded = percent_decode(value.to_string());
      pattern.search = URLPatternSearch{decoded.c_str()};
    }

    if (input.defines("hash")) {
      const auto &value{input.at("hash")};
      if (!value.is_string()) {
        return std::nullopt;
      }
      const auto decoded = percent_decode(value.to_string());
      pattern.hash = URLPatternHash{decoded.c_str()};
    }
  } catch (const URLPatternParseError &) {
    return std::nullopt;
  }

  return pattern;
}

auto URLPattern::match(const URLPatternInput &input) const -> URLPatternResult {
  URLPatternResult result;
  result.protocol = this->protocol.match(input.protocol);
  result.username = this->username.match(input.username);
  result.password = this->password.match(input.password);
  result.hostname = this->hostname.match(input.hostname);
  result.port = this->port.match(input.port);
  result.pathname = this->pathname.match(input.pathname);
  result.search = this->search.match(input.search);
  result.hash = this->hash.match(input.hash);

  if (!result.protocol.has_value() || !result.username.has_value() ||
      !result.password.has_value() || !result.hostname.has_value() ||
      !result.port.has_value() || !result.pathname.has_value() ||
      !result.search.has_value() || !result.hash.has_value()) {
    return URLPatternResult{};
  }

  return result;
}

auto URLPattern::operator<=>(const URLPattern &other) const
    -> std::strong_ordering {
  if (auto cmp = this->protocol <=> other.protocol;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->username <=> other.username;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->password <=> other.password;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->hostname <=> other.hostname;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->port <=> other.port;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->pathname <=> other.pathname;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  if (auto cmp = this->search <=> other.search;
      cmp != std::strong_ordering::equal) {
    return cmp;
  }
  return this->hash <=> other.hash;
}

} // namespace sourcemeta::core
