#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, port_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.port = "8080"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, port_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.port = "8080"};
  const auto result{pattern.port.match("3000")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, port_name_basic) {
  const sourcemeta::core::URLPattern pattern{.port = ":port"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "8080");
  EXPECT_TRUE(result.value().at("port").has_value());
  EXPECT_EQ(result.value().at("port").value(), "8080");
}

TEST(URLPattern_match, port_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.port = ":port"};
  const auto result{pattern.port.match("443")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "443");
  EXPECT_TRUE(result.value().at("port").has_value());
  EXPECT_EQ(result.value().at("port").value(), "443");
}

TEST(URLPattern_match, port_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.port = ":port(\\d+)"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "8080");
  EXPECT_TRUE(result.value().at("port").has_value());
  EXPECT_EQ(result.value().at("port").value(), "8080");
}

TEST(URLPattern_match, port_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.port = ":port(\\d+)"};
  const auto result{pattern.port.match("http")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, port_regex_match) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "8080");
}

TEST(URLPattern_match, port_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.port = "(\\d+)"};
  const auto result{pattern.port.match("http")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, port_asterisk) {
  const sourcemeta::core::URLPattern pattern{.port = "*"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "8080");
}

TEST(URLPattern_match, port_group_match) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}"};
  const auto result{pattern.port.match("8080")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, port_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.port = "{8080}"};
  const auto result{pattern.port.match("3000")};
  EXPECT_FALSE(result.has_value());
}
