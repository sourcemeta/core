#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <string> // std::string

TEST(single_pair) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "grant_type",
                                               "client_credentials");
  EXPECT_EQ(body, "grant_type=client_credentials");
}

TEST(joins_later_pairs_with_ampersand) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "grant_type",
                                               "client_credentials");
  sourcemeta::core::URI::append_form_parameter(body, "scope", "read write");
  EXPECT_EQ(body, "grant_type=client_credentials&scope=read+write");
}

TEST(empty_value) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "scope", "");
  EXPECT_EQ(body, "scope=");
}

TEST(empty_name) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "", "value");
  EXPECT_EQ(body, "=value");
}

TEST(the_name_is_encoded_too) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "a b", "c d");
  EXPECT_EQ(body, "a+b=c+d");
}

TEST(delimiters_in_a_value_are_encoded) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "a", "x=1&y=2");
  sourcemeta::core::URI::append_form_parameter(body, "b", "2");
  EXPECT_EQ(body, "a=x%3D1%26y%3D2&b=2");
}

TEST(rfc6749_appendix_b_example) {
  std::string body;
  sourcemeta::core::URI::append_form_parameter(body, "value",
                                               " %&+\xC2\xA3\xE2\x82\xAC");
  EXPECT_EQ(body, "value=+%25%26%2B%C2%A3%E2%82%AC");
}
