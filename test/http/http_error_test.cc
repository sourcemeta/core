#include <gtest/gtest.h>

#include <sourcemeta/core/http_error.h>
#include <sourcemeta/core/http_method.h>

#include <stdexcept> // std::runtime_error

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

TEST(HTTP_error, catchable_as_runtime_error) {
  EXPECT_THROW(
      throw sourcemeta::core::HTTPError(sourcemeta::core::HTTPMethod::GET,
                                        "https://example.com", "failure"),
      std::runtime_error);
}
