#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <fcntl.h>  // open, O_RDONLY
#include <unistd.h> // close

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
