#include <sourcemeta/core/http_message.h>
#include <sourcemeta/core/test.h>

TEST(http_1_1) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/1.1 200 OK"));
}

TEST(http_1_0) {
  EXPECT_TRUE(
      sourcemeta::core::http_is_status_line("HTTP/1.0 301 Moved Permanently"));
}

TEST(http_2) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/2 200"));
}

TEST(status_line_without_reason_phrase) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/1.1 204 "));
}

TEST(prefix_only) {
  EXPECT_TRUE(sourcemeta::core::http_is_status_line("HTTP/"));
}

TEST(field_line) {
  EXPECT_FALSE(
      sourcemeta::core::http_is_status_line("Content-Type: text/html"));
}

TEST(empty) { EXPECT_FALSE(sourcemeta::core::http_is_status_line("")); }

TEST(lowercase_protocol_name) {
  EXPECT_FALSE(sourcemeta::core::http_is_status_line("http/1.1 200 OK"));
}

TEST(truncated_protocol_name) {
  EXPECT_FALSE(sourcemeta::core::http_is_status_line("HTTP"));
}
