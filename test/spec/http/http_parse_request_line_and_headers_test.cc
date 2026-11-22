#include <sourcemeta/core/http_request.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(
    request_line_and_headers_complete_preserves_method_target_and_field_order) {
  constexpr std::string_view INPUT{
      "custom! /path?key=value HTTP/1.1\r\nHost: example.com\r\n"
      "X-Value:\t one \t\r\nx-value:two\r\nEmpty: \t\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "custom!");
  EXPECT_EQ(result.target, "/path?key=value");
  EXPECT_EQ(result.headers.size(), 4);
  EXPECT_EQ(result.headers.at(0).first, "Host");
  EXPECT_EQ(result.headers.at(0).second, "example.com");
  EXPECT_EQ(result.headers.at(1).first, "X-Value");
  EXPECT_EQ(result.headers.at(1).second, "one");
  EXPECT_EQ(result.headers.at(2).first, "x-value");
  EXPECT_EQ(result.headers.at(2).second, "two");
  EXPECT_TRUE(result.headers.at(3).second.empty());
}

TEST(request_line_and_headers_stops_before_body) {
  constexpr std::string_view INPUT{
      "POST / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\ndata"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(INPUT.substr(result.consumed), "data");
}

TEST(request_line_and_headers_empty_input_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers("")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_line_and_headers_split_terminator_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\n\r")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_other_versions) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.0\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_line_and_headers_rejects_missing_target) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET  HTTP/1.1\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

TEST(request_line_and_headers_rejects_bare_lf) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\n\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

TEST(request_line_and_headers_rejects_whitespace_before_colon) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost : example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_field_value_control_byte) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\nX-Value: \x01\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_obsolete_line_folding) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com\r\n continued\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

TEST(request_line_and_headers_partial_request_line_is_incomplete) {
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers("GET / HTTP/1.")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_line_and_headers_partial_field_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\r\nHost: example.com")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.headers.empty());
}

TEST(request_line_and_headers_rejects_carriage_return_without_line_feed) {
  const auto result{sourcemeta::core::http_parse_request_line_and_headers(
      "GET / HTTP/1.1\rHost: example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Malformed);
}

// Adapted from llhttp's "Simple request" fixture (MIT):
// https://github.com/nodejs/llhttp/blob/85c5d42cd563d5426a478d32248a45ee81f4b635/test/request/sample.md#simple-request
TEST(llhttp_simple_request) {
  constexpr std::string_view INPUT{
      "OPTIONS /url HTTP/1.1\r\nHeader1: Value1\r\nHeader2:\t Value2\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "OPTIONS");
  EXPECT_EQ(result.target, "/url");
  EXPECT_EQ(result.headers.size(), 2);
  EXPECT_EQ(result.headers.at(0).first, "Header1");
  EXPECT_EQ(result.headers.at(0).second, "Value1");
  EXPECT_EQ(result.headers.at(1).first, "Header2");
  EXPECT_EQ(result.headers.at(1).second, "Value2");
}

// Adapted from the same suite's "No headers and no body" fixture.
TEST(llhttp_no_headers_and_no_body) {
  constexpr std::string_view INPUT{
      "GET /get_no_headers_no_body/world HTTP/1.1\r\n\r\n"};
  const auto result{
      sourcemeta::core::http_parse_request_line_and_headers(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestLineAndHeadersStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "GET");
  EXPECT_EQ(result.target, "/get_no_headers_no_body/world");
  EXPECT_TRUE(result.headers.empty());
}
