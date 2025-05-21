#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <sstream>

TEST(URI_unescape, example_1) {
  std::istringstream input{"/c%25d"};
  std::ostringstream output;
  sourcemeta::core::URI::unescape(input, output);
  EXPECT_EQ(output.str(), "/c%d");
}

TEST(URI_unescape, example_2) {
  std::istringstream input{"foo%20bar"};
  std::ostringstream output;
  sourcemeta::core::URI::unescape(input, output);
  EXPECT_EQ(output.str(), "foo bar");
}
