#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <chrono> // std::chrono::seconds

TEST(single_directive) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=600")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{600});
}

TEST(zero) {
  const auto result{sourcemeta::core::http_cache_control_max_age("max-age=0")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{0});
}

TEST(among_other_directives) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "public, max-age=3600, must-revalidate")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{3600});
}

TEST(leading_other_directive) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("no-cache, max-age=120")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{120});
}

TEST(case_insensitive_directive_name) {
  const auto result{sourcemeta::core::http_cache_control_max_age("Max-Age=42")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{42});
}

TEST(surrounding_whitespace_trimmed) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "  public ,  max-age=90  , no-store ")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{90});
}

TEST(absent) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("public, no-store")
                   .has_value());
}

TEST(empty_header) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("").has_value());
}

TEST(no_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age").has_value());
}

TEST(empty_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=").has_value());
}

TEST(non_numeric_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=abc").has_value());
}

TEST(partly_numeric_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=12x").has_value());
}

TEST(prefix_only_directive_ignored) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "max-age-extension=5, max-age=7")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{7});
}

TEST(quoted_string_value) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=\"600\"")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{600});
}

TEST(quoted_string_value_among_directives) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "public, max-age=\"42\", must-revalidate")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{42});
}

TEST(empty_quoted_string_value) {
  EXPECT_FALSE(
      sourcemeta::core::http_cache_control_max_age("max-age=\"\"").has_value());
}

TEST(unterminated_quote_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("max-age=\"600")
                   .has_value());
}

TEST(non_numeric_quoted_value_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("max-age=\"abc\"")
                   .has_value());
}

TEST(quoted_pair_escaped_digits) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=\"6\\0\\0\"")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{600});
}

TEST(quoted_pair_escaped_non_digit_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("max-age=\"6\\a0\"")
                   .has_value());
}

TEST(dangling_escape_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("max-age=\"60\\\"")
                   .has_value());
}

TEST(backslash_in_token_form_rejected) {
  EXPECT_FALSE(sourcemeta::core::http_cache_control_max_age("max-age=6\\00")
                   .has_value());
}

TEST(far_above_overflow_boundary) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=4294967296")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483648});
}

TEST(overflow_saturates) {
  const auto result{sourcemeta::core::http_cache_control_max_age(
      "max-age=999999999999999999999999")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483648});
}

TEST(at_overflow_boundary) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=2147483648")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483648});
}

TEST(just_below_overflow_boundary) {
  const auto result{
      sourcemeta::core::http_cache_control_max_age("max-age=2147483647")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::chrono::seconds{2147483647});
}
