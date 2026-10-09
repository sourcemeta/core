#include <sourcemeta/core/test.h>

namespace {

// Compares but does not stream, which is what it takes to reach the arm that
// stands in for a value it cannot describe
struct Opaque {
  int value{0};
  auto operator==(const Opaque &other) const -> bool {
    return this->value == other.value;
  }
};

} // namespace

TEST(truth) { EXPECT_TRUE(1 == 2); }

TEST(falsehood) { EXPECT_FALSE(1 == 1); }

TEST(double_approximate) { EXPECT_DOUBLE_EQ(1.0, 2.0); }

TEST(float_approximate) { EXPECT_FLOAT_EQ(1.0F, 2.0F); }

TEST(absent_string) { EXPECT_STREQ(nullptr, "beta"); }

TEST(unprintable) {
  const Opaque actual{1};
  const Opaque expected{2};
  EXPECT_EQ(actual, expected);
}
