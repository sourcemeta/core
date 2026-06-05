#include <gtest/gtest.h>

#include <sourcemeta/core/http_problem.h>
#include <sourcemeta/core/http_status.h>
#include <sourcemeta/core/json.h>

TEST(HTTP_make_problem_details, defaults_only_with_not_found_status) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Not Found",
    "status": 404
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, defaults_only_with_internal_server_error) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Internal Server Error",
    "status": 500
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, type_override_replaces_about_blank) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
       .type = "https://example.com/probs/invalid-uri"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "https://example.com/probs/invalid-uri",
    "title": "Bad Request",
    "status": 400
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, title_override_replaces_status_phrase) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
       .title = "Schema unavailable"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Schema unavailable",
    "status": 404
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, detail_added_when_non_empty) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
       .detail = "There is nothing at this URL"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Not Found",
    "status": 404,
    "detail": "There is nothing at this URL"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, instance_added_when_non_empty) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
       .instance = "/requests/abc-123"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Not Found",
    "status": 404,
    "instance": "/requests/abc-123"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, every_field_set) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_BAD_REQUEST,
       .type = "https://example.com/probs/invalid-uri",
       .title = "Invalid URI",
       .detail = "The schema URI must not contain a fragment",
       .instance = "/requests/xyz-789"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "https://example.com/probs/invalid-uri",
    "title": "Invalid URI",
    "status": 400,
    "detail": "The schema URI must not contain a fragment",
    "instance": "/requests/xyz-789"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, sourcemeta_urn_type) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
       .type = "sourcemeta:one/not-found",
       .detail = "There is nothing at this URL"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "sourcemeta:one/not-found",
    "title": "Not Found",
    "status": 404,
    "detail": "There is nothing at this URL"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, ok_status_200) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_OK})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "OK",
    "status": 200
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, method_not_allowed_with_detail) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
       .detail = "This HTTP method is invalid for this URL"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "about:blank",
    "title": "Method Not Allowed",
    "status": 405,
    "detail": "This HTTP method is invalid for this URL"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, payload_too_large_with_full_fields) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_PAYLOAD_TOO_LARGE,
       .type = "sourcemeta:one/payload-too-large",
       .detail = "The request body exceeded the maximum allowed size",
       .instance = "/requests/abc-123"})};
  const auto expected{sourcemeta::core::parse_json(R"JSON({
    "type": "sourcemeta:one/payload-too-large",
    "title": "Payload Too Large",
    "status": 413,
    "detail": "The request body exceeded the maximum allowed size",
    "instance": "/requests/abc-123"
  })JSON")};
  EXPECT_EQ(body, expected);
}

TEST(HTTP_make_problem_details, output_is_object) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_OK})};
  EXPECT_TRUE(body.is_object());
}

TEST(HTTP_make_problem_details, key_order_is_canonical) {
  const auto body{sourcemeta::core::http_make_problem_details(
      {.status = sourcemeta::core::HTTP_STATUS_NOT_FOUND,
       .type = "t",
       .title = "T",
       .detail = "d",
       .instance = "i"})};
  EXPECT_TRUE(body.defines("type"));
  EXPECT_TRUE(body.defines("title"));
  EXPECT_TRUE(body.defines("status"));
  EXPECT_TRUE(body.defines("detail"));
  EXPECT_TRUE(body.defines("instance"));
  EXPECT_EQ(body.size(), 5);
}
