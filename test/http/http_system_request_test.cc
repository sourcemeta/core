#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/time.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

namespace {

// The authorization a GET with no body must produce when signed at
// 20150830T123600Z for the given Host header value
auto expected_get_authorization(const std::string_view host) -> std::string {
  const std::vector<std::pair<std::string_view, std::string_view>> signed_set{
      {"host", host},
      {"x-amz-date", "20150830T123600Z"},
      {"x-amz-content-sha256",
       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/", "", signed_set,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  const auto scope{sourcemeta::core::http_aws_sigv4_credential_scope(
      "20150830", "us-east-1", "service")};
  const auto string_to_sign{sourcemeta::core::http_aws_sigv4_string_to_sign(
      "20150830T123600Z", scope, canonical)};
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", "20150830", "us-east-1",
      "service")};
  return sourcemeta::core::http_aws_sigv4_authorization(
      "AKIDEXAMPLE", scope,
      sourcemeta::core::http_aws_sigv4_signed_headers(signed_set),
      sourcemeta::core::http_aws_sigv4_signature(key, string_to_sign));
}

} // namespace

TEST(headers_empty_by_default) {
  const sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  EXPECT_TRUE(request.headers().empty());
}

TEST(headers_returns_added_headers_in_order) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Accept", "application/json");
  request.header("X-Custom", "value");
  EXPECT_EQ(request.headers().size(), 2);
  EXPECT_EQ(request.headers().at(0).first, "Accept");
  EXPECT_EQ(request.headers().at(0).second, "application/json");
  EXPECT_EQ(request.headers().at(1).first, "X-Custom");
  EXPECT_EQ(request.headers().at(1).second, "value");
}

TEST(header_lookup_present) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Accept", "application/json");
  EXPECT_TRUE(request.header("Accept").has_value());
  EXPECT_EQ(request.header("Accept").value(), "application/json");
}

TEST(header_lookup_case_insensitive) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("Content-Type", "text/plain");
  EXPECT_EQ(request.header("content-type").value(), "text/plain");
  EXPECT_EQ(request.header("CONTENT-TYPE").value(), "text/plain");
}

TEST(header_lookup_absent) {
  const sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  EXPECT_FALSE(request.header("Accept").has_value());
}

TEST(header_lookup_returns_first_of_repeated) {
  sourcemeta::core::HTTPSystemRequest request{"https://example.com"};
  request.header("X-Repeated", "first");
  request.header("X-Repeated", "second");
  EXPECT_EQ(request.header("X-Repeated").value(), "first");
}

TEST(sign_aws_sigv4_stamps_amz_headers_and_authorization) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("x-amz-date").value(), "20150830T123600Z");
  EXPECT_EQ(request.header("x-amz-content-sha256").value(),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

  // The authorization the adapter produces must equal the pure functions
  // composed over the same signed header set
  const std::vector<std::pair<std::string_view, std::string_view>> signed_set{
      {"host", "example.amazonaws.com"},
      {"x-amz-date", "20150830T123600Z"},
      {"x-amz-content-sha256",
       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/", "", signed_set,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  const auto scope{sourcemeta::core::http_aws_sigv4_credential_scope(
      "20150830", "us-east-1", "service")};
  const auto string_to_sign{sourcemeta::core::http_aws_sigv4_string_to_sign(
      "20150830T123600Z", scope, canonical)};
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", "20150830", "us-east-1",
      "service")};
  const auto expected{sourcemeta::core::http_aws_sigv4_authorization(
      "AKIDEXAMPLE", scope,
      sourcemeta::core::http_aws_sigv4_signed_headers(signed_set),
      sourcemeta::core::http_aws_sigv4_signature(key, string_to_sign))};
  EXPECT_EQ(request.header("Authorization").value(), expected);
}

TEST(sign_aws_sigv4_passes_encoded_query_through) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/?a=b%26c",
      sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  const std::vector<std::pair<std::string_view, std::string_view>> signed_set{
      {"host", "example.amazonaws.com"},
      {"x-amz-date", "20150830T123600Z"},
      {"x-amz-content-sha256",
       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/", "a=b%26c", signed_set,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  const auto scope{sourcemeta::core::http_aws_sigv4_credential_scope(
      "20150830", "us-east-1", "service")};
  const auto string_to_sign{sourcemeta::core::http_aws_sigv4_string_to_sign(
      "20150830T123600Z", scope, canonical)};
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", "20150830", "us-east-1",
      "service")};
  const auto expected{sourcemeta::core::http_aws_sigv4_authorization(
      "AKIDEXAMPLE", scope,
      sourcemeta::core::http_aws_sigv4_signed_headers(signed_set),
      sourcemeta::core::http_aws_sigv4_signature(key, string_to_sign))};
  EXPECT_EQ(request.header("Authorization").value(), expected);
}

TEST(sign_aws_sigv4_hashes_the_body) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::POST};
  request.body("Param1=value1", "application/x-www-form-urlencoded");
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("x-amz-content-sha256").value(),
            sourcemeta::core::sha256("Param1=value1"));
  EXPECT_TRUE(request.header("Authorization").has_value());
}

TEST(sign_aws_sigv4_adds_session_token_when_present) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = "SESSIONTOKEN"},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("x-amz-security-token").value(), "SESSIONTOKEN");
  EXPECT_TRUE(
      request.header("Authorization").value().find("x-amz-security-token") !=
      std::string_view::npos);
}

TEST(sign_aws_sigv4_omits_session_token_when_absent) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_FALSE(request.header("x-amz-security-token").has_value());
}

TEST(sign_aws_sigv4_signs_host_with_non_default_port) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com:8443/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("Authorization").value(),
            expected_get_authorization("example.amazonaws.com:8443"));
}

TEST(sign_aws_sigv4_omits_default_port_from_host) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com:443/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("Authorization").value(),
            expected_get_authorization("example.amazonaws.com"));
}

TEST(sign_aws_sigv4_brackets_ipv6_host) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://[fd00::1]/", sourcemeta::core::HTTPMethod::GET};
  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.header("Authorization").value(),
            expected_get_authorization("[fd00::1]"));
}

TEST(sign_aws_sigv4_is_idempotent_on_resign) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::GET};
  const sourcemeta::core::HTTPAWSCredentials credentials{
      .access_key_id = "AKIDEXAMPLE",
      .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
      .session_token = ""};

  request.sign_aws_sigv4(credentials, "us-east-1", "service", moment);
  EXPECT_EQ(request.headers().size(), 3);
  const std::string first{request.header("Authorization").value()};

  request.sign_aws_sigv4(credentials, "us-east-1", "service", moment);
  // Re-signing replaces the previous signing headers rather than duplicating
  EXPECT_EQ(request.headers().size(), 3);
  EXPECT_EQ(request.header("Authorization").value(), first);
}

TEST(sign_aws_sigv4_replaces_stale_signing_headers) {
  const auto moment{
      sourcemeta::core::from_iso8601_basic("20150830T123600Z").value()};
  sourcemeta::core::HTTPSystemRequest request{
      "https://example.amazonaws.com/", sourcemeta::core::HTTPMethod::GET};
  request.header("X-Amz-Date", "STALE");
  request.header("Authorization", "STALE");

  request.sign_aws_sigv4(
      {.access_key_id = "AKIDEXAMPLE",
       .secret_access_key = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY",
       .session_token = ""},
      "us-east-1", "service", moment);

  EXPECT_EQ(request.headers().size(), 3);
  EXPECT_EQ(request.header("x-amz-date").value(), "20150830T123600Z");
  EXPECT_EQ(request.header("Authorization").value(),
            expected_get_authorization("example.amazonaws.com"));
}
