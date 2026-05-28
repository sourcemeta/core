#include <gtest/gtest.h>

#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstdint>     // std::uint64_t
#include <ranges>      // std::views::transform
#include <sstream>     // std::ostringstream
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(Text_join_to, multiple_integers) {
  std::ostringstream stream;
  const std::array<int, 3> values{{1, 2, 3}};
  sourcemeta::core::join_to(stream, values, ", ");
  EXPECT_EQ(stream.str(), "1, 2, 3");
}

TEST(Text_join_to, single_element_no_separator_written) {
  std::ostringstream stream;
  const std::array<int, 1> values{{42}};
  sourcemeta::core::join_to(stream, values, ", ");
  EXPECT_EQ(stream.str(), "42");
}

TEST(Text_join_to, empty_range_writes_nothing) {
  std::ostringstream stream;
  const std::vector<int> values;
  sourcemeta::core::join_to(stream, values, ", ");
  EXPECT_EQ(stream.str(), "");
}

TEST(Text_join_to, empty_separator) {
  std::ostringstream stream;
  const std::array<int, 3> values{{1, 2, 3}};
  sourcemeta::core::join_to(stream, values, "");
  EXPECT_EQ(stream.str(), "123");
}

TEST(Text_join_to, string_views) {
  std::ostringstream stream;
  const std::array<std::string_view, 3> values{{"foo", "bar", "baz"}};
  sourcemeta::core::join_to(stream, values, " | ");
  EXPECT_EQ(stream.str(), "foo | bar | baz");
}

TEST(Text_join_to, transformed_range) {
  std::ostringstream stream;
  const std::array<int, 3> values{{1, 2, 3}};
  const auto doubled{values | std::views::transform(
                                  [](const int value) { return value * 2; })};
  sourcemeta::core::join_to(stream, doubled, ", ");
  EXPECT_EQ(stream.str(), "2, 4, 6");
}

TEST(Text_join_to, widened_byte_values) {
  std::ostringstream stream;
  const std::array<std::uint8_t, 3> values{{1, 2, 3}};
  const auto widened{values |
                     std::views::transform([](const std::uint8_t value) {
                       return static_cast<std::uint64_t>(value);
                     })};
  sourcemeta::core::join_to(stream, widened, ", ");
  EXPECT_EQ(stream.str(), "1, 2, 3");
}
