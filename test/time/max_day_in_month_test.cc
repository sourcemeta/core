#include <gtest/gtest.h>

#include <sourcemeta/core/time.h>

TEST(Time_max_day_in_month, january_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(1, 2024), 31);
}

TEST(Time_max_day_in_month, february_2020_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2020), 29);
}

TEST(Time_max_day_in_month, february_2021_non_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2021), 28);
}

TEST(Time_max_day_in_month, february_2000_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2000), 29);
}

TEST(Time_max_day_in_month, february_1900_non_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 1900), 28);
}

TEST(Time_max_day_in_month, february_year_zero_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 0), 29);
}

TEST(Time_max_day_in_month, march_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(3, 2024), 31);
}

TEST(Time_max_day_in_month, april_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(4, 2024), 30);
}

TEST(Time_max_day_in_month, may_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(5, 2024), 31);
}

TEST(Time_max_day_in_month, june_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(6, 2024), 30);
}

TEST(Time_max_day_in_month, july_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(7, 2024), 31);
}

TEST(Time_max_day_in_month, august_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(8, 2024), 31);
}

TEST(Time_max_day_in_month, september_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(9, 2024), 30);
}

TEST(Time_max_day_in_month, october_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(10, 2024), 31);
}

TEST(Time_max_day_in_month, november_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(11, 2024), 30);
}

TEST(Time_max_day_in_month, december_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(12, 2024), 31);
}
