#include <gtest/gtest.h>

#include <sourcemeta/core/http_message.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace {

auto collect(const std::string_view input)
    -> std::vector<std::pair<std::string, std::string>> {
  std::vector<std::pair<std::string, std::string>> result;
  sourcemeta::core::http_parse_headers(
      input,
      [&result](const std::string_view name, const std::string_view value) {
        result.emplace_back(std::string{name}, std::string{value});
      });
  return result;
}

} // namespace

TEST(HTTP_parse_headers, callback_empty_input) {
  EXPECT_TRUE(collect("").empty());
}

TEST(HTTP_parse_headers, callback_status_line_only) {
  EXPECT_TRUE(collect("HTTP/1.1 200 OK\r\n\r\n").empty());
}

TEST(HTTP_parse_headers, callback_status_line_without_terminator) {
  EXPECT_TRUE(collect("HTTP/1.1 200 OK").empty());
}

TEST(HTTP_parse_headers, callback_single_field_line) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Content-Type");
  EXPECT_EQ(headers.at(0).second, "text/html");
}

TEST(HTTP_parse_headers, callback_multiple_field_lines) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nServer: test\r\nContent-Length: 12\r\n\r\n")};
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "Server");
  EXPECT_EQ(headers.at(0).second, "test");
  EXPECT_EQ(headers.at(1).first, "Content-Length");
  EXPECT_EQ(headers.at(1).second, "12");
}

TEST(HTTP_parse_headers, callback_preserves_name_case) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nX-FOO-Bar: baz\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "X-FOO-Bar");
  EXPECT_EQ(headers.at(0).second, "baz");
}

TEST(HTTP_parse_headers, callback_no_space_after_colon) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nServer:test\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Server");
  EXPECT_EQ(headers.at(0).second, "test");
}

TEST(HTTP_parse_headers, callback_trims_multiple_leading_spaces) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nServer:   test\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).second, "test");
}

TEST(HTTP_parse_headers, callback_trims_leading_horizontal_tab) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nServer:\ttest\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).second, "test");
}

TEST(HTTP_parse_headers, callback_trims_trailing_whitespace) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nServer: test \t\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).second, "test");
}

TEST(HTTP_parse_headers, callback_preserves_internal_whitespace) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nUser-Agent: foo bar\tbaz\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).second, "foo bar\tbaz");
}

TEST(HTTP_parse_headers, callback_empty_value_without_space) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nX-Empty:\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "X-Empty");
  EXPECT_TRUE(headers.at(0).second.empty());
}

TEST(HTTP_parse_headers, callback_empty_value_with_space) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nX-Empty: \r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "X-Empty");
  EXPECT_TRUE(headers.at(0).second.empty());
}

TEST(HTTP_parse_headers, callback_value_with_colons) {
  const auto headers{collect(
      "HTTP/1.1 200 OK\r\nLocation: https://example.com:8080/x\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Location");
  EXPECT_EQ(headers.at(0).second, "https://example.com:8080/x");
}

TEST(HTTP_parse_headers, callback_repeated_field_names_in_order) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nSet-Cookie: a=1\r\n"
                             "Set-Cookie: b=2\r\n\r\n")};
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "Set-Cookie");
  EXPECT_EQ(headers.at(0).second, "a=1");
  EXPECT_EQ(headers.at(1).first, "Set-Cookie");
  EXPECT_EQ(headers.at(1).second, "b=2");
}

TEST(HTTP_parse_headers, callback_skips_line_without_colon) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\ngarbage\r\nServer: test\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Server");
}

TEST(HTTP_parse_headers, callback_skips_empty_field_name) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\n: anonymous\r\nServer: test\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Server");
}

TEST(HTTP_parse_headers, callback_skips_whitespace_before_colon) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nServer : nginx\r\nDate: now\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Date");
  EXPECT_EQ(headers.at(0).second, "now");
}

TEST(HTTP_parse_headers, callback_obs_fold_reported_with_empty_name) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nX-Long: first\r\n second part\r\n\r\n")};
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "X-Long");
  EXPECT_EQ(headers.at(0).second, "first");
  EXPECT_TRUE(headers.at(1).first.empty());
  EXPECT_EQ(headers.at(1).second, "second part");
}

TEST(HTTP_parse_headers, callback_obs_fold_with_embedded_colon) {
  const auto headers{
      collect("HTTP/1.1 200 OK\r\nX-Long: first\r\n\tnote: detail\r\n\r\n")};
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "X-Long");
  EXPECT_EQ(headers.at(0).second, "first");
  EXPECT_TRUE(headers.at(1).first.empty());
  EXPECT_EQ(headers.at(1).second, "note: detail");
}

TEST(HTTP_parse_headers, callback_first_line_always_skipped) {
  const auto headers{collect("Fake: line\r\nReal: value\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "Real");
  EXPECT_EQ(headers.at(0).second, "value");
}

TEST(HTTP_parse_headers, callback_ignores_field_line_without_terminator) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nA: b\r\nB: c")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "A");
  EXPECT_EQ(headers.at(0).second, "b");
}

TEST(HTTP_parse_headers, callback_stops_at_blank_line) {
  const auto headers{collect("HTTP/1.1 200 OK\r\nA: b\r\n\r\nB: c\r\n\r\n")};
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "A");
  EXPECT_EQ(headers.at(0).second, "b");
}

TEST(HTTP_parse_headers, container_empty_input) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers("", headers);
  EXPECT_TRUE(headers.empty());
}

TEST(HTTP_parse_headers, container_lowercases_names) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nContent-TYPE: text/html\r\nX-Foo-9: bar\r\n\r\n",
      headers);
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "content-type");
  EXPECT_EQ(headers.at(0).second, "text/html");
  EXPECT_EQ(headers.at(1).first, "x-foo-9");
  EXPECT_EQ(headers.at(1).second, "bar");
}

TEST(HTTP_parse_headers, container_preserves_repeated_fields_in_order) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nSet-Cookie: a=1\r\nSet-Cookie: b=2\r\n\r\n", headers);
  EXPECT_EQ(headers.size(), 2);
  EXPECT_EQ(headers.at(0).first, "set-cookie");
  EXPECT_EQ(headers.at(0).second, "a=1");
  EXPECT_EQ(headers.at(1).first, "set-cookie");
  EXPECT_EQ(headers.at(1).second, "b=2");
}

TEST(HTTP_parse_headers, container_joins_obs_fold_with_space) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nX-Long: first\r\n second part\r\n\r\n", headers);
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "x-long");
  EXPECT_EQ(headers.at(0).second, "first second part");
}

TEST(HTTP_parse_headers, container_joins_obs_fold_with_horizontal_tab) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\nX-Long: first\r\n\tsecond\r\n\r\n", headers);
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "x-long");
  EXPECT_EQ(headers.at(0).second, "first second");
}

TEST(HTTP_parse_headers, container_ignores_orphan_obs_fold) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers(
      "HTTP/1.1 200 OK\r\n orphan\r\nServer: test\r\n\r\n", headers);
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "server");
  EXPECT_EQ(headers.at(0).second, "test");
}

TEST(HTTP_parse_headers, container_empty_value) {
  std::vector<std::pair<std::string, std::string>> headers;
  sourcemeta::core::http_parse_headers("HTTP/1.1 200 OK\r\nX-Empty:\r\n\r\n",
                                       headers);
  EXPECT_EQ(headers.size(), 1);
  EXPECT_EQ(headers.at(0).first, "x-empty");
  EXPECT_TRUE(headers.at(0).second.empty());
}
