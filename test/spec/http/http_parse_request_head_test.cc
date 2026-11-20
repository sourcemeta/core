#include <sourcemeta/core/http_request.h>
#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

TEST(request_head_complete_preserves_method_target_and_field_order) {
  constexpr std::string_view INPUT{
      "custom! /path?key=value HTTP/1.1\r\nHost: example.com\r\n"
      "X-Value:\t one \t\r\nx-value:two\r\nEmpty: \t\r\n\r\n"};
  const auto result{sourcemeta::core::http_parse_request_head(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Complete);
  EXPECT_EQ(result.consumed, INPUT.size());
  EXPECT_EQ(result.method, "custom!");
  EXPECT_EQ(result.target, "/path?key=value");
  EXPECT_EQ(result.fields.size(), 4);
  EXPECT_EQ(result.fields.at(0).first, "Host");
  EXPECT_EQ(result.fields.at(0).second, "example.com");
  EXPECT_EQ(result.fields.at(1).first, "X-Value");
  EXPECT_EQ(result.fields.at(1).second, "one");
  EXPECT_EQ(result.fields.at(2).first, "x-value");
  EXPECT_EQ(result.fields.at(2).second, "two");
  EXPECT_TRUE(result.fields.at(3).second.empty());
}

TEST(request_head_stops_before_body) {
  constexpr std::string_view INPUT{
      "POST / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\ndata"};
  const auto result{sourcemeta::core::http_parse_request_head(INPUT)};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Complete);
  EXPECT_EQ(INPUT.substr(result.consumed), "data");
}

TEST(request_head_empty_input_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_head("")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_head_split_terminator_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\r\nHost: example.com\r\n\r")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.fields.empty());
}

TEST(request_head_rejects_other_versions) {
  const auto result{
      sourcemeta::core::http_parse_request_head("GET / HTTP/1.0\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_head_rejects_missing_target) {
  const auto result{
      sourcemeta::core::http_parse_request_head("GET  HTTP/1.1\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
}

TEST(request_head_rejects_bare_lf) {
  const auto result{
      sourcemeta::core::http_parse_request_head("GET / HTTP/1.1\n\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
}

TEST(request_head_rejects_whitespace_before_colon) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\r\nHost : example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.fields.empty());
}

TEST(request_head_rejects_field_value_control_byte) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\r\nHost: example.com\r\nX-Value: \x01\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
  EXPECT_TRUE(result.fields.empty());
}

TEST(request_head_rejects_obsolete_line_folding) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\r\nHost: example.com\r\n continued\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
}

TEST(request_head_partial_request_line_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_head("GET / HTTP/1.")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
}

TEST(request_head_partial_field_is_incomplete) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\r\nHost: example.com")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Incomplete);
  EXPECT_EQ(result.consumed, 0);
  EXPECT_TRUE(result.method.empty());
  EXPECT_TRUE(result.target.empty());
  EXPECT_TRUE(result.fields.empty());
}

TEST(request_head_rejects_carriage_return_without_line_feed) {
  const auto result{sourcemeta::core::http_parse_request_head(
      "GET / HTTP/1.1\rHost: example.com\r\n\r\n")};
  EXPECT_TRUE(result.status ==
              sourcemeta::core::HTTPRequestHeadStatus::Malformed);
}
