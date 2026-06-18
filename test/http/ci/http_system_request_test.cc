#include <gtest/gtest.h>

#include <sourcemeta/core/http.h>
#include <sourcemeta/core/json.h>

#include <chrono> // std::chrono::milliseconds

TEST(HTTP_SystemRequest, get_health_ok) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  // Backends may normalise the effective URL differently, so match its
  // meaningful prefix rather than an exact string
  EXPECT_TRUE(response.url.starts_with(
      "https://schemas.sourcemeta.com/self/v1/health"));
}

TEST(HTTP_SystemRequest, head_health_ok) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health",
      sourcemeta::core::HTTPMethod::HEAD};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  EXPECT_TRUE(response.body.empty());
}

TEST(HTTP_SystemRequest, get_list_json_body) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  const auto content_type{
      sourcemeta::core::http_header_find(response.headers, "content-type")};
  ASSERT_TRUE(content_type.has_value());
  EXPECT_NE(content_type.value().find("application/json"),
            std::string_view::npos);
  const auto body{sourcemeta::core::parse_json(response.body)};
  EXPECT_TRUE(body.is_object());
  EXPECT_TRUE(body.defines("entries"));
}

TEST(HTTP_SystemRequest, get_missing_path_not_found) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list/"
      "this-directory-does-not-exist-xyz"};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_NOT_FOUND);
  const auto content_type{
      sourcemeta::core::http_header_find(response.headers, "content-type")};
  ASSERT_TRUE(content_type.has_value());
  EXPECT_NE(content_type.value().find("application/problem+json"),
            std::string_view::npos);
}

TEST(HTTP_SystemRequest, post_health_method_not_allowed) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health",
      sourcemeta::core::HTTPMethod::POST};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED);
}

TEST(HTTP_SystemRequest, post_evaluate_with_body) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/schemas/evaluate/"
      "cloudevents/v1.0.2/cloudevents",
      sourcemeta::core::HTTPMethod::POST};
  request.body("{}", "application/json");
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  const auto body{sourcemeta::core::parse_json(response.body)};
  EXPECT_TRUE(body.is_object());
  ASSERT_TRUE(body.defines("valid"));
  EXPECT_TRUE(body.at("valid").is_boolean());
  EXPECT_FALSE(body.at("valid").to_boolean());
}

TEST(HTTP_SystemRequest, post_evaluate_missing_instance_bad_request) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/schemas/evaluate/"
      "cloudevents/v1.0.2/cloudevents",
      sourcemeta::core::HTTPMethod::POST};
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(HTTP_SystemRequest, follow_redirect_to_https) {
  sourcemeta::core::HTTPSystemRequest request{
      "http://schemas.sourcemeta.com/self/v1/health"};
  request.follow_redirects(true);
  const auto response{request.send()};
  EXPECT_EQ(response.status, sourcemeta::core::HTTP_STATUS_OK);
  // The redirect upgrades the scheme to HTTPS, which the effective URL
  // reflects regardless of backend-specific normalisation
  EXPECT_TRUE(response.url.starts_with(
      "https://schemas.sourcemeta.com/self/v1/health"));
}

TEST(HTTP_SystemRequest, no_follow_redirect_returns_redirect) {
  sourcemeta::core::HTTPSystemRequest request{
      "http://schemas.sourcemeta.com/self/v1/health"};
  request.follow_redirects(false);
  const auto response{request.send()};
  EXPECT_GE(response.status.code, 300);
  EXPECT_LT(response.status.code, 400);
}

TEST(HTTP_SystemRequest, total_timeout_too_small_throws) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/health"};
  request.timeout(std::chrono::milliseconds{1});
  EXPECT_THROW(
      { [[maybe_unused]] const auto response{request.send()}; },
      sourcemeta::core::HTTPError);
}

TEST(HTTP_SystemRequest, unresolvable_host_throws) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://this-host-does-not-exist.sourcemeta.invalid/"};
  EXPECT_THROW(
      { [[maybe_unused]] const auto response{request.send()}; },
      sourcemeta::core::HTTPError);
}

TEST(HTTP_SystemRequest, maximum_response_size_exceeded_throws) {
  sourcemeta::core::HTTPSystemRequest request{
      "https://schemas.sourcemeta.com/self/v1/api/list"};
  request.maximum_response_size(1);
  EXPECT_THROW(
      { [[maybe_unused]] const auto response{request.send()}; },
      sourcemeta::core::HTTPError);
}
