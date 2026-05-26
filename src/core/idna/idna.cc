#include <sourcemeta/core/idna.h>

#include <sourcemeta/core/unicode.h>

#include <cstddef>     // std::size_t
#include <string_view> // std::u32string_view

namespace sourcemeta::core {

auto idna_passes_contexto(const std::u32string_view label,
                          const std::size_t position) -> bool {
  if (position >= label.size()) {
    return false;
  }

  const auto codepoint{label[position]};

  switch (codepoint) {
    // RFC 5892 Appendix A.3 MIDDLE DOT (U+00B7)
    case 0x00B7: {
      if (position == 0 || position + 1 >= label.size()) {
        return false;
      }
      return label[position - 1] == 0x006C && label[position + 1] == 0x006C;
    }

    // RFC 5892 Appendix A.4 GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
    case 0x0375: {
      if (position + 1 >= label.size()) {
        return false;
      }
      return script(label[position + 1]) == UnicodeScript::Greek;
    }

    // RFC 5892 Appendix A.5 HEBREW PUNCTUATION GERESH (U+05F3)
    // RFC 5892 Appendix A.6 HEBREW PUNCTUATION GERSHAYIM (U+05F4)
    case 0x05F3:
    case 0x05F4: {
      if (position == 0) {
        return false;
      }
      return script(label[position - 1]) == UnicodeScript::Hebrew;
    }

    // RFC 5892 Appendix A.7 KATAKANA MIDDLE DOT (U+30FB)
    case 0x30FB: {
      for (const auto other : label) {
        const auto other_script{script(other)};
        if (other_script == UnicodeScript::Hiragana ||
            other_script == UnicodeScript::Katakana ||
            other_script == UnicodeScript::Han) {
          return true;
        }
      }
      return false;
    }

    default:
      break;
  }

  // RFC 5892 Appendix A.8 ARABIC-INDIC DIGITS (U+0660..U+0669)
  if (codepoint >= 0x0660 && codepoint <= 0x0669) {
    for (const auto other : label) {
      if (other >= 0x06F0 && other <= 0x06F9) {
        return false;
      }
    }
    return true;
  }

  // RFC 5892 Appendix A.9 EXTENDED ARABIC-INDIC DIGITS (U+06F0..U+06F9)
  if (codepoint >= 0x06F0 && codepoint <= 0x06F9) {
    for (const auto other : label) {
      if (other >= 0x0660 && other <= 0x0669) {
        return false;
      }
    }
    return true;
  }

  // No RFC 5892 Appendix A.3-A.9 rule applies to this codepoint, so there
  // is nothing to violate.
  return true;
}

} // namespace sourcemeta::core
