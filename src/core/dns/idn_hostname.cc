#include <sourcemeta/core/dns.h>

#include <sourcemeta/core/idna.h>
#include <sourcemeta/core/unicode.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string, std::u32string
#include <string_view> // std::string_view, std::u32string_view
#include <vector>      // std::vector

namespace sourcemeta::core {

// UTS #46 §3.1: the four IDNA label separators
static constexpr auto is_idna_label_separator(const char32_t codepoint)
    -> bool {
  return codepoint == U'.' || codepoint == U'。' || codepoint == U'．' ||
         codepoint == U'｡';
}

// Validate one label per RFC 5891 §4.2 plus the DNS-specific RFC 1035
// length and RFC 1123 ASCII LDH constraints. Stores the U-label codepoint
// form in `decoded` for the later Bidi rule check.
static auto validate_label(const std::u32string_view label,
                           std::u32string &decoded) -> bool {
  // RFC 1035 §2.3.4: every label must be small enough that the
  // corresponding A-label form fits in 63 octets. The codepoint count is an
  // upper bound that catches all overly-long labels in practice.
  if (label.size() > 63) {
    return false;
  }

  const auto kind{idna_classify_label(label, decoded)};
  if (!kind.has_value()) {
    return false;
  }

  // RFC 1123 §2.1: ASCII labels still need the LDH grammar check
  if (*kind == IDNALabelKind::Ascii) {
    std::string ascii;
    ascii.reserve(label.size());
    for (const auto codepoint : label) {
      ascii.push_back(static_cast<char>(codepoint));
    }
    if (!is_hostname(ascii)) {
      return false;
    }
  }

  return true;
}

auto is_idn_hostname(const std::string_view value) -> bool {
  // TODO: Once NFC normalisation lands, also exercise this implementation
  // against `vendor/unicodetools/IdnaTestV2.txt`. The current test corpus is
  // inspired by the JSON Schema idn-hostname format test suite, whose inputs
  // are already in NFC.

  if (value.empty()) {
    return false;
  }

  // RFC 1123 §2.1: SHOULD handle host names of up to 255 characters
  if (value.size() > 255) {
    return false;
  }

  const auto codepoints{utf8_to_utf32(value)};
  if (!codepoints.has_value() || codepoints->empty()) {
    return false;
  }

  // UTS #46 §3.1: a leading or trailing separator means an empty label
  if (is_idna_label_separator(codepoints->front()) ||
      is_idna_label_separator(codepoints->back())) {
    return false;
  }

  std::vector<std::u32string> decoded_labels;
  std::size_t label_start{0};
  for (std::size_t position = 0; position <= codepoints->size(); ++position) {
    const bool at_end{position == codepoints->size()};
    if (!at_end && !is_idna_label_separator((*codepoints)[position])) {
      continue;
    }
    const std::u32string_view label{codepoints->data() + label_start,
                                    position - label_start};
    std::u32string decoded;
    if (!validate_label(label, decoded)) {
      return false;
    }
    decoded_labels.push_back(std::move(decoded));
    label_start = position + 1;
  }

  // RFC 5893 §1.4: a Bidi domain name has at least one codepoint with
  // Bidi_Class R, AL, or AN
  bool is_bidi_domain{false};
  for (const auto &label : decoded_labels) {
    for (const auto codepoint : label) {
      const auto class_value{bidi_class(codepoint)};
      if (class_value == BidiClass::RightToLeft ||
          class_value == BidiClass::ArabicLetter ||
          class_value == BidiClass::ArabicNumber) {
        is_bidi_domain = true;
        break;
      }
    }
    if (is_bidi_domain) {
      break;
    }
  }

  // RFC 5893 §2: every label of a Bidi domain must satisfy the Bidi rule
  if (is_bidi_domain) {
    for (const auto &label : decoded_labels) {
      if (!idna_passes_bidi_rule(label)) {
        return false;
      }
    }
  }

  return true;
}

} // namespace sourcemeta::core
