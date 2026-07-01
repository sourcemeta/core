#include <sourcemeta/core/test.h>

#include <sourcemeta/core/http_status.h>

TEST(ok_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_OK.code, 200);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_OK.phrase, "OK");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_OK.wire, "200 OK");
}

TEST(created_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CREATED.code, 201);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CREATED.phrase, "Created");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CREATED.wire, "201 Created");
}

TEST(no_content_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NO_CONTENT.code, 204);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NO_CONTENT.phrase, "No Content");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NO_CONTENT.wire, "204 No Content");
}

TEST(not_modified_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED.code, 304);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED.phrase, "Not Modified");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_MODIFIED.wire,
            "304 Not Modified");
}

TEST(bad_request_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_BAD_REQUEST.code, 400);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_BAD_REQUEST.phrase, "Bad Request");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_BAD_REQUEST.wire, "400 Bad Request");
}

TEST(forbidden_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_FORBIDDEN.code, 403);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_FORBIDDEN.phrase, "Forbidden");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_FORBIDDEN.wire, "403 Forbidden");
}

TEST(not_found_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_FOUND.code, 404);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_FOUND.phrase, "Not Found");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_FOUND.wire, "404 Not Found");
}

TEST(method_not_allowed_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED.code, 405);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED.phrase,
            "Method Not Allowed");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED.wire,
            "405 Method Not Allowed");
}

TEST(not_acceptable_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE.code, 406);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE.phrase,
            "Not Acceptable");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE.wire,
            "406 Not Acceptable");
}

TEST(content_too_large_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE.code, 413);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE.phrase,
            "Content Too Large");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE.wire,
            "413 Content Too Large");
}

TEST(unsupported_media_type_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE.code, 415);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE.phrase,
            "Unsupported Media Type");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE.wire,
            "415 Unsupported Media Type");
}

TEST(internal_server_error_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR.code, 500);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR.phrase,
            "Internal Server Error");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR.wire,
            "500 Internal Server Error");
}

TEST(service_unavailable_fields) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE.code, 503);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE.phrase,
            "Service Unavailable");
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE.wire,
            "503 Service Unavailable");
}

TEST(equality_reflexive) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_OK, sourcemeta::core::HTTP_STATUS_OK);
}

TEST(inequality_different_codes) {
  EXPECT_NE(sourcemeta::core::HTTP_STATUS_OK,
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
}

TEST(equality_copy) {
  constexpr auto copy{sourcemeta::core::HTTP_STATUS_OK};
  EXPECT_EQ(copy, sourcemeta::core::HTTP_STATUS_OK);
}

TEST(webdav_multi_status) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_MULTI_STATUS.code, 207);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_MULTI_STATUS.wire,
            "207 Multi-Status");
}

TEST(im_a_teapot) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_IM_A_TEAPOT.code, 418);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_IM_A_TEAPOT.wire, "418 I'm a Teapot");
}

TEST(unavailable_for_legal_reasons) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS.code,
            451);
}

TEST(switching_protocols) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_SWITCHING_PROTOCOLS.code, 101);
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_SWITCHING_PROTOCOLS.wire,
            "101 Switching Protocols");
}

TEST(network_authentication_required) {
  EXPECT_EQ(sourcemeta::core::HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED.code,
            511);
}

TEST(wire_starts_with_code_string) {
  EXPECT_TRUE(sourcemeta::core::HTTP_STATUS_OK.wire.starts_with("200"));
  EXPECT_TRUE(sourcemeta::core::HTTP_STATUS_NOT_FOUND.wire.starts_with("404"));
  EXPECT_TRUE(
      sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR.wire.starts_with(
          "500"));
}
