#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::hours, std::chrono::system_clock

TEST(the_start_time_is_reported) {
  const auto started{sourcemeta::core::process_start_time()};
  EXPECT_TRUE(started.has_value());
}

TEST(the_start_time_is_the_same_on_every_call) {
  const auto first{sourcemeta::core::process_start_time()};
  const auto second{sourcemeta::core::process_start_time()};
  EXPECT_EQ(first.value(), second.value());
}

TEST(the_start_time_is_after_the_unix_epoch) {
  const auto started{sourcemeta::core::process_start_time()};
  EXPECT_GT(started.value(), std::chrono::system_clock::from_time_t(0));
}

TEST(the_start_time_is_not_in_the_future) {
  const auto started{sourcemeta::core::process_start_time()};
  EXPECT_LE(started.value(), std::chrono::system_clock::now());
}

TEST(the_start_time_is_within_the_hour_this_test_began) {
  const auto started{sourcemeta::core::process_start_time()};
  EXPECT_GT(started.value(),
            std::chrono::system_clock::now() - std::chrono::hours{1});
}
