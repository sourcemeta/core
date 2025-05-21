#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_empty_pointer, is_empty) {
  EXPECT_TRUE(sourcemeta::core::empty_pointer.empty());
  EXPECT_EQ(sourcemeta::core::empty_pointer, sourcemeta::core::Pointer{});
}
