#include <gtest/gtest.h>

#include <sourcemeta/core/http_method.h>

TEST(HTTP_method, get_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::GET),
      "GET");
}

TEST(HTTP_method, head_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::HEAD),
      "HEAD");
}

TEST(HTTP_method, post_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::POST),
      "POST");
}

TEST(HTTP_method, put_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::PUT),
      "PUT");
}

TEST(HTTP_method, delete_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::DELETE),
            "DELETE");
}

TEST(HTTP_method, connect_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::CONNECT),
            "CONNECT");
}

TEST(HTTP_method, options_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::OPTIONS),
            "OPTIONS");
}

TEST(HTTP_method, trace_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::TRACE),
      "TRACE");
}

TEST(HTTP_method, patch_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::PATCH),
      "PATCH");
}
