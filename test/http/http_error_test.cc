#include <gtest/gtest.h>

#include <sourcemeta/core/http_error.h>
#include <sourcemeta/core/http_method.h>
#include <sourcemeta/core/http_status.h>

#include <optional> // std::optional
#include <string>   // std::string

TEST(HTTP_error, message) {
  const sourcemeta::core::HTTPError error{sourcemeta::core::HTTPMethod::GET,
                                          "https://example.com",
                                          "Connection refused"};
  EXPECT_STREQ(error.what(), "Connection refused");
}

TEST(HTTP_error, method) {
  const sourcemeta::core::HTTPError error{sourcemeta::core::HTTPMethod::POST,
                                          "https://example.com",
                                          "Connection refused"};
  EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::POST);
}

TEST(HTTP_error, url) {
  const sourcemeta::core::HTTPError error{sourcemeta::core::HTTPMethod::GET,
                                          "https://example.com/schema.json",
                                          "Connection refused"};
  EXPECT_EQ(error.url(), "https://example.com/schema.json");
}

TEST(HTTP_error, status_error_message) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_STREQ(error.what(), "Unsuccessful HTTP response");
}

TEST(HTTP_error, status_error_method) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::POST, "https://example.com",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_EQ(error.method(), sourcemeta::core::HTTPMethod::POST);
}

TEST(HTTP_error, status_error_url) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com/schema.json",
      sourcemeta::core::HTTP_STATUS_NOT_FOUND};
  EXPECT_EQ(error.url(), "https://example.com/schema.json");
}

TEST(HTTP_error, status_error_status) {
  const sourcemeta::core::HTTPStatusError error{
      sourcemeta::core::HTTPMethod::GET, "https://example.com",
      sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE};
  EXPECT_EQ(error.status(), sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE);
  EXPECT_EQ(error.status().code, 503);
}

TEST(HTTP_error, status_error_owns_status_data) {
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
