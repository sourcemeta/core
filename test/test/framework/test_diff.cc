#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(multiple_lines) {
  const std::string actual{"alpha\nbeta\ngamma\n"};
  const std::string expected{"alpha\ndelta\ngamma\n"};
  EXPECT_EQ(actual, expected);
}

TEST(single_line) {
  const std::string actual{"alpha"};
  const std::string expected{"beta"};
  EXPECT_EQ(actual, expected);
}

TEST(missing_line_terminator) {
  const std::string actual{"alpha\nbeta"};
  const std::string expected{"alpha\nbeta\n"};
  EXPECT_EQ(actual, expected);
}

TEST(unexpected_equality) {
  const std::string actual{"alpha\nbeta\n"};
  const std::string expected{"alpha\nbeta\n"};
  EXPECT_NE(actual, expected);
}

TEST(c_string) { EXPECT_STREQ("alpha\nbeta\n", "alpha\ngamma\n"); }
