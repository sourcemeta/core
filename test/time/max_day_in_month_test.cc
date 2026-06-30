#include <sourcemeta/core/test.h>

#include <sourcemeta/core/time.h>

TEST(january_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(1, 2024), 31);
}

TEST(february_2020_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2020), 29);
}

TEST(february_2021_non_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2021), 28);
}

TEST(february_2000_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 2000), 29);
}

TEST(february_1900_non_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 1900), 28);
}

TEST(february_year_zero_leap) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(2, 0), 29);
}

TEST(march_2024) { EXPECT_EQ(sourcemeta::core::max_day_in_month(3, 2024), 31); }

TEST(april_2024) { EXPECT_EQ(sourcemeta::core::max_day_in_month(4, 2024), 30); }

TEST(may_2024) { EXPECT_EQ(sourcemeta::core::max_day_in_month(5, 2024), 31); }

TEST(june_2024) { EXPECT_EQ(sourcemeta::core::max_day_in_month(6, 2024), 30); }

TEST(july_2024) { EXPECT_EQ(sourcemeta::core::max_day_in_month(7, 2024), 31); }

TEST(august_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(8, 2024), 31);
}

TEST(september_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(9, 2024), 30);
}

TEST(october_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(10, 2024), 31);
}

TEST(november_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(11, 2024), 30);
}

TEST(december_2024) {
  EXPECT_EQ(sourcemeta::core::max_day_in_month(12, 2024), 31);
}
