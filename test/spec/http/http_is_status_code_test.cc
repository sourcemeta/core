#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(the_lowest_and_highest_codes_the_form_admits) {
  // The predicate is constexpr, so an argument the compiler can read folds the
  // call away and leaves the comparisons behind it unexecuted
  std::string lowest{"100"};
  std::string highest{"599"};
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(lowest));
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(highest));
}

TEST(a_code_the_registry_does_not_name) {
  std::string value{"299"};
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(value));
  EXPECT_TRUE(sourcemeta::core::http_status_from_code(299).phrase.empty());
}

TEST(one_of_each_class) {
  std::string informational{"100"};
  std::string successful{"200"};
  std::string redirection{"301"};
  std::string client_error{"404"};
  std::string server_error{"500"};
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(informational));
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(successful));
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(redirection));
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(client_error));
  EXPECT_TRUE(sourcemeta::core::http_is_status_code(server_error));
}

TEST(the_class_below_the_first) {
  std::string value{"099"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(value));
}

TEST(the_class_above_the_last) {
  std::string first{"600"};
  std::string second{"999"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(first));
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(second));
}

TEST(too_few_digits) {
  std::string value{"20"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(value));
}

TEST(too_many_digits) {
  std::string value{"2000"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(value));
}

TEST(empty_input) {
  std::string value;
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(value));
}

TEST(a_range_is_not_a_code) {
  std::string value{"2XX"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(value));
}

TEST(letters_where_digits_belong) {
  std::string middle{"2a0"};
  std::string last{"20a"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(middle));
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(last));
}

TEST(a_sign_is_not_a_digit) {
  std::string sign{"+20"};
  std::string space{"2 0"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(sign));
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(space));
}

TEST(a_digit_below_the_range_in_each_position) {
  std::string middle{"2/0"};
  std::string last{"20/"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(middle));
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(last));
}

TEST(a_digit_above_the_range_in_each_position) {
  std::string middle{"2:0"};
  std::string last{"20:"};
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(middle));
  EXPECT_FALSE(sourcemeta::core::http_is_status_code(last));
}
