#include <sourcemeta/core/http_method.h>
#include <sourcemeta/core/test.h>

TEST(get_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::GET),
      "GET");
}

TEST(head_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::HEAD),
      "HEAD");
}

TEST(post_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::POST),
      "POST");
}

TEST(put_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::PUT),
      "PUT");
}

TEST(delete_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::DELETE),
            "DELETE");
}

TEST(connect_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::CONNECT),
            "CONNECT");
}

TEST(options_token) {
  EXPECT_EQ(sourcemeta::core::http_method_string(
                sourcemeta::core::HTTPMethod::OPTIONS),
            "OPTIONS");
}

TEST(trace_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::TRACE),
      "TRACE");
}

TEST(patch_token) {
  EXPECT_EQ(
      sourcemeta::core::http_method_string(sourcemeta::core::HTTPMethod::PATCH),
      "PATCH");
}
