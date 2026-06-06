#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>

TEST(HTTP_content_type_matches, exact_match) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json",
                                                          "application/json"));
}

TEST(HTTP_content_type_matches, exact_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("application/xml",
                                                           "application/json"));
}

TEST(HTTP_content_type_matches, charset_parameter_stripped) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json; charset=UTF-8", "application/json"));
}

TEST(HTTP_content_type_matches, charset_parameter_no_space_after_semicolon) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json;charset=UTF-8", "application/json"));
}

TEST(HTTP_content_type_matches, multiple_parameters_stripped) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json; charset=UTF-8; boundary=foo", "application/json"));
}

TEST(HTTP_content_type_matches, leading_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("   application/json",
                                                          "application/json"));
}

TEST(HTTP_content_type_matches, trailing_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json   ",
                                                          "application/json"));
}

TEST(HTTP_content_type_matches, whitespace_before_semicolon_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/json ; charset=UTF-8", "application/json"));
}

TEST(HTTP_content_type_matches, tab_whitespace_trimmed) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "\tapplication/json\t", "application/json"));
}

TEST(HTTP_content_type_matches, case_insensitive_uppercase_header) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("APPLICATION/JSON",
                                                          "application/json"));
}

TEST(HTTP_content_type_matches, case_insensitive_mixed_case_header) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("Application/Json",
                                                          "application/json"));
}

TEST(HTTP_content_type_matches, case_insensitive_uppercase_media_type) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches("application/json",
                                                          "APPLICATION/JSON"));
}

TEST(HTTP_content_type_matches, empty_header_does_not_match) {
  EXPECT_FALSE(
      sourcemeta::core::http_content_type_matches("", "application/json"));
}

TEST(HTTP_content_type_matches, header_with_only_parameters_does_not_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("; charset=UTF-8",
                                                           "application/json"));
}

TEST(HTTP_content_type_matches, problem_json_does_not_match_json) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches(
      "application/problem+json", "application/json"));
}

TEST(HTTP_content_type_matches, text_html_matches) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "text/html; charset=UTF-8", "text/html"));
}

TEST(HTTP_content_type_matches, problem_json_exact_match) {
  EXPECT_TRUE(sourcemeta::core::http_content_type_matches(
      "application/problem+json", "application/problem+json"));
}

TEST(HTTP_content_type_matches, different_length_no_match) {
  EXPECT_FALSE(sourcemeta::core::http_content_type_matches("application/jsons",
                                                           "application/json"));
}
