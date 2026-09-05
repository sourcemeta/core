#include <sourcemeta/core/allocator.h>
#include <sourcemeta/core/test.h>

TEST(Allocator_name_is_one_of_the_allocators_we_ship) {
  const auto name{sourcemeta::core::allocator_name()};
  EXPECT_TRUE(name == "mimalloc" || name == "system");
}

TEST(Allocator_name_is_stable_across_calls) {
  EXPECT_EQ(sourcemeta::core::allocator_name(),
            sourcemeta::core::allocator_name());
}

TEST(Allocator_name_is_the_one_this_build_selected) {
  EXPECT_EQ(sourcemeta::core::allocator_name(), SOURCEMETA_CORE_TEST_ALLOCATOR);
}
