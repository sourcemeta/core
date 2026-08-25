#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>  // open, O_RDONLY
#include <unistd.h> // close
#endif

TEST(what_is_open_is_reported) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_TRUE(descriptors.open.has_value());
  EXPECT_GT(descriptors.open.value(), 0U);
}

// Windows enforces no per-process handle ceiling comparable to RLIMIT_NOFILE
#if defined(_WIN32)
TEST(the_maximum_is_absent_where_the_platform_enforces_none) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_FALSE(descriptors.maximum.has_value());
}
#endif

// The two platforms that count descriptors rather than handles, and that
// enforce a ceiling on them
#if defined(__linux__) || defined(__APPLE__)
TEST(the_maximum_is_reported_and_finite) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_TRUE(descriptors.maximum.has_value());
  EXPECT_GT(descriptors.maximum.value(), 0U);
}

TEST(what_is_open_stays_within_the_maximum) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_LE(descriptors.open.value(), descriptors.maximum.value());
}

TEST(what_is_open_grows_by_exactly_what_was_opened) {
  const auto before{sourcemeta::core::process_descriptors()};
  const auto first{open("/dev/null", O_RDONLY)};
  const auto second{open("/dev/null", O_RDONLY)};
  const auto third{open("/dev/null", O_RDONLY)};
  const auto after{sourcemeta::core::process_descriptors()};
  close(first);
  close(second);
  close(third);
  const auto restored{sourcemeta::core::process_descriptors()};
  EXPECT_GE(first, 0);
  EXPECT_GE(second, 0);
  EXPECT_GE(third, 0);
  EXPECT_EQ(after.open.value(), before.open.value() + 3);
  EXPECT_EQ(restored.open.value(), before.open.value());
}
#endif
