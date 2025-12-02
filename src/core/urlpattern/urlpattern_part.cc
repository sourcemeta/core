#include <sourcemeta/core/urlpattern_part.h>

namespace sourcemeta::core {

auto URLPatternPartRegExp::matches(
    const std::string_view segment) const noexcept -> bool {
  // TODO: Avoid this std::string conversion by updating
  // sourcemeta::core::matches to accept std::string_view
  return sourcemeta::core::matches(this->value, std::string{segment});
}

auto URLPatternPartRegExpOptional::matches(
    const std::string_view segment) const noexcept -> bool {
  // TODO: Avoid this std::string conversion by updating
  // sourcemeta::core::matches to accept std::string_view
  return sourcemeta::core::matches(this->value, std::string{segment});
}

auto URLPatternPartRegExpMultiple::matches(
    const std::string_view segment) const noexcept -> bool {
  // TODO: Avoid this std::string conversion by updating
  // sourcemeta::core::matches to accept std::string_view
  return sourcemeta::core::matches(this->value, std::string{segment});
}

auto URLPatternPartRegExpAsterisk::matches(
    const std::string_view segment) const noexcept -> bool {
  // TODO: Avoid this std::string conversion by updating
  // sourcemeta::core::matches to accept std::string_view
  return sourcemeta::core::matches(this->value, std::string{segment});
}

auto URLPatternPartChar::matches(const std::string_view segment) const noexcept
    -> bool {
  return segment == this->value;
}

auto URLPatternPartName::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartNameRegExp::matches(
    const std::string_view segment) const noexcept -> bool {
  // TODO: Avoid this std::string conversion by updating
  // sourcemeta::core::matches to accept std::string_view
  return sourcemeta::core::matches(this->modifier, std::string{segment});
}

auto URLPatternPartNameOptional::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartNameMultiple::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartNameAsterisk::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartAsterisk::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartAsteriskOptional::matches(
    const std::string_view) const noexcept -> bool {
  return true;
}

auto URLPatternPartAsteriskMultiple::matches(
    const std::string_view) const noexcept -> bool {
  return true;
}

auto URLPatternPartAsteriskAsterisk::matches(
    const std::string_view) const noexcept -> bool {
  return true;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
auto URLPatternPartGroup::matches(const std::string_view segment) const noexcept
    -> bool {
  if (this->value.size() == 1 &&
      std::holds_alternative<URLPatternPartChar>(this->value.front())) {
    const auto &char_token = std::get<URLPatternPartChar>(this->value.front());
    return char_token.value == segment;
  }

  return true;
}

auto URLPatternPartGroupOptional::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartGroupMultiple::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartGroupAsterisk::matches(const std::string_view) const noexcept
    -> bool {
  return true;
}

auto URLPatternPartComplexSegment::matches(
    const std::string_view) const noexcept -> bool {
  return true;
}

} // namespace sourcemeta::core
