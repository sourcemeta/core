#include <sourcemeta/core/test.h>

namespace {
int live_instances{0};
} // namespace

class ExampleFixture {
protected:
  ExampleFixture() { live_instances += 1; }
  ~ExampleFixture() { live_instances -= 1; }
  int value_{42};
};

TEST_F(ExampleFixture, sees_constructed_member) {
  EXPECT_EQ(this->value_, 42);
  EXPECT_EQ(live_instances, 1);
}

TEST_F(ExampleFixture, teardown_runs_between_tests) {
  EXPECT_EQ(this->value_, 42);
  EXPECT_EQ(live_instances, 1);
}
