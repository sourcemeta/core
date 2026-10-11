#include <sourcemeta/core/http.h>
#include <sourcemeta/core/text.h>

#include <algorithm>   // std::all_of
#include <cstddef>     // std::size_t
#include <set>         // std::set
#include <string_view> // std::string_view

namespace {

// A key is either a plain identifier or a tenant and a system joined by an at
// sign, and the three differ in how long they may be and in what may start
// them: only a tenant admits a leading digit, where the other two require a
// lowercase letter
// https://www.w3.org/TR/trace-context/#key
auto key_part(const std::string_view value, const bool tenant,
              const std::size_t maximum) noexcept -> bool {
  if (value.empty() || value.size() > maximum ||
      !((value.front() >= 'a' && value.front() <= 'z') ||
        (tenant && sourcemeta::core::is_digit(value.front())))) {
    return false;
  }
  return std::all_of(value.begin(), value.end(), [](const char character) {
    return (character >= 'a' && character <= 'z') ||
           sourcemeta::core::is_digit(character) || character == '_' ||
           character == '-' || character == '*' || character == '/';
  });
}

} // namespace

namespace sourcemeta::core {

auto http_is_tracestate(const std::string_view value) noexcept -> bool {
  std::set<std::string_view> keys;
  std::size_t members = 0;
  auto rest{value};
  bool first = true;
  while (true) {
    // "There can be a maximum of 32 list-members in a list", counted including
    // the empty ones, since each is a list-member of its own
    // https://www.w3.org/TR/trace-context/#tracestate-header-field-values
    if (++members > 32) {
      return false;
    }
    const auto comma{rest.find(',')};
    const auto last{comma == std::string_view::npos};

    // Optional whitespace belongs to the list production, which places it only
    // on either side of a comma, so the first member cannot lead with it and
    // the last cannot trail it. That is what makes a value ending in a space
    // malformed rather than merely untidy, and the upstream reference parser
    // agrees, splitting on whitespace adjacent to a comma alone
    // https://www.w3.org/TR/trace-context/#list
    auto entry{rest.substr(0, comma)};
    if (!first) {
      entry = http_trim_leading_ows(entry);
    }
    if (!last) {
      entry = http_trim_trailing_ows(entry);
    }
    first = false;

    // "Empty and whitespace-only list members are allowed. Vendors MUST accept
    // empty tracestate headers but SHOULD avoid sending them". Baggage forbids
    // both, so a reader coming from that parser should not expect this
    // https://www.w3.org/TR/trace-context/#tracestate-header-field-values
    if (!entry.empty()) {
      const auto equal{entry.find('=')};
      if (equal == std::string_view::npos) {
        return false;
      }
      const auto key{entry.substr(0, equal)};
      const auto content{entry.substr(equal + 1)};
      const auto at_position{key.find('@')};

      // A simple key is one character plus 255, a tenant 1 plus 240 and a
      // system 1 plus 13, which is where the three bounds come from
      // https://www.w3.org/TR/trace-context/#key
      const bool valid_key{
          at_position == std::string_view::npos
              ? key_part(key, false, 256)
              : key_part(key.substr(0, at_position), true, 241) &&
                    key_part(key.substr(at_position + 1), false, 14)};

      // A repeated key is rejected, which is stricter than the grammar. The
      // grammar permits one and the only guidance for a reader is
      // non-normative, but a key records a position in the trace and "Only one
      // entry per key is allowed", so a repeat is a producer that appended
      // where it should have overwritten. Baggage is the opposite again, where
      // duplicates are legal and their order carries meaning
      // https://www.w3.org/TR/trace-context/#combined-header-value
      if (!valid_key || !keys.insert(key).second) {
        return false;
      }

      // A value is at most 256 characters and ends in a mandatory non-blank
      // one, so it can neither be empty nor end in a space, and it excludes
      // the comma at %x2C and the equals at %x3D that the two ranges of
      // nblk-chr leave out. Baggage permits an empty value
      // https://www.w3.org/TR/trace-context/#value
      if (content.empty() || content.size() > 256 || content.back() == ' ' ||
          !std::all_of(content.begin(), content.end(),
                       [](const char character) {
                         return character >= 0x20 && character <= 0x7e &&
                                character != ',' && character != '=';
                       })) {
        return false;
      }
    }

    if (last) {
      return true;
    }
    rest.remove_prefix(comma + 1);
  }
}

} // namespace sourcemeta::core
