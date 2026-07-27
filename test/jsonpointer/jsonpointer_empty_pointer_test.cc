#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(is_empty) {
  EXPECT_TRUE(sourcemeta::core::empty_pointer.empty());
  EXPECT_EQ(sourcemeta::core::empty_pointer, sourcemeta::core::Pointer{});
}
