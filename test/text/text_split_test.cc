#include <sourcemeta/core/test.h>

#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

TEST(multiple_parts) {
  std::vector<std::string> parts;
  sourcemeta::core::split(
      "alpha;beta;gamma", ';',
      [&](const std::string_view part) { parts.emplace_back(part); });
  EXPECT_EQ(parts.size(), 3);
  EXPECT_EQ(parts.at(0), "alpha");
  EXPECT_EQ(parts.at(1), "beta");
  EXPECT_EQ(parts.at(2), "gamma");
}

TEST(no_delimiter_yields_full_input) {
  std::vector<std::string> parts;
  sourcemeta::core::split("solo", ';', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts.at(0), "solo");
}

TEST(empty_input_yields_single_empty_part) {
  std::vector<std::string> parts;
  sourcemeta::core::split(
      "", ';', [&](const std::string_view part) { parts.emplace_back(part); });
  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts.at(0), "");
}

TEST(leading_delimiter_yields_empty_first_part) {
  std::vector<std::string> parts;
  sourcemeta::core::split(";beta", ';', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 2);
  EXPECT_EQ(parts.at(0), "");
  EXPECT_EQ(parts.at(1), "beta");
}

TEST(trailing_delimiter_yields_empty_last_part) {
  std::vector<std::string> parts;
  sourcemeta::core::split("alpha;", ';', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 2);
  EXPECT_EQ(parts.at(0), "alpha");
  EXPECT_EQ(parts.at(1), "");
}

TEST(consecutive_delimiters_yield_empty_parts) {
  std::vector<std::string> parts;
  sourcemeta::core::split("a;;c", ';', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 3);
  EXPECT_EQ(parts.at(0), "a");
  EXPECT_EQ(parts.at(1), "");
  EXPECT_EQ(parts.at(2), "c");
}

TEST(only_delimiters) {
  std::vector<std::string> parts;
  sourcemeta::core::split(";;", ';', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 3);
  EXPECT_EQ(parts.at(0), "");
  EXPECT_EQ(parts.at(1), "");
  EXPECT_EQ(parts.at(2), "");
}

TEST(comma_delimiter) {
  std::vector<std::string> parts;
  sourcemeta::core::split("1,2,3,4", ',', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 4);
  EXPECT_EQ(parts.at(0), "1");
  EXPECT_EQ(parts.at(3), "4");
}

TEST(newline_delimiter) {
  std::vector<std::string> parts;
  sourcemeta::core::split("a\nb\nc", '\n', [&](const std::string_view part) {
    parts.emplace_back(part);
  });
  EXPECT_EQ(parts.size(), 3);
  EXPECT_EQ(parts.at(0), "a");
  EXPECT_EQ(parts.at(1), "b");
  EXPECT_EQ(parts.at(2), "c");
}

TEST(parts_reference_input_buffer) {
  const std::string input{"alpha;beta"};
  std::vector<std::size_t> sizes;
  sourcemeta::core::split(input, ';', [&](const std::string_view part) {
    sizes.push_back(part.size());
    EXPECT_GE(part.data(), input.data());
    EXPECT_LE(part.data() + part.size(), input.data() + input.size());
  });
  EXPECT_EQ(sizes.size(), 2);
  EXPECT_EQ(sizes.at(0), 5);
  EXPECT_EQ(sizes.at(1), 4);
}
