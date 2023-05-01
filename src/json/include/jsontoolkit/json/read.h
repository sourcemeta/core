#ifndef JSONTOOLKIT_JSON_READ_H_
#define JSONTOOLKIT_JSON_READ_H_

#if defined(JSONTOOLKIT_BACKEND_RAPIDJSON)
#include "rapidjson/read.h"
#else
#error Unknown JSON Toolkit backend
#endif

#include <algorithm> // std::any_of, std::transform

namespace sourcemeta::jsontoolkit {

inline auto is_number(const Value &value) -> bool {
  return is_integer(value) || is_real(value);
}

template <typename Iterator>
auto defines_any(const Value &value, Iterator begin, Iterator end) -> bool {
  return std::any_of(begin, end, [&value](const auto &keyword) {
    return sourcemeta::jsontoolkit::defines(value, keyword);
  });
}

inline auto defines_any(const Value &value,
                        std::initializer_list<std::string> keys) -> bool {
  return defines_any(value, keys.begin(), keys.end());
}

template <typename T>
auto defines_any(const Value &value, const T &keys) -> bool {
  return defines_any(value, std::cbegin(keys), std::cend(keys));
}

template <typename InputIt, typename OutputIt>
auto copy(InputIt begin, InputIt end, OutputIt output) -> void {
  std::transform(begin, end, output,
                 [](const auto &json) { return from(json); });
}

} // namespace sourcemeta::jsontoolkit

#endif
