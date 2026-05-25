#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_is_leap_year, year_zero) {
  EXPECT_TRUE(sourcemeta::core::is_leap_year(0));
}

TEST(Time_is_leap_year, year_one) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(1));
}

TEST(Time_is_leap_year, year_4) {
  EXPECT_TRUE(sourcemeta::core::is_leap_year(4));
}

TEST(Time_is_leap_year, year_100) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(100));
}

TEST(Time_is_leap_year, year_400) {
  EXPECT_TRUE(sourcemeta::core::is_leap_year(400));
}

TEST(Time_is_leap_year, year_1900) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(1900));
}

TEST(Time_is_leap_year, year_2000) {
  EXPECT_TRUE(sourcemeta::core::is_leap_year(2000));
}

TEST(Time_is_leap_year, year_2020) {
  EXPECT_TRUE(sourcemeta::core::is_leap_year(2020));
}

TEST(Time_is_leap_year, year_2021) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(2021));
}

TEST(Time_is_leap_year, year_2100) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(2100));
}

TEST(Time_is_leap_year, year_9999) {
  EXPECT_FALSE(sourcemeta::core::is_leap_year(9999));
}
