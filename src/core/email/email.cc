#include <sourcemeta/core/email.h>

#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include "helpers.h"

namespace sourcemeta::core {

// RFC 5321 §4.1.2 Mailbox grammar, returning the position of the separator
// between the local part and the domain when the mailbox is valid. When
// AllowUtf8 is true, RFC 6531 §3.3 extends atext, qtextSMTP, and sub-domain
// with UTF8-non-ascii alternatives. When UseUts46 is also true, the domain is
// validated under UTS #46 processing rather than strict IDNA 2008.
template <bool AllowUtf8, bool UseUts46 = false>
static auto mailbox_separator(const std::string_view value)
    -> std::optional<std::string_view::size_type> {
  if (value.empty()) {
    return std::nullopt;
  }

  // RFC 5321 §4.5.3.1.3: a path is at most 256 octets including the enclosing
  // angle brackets, so the mailbox it carries is at most 254
  if (value.size() > 254) {
    return std::nullopt;
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
          return std::nullopt;
        }
        const auto body{static_cast<unsigned char>(value[position])};
        if (body < 32 || body > 126) {
          return std::nullopt;
        }
        position += 1;
        continue;
      }

      if (is_qtext_smtp(static_cast<unsigned char>(value[position]))) {
        position += 1;
        continue;
      }

      if constexpr (AllowUtf8) {
        // RFC 6531 §3.3: qtextSMTP =/ UTF8-non-ascii
        const auto utf8_length{utf8_codepoint_length(value, position)};
        if (utf8_length < 2) {
          return std::nullopt;
        }
        position += utf8_length;
      } else {
        return std::nullopt;
      }
    }
    if (position >= value.size()) {
      return std::nullopt;
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
          return std::nullopt;
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

      if constexpr (AllowUtf8) {
        // RFC 6531 §3.3: atext =/ UTF8-non-ascii
        const auto utf8_length{utf8_codepoint_length(value, position)};
        if (utf8_length < 2) {
          return std::nullopt;
        }
        previous_was_dot = false;
        atom_started = true;
        position += utf8_length;
      } else {
        return std::nullopt;
      }
    }
    if (position == 0 || previous_was_dot) {
      return std::nullopt;
    }
  }

  // RFC 5321 §4.5.3.1.1: Local-part octet limit is 64
  if (position > 64) {
    return std::nullopt;
  }

  // RFC 5321 §4.1.2: Mailbox = Local-part "@" ( Domain / address-literal )
  if (position >= value.size() || value[position] != '@') {
    return std::nullopt;
  }

  const auto domain{value.substr(position + 1)};

  // RFC 5321 §4.1.3: address-literal = "[" ( IPv4 / IPv6 / General ) "]"
  if (!domain.empty() && domain.front() == '[') {
    if (is_address_literal(domain)) {
      return position;
    }
    return std::nullopt;
  }

  if constexpr (AllowUtf8) {
    // RFC 6531 §3.3: sub-domain =/ U-label
    if constexpr (UseUts46) {
      if (is_idn_hostname_uts46(domain)) {
        return position;
      }
    } else {
      if (is_idn_hostname(domain)) {
        return position;
      }
    }
    return std::nullopt;
  } else {
    // RFC 5321 §4.1.2 Domain matches is_hostname (RFC 1123 §2.1) by
    // grammar, by 63-octet label cap (RFC 1035 §2.3.4), and by
    // 255-octet total cap (RFC 5321 §4.5.3.1.2)
    if (is_hostname(domain)) {
      return position;
    }
    return std::nullopt;
  }
}

auto is_email(const std::string_view value) -> bool {
  return mailbox_separator<false>(value).has_value();
}

auto is_idn_email(const std::string_view value) -> bool {
  return mailbox_separator<true>(value).has_value();
}

auto is_idn_email_uts46(const std::string_view value) -> bool {
  return mailbox_separator<true, true>(value).has_value();
}

auto mailto_iri(const std::string_view value) -> std::optional<std::string> {
  const auto separator{mailbox_separator<false>(value)};
  if (!separator.has_value()) {
    return std::nullopt;
  }

  std::string result;
  result.reserve(value.size() * 3 + 7);
  result.append("mailto:");
  for (std::string_view::size_type position{0}; position < value.size();
       position += 1) {
    const auto character{value[position]};
    if (position == separator.value() || is_mailto_verbatim(character)) {
      result.push_back(character);
    } else {
      percent_encode(static_cast<unsigned char>(character), result);
    }
  }

  return result;
}

auto acct_iri(const std::string_view value) -> std::optional<std::string> {
  const auto parts{rsplit_once(value, '@')};
  if (!parts.has_value() || parts->first.empty()) {
    return std::nullopt;
  }

  // RFC 7565 §6 requires the userpart to conform to the PRECIS
  // IdentifierClass, whose ASCII repertoire is %x21-7E (RFC 7564 §9.11)
  for (const auto character : parts->first) {
    const auto byte{static_cast<unsigned char>(character)};
    if (byte < 0x21 || byte > 0x7E) {
      return std::nullopt;
    }
  }

  // RFC 7565 §4: the host portion is the DNS domain name of the service
  // provider
  if (!is_hostname(parts->second)) {
    return std::nullopt;
  }

  std::string result;
  result.reserve(value.size() * 3 + 5);
  result.append("acct:");
  for (const auto character : parts->first) {
    if (is_acct_userpart_verbatim(character)) {
      result.push_back(character);
    } else {
      percent_encode(static_cast<unsigned char>(character), result);
    }
  }
  result.push_back('@');
  // RFC 7565 §4: acct URIs compare under RFC 3986 §6.2.2.1 case
  // normalization, so the canonical spelling lowercases the host
  for (const auto character : parts->second) {
    result.push_back(to_lowercase(character));
  }

  return result;
}

} // namespace sourcemeta::core
