#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/test.h>

TEST(out_of_range_error_message) {
  const sourcemeta::core::NumericOutOfRangeError error{};
  EXPECT_STREQ(error.what(), "Numeric value is out of range");
}

TEST(out_of_memory_error_message) {
  const sourcemeta::core::NumericOutOfMemoryError error{};
  EXPECT_STREQ(error.what(), "Out of memory");
}
