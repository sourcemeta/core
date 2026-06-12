#include <gtest/gtest.h>

#include <sourcemeta/core/http_message.h>

#include <string>  // std::string
#include <utility> // std::pair
#include <vector>  // std::vector

TEST(HTTP_header_find, present) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"content-type", "text/html"}, {"server", "test"}};
  const auto result{
      sourcemeta::core::http_header_find(headers, "content-type")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "text/html");
}

TEST(HTTP_header_find, repeated_returns_first) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"set-cookie", "a=1"}, {"set-cookie", "b=2"}};
  const auto result{sourcemeta::core::http_header_find(headers, "set-cookie")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a=1");
}

TEST(HTTP_header_find, missing) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"server", "test"}};
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "content-type").has_value());
}

TEST(HTTP_header_find, empty_headers) {
  const std::vector<std::pair<std::string, std::string>> headers;
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "server").has_value());
}

TEST(HTTP_header_find, lookup_is_exact_match) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"server", "test"}};
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "Server").has_value());
}

TEST(HTTP_header_find, empty_value_is_a_match) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"x-empty", ""}};
  const auto result{sourcemeta::core::http_header_find(headers, "x-empty")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}
