#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

TEST(what_is_open_is_reported) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_TRUE(descriptors.open.has_value());
  EXPECT_GT(descriptors.open.value(), 0U);
}
