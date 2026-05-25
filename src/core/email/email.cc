#include <sourcemeta/core/email.h>

#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/ip.h>
#include <sourcemeta/core/unicode.h>

namespace sourcemeta::core {

// RFC 5321 §4.1.2: atext = ALPHA / DIGIT / "!" / "#" / "$" / "%" /
// "&" / "'" / "*" / "+" / "-" / "/" / "=" / "?" / "^" / "_" / "`" /
// "{" / "|" / "}" / "~"
static constexpr auto is_atext(const char character) -> bool {
  switch (character) {
    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '*':
    case '+':
    case '-':
    case '/':
    case '=':
    case '?':
    case '^':
    case '_':
    case '`':
    case '{':
    case '|':
    case '}':
    case '~':
      return true;
    default:
      return (character >= 'A' && character <= 'Z') ||
             (character >= 'a' && character <= 'z') ||
             (character >= '0' && character <= '9');
  }
}

// RFC 5321 §4.1.2: qtextSMTP = %d32-33 / %d35-91 / %d93-126
static constexpr auto is_qtext_smtp(const unsigned char character) -> bool {
  return (character >= 32 && character <= 33) ||
         (character >= 35 && character <= 91) ||
         (character >= 93 && character <= 126);
}

// RFC 5321 §4.1.2: Let-dig = ALPHA / DIGIT
static constexpr auto is_let_dig(const char character) -> bool {
  return (character >= 'A' && character <= 'Z') ||
         (character >= 'a' && character <= 'z') ||
         (character >= '0' && character <= '9');
}

// RFC 5321 §4.1.3: dcontent = %d33-90 / %d94-126
static constexpr auto is_dcontent(const unsigned char character) -> bool {
  return (character >= 33 && character <= 90) ||
         (character >= 94 && character <= 126);
}

// RFC 5321 §4.1.2: Ldh-str = *( ALPHA / DIGIT / "-" ) Let-dig
// RFC 5321 §4.1.3: Standardized-tag = Ldh-str
static constexpr auto is_ldh_str(const std::string_view value) -> bool {
  if (value.empty() || !is_let_dig(value.back())) {
    return false;
  }
  for (std::string_view::size_type position{0}; position + 1 < value.size();
       position += 1) {
    const auto character{value[position]};
    if (!is_let_dig(character) && character != '-') {
      return false;
    }
  }
  return true;
}

// RFC 5234 §2.3: ABNF literal strings are case-insensitive by default
// RFC 5321 §4.1.3: IPv6-address-literal prefix is the literal "IPv6:"
static constexpr auto matches_ipv6_tag(const std::string_view value) -> bool {
  return value.size() >= 5 && (value[0] == 'I' || value[0] == 'i') &&
         (value[1] == 'P' || value[1] == 'p') &&
         (value[2] == 'v' || value[2] == 'V') && value[3] == '6' &&
         value[4] == ':';
}

// RFC 5321 §4.1.3: General-address-literal = Standardized-tag ":" 1*dcontent
static constexpr auto is_general_address_literal(const std::string_view value)
    -> bool {
  const auto colon_position{value.find(':')};
  if (colon_position == std::string_view::npos) {
    return false;
  }
  if (!is_ldh_str(value.substr(0, colon_position))) {
    return false;
  }
  const auto content{value.substr(colon_position + 1)};
  if (content.empty()) {
    return false;
  }
  for (const auto character : content) {
    if (!is_dcontent(static_cast<unsigned char>(character))) {
      return false;
    }
  }
  return true;
}

// RFC 6531 §3.3: sub-domain =/ U-label
// Relaxed sub-domain grammar where each label is a non-empty sequence of
// LetDig / hyphen / UTF8-non-ascii bytes, with no leading or trailing hyphen,
// length limits per RFC 5321 §4.5.3.1.2 and RFC 1035 §2.3.4
static auto is_idn_domain(const std::string_view value) -> bool {
  if (value.empty() || value.size() > 255) {
    return false;
  }

  std::string_view::size_type position{0};
  while (position < value.size()) {
    const auto label_start{position};
    bool last_was_hyphen{false};
    bool label_has_content{false};

    while (position < value.size() && value[position] != '.') {
      const auto character{value[position]};
      if (character == '-') {
        if (!label_has_content) {
          return false;
        }
        last_was_hyphen = true;
        position += 1;
        label_has_content = true;
        continue;
      }

      if (is_let_dig(character)) {
        last_was_hyphen = false;
        position += 1;
        label_has_content = true;
        continue;
      }

      const auto utf8_length{utf8_codepoint_length(value, position)};
      if (utf8_length < 2) {
        return false;
      }
      last_was_hyphen = false;
      position += utf8_length;
      label_has_content = true;
    }

    const auto label_length{position - label_start};
    if (label_length == 0 || label_length > 63 || last_was_hyphen) {
      return false;
    }

    if (position < value.size()) {
      position += 1;
      if (position == value.size()) {
        // RFC 5321 §4.1.2 Domain has no trailing dot
        return false;
      }
    }
  }

  return true;
}

auto is_email(const std::string_view value) -> bool {
  if (value.empty()) {
    return false;
  }

  std::string_view::size_type position{0};

  if (value[0] == '"') {
    // RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE
    position = 1;
    while (position < value.size() && value[position] != '"') {
      if (value[position] == '\\') {
        // RFC 5321 §4.1.2: quoted-pairSMTP = %d92 %d32-126
        position += 1;
        if (position >= value.size()) {
          return false;
        }
        const auto body{static_cast<unsigned char>(value[position])};
        if (body < 32 || body > 126) {
          return false;
        }
        position += 1;
      } else {
        if (!is_qtext_smtp(static_cast<unsigned char>(value[position]))) {
          return false;
        }
        position += 1;
      }
    }
    if (position >= value.size()) {
      return false;
    }
    // value[position] is the closing DQUOTE
    position += 1;
  } else {
    // RFC 5321 §4.1.2: Dot-string = Atom *("." Atom), Atom = 1*atext
    bool previous_was_dot{false};
    bool atom_started{false};
    while (position < value.size() && value[position] != '@') {
      const auto character{value[position]};
      if (character == '.') {
        if (!atom_started || previous_was_dot) {
          return false;
        }
        previous_was_dot = true;
        atom_started = false;
      } else if (is_atext(character)) {
        previous_was_dot = false;
        atom_started = true;
      } else {
        return false;
      }
      position += 1;
    }
    if (position == 0 || previous_was_dot) {
      return false;
    }
  }

  // RFC 5321 §4.5.3.1.1: Local-part octet limit is 64
  if (position > 64) {
    return false;
  }

  // RFC 5321 §4.1.2: Mailbox = Local-part "@" ( Domain / address-literal )
  if (position >= value.size() || value[position] != '@') {
    return false;
  }

  const auto domain{value.substr(position + 1)};

  // RFC 5321 §4.1.3: address-literal = "[" ( IPv4 / IPv6 / General ) "]"
  if (!domain.empty() && domain.front() == '[') {
    if (domain.back() != ']') {
      return false;
    }
    // RFC 5321 §4.5.3.1.2: 255-octet cap on a domain "name or number"
    if (domain.size() > 255) {
      return false;
    }
    const auto inner{domain.substr(1, domain.size() - 2)};
    // RFC 5321 §4.1.3: IPv6-address-literal = "IPv6:" IPv6-addr
    if (matches_ipv6_tag(inner) && is_ipv6(inner.substr(5))) {
      return true;
    }
    // RFC 5234 §3.2: ABNF alternatives are unordered. A failed IPv6 match
    // falls through to IPv4 or General-address-literal.
    // RFC 5321 §4.1.3: IPv4-address-literal = Snum 3("." Snum) has no ":",
    // General-address-literal requires ":"
    if (inner.find(':') == std::string_view::npos) {
      return is_ipv4(inner);
    }
    return is_general_address_literal(inner);
  }

  // RFC 5321 §4.1.2 Domain matches is_hostname (RFC 1123 §2.1) by
  // grammar, by 63-octet label cap (RFC 1035 §2.3.4), and by
  // 255-octet total cap (RFC 5321 §4.5.3.1.2)
  return is_hostname(domain);
}

auto is_idn_email(const std::string_view value) -> bool {
  if (value.empty()) {
    return false;
  }

  std::string_view::size_type position{0};

  if (value[0] == '"') {
    // RFC 5321 §4.1.2: Quoted-string = DQUOTE *QcontentSMTP DQUOTE
    // RFC 6531 §3.3: qtextSMTP =/ UTF8-non-ascii
    position = 1;
    while (position < value.size() && value[position] != '"') {
      if (value[position] == '\\') {
        // RFC 5321 §4.1.2: quoted-pairSMTP = %d92 %d32-126
        position += 1;
        if (position >= value.size()) {
          return false;
        }
        const auto body{static_cast<unsigned char>(value[position])};
        if (body < 32 || body > 126) {
          return false;
        }
        position += 1;
        continue;
      }

      if (is_qtext_smtp(static_cast<unsigned char>(value[position]))) {
        position += 1;
        continue;
      }

      const auto utf8_length{utf8_codepoint_length(value, position)};
      if (utf8_length < 2) {
        return false;
      }
      position += utf8_length;
    }
    if (position >= value.size()) {
      return false;
    }
    // value[position] is the closing DQUOTE
    position += 1;
  } else {
    // RFC 5321 §4.1.2: Dot-string = Atom *("." Atom), Atom = 1*atext
    // RFC 6531 §3.3: atext =/ UTF8-non-ascii
    bool previous_was_dot{false};
    bool atom_started{false};
    while (position < value.size() && value[position] != '@') {
      const auto character{value[position]};
      if (character == '.') {
        if (!atom_started || previous_was_dot) {
          return false;
        }
        previous_was_dot = true;
        atom_started = false;
        position += 1;
        continue;
      }

      if (is_atext(character)) {
        previous_was_dot = false;
        atom_started = true;
        position += 1;
        continue;
      }

      const auto utf8_length{utf8_codepoint_length(value, position)};
      if (utf8_length < 2) {
        return false;
      }
      previous_was_dot = false;
      atom_started = true;
      position += utf8_length;
    }
    if (position == 0 || previous_was_dot) {
      return false;
    }
  }

  // RFC 5321 §4.5.3.1.1: Local-part octet limit is 64
  if (position > 64) {
    return false;
  }

  // RFC 5321 §4.1.2: Mailbox = Local-part "@" ( Domain / address-literal )
  if (position >= value.size() || value[position] != '@') {
    return false;
  }

  const auto domain{value.substr(position + 1)};

  // RFC 5321 §4.1.3: address-literal = "[" ( IPv4 / IPv6 / General ) "]"
  // Address literals are ASCII-only — no IDNA applies
  if (!domain.empty() && domain.front() == '[') {
    if (domain.back() != ']') {
      return false;
    }
    if (domain.size() > 255) {
      return false;
    }
    const auto inner{domain.substr(1, domain.size() - 2)};
    if (matches_ipv6_tag(inner) && is_ipv6(inner.substr(5))) {
      return true;
    }
    if (inner.find(':') == std::string_view::npos) {
      return is_ipv4(inner);
    }
    return is_general_address_literal(inner);
  }

  // RFC 6531 §3.3: sub-domain =/ U-label
  return is_idn_domain(domain);
}

} // namespace sourcemeta::core
