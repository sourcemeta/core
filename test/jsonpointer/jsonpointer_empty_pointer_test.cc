#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(is_empty) {
  EXPECT_TRUE(sourcemeta::core::EMPTY_POINTER.empty());
  EXPECT_EQ(sourcemeta::core::EMPTY_POINTER, sourcemeta::core::Pointer{});
}
