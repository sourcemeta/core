#include <sourcemeta/core/http.h>
#include <sourcemeta/core/test.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair
#include <vector>      // std::vector

TEST(credential_scope) {
  EXPECT_EQ(sourcemeta::core::http_aws_sigv4_credential_scope(
                "20150830", "us-east-1", "service"),
            "20150830/us-east-1/service/aws4_request");
}

TEST(canonical_request_empty_path_becomes_root) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "", "", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  EXPECT_TRUE(canonical.starts_with("GET\n/\n"));
}

TEST(canonical_request_default_normalizes_path) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/foo/../bar", "", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  EXPECT_TRUE(canonical.starts_with("GET\n/bar\n"));
}

TEST(canonical_request_s3_preserves_dot_segments) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/foo/../bar", "", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
      false)};
  EXPECT_TRUE(canonical.starts_with("GET\n/foo/../bar\n"));
}

TEST(canonical_request_preserves_encoded_query_delimiter) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/", "a=b%26c&x=y", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  EXPECT_TRUE(canonical.find("\na=b%26c&x=y\n") != std::string::npos);
}

TEST(canonical_request_does_not_double_encode_query) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/", "a=b%20c", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  EXPECT_TRUE(canonical.find("\na=b%20c\n") != std::string::npos);
}

TEST(canonical_request_preserves_encoded_path_slash) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"Host", "example.amazonaws.com"}};
  const auto canonical{sourcemeta::core::http_aws_sigv4_canonical_request(
      "GET", "/foo%2Fbar", "", headers,
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")};
  EXPECT_TRUE(canonical.starts_with("GET\n/foo%2Fbar\n"));
}

TEST(signed_headers_lowercases_and_sorts) {
  const std::vector<std::pair<std::string_view, std::string_view>> headers{
      {"X-Amz-Date", "20150830T123600Z"}, {"Host", "example.amazonaws.com"}};
  EXPECT_EQ(sourcemeta::core::http_aws_sigv4_signed_headers(headers),
            "host;x-amz-date");
}

TEST(authorization_format) {
  EXPECT_EQ(
      sourcemeta::core::http_aws_sigv4_authorization(
          "AKIDEXAMPLE", "20150830/us-east-1/service/aws4_request",
          "host;x-amz-date", "abc123"),
      "AWS4-HMAC-SHA256 Credential=AKIDEXAMPLE/20150830/us-east-1/service/"
      "aws4_request, SignedHeaders=host;x-amz-date, Signature=abc123");
}

TEST(string_to_sign_format) {
  EXPECT_EQ(
      sourcemeta::core::http_aws_sigv4_string_to_sign(
          "20150830T123600Z", "20150830/us-east-1/service/aws4_request", ""),
      "AWS4-HMAC-SHA256\n20150830T123600Z\n"
      "20150830/us-east-1/service/aws4_request\n"
      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(signing_key_size) {
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", "20150830", "us-east-1",
      "service")};
  EXPECT_EQ(key.size(), 32);
}

// The signing key and signature for the suite's get-vanilla string to sign
TEST(signature_known_vector) {
  const std::string string_to_sign{
      "AWS4-HMAC-SHA256\n20150830T123600Z\n"
      "20150830/us-east-1/service/aws4_request\n"
      "bb579772317eb040ac9ed261061d46c1f17a8133879d6129b6e1c25292927e63"};
  const auto key{sourcemeta::core::http_aws_sigv4_signing_key(
      "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY", "20150830", "us-east-1",
      "service")};
  EXPECT_EQ(sourcemeta::core::http_aws_sigv4_signature(key, string_to_sign),
            "5fa00fa31553b73ebf1942676e86291e8372ff2a2260956d9b8aae1d763fbf31");
}
