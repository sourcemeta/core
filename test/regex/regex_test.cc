#include <gtest/gtest.h>

#include <sourcemeta/core/regex.h>

#include <utility> // std::move

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
