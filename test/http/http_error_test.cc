#include <sourcemeta/core/http_error.h>
#include <sourcemeta/core/http_method.h>
#include <sourcemeta/core/http_status.h>
#include <sourcemeta/core/test.h>

#include <chrono>   // std::chrono::milliseconds, std::chrono::seconds
#include <optional> // std::optional
#include <string>   // std::string

TEST(message) {
  const sourcemeta::core::HTTPError error{"Connection refused",
                                          sourcemeta::core::HTTPMethod::GET,
                                          "https://example.com"};
  EXPECT_STREQ(error.what(), "Connection refused");
}

TEST(method) {
  const sourcemeta::core::HTTPError error{"Connection refused",
                                          sourcemeta::core::HTTPMethod::POST,
                                          "https://example.com"};
  EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::POST);
}

TEST(detail) {
  const sourcemeta::core::HTTPError error{
      "The HTTP request failed", sourcemeta::core::HTTPMethod::GET,
      "https://example.com", "Connection refused by peer"};
  EXPECT_EQ(error.detail(), "Connection refused by peer");
}

TEST(detail_is_empty_without_one) {
  const sourcemeta::core::HTTPError error{"Connection refused",
                                          sourcemeta::core::HTTPMethod::GET,
                                          "https://example.com"};
  EXPECT_EQ(error.detail(), "");
}

TEST(url) {
  const sourcemeta::core::HTTPError error{"Connection refused",
                                          sourcemeta::core::HTTPMethod::GET,
                                          "https://example.com/schema.json"};
  EXPECT_EQ(error.url(), "https://example.com/schema.json");
}

TEST(timeout_error_message) {
  const sourcemeta::core::HTTPTimeoutError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      std::chrono::seconds{30}};
  EXPECT_STREQ(error.what(), "The HTTP request timed out");
}

TEST(timeout_error_method) {
  const sourcemeta::core::HTTPTimeoutError error{
      sourcemeta::core::HTTPMethod::POST, "https://example.com",
      std::chrono::seconds{30}};
  EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::POST);
}

TEST(timeout_error_url) {
  const sourcemeta::core::HTTPTimeoutError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com/schema.json",
      std::chrono::seconds{30}};
  EXPECT_EQ(error.url(), "https://example.com/schema.json");
}

TEST(timeout_error_timeout) {
  const sourcemeta::core::HTTPTimeoutError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      std::chrono::milliseconds{1500}};
  EXPECT_EQ(error.timeout(), std::chrono::milliseconds{1500});
}

TEST(timeout_error_caught_as_base) {
  try {
    throw sourcemeta::core::HTTPTimeoutError{sourcemeta::core::HTTPMethod::GET,
                                             "https://example.com",
                                             std::chrono::seconds{30}};
  } catch (const sourcemeta::core::HTTPError &error) {
    EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::GET);
    EXPECT_EQ(error.url(), "https://example.com");
    EXPECT_STREQ(error.what(), "The HTTP request timed out");
  }
}

TEST(status_error_message) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_STREQ(error.what(), "Unsuccessful HTTP response");
}

TEST(status_error_method) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::POST, "https://example.com",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::POST);
}

TEST(status_error_url) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com/schema.json",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_EQ(error.url(), "https://example.com/schema.json");
}

TEST(status_error_status) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE};
  EXPECT_EQ(error.status(), sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE);
  EXPECT_EQ(error.status().code, 503);
}

TEST(status_error_owns_status_data) {
  std::optional<sourcemeta::core::HTTPStatusError> error;
  {
    const std::string phrase{"Custom Experimental Phrase"};
    const std::string wire{"599 Custom Experimental Phrase"};
    const sourcemeta::core::HTTPStatus status{
        .code = 599, .phrase = phrase, .wire = wire};
    error.emplace(sourcemeta::core::HTTPMethod::GET, "https://example.com",
                  status);
  }

  EXPECT_EQ(error.value().status().code, 599);
  EXPECT_EQ(error.value().status().phrase, "Custom Experimental Phrase");
  EXPECT_EQ(error.value().status().wire, "599 Custom Experimental Phrase");
}

TEST(entropy_error_message) {
  const sourcemeta::core::HTTPEntropyError error{};
  EXPECT_STREQ(error.what(),
               "Could not draw a non-zero trace context identifier");
}
