#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <stdexcept>

#include <sourcemeta/core/time.h>

TEST(Time, time_to_gmt) {
  std::tm parts = {};
  parts.tm_year = 115;
  parts.tm_mon = 9;
  parts.tm_mday = 21;
  parts.tm_hour = 11;
  parts.tm_min = 28;
  parts.tm_sec = 0;
  parts.tm_isdst = 0;

#if defined(_MSC_VER)
  const auto point{std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto point{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif

  EXPECT_EQ(sourcemeta::core::to_gmt(point), "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(Time, gmt_to_time) {
  const auto point{sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT")};

  std::tm parts = {};
  parts.tm_year = 115;
  parts.tm_mon = 9;
  parts.tm_mday = 21;
  parts.tm_hour = 11;
  parts.tm_min = 28;
  parts.tm_sec = 0;
  parts.tm_isdst = 0;

#if defined(_MSC_VER)
  const auto expected{
      std::chrono::system_clock::from_time_t(_mkgmtime(&parts))};
#else
  const auto expected{std::chrono::system_clock::from_time_t(timegm(&parts))};
#endif

  EXPECT_EQ(point, expected);
}

TEST(Time, gmt_e2e) {
  const auto point{sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT")};
  const auto timestamp{sourcemeta::core::to_gmt(point)};
  EXPECT_EQ(timestamp, "Wed, 21 Oct 2015 11:28:00 GMT");
}

TEST(Time, gmt_invalid) {
  EXPECT_THROW(sourcemeta::core::from_gmt("FOO"), std::invalid_argument);
}

TEST(Time, gmt_comparison_equal_1) {
  EXPECT_EQ(sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT"),
            sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT"));
}

TEST(Time, gmt_comparison_equal_2) {
  EXPECT_EQ(sourcemeta::core::from_gmt("Mon, 29 Jul 2024 16:30:29 GMT"),
            sourcemeta::core::from_gmt("Mon, 29 Jul 2024 16:30:29 GMT"));
}

TEST(Time, gmt_comparison_less_than) {
  EXPECT_TRUE(sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:27:00 GMT") <
              sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT"));
}

TEST(Time, gmt_comparison_greater_than) {
  EXPECT_TRUE(sourcemeta::core::from_gmt("Wed, 21 Oct 2100 11:28:00 GMT") >
              sourcemeta::core::from_gmt("Wed, 21 Oct 2015 11:28:00 GMT"));
}
