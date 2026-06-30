#include <sourcemeta/core/test.h>

#include <sourcemeta/core/http.h>

TEST(exact_match) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json",
                                                          "application/json"));
}

TEST(exact_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("application/xml",
                                                           "application/json"));
}

TEST(charset_parameter_stripped) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json; charset=UTF-8", "application/json"));
}

TEST(charset_parameter_no_space_after_semicolon) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json;charset=UTF-8", "application/json"));
}

TEST(multiple_parameters_stripped) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json; charset=UTF-8; boundary=foo", "application/json"));
}

TEST(leading_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("   application/json",
                                                          "application/json"));
}

TEST(trailing_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json   ",
                                                          "application/json"));
}

TEST(whitespace_before_semicolon_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json ; charset=UTF-8", "application/json"));
}

TEST(tab_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "\tapplication/json\t", "application/json"));
}

TEST(case_insensitive_uppercase_header) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("APPLICATION/JSON",
                                                          "application/json"));
}

TEST(case_insensitive_mixed_case_header) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("Application/Json",
                                                          "application/json"));
}

TEST(case_insensitive_uppercase_media_type) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json",
                                                          "APPLICATION/JSON"));
}

TEST(empty_header_does_not_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_content_type_matches("", "application/json"));
}

TEST(header_with_only_parameters_does_not_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("; charset=UTF-8",
                                                           "application/json"));
}

TEST(problem_json_does_not_match_json) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches(
      "application/problem+json", "application/json"));
}

TEST(text_html_matches) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "text/html; charset=UTF-8", "text/html"));
}

TEST(problem_json_exact_match) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/problem+json", "application/problem+json"));
}

TEST(different_length_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("application/jsons",
                                                           "application/json"));
}
