#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, username_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.username = "admin"};
  const auto result{pattern.username.match("admin")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, username_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.username = "admin"};
  const auto result{pattern.username.match("user")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, username_name_basic) {
  const sourcemeta::core::URLPattern pattern{.username = ":user"};
  const auto result{pattern.username.match("admin")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "admin");
  EXPECT_TRUE(result.value().at("user").has_value());
  EXPECT_EQ(result.value().at("user").value(), "admin");
}

TEST(URLPattern_match, username_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.username = ":user"};
  const auto result{pattern.username.match("john")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "john");
  EXPECT_TRUE(result.value().at("user").has_value());
  EXPECT_EQ(result.value().at("user").value(), "john");
}

TEST(URLPattern_match, username_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.username = ":user(\\w+)"};
  const auto result{pattern.username.match("admin123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "admin123");
  EXPECT_TRUE(result.value().at("user").has_value());
  EXPECT_EQ(result.value().at("user").value(), "admin123");
}

TEST(URLPattern_match, username_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.username = ":user(\\d+)"};
  const auto result{pattern.username.match("admin")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, username_regex_match) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\w+)"};
  const auto result{pattern.username.match("user123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "user123");
}

TEST(URLPattern_match, username_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.username = "(\\d+)"};
  const auto result{pattern.username.match("admin")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, username_asterisk) {
  const sourcemeta::core::URLPattern pattern{.username = "*"};
  const auto result{pattern.username.match("admin")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "admin");
}

TEST(URLPattern_match, username_group_match) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}"};
  const auto result{pattern.username.match("admin")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, username_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.username = "{admin}"};
  const auto result{pattern.username.match("user")};
  EXPECT_FALSE(result.has_value());
}
