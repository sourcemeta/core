#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <limits>

#include <sourcemeta/core/time.h>

TEST(Time_unix_timestamp, from_epoch) {
  const auto point{
      sourcemeta::core::from_unix_timestamp(std::chrono::duration<double>{0})};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), std::chrono::system_clock::from_time_t(0));
}

TEST(Time_unix_timestamp, from_positive_integer) {
  const auto point{sourcemeta::core::from_unix_timestamp(
      std::chrono::duration<double>{1700000000})};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(point.value(), std::chrono::system_clock::from_time_t(1700000000));
}

TEST(Time_unix_timestamp, from_negative_integer) {
  const auto point{
      sourcemeta::core::from_unix_timestamp(std::chrono::duration<double>{-1})};
  ASSERT_TRUE(point.has_value());
  // Expressed without `from_time_t(-1)`, as `time_t` is not guaranteed to be
  // signed
  EXPECT_EQ(point.value(), std::chrono::system_clock::from_time_t(0) -
                               std::chrono::seconds{1});
}

TEST(Time_unix_timestamp, from_fractional) {
  const auto point{sourcemeta::core::from_unix_timestamp(
      std::chrono::duration<double>{1.5})};
  ASSERT_TRUE(point.has_value());
  EXPECT_EQ(sourcemeta::core::to_unix_timestamp(point.value()),
            std::chrono::duration<double>{1.5});
}

TEST(Time_unix_timestamp, from_rejects_nan) {
  EXPECT_FALSE(sourcemeta::core::from_unix_timestamp(
                   std::chrono::duration<double>{
                       std::numeric_limits<double>::quiet_NaN()})
                   .has_value());
}

TEST(Time_unix_timestamp, from_rejects_positive_infinity) {
  EXPECT_FALSE(sourcemeta::core::from_unix_timestamp(
                   std::chrono::duration<double>{
                       std::numeric_limits<double>::infinity()})
                   .has_value());
}

TEST(Time_unix_timestamp, from_rejects_negative_infinity) {
  EXPECT_FALSE(sourcemeta::core::from_unix_timestamp(
                   std::chrono::duration<double>{
                       -std::numeric_limits<double>::infinity()})
                   .has_value());
}

TEST(Time_unix_timestamp, from_rejects_out_of_range_positive) {
  EXPECT_FALSE(sourcemeta::core::from_unix_timestamp(
                   std::chrono::duration<double>{1e300})
                   .has_value());
}

TEST(Time_unix_timestamp, from_rejects_out_of_range_negative) {
  EXPECT_FALSE(sourcemeta::core::from_unix_timestamp(
                   std::chrono::duration<double>{-1e300})
                   .has_value());
}

TEST(Time_unix_timestamp, to_epoch) {
  EXPECT_EQ(sourcemeta::core::to_unix_timestamp(
                std::chrono::system_clock::from_time_t(0)),
            std::chrono::duration<double>{0});
}

TEST(Time_unix_timestamp, to_positive_integer) {
  EXPECT_EQ(sourcemeta::core::to_unix_timestamp(
                std::chrono::system_clock::from_time_t(1700000000)),
            std::chrono::duration<double>{1700000000});
}

TEST(Time_unix_timestamp, round_trip_through_time_point) {
  const auto point{std::chrono::system_clock::from_time_t(1700000000)};
  const auto timestamp{sourcemeta::core::to_unix_timestamp(point)};
  const auto restored{sourcemeta::core::from_unix_timestamp(timestamp)};
  ASSERT_TRUE(restored.has_value());
  EXPECT_EQ(restored.value(), point);
}
