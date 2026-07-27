#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

TEST(year_zero) { EXPECT_TRUE(sourcemeta::core::is_leap_year(0)); }

TEST(year_one) { EXPECT_FALSE(sourcemeta::core::is_leap_year(1)); }

TEST(year_4) { EXPECT_TRUE(sourcemeta::core::is_leap_year(4)); }

TEST(year_100) { EXPECT_FALSE(sourcemeta::core::is_leap_year(100)); }

TEST(year_400) { EXPECT_TRUE(sourcemeta::core::is_leap_year(400)); }

TEST(year_1900) { EXPECT_FALSE(sourcemeta::core::is_leap_year(1900)); }

TEST(year_2000) { EXPECT_TRUE(sourcemeta::core::is_leap_year(2000)); }

TEST(year_2020) { EXPECT_TRUE(sourcemeta::core::is_leap_year(2020)); }

TEST(year_2021) { EXPECT_FALSE(sourcemeta::core::is_leap_year(2021)); }

TEST(year_2100) { EXPECT_FALSE(sourcemeta::core::is_leap_year(2100)); }

TEST(year_9999) { EXPECT_FALSE(sourcemeta::core::is_leap_year(9999)); }
