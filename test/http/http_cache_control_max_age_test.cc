#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

#include <chrono> // std::chrono::seconds

TEST(HTTP_cache_control_max_age, single_directive) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=600")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{600});
}

TEST(HTTP_cache_control_max_age, zero) {
  const auto result{sourcemeta::core::http_cache_control_max_age("max-age=0")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{0});
}

TEST(HTTP_cache_control_max_age, among_other_directives) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "public, max-age=3600, must-revalidate")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{3600});
}

TEST(HTTP_cache_control_max_age, leading_other_directive) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("no-cache, max-age=120")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{120});
}

TEST(HTTP_cache_control_max_age, case_insensitive_directive_name) {
  const auto result{sourcemeta::core::http_cache_control_max_age("Max-Age=42")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{42});
}

TEST(HTTP_cache_control_max_age, surrounding_whitespace_trimmed) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "  public ,  max-age=90  , no-store ")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{90});
}

TEST(HTTP_cache_control_max_age, absent) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("public, no-store")
                   .has_value());
}

TEST(HTTP_cache_control_max_age, empty_header) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("").has_value());
}

TEST(HTTP_cache_control_max_age, no_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age").has_value());
}

TEST(HTTP_cache_control_max_age, empty_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=").has_value());
}

TEST(HTTP_cache_control_max_age, non_numeric_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=abc").has_value());
}

TEST(HTTP_cache_control_max_age, partly_numeric_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=12x").has_value());
}

TEST(HTTP_cache_control_max_age, prefix_only_directive_ignored) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "max-age-extension=5, max-age=7")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{7});
}

TEST(HTTP_cache_control_max_age, overflow_saturates) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "max-age=999999999999999999999999")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483648});
}

TEST(HTTP_cache_control_max_age, at_overflow_boundary) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=2147483648")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483648});
}

TEST(HTTP_cache_control_max_age, just_below_overflow_boundary) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=2147483647")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483647});
}
