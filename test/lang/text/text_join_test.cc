#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(no_items) {
  const std::vector<std::string_view> items;
  EXPECT_EQ(sourcemeta::core::join(items, ", "), "");
}

TEST(a_single_item_carries_no_separator) {
  const std::vector<std::string_view> items{"alpha"};
  EXPECT_EQ(sourcemeta::core::join(items, ", "), "alpha");
}

TEST(several_items) {
  const std::vector<std::string_view> items{"alpha", "beta", "gamma"};
  EXPECT_EQ(sourcemeta::core::join(items, ", "), "alpha, beta, gamma");
}

TEST(an_empty_separator) {
  const std::vector<std::string_view> items{"alpha", "beta"};
  EXPECT_EQ(sourcemeta::core::join(items, ""), "alphabeta");
}

TEST(empty_items_still_take_separators) {
  const std::vector<std::string_view> items{"", "", ""};
  EXPECT_EQ(sourcemeta::core::join(items, "-"), "--");
}

TEST(a_multiple_character_separator) {
  const std::vector<std::string_view> items{"alpha", "beta"};
  EXPECT_EQ(sourcemeta::core::join(items, " and "), "alpha and beta");
}

TEST(a_line_terminator_as_the_separator) {
  const std::vector<std::string_view> items{"alpha", "beta"};
  EXPECT_EQ(sourcemeta::core::join(items, "\n"), "alpha\nbeta");
}

TEST(an_array_of_views) {
  constexpr std::array<std::string_view, 3> ITEMS{{"a", "b", "c"}};
  EXPECT_EQ(sourcemeta::core::join(ITEMS, ", "), "a, b, c");
}

TEST(a_vector_of_strings) {
  const std::vector<std::string> items{"alpha", "beta"};
  EXPECT_EQ(sourcemeta::core::join(items, ", "), "alpha, beta");
}
