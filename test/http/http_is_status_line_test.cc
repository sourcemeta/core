#include <gtest/gtest.h>

#include <sourcemeta/core/http_message.h>

TEST(HTTP_is_status_line, http_1_1) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/1.1 200 OK"));
}

TEST(HTTP_is_status_line, http_1_0) {
  EXPECT_TRUE(
      sourcemeta::core::http_is_status_line("HTTP/1.0 301 Moved Permanently"));
}

TEST(HTTP_is_status_line, http_2) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/2 200"));
}

TEST(HTTP_is_status_line, status_line_without_reason_phrase) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/1.1 204 "));
}

TEST(HTTP_is_status_line, prefix_only) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/"));
}

TEST(HTTP_is_status_line, field_line) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_status_line("Content-Type: text/html"));
}

TEST(HTTP_is_status_line, empty) {
  EXPECT_FALSE(sourcemeta::core::http_is_status_line(""));
}

TEST(HTTP_is_status_line, lowercase_protocol_name) {
  EXPECT_FALSE(sourcemeta::core::http_is_status_line("http/1.1 200 OK"));
}

TEST(HTTP_is_status_line, truncated_protocol_name) {
  EXPECT_FALSE(sourcemeta::core::http_is_status_line("HTTP"));
}
