#include <sourcemeta/core/http.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_is_traceparent(const std::string_view value) noexcept -> bool {
  // The dashes sit where the four fields of version 00 meet, and a value
  // carrying fields of a later version must put one where the flags end:
  // "Vendors MUST check that the 2 characters are either the end of the string
  // or a dash". Version ff is excluded by its own comment in the grammar, and
  // version 00 admits nothing beyond the flags
  // https://www.w3.org/TR/trace-context/#versioning-of-traceparent
  if (value.size() < 55 || value[2] != '-' || value[35] != '-' ||
      value[52] != '-' || value.starts_with("ff") ||
      (value.starts_with("00") && value.size() != 55) ||
      (value.size() > 55 && value[55] != '-')) {
    return false;
  }

  // Every field is lowercase hexadecimal, including the version prefix, whose
  // failure to parse is what tells a reader to start a new trace:
  // "it's not 2 hex characters followed by a dash". The flags are read as a
  // bit field rather than compared, so no value of them is rejected here
  // https://www.w3.org/TR/trace-context/#traceparent-header-field-values
  for (std::size_t index = 0; index < 55; ++index) {
    if (index != 2 && index != 35 && index != 52 &&
        (!is_hex_digit(value[index]) ||
         to_lowercase(value[index]) != value[index])) {
      return false;
    }
  }

  // "All bytes as zero (00000000000000000000000000000000) is considered an
  // invalid value", and the same for the eight byte identifier
  // https://www.w3.org/TR/trace-context/#trace-id
  return value.substr(3, 32).find_first_not_of('0') != std::string_view::npos &&
         value.substr(36, 16).find_first_not_of('0') != std::string_view::npos;
}

} // namespace sourcemeta::core
