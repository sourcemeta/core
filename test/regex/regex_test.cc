#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

TEST(Regex, copy_construct) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  const sourcemeta::core::Regex copy{regex.value()};
  EXPECT_TRUE(sourcemeta::core::matches(copy, "foo bar"));
  EXPECT_FALSE(sourcemeta::core::matches(copy, "bar foo"));
}

TEST(Regex, copy_assign) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  sourcemeta::core::Regex copy{sourcemeta::core::RegexTypeNoop{}};
  copy = regex.value();
  EXPECT_TRUE(sourcemeta::core::matches(copy, "foo bar"));
  EXPECT_FALSE(sourcemeta::core::matches(copy, "bar foo"));
}

TEST(Regex, move_construct) {
  auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  const sourcemeta::core::Regex moved{std::move(regex).value()};
  EXPECT_TRUE(sourcemeta::core::matches(moved, "foo bar"));
  EXPECT_FALSE(sourcemeta::core::matches(moved, "bar foo"));
}

TEST(Regex, move_assign) {
  auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  sourcemeta::core::Regex moved{sourcemeta::core::RegexTypeNoop{}};
  moved = std::move(regex).value();
  EXPECT_TRUE(sourcemeta::core::matches(moved, "foo bar"));
  EXPECT_FALSE(sourcemeta::core::matches(moved, "bar foo"));
}

TEST(Regex, to_regex_with_string_view) {
  const std::string_view pattern{"^foo"};
  const auto regex{sourcemeta::core::to_regex(pattern)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo bar"));
}

TEST(Regex, to_regex_with_string_view_subview) {
  const std::string buffer{"prefix^foosuffix"};
  const std::string_view pattern{buffer.data() + 6, 4};
  const auto regex{sourcemeta::core::to_regex(pattern)};
  EXPECT_TRUE(regex.has_value());
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), "foo bar"));
}

TEST(Regex, matches_with_string_view) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  const std::string_view value{"foo bar"};
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), value));
}

TEST(Regex, matches_with_string_view_subview) {
  const auto regex{sourcemeta::core::to_regex("^foo")};
  EXPECT_TRUE(regex.has_value());
  const std::string buffer{"xxxfoo barxxx"};
  const std::string_view value{buffer.data() + 3, 7};
  EXPECT_TRUE(sourcemeta::core::matches(regex.value(), value));
}

TEST(Regex, matches_if_valid_with_string_view) {
  const std::string_view pattern{"^foo"};
  const std::string_view value{"foo bar"};
  EXPECT_TRUE(sourcemeta::core::matches_if_valid(pattern, value));
}

TEST(Regex, to_regex_default_string_view_does_not_invoke_ub) {
  const std::string_view pattern{};
  EXPECT_EQ(pattern.data(), nullptr);
  const auto regex{sourcemeta::core::to_regex(pattern)};
  EXPECT_TRUE(regex.has_value());
}

TEST(Regex, catastrophic_backtracking_terminates) {
  const auto regex{sourcemeta::core::to_regex("(a+)+$")};
  EXPECT_TRUE(regex.has_value());
  const std::string value{std::string(64, 'a') + "!"};
  EXPECT_FALSE(sourcemeta::core::matches(regex.value(), value));
}
