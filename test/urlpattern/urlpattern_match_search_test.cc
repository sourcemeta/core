#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, search_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.search = "q=test"};
  const auto result{pattern.search.match("q=test")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, search_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.search = "q=test"};
  const auto result{pattern.search.match("q=search")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, search_name_basic) {
  const sourcemeta::core::URLPattern pattern{.search = ":query"};
  const auto result{pattern.search.match("q=test&lang=en")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "q=test&lang=en");
  EXPECT_TRUE(result.value().at("query").has_value());
  EXPECT_EQ(result.value().at("query").value(), "q=test&lang=en");
}

TEST(URLPattern_match, search_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.search = ":query"};
  const auto result{pattern.search.match("page=1")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "page=1");
  EXPECT_TRUE(result.value().at("query").has_value());
  EXPECT_EQ(result.value().at("query").value(), "page=1");
}

TEST(URLPattern_match, search_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.search = ":query(q=.*)"};
  const auto result{pattern.search.match("q=test")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "q=test");
  EXPECT_TRUE(result.value().at("query").has_value());
  EXPECT_EQ(result.value().at("query").value(), "q=test");
}

TEST(URLPattern_match, search_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.search = ":query(q=.*)"};
  const auto result{pattern.search.match("page=1")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, search_regex_match) {
  const sourcemeta::core::URLPattern pattern{.search = "(\\w+=\\w+)"};
  const auto result{pattern.search.match("page=1")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "page=1");
}

TEST(URLPattern_match, search_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.search = "(\\d+)"};
  const auto result{pattern.search.match("page=1")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, search_asterisk) {
  const sourcemeta::core::URLPattern pattern{.search = "*"};
  const auto result{pattern.search.match("q=test&lang=en")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "q=test&lang=en");
}

TEST(URLPattern_match, search_group_match) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}"};
  const auto result{pattern.search.match("q=test")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, search_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.search = "{q=test}"};
  const auto result{pattern.search.match("page=1")};
  EXPECT_FALSE(result.has_value());
}
