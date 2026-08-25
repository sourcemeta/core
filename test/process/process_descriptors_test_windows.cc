#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

TEST(the_maximum_is_absent_where_the_platform_enforces_none) {
  const auto descriptors{sourcemeta::core::process_descriptors()};
  EXPECT_FALSE(descriptors.maximum.has_value());
}
