#include <sourcemeta/core/http_message.h>
#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(status_line_into_empty_buffer) {
  std::string buffer;
  sourcemeta::core::http_accumulate_header_line(buffer, "HTTP/1.1 200 OK\r\n");
  EXPECT_EQ(buffer, "HTTP/1.1 200 OK\r\n");
}

TEST(field_lines_after_status_line) {
  std::string buffer;
  sourcemeta::core::http_accumulate_header_line(buffer, "HTTP/1.1 200 OK\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer,
                                                "Content-Length: 0\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "Server: test\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "\r\n");
  EXPECT_EQ(buffer,
            "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nServer: test\r\n\r\n");
}

TEST(redirect_block_replaces_previous_block) {
  std::string buffer;
  sourcemeta::core::http_accumulate_header_line(
      buffer, "HTTP/1.1 301 Moved Permanently\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "Location: /other\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "HTTP/1.1 200 OK\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "Server: test\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "\r\n");
  EXPECT_EQ(buffer, "HTTP/1.1 200 OK\r\nServer: test\r\n\r\n");
}

TEST(interim_response_block_replaced) {
  std::string buffer;
  sourcemeta::core::http_accumulate_header_line(buffer,
                                                "HTTP/1.1 100 Continue\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "HTTP/1.1 200 OK\r\n");
  sourcemeta::core::http_accumulate_header_line(buffer, "\r\n");
  EXPECT_EQ(buffer, "HTTP/1.1 200 OK\r\n\r\n");
}

TEST(field_line_without_status_line) {
  std::string buffer;
  sourcemeta::core::http_accumulate_header_line(buffer, "Server: test\r\n");
  EXPECT_EQ(buffer, "Server: test\r\n");
}
