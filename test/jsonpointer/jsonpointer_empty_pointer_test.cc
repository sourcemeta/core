#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(is_empty) {
  EXPECT_TRUE(sourcemeta::core::empty_pointer.empty());
  EXPECT_EQ(sourcemeta::core::empty_pointer, sourcemeta::core::Pointer{});
}
