#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

TEST(orders_ignoring_case) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("apple", "Banana"));
}

TEST(reverse_order_is_false) {
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("Banana", "apple"));
}

TEST(case_insensitive_equal_is_not_less) {
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("Hello", "hello"));
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("hello", "Hello"));
}

TEST(uppercase_orders_with_lowercase) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("a", "B"));
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("B", "a"));
}

TEST(prefix_orders_first) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("hell", "hello"));
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("hello", "hell"));
}

TEST(empty_orders_first) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("", "a"));
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("a", ""));
}

TEST(both_empty_is_not_less) {
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("", ""));
}

TEST(header_names) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("host", "x-amz-date"));
  EXPECT_FALSE(sourcemeta::core::less_ignore_case("x-amz-date", "host"));
}

TEST(digits_before_letters) {
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("1", "a"));
  EXPECT_TRUE(sourcemeta::core::less_ignore_case("1", "A"));
}
