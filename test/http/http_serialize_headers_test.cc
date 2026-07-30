#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

TEST(empty) {
  const std::vector<std::pair<std::string, std::string>> headers;
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(single_header) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"Accept", "application/json"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers),
            "Accept: application/json\r\n");
}

TEST(multiple_headers_in_order) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"Accept", "application/json"}, {"User-Agent", "test/1.0"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers),
            "Accept: application/json\r\nUser-Agent: test/1.0\r\n");
}

TEST(repeated_field_names) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"Set-Cookie", "a=1"}, {"Set-Cookie", "b=2"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers),
            "Set-Cookie: a=1\r\nSet-Cookie: b=2\r\n");
}

TEST(empty_value) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Empty", ""}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "X-Empty: \r\n");
}

TEST(view_pairs) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Accept", "application/json"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers),
            "Accept: application/json\r\n");
}

TEST(request_headers_with_wiping_values) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Accept", "application/json");
  request.header("Authorization", sourcemeta::core::SecureString{"Basic Zm9v"});
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(request.headers()),
            "Accept: application/json\r\nAuthorization: Basic Zm9v\r\n");
}

TEST(round_trip_through_parse) {
  std::vector<std::pair<std::string, std::string>> parsed;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nserver: test\r\ndate: now\r\n\r\n", parsed);
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(parsed),
            "server: test\r\ndate: now\r\n");
}

TEST(rejects_value_with_crlf) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Injected", "value\r\nEvil: yes"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(rejects_value_with_bare_carriage_return) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Injected", "value\rmore"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(rejects_value_with_bare_line_feed) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Injected", "value\nmore"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(rejects_value_with_nul) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Injected", std::string{"value\0more", 10}}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(rejects_name_with_crlf) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"X-Bad\r\nEvil", "value"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers), "");
}

TEST(drops_only_the_injected_header) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"Accept", "application/json"},
      {"X-Injected", "value\r\nEvil: yes"},
      {"User-Agent", "test/1.0"}};
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(headers),
            "Accept: application/json\r\nUser-Agent: test/1.0\r\n");
}
