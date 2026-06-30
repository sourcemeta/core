#include <sourcemeta/core/test.h>

#include <sourcemeta/core/http_message.h>

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

TEST(round_trip_through_parse) {
  std::vector<std::pair<std::string, std::string>> parsed;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nserver: test\r\ndate: now\r\n\r\n", parsed);
  EXPECT_EQ(sourcemeta::core::http_serialize_headers(parsed),
            "server: test\r\ndate: now\r\n");
}
