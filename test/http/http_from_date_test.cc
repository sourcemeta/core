#include <sourcemeta/core/test.h>

#include <chrono>
#include <ctime>

#include <sourcemeta/core/http.h>

namespace {

auto make_nov_6_1994_08_49_37() -> std::chrono::system_clock::time_point {
  std::tm parts = {};
  parts.tm_year = 94;
  parts.tm_mon = 10;
  parts.tm_mday = 6;
  parts.tm_hour = 8;
  parts.tm_min = 49;
  parts.tm_sec = 37;
  parts.tm_isdst = 0;
#if defined(_MSC_VER)
  return std::chrono::system_clock::from_time_t(_mkgmtime(&parts));
#else
  return std::chrono::system_clock::from_time_t(timegm(&parts));
#endif
}

} // namespace

TEST(accepts_imf_fixdate) {
  const auto result{
      sourcemeta::core::http_from_date("Sun, 06 Nov 1994 08:49:37 GMT")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), make_nov_6_1994_08_49_37());
}

TEST(accepts_rfc850_date) {
  const auto result{
      sourcemeta::core::http_from_date("Sunday, 06-Nov-94 08:49:37 GMT")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), make_nov_6_1994_08_49_37());
}

TEST(accepts_asctime) {
  const auto result{
      sourcemeta::core::http_from_date("Sun Nov  6 08:49:37 1994")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), make_nov_6_1994_08_49_37());
}

TEST(rejects_empty) {
  EXPECT_FALSE(sourcemeta::core::http_from_date("").has_value());
}

TEST(rejects_garbage) {
  EXPECT_FALSE(sourcemeta::core::http_from_date("FOO").has_value());
}

TEST(rejects_iso8601) {
  EXPECT_FALSE(
      sourcemeta::core::http_from_date("1994-11-06T08:49:37Z").has_value());
}

TEST(rejects_lowercase_imf_fixdate_gmt) {
  EXPECT_FALSE(sourcemeta::core::http_from_date("Sun, 06 Nov 1994 08:49:37 gmt")
                   .has_value());
}

TEST(accepts_imf_fixdate_epoch) {
  EXPECT_TRUE(sourcemeta::core::http_from_date("Thu, 01 Jan 1970 00:00:00 GMT")
                  .has_value());
}

TEST(accepts_rfc850_recent) {
  EXPECT_TRUE(
      sourcemeta::core::http_from_date("Wednesday, 21-Oct-15 11:28:00 GMT")
          .has_value());
}

TEST(accepts_asctime_two_digit_day) {
  EXPECT_TRUE(
      sourcemeta::core::http_from_date("Wed Oct 21 11:28:00 2015").has_value());
}

TEST(rejects_imf_fixdate_missing_day_of_week) {
  EXPECT_FALSE(
      sourcemeta::core::http_from_date("06 Nov 1994 08:49:37 GMT").has_value());
}

TEST(rejects_truncated_input) {
  EXPECT_FALSE(
      sourcemeta::core::http_from_date("Sun, 06 Nov 1994").has_value());
}

TEST(rejects_garbage_with_correct_length_imf) {
  EXPECT_FALSE(sourcemeta::core::http_from_date("xxx, 06 Nov 1994 08:49:37 GMT")
                   .has_value());
}
