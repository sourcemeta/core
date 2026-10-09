#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view

// The predicate is constexpr, so an argument the compiler can read folds the
// call away at compile time and leaves the comparisons behind it unexecuted.
// Every input goes through a string built here so that each case is decided
// where it can be observed
static auto status_code(const std::string_view value) -> bool {
  const std::string runtime{value};
  return sourcemeta::core::http_is_status_code(runtime);
}

TEST(the_lowest_and_highest_codes_the_form_admits) {
  EXPECT_TRUE(status_code("100"));
  EXPECT_TRUE(status_code("599"));
}

TEST(a_code_the_registry_does_not_name) {
  EXPECT_TRUE(status_code("299"));
  EXPECT_TRUE(sourcemeta::core::http_status_from_code(299).phrase.empty());
}

TEST(one_of_each_class) {
  EXPECT_TRUE(status_code("100"));
  EXPECT_TRUE(status_code("200"));
  EXPECT_TRUE(status_code("301"));
  EXPECT_TRUE(status_code("404"));
  EXPECT_TRUE(status_code("500"));
}

TEST(the_class_below_the_first) { EXPECT_FALSE(status_code("099")); }

TEST(the_class_above_the_last) {
  EXPECT_FALSE(status_code("600"));
  EXPECT_FALSE(status_code("999"));
}

TEST(too_few_digits) { EXPECT_FALSE(status_code("20")); }

TEST(too_many_digits) { EXPECT_FALSE(status_code("2000")); }

TEST(empty_input) { EXPECT_FALSE(status_code("")); }

TEST(a_range_is_not_a_code) { EXPECT_FALSE(status_code("2XX")); }

TEST(letters_where_digits_belong) {
  EXPECT_FALSE(status_code("2a0"));
  EXPECT_FALSE(status_code("20a"));
}

TEST(a_sign_is_not_a_digit) {
  EXPECT_FALSE(status_code("+20"));
  EXPECT_FALSE(status_code("2 0"));
}

TEST(a_digit_below_the_range_in_each_position) {
  EXPECT_FALSE(status_code("2/0"));
  EXPECT_FALSE(status_code("20/"));
}

TEST(a_digit_above_the_range_in_each_position) {
  EXPECT_FALSE(status_code("2:0"));
  EXPECT_FALSE(status_code("20:"));
}
