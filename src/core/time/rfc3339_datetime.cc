#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/time.h>

#include <cstdint> // std::uint8_t, std::uint16_t

namespace sourcemeta::core {

auto is_rfc3339_datetime(const std::string_view value) -> bool {
  const auto size{value.size()};

  // Minimum valid date-time: "YYYY-MM-DDTHH:MM:SSZ" = 20 characters
  if (size < 20) {
    return false;
  }

  // --- full-date: date-fullyear "-" date-month "-" date-mday ---
  if (!is_rfc3339_fulldate(value.substr(0, 10))) {
    return false;
  }

  const auto year{static_cast<std::uint16_t>(
      (value[0] - '0') * 1000 + (value[1] - '0') * 100 + (value[2] - '0') * 10 +
      (value[3] - '0'))};
  const auto month{
      static_cast<std::uint8_t>((value[5] - '0') * 10 + (value[6] - '0'))};
  const auto day{
      static_cast<std::uint8_t>((value[8] - '0') * 10 + (value[9] - '0'))};

  std::string_view::size_type position{10};

  // --- "T" or "t" separator ---
  if (value[position] != 'T' && value[position] != 't') {
    return false;
  }
  position += 1;

  // --- partial-time: time-hour ":" time-minute ":" time-second ---

  // time-hour = 2DIGIT ; 00-23
  if (!is_digit(value[position]) || !is_digit(value[position + 1])) {
    return false;
  }
  const auto hour{static_cast<unsigned int>(value[position] - '0') * 10 +
                  static_cast<unsigned int>(value[position + 1] - '0')};
  if (hour > 23) {
    return false;
  }
  position += 2;

  // ":"
  if (value[position] != ':') {
    return false;
  }
  position += 1;

  // time-minute = 2DIGIT ; 00-59
  if (!is_digit(value[position]) || !is_digit(value[position + 1])) {
    return false;
  }
  const auto minute{static_cast<unsigned int>(value[position] - '0') * 10 +
                    static_cast<unsigned int>(value[position + 1] - '0')};
  if (minute > 59) {
    return false;
  }
  position += 2;

  // ":"
  if (value[position] != ':') {
    return false;
  }
  position += 1;

  // time-second = 2DIGIT ; 00-60 (60 = leap second per §5.7)
  if (!is_digit(value[position]) || !is_digit(value[position + 1])) {
    return false;
  }
  const auto second{static_cast<unsigned int>(value[position] - '0') * 10 +
                    static_cast<unsigned int>(value[position + 1] - '0')};
  if (second > 60) {
    return false;
  }
  position += 2;

  // --- [time-secfrac] = "." 1*DIGIT ---
  if (position < size && value[position] == '.') {
    position += 1;
    if (position >= size || !is_digit(value[position])) {
      // "." must be followed by at least 1 digit
      return false;
    }
    while (position < size && is_digit(value[position])) {
      position += 1;
    }
  }

  // --- time-offset = "Z" / time-numoffset ---
  if (position >= size) {
    // No time offset present — invalid
    return false;
  }

  char offset_sign{'+'};
  unsigned int offset_hour{0};
  unsigned int offset_minute{0};

  if (value[position] == 'Z' || value[position] == 'z') {
    position += 1;
  } else if (value[position] == '+' || value[position] == '-') {
    offset_sign = value[position];
    position += 1;

    // time-numoffset = ("+" / "-") time-hour ":" time-minute
    if (position + 5 > size) {
      return false;
    }

    // Offset time-hour = 2DIGIT ; 00-23
    if (!is_digit(value[position]) || !is_digit(value[position + 1])) {
      return false;
    }
    offset_hour = static_cast<unsigned int>(value[position] - '0') * 10 +
                  static_cast<unsigned int>(value[position + 1] - '0');
    if (offset_hour > 23) {
      return false;
    }
    position += 2;

    // ":" — REQUIRED (colonless offsets like +0530 are invalid per §5.6)
    if (value[position] != ':') {
      return false;
    }
    position += 1;

    // Offset time-minute = 2DIGIT ; 00-59
    if (!is_digit(value[position]) || !is_digit(value[position + 1])) {
      return false;
    }
    offset_minute = static_cast<unsigned int>(value[position] - '0') * 10 +
                    static_cast<unsigned int>(value[position + 1] - '0');
    if (offset_minute > 59) {
      return false;
    }
    position += 2;
  } else {
    // Not Z, not +/-, invalid character for time-offset
    return false;
  }

  // String must be fully consumed — no trailing characters
  if (position != size) {
    return false;
  }

  // --- Validate leap second (§5.7) ---
  // The value 60 is only legal at the end of months in which a leap second
  // occurs: June (XXXX-06-30T23:59:60Z) or December (XXXX-12-31T23:59:60Z),
  // evaluated in UTC after applying the time offset
  if (second == 60) {
    const auto local_minute_of_day{hour * 60 + minute};
    const auto offset_total_minutes{offset_hour * 60 + offset_minute};

    unsigned int utc_minute_of_day{0};
    bool previous_utc_day{false};

    if (offset_sign == '+') {
      if (local_minute_of_day >= offset_total_minutes) {
        utc_minute_of_day = local_minute_of_day - offset_total_minutes;
      } else {
        utc_minute_of_day = local_minute_of_day + 1440 - offset_total_minutes;
        previous_utc_day = true;
      }
    } else {
      utc_minute_of_day = (local_minute_of_day + offset_total_minutes) % 1440;
    }

    if (utc_minute_of_day != 23 * 60 + 59) {
      return false;
    }

    // A "next UTC day" shift cannot coexist with UTC time 23:59, since
    // max(local) + max(offset) = 1439 + 1439 = 2878 < 1440 + 1439

    std::uint8_t utc_month{month};
    std::uint8_t utc_day{day};
    if (previous_utc_day) {
      if (utc_day > 1) {
        utc_day -= 1;
      } else if (utc_month > 1) {
        utc_month -= 1;
        utc_day = max_day_in_month(utc_month, year);
      } else {
        // Going back from year 0000 January 1 would yield year -1
        if (year == 0) {
          return false;
        }
        utc_month = 12;
        utc_day = 31;
      }
    }

    if (!((utc_month == 6 && utc_day == 30) ||
          (utc_month == 12 && utc_day == 31))) {
      return false;
    }
  }

  return true;
}

} // namespace sourcemeta::core
