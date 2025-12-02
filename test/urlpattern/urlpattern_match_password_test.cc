#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, password_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.password = "secret"};
  const auto result{pattern.password.match("secret")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, password_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.password = "secret"};
  const auto result{pattern.password.match("password")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, password_name_basic) {
  const sourcemeta::core::URLPattern pattern{.password = ":pass"};
  const auto result{pattern.password.match("secret123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "secret123");
  EXPECT_TRUE(result.value().at("pass").has_value());
  EXPECT_EQ(result.value().at("pass").value(), "secret123");
}

TEST(URLPattern_match, password_name_any_value) {
  const sourcemeta::core::URLPattern pattern{.password = ":pass"};
  const auto result{pattern.password.match("mypassword")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "mypassword");
  EXPECT_TRUE(result.value().at("pass").has_value());
  EXPECT_EQ(result.value().at("pass").value(), "mypassword");
}

TEST(URLPattern_match, password_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.password = ":pass(\\w+)"};
  const auto result{pattern.password.match("pass123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "pass123");
  EXPECT_TRUE(result.value().at("pass").has_value());
  EXPECT_EQ(result.value().at("pass").value(), "pass123");
}

TEST(URLPattern_match, password_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.password = ":pass(\\d+)"};
  const auto result{pattern.password.match("secret")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, password_regex_match) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\w+)"};
  const auto result{pattern.password.match("secret")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "secret");
}

TEST(URLPattern_match, password_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.password = "(\\d+)"};
  const auto result{pattern.password.match("secret")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, password_asterisk) {
  const sourcemeta::core::URLPattern pattern{.password = "*"};
  const auto result{pattern.password.match("secret")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "secret");
}

TEST(URLPattern_match, password_group_match) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}"};
  const auto result{pattern.password.match("secret")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, password_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.password = "{secret}"};
  const auto result{pattern.password.match("password")};
  EXPECT_FALSE(result.has_value());
}
