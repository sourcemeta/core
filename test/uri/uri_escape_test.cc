#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/uri.h>
#include <sstream>

TEST(URI_escape, example_1) {
  std::istringstream input{"foo bar"};
  std::ostringstream output;
  sourcemeta::jsontoolkit::URI::escape(input, output);
  EXPECT_EQ(output.str(), "foo%20bar");
}
