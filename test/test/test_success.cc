#include <sourcemeta/core/test.h>

TEST(arithmetic) { EXPECT_EQ(1 + 1, 2); }

TEST(truth) { EXPECT_TRUE(true); }

TEST(ordering) { EXPECT_LT(1, 2); }
