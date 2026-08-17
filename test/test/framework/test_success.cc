#include <sourcemeta/core/test.h>

TEST(arithmetic) { EXPECT_EQ(1 + 1, 2); }

TEST(truth) { EXPECT_TRUE(true); }

TEST(ordering) { EXPECT_LT(1, 2); }

TEST(double_approximate) { EXPECT_DOUBLE_EQ(0.1 + 0.2, 0.3); }

TEST(float_approximate) { EXPECT_FLOAT_EQ(0.1F + 0.2F, 0.3F); }

TEST(string_equal) { EXPECT_STREQ("hello", "hello"); }
