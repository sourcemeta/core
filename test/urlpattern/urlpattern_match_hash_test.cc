#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, hash_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "section"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, hash_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "section"};
  const auto result{pattern.hash.match("intro")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, hash_name_basic) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "section");
  EXPECT_TRUE(result.value().at("fragment").has_value());
  EXPECT_EQ(result.value().at("fragment").value(), "section");
}

TEST(URLPattern_match, hash_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment"};
  const auto result{pattern.hash.match("heading-1")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "heading-1");
  EXPECT_TRUE(result.value().at("fragment").has_value());
  EXPECT_EQ(result.value().at("fragment").value(), "heading-1");
}

TEST(URLPattern_match, hash_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment(\\w+)"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "section");
  EXPECT_TRUE(result.value().at("fragment").has_value());
  EXPECT_EQ(result.value().at("fragment").value(), "section");
}

TEST(URLPattern_match, hash_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.hash = ":fragment(\\d+)"};
  const auto result{pattern.hash.match("section")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, hash_regex_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\w+)"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "section");
}

TEST(URLPattern_match, hash_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "(\\d+)"};
  const auto result{pattern.hash.match("section")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, hash_asterisk) {
  const sourcemeta::core::URLPattern pattern{.hash = "*"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "section");
}

TEST(URLPattern_match, hash_group_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}"};
  const auto result{pattern.hash.match("section")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, hash_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.hash = "{section}"};
  const auto result{pattern.hash.match("intro")};
  EXPECT_FALSE(result.has_value());
}
