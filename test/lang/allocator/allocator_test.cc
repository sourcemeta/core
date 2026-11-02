#include <sourcemeta/core/allocator.h>
#include <sourcemeta/core/test.h>

TEST(Allocator_is_stable_across_calls) {
  EXPECT_EQ(sourcemeta::core::allocator(), sourcemeta::core::allocator());
}

TEST(Allocator_is_the_one_this_build_selected) {
#ifdef SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC
  EXPECT_EQ(sourcemeta::core::allocator(),
            sourcemeta::core::Allocator::Mimalloc);
#else
  EXPECT_EQ(sourcemeta::core::allocator(), sourcemeta::core::Allocator::System);
#endif
}
