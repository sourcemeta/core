#include <sourcemeta/core/test.h>

#include <sourcemeta/core/http_message.h>

#include <functional> // std::less
#include <map>        // std::map
#include <string>     // std::string
#include <utility>    // std::pair
#include <vector>     // std::vector

TEST(present) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"content-type", "text/html"}, {"server", "test"}};
  const auto result{
      sourcemeta::core::http_header_find(headers, "content-type")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "text/html");
}

TEST(repeated_returns_first) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"set-cookie", "a=1"}, {"set-cookie", "b=2"}};
  const auto result{sourcemeta::core::http_header_find(headers, "set-cookie")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "a=1");
}

TEST(missing) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"server", "test"}};
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "content-type").has_value());
}

TEST(empty_headers) {
  const std::vector<std::pair<std::string, std::string>> headers;
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "server").has_value());
}

TEST(lookup_is_exact_match) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"server", "test"}};
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "Server").has_value());
}

TEST(empty_value_is_a_match) {
  const std::vector<std::pair<std::string, std::string>> headers{
      {"x-empty", ""}};
  const auto result{sourcemeta::core::http_header_find(headers, "x-empty")};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().empty());
}

TEST(associative_container_present) {
  const std::map<std::string, std::string, std::less<>> headers{
      {"content-type", "text/html"}, {"server", "test"}};
  const auto result{sourcemeta::core::http_header_find(headers, "server")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "test");
}

TEST(associative_container_missing) {
  const std::map<std::string, std::string, std::less<>> headers{
      {"server", "test"}};
  EXPECT_FALSE(
      sourcemeta::core::http_header_find(headers, "content-type").has_value());
}

TEST(associative_container_without_transparent_lookup) {
  const std::map<std::string, std::string> headers{{"server", "test"}};
  const auto result{sourcemeta::core::http_header_find(headers, "server")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "test");
}
