#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

TEST(virtual_memory_covers_at_least_the_resident_set) {
  const auto usage{sourcemeta::core::process_usage()};
  EXPECT_GE(usage.virtual_bytes.value(), usage.resident_bytes.value());
}
