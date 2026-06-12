#include <gtest/gtest.h>

#include <sourcemeta/core/http_status.h>

TEST(HTTP_status_from_code, continue_100) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(100),
            sourcemeta::core::HTTP_STATUS_CONTINUE);
}

TEST(HTTP_status_from_code, switching_protocols_101) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(101),
            sourcemeta::core::HTTP_STATUS_SWITCHING_PROTOCOLS);
}

TEST(HTTP_status_from_code, processing_102) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(102),
            sourcemeta::core::HTTP_STATUS_PROCESSING);
}

TEST(HTTP_status_from_code, early_hints_103) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(103),
            sourcemeta::core::HTTP_STATUS_EARLY_HINTS);
}

TEST(HTTP_status_from_code, ok_200) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(200),
            sourcemeta::core::HTTP_STATUS_OK);
}

TEST(HTTP_status_from_code, created_201) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(201),
            sourcemeta::core::HTTP_STATUS_CREATED);
}

TEST(HTTP_status_from_code, accepted_202) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(202),
            sourcemeta::core::HTTP_STATUS_ACCEPTED);
}

TEST(HTTP_status_from_code, non_authoritative_information_203) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(203),
            sourcemeta::core::HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION);
}

TEST(HTTP_status_from_code, no_content_204) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(204),
            sourcemeta::core::HTTP_STATUS_NO_CONTENT);
}

TEST(HTTP_status_from_code, reset_content_205) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(205),
            sourcemeta::core::HTTP_STATUS_RESET_CONTENT);
}

TEST(HTTP_status_from_code, partial_content_206) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(206),
            sourcemeta::core::HTTP_STATUS_PARTIAL_CONTENT);
}

TEST(HTTP_status_from_code, multi_status_207) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(207),
            sourcemeta::core::HTTP_STATUS_MULTI_STATUS);
}

TEST(HTTP_status_from_code, already_reported_208) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(208),
            sourcemeta::core::HTTP_STATUS_ALREADY_REPORTED);
}

TEST(HTTP_status_from_code, im_used_226) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(226),
            sourcemeta::core::HTTP_STATUS_IM_USED);
}

TEST(HTTP_status_from_code, multiple_choices_300) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(300),
            sourcemeta::core::HTTP_STATUS_MULTIPLE_CHOICES);
}

TEST(HTTP_status_from_code, moved_permanently_301) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(301),
            sourcemeta::core::HTTP_STATUS_MOVED_PERMANENTLY);
}

TEST(HTTP_status_from_code, found_302) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(302),
            sourcemeta::core::HTTP_STATUS_FOUND);
}

TEST(HTTP_status_from_code, see_other_303) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(303),
            sourcemeta::core::HTTP_STATUS_SEE_OTHER);
}

TEST(HTTP_status_from_code, not_modified_304) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(304),
            sourcemeta::core::HTTP_STATUS_NOT_MODIFIED);
}

TEST(HTTP_status_from_code, use_proxy_305) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(305),
            sourcemeta::core::HTTP_STATUS_USE_PROXY);
}

TEST(HTTP_status_from_code, temporary_redirect_307) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(307),
            sourcemeta::core::HTTP_STATUS_TEMPORARY_REDIRECT);
}

TEST(HTTP_status_from_code, permanent_redirect_308) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(308),
            sourcemeta::core::HTTP_STATUS_PERMANENT_REDIRECT);
}

TEST(HTTP_status_from_code, bad_request_400) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(400),
            sourcemeta::core::HTTP_STATUS_BAD_REQUEST);
}

TEST(HTTP_status_from_code, unauthorized_401) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(401),
            sourcemeta::core::HTTP_STATUS_UNAUTHORIZED);
}

TEST(HTTP_status_from_code, payment_required_402) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(402),
            sourcemeta::core::HTTP_STATUS_PAYMENT_REQUIRED);
}

TEST(HTTP_status_from_code, forbidden_403) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(403),
            sourcemeta::core::HTTP_STATUS_FORBIDDEN);
}

TEST(HTTP_status_from_code, not_found_404) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(404),
            sourcemeta::core::HTTP_STATUS_NOT_FOUND);
}

TEST(HTTP_status_from_code, method_not_allowed_405) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(405),
            sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED);
}

TEST(HTTP_status_from_code, not_acceptable_406) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(406),
            sourcemeta::core::HTTP_STATUS_NOT_ACCEPTABLE);
}

TEST(HTTP_status_from_code, proxy_authentication_required_407) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(407),
            sourcemeta::core::HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED);
}

TEST(HTTP_status_from_code, request_timeout_408) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(408),
            sourcemeta::core::HTTP_STATUS_REQUEST_TIMEOUT);
}

TEST(HTTP_status_from_code, conflict_409) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(409),
            sourcemeta::core::HTTP_STATUS_CONFLICT);
}

TEST(HTTP_status_from_code, gone_410) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(410),
            sourcemeta::core::HTTP_STATUS_GONE);
}

TEST(HTTP_status_from_code, length_required_411) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(411),
            sourcemeta::core::HTTP_STATUS_LENGTH_REQUIRED);
}

TEST(HTTP_status_from_code, precondition_failed_412) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(412),
            sourcemeta::core::HTTP_STATUS_PRECONDITION_FAILED);
}

TEST(HTTP_status_from_code, content_too_large_413) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(413),
            sourcemeta::core::HTTP_STATUS_CONTENT_TOO_LARGE);
}

TEST(HTTP_status_from_code, uri_too_long_414) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(414),
            sourcemeta::core::HTTP_STATUS_URI_TOO_LONG);
}

TEST(HTTP_status_from_code, unsupported_media_type_415) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(415),
            sourcemeta::core::HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);
}

TEST(HTTP_status_from_code, range_not_satisfiable_416) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(416),
            sourcemeta::core::HTTP_STATUS_RANGE_NOT_SATISFIABLE);
}

TEST(HTTP_status_from_code, expectation_failed_417) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(417),
            sourcemeta::core::HTTP_STATUS_EXPECTATION_FAILED);
}

TEST(HTTP_status_from_code, im_a_teapot_418) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(418),
            sourcemeta::core::HTTP_STATUS_IM_A_TEAPOT);
}

TEST(HTTP_status_from_code, misdirected_request_421) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(421),
            sourcemeta::core::HTTP_STATUS_MISDIRECTED_REQUEST);
}

TEST(HTTP_status_from_code, unprocessable_content_422) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(422),
            sourcemeta::core::HTTP_STATUS_UNPROCESSABLE_CONTENT);
}

TEST(HTTP_status_from_code, locked_423) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(423),
            sourcemeta::core::HTTP_STATUS_LOCKED);
}

TEST(HTTP_status_from_code, failed_dependency_424) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(424),
            sourcemeta::core::HTTP_STATUS_FAILED_DEPENDENCY);
}

TEST(HTTP_status_from_code, too_early_425) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(425),
            sourcemeta::core::HTTP_STATUS_TOO_EARLY);
}

TEST(HTTP_status_from_code, upgrade_required_426) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(426),
            sourcemeta::core::HTTP_STATUS_UPGRADE_REQUIRED);
}

TEST(HTTP_status_from_code, precondition_required_428) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(428),
            sourcemeta::core::HTTP_STATUS_PRECONDITION_REQUIRED);
}

TEST(HTTP_status_from_code, too_many_requests_429) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(429),
            sourcemeta::core::HTTP_STATUS_TOO_MANY_REQUESTS);
}

TEST(HTTP_status_from_code, request_header_fields_too_large_431) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(431),
            sourcemeta::core::HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE);
}

TEST(HTTP_status_from_code, unavailable_for_legal_reasons_451) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(451),
            sourcemeta::core::HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS);
}

TEST(HTTP_status_from_code, internal_server_error_500) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(500),
            sourcemeta::core::HTTP_STATUS_INTERNAL_SERVER_ERROR);
}

TEST(HTTP_status_from_code, not_implemented_501) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(501),
            sourcemeta::core::HTTP_STATUS_NOT_IMPLEMENTED);
}

TEST(HTTP_status_from_code, bad_gateway_502) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(502),
            sourcemeta::core::HTTP_STATUS_BAD_GATEWAY);
}

TEST(HTTP_status_from_code, service_unavailable_503) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(503),
            sourcemeta::core::HTTP_STATUS_SERVICE_UNAVAILABLE);
}

TEST(HTTP_status_from_code, gateway_timeout_504) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(504),
            sourcemeta::core::HTTP_STATUS_GATEWAY_TIMEOUT);
}

TEST(HTTP_status_from_code, http_version_not_supported_505) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(505),
            sourcemeta::core::HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED);
}

TEST(HTTP_status_from_code, variant_also_negotiates_506) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(506),
            sourcemeta::core::HTTP_STATUS_VARIANT_ALSO_NEGOTIATES);
}

TEST(HTTP_status_from_code, insufficient_storage_507) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(507),
            sourcemeta::core::HTTP_STATUS_INSUFFICIENT_STORAGE);
}

TEST(HTTP_status_from_code, loop_detected_508) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(508),
            sourcemeta::core::HTTP_STATUS_LOOP_DETECTED);
}

TEST(HTTP_status_from_code, not_extended_510) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(510),
            sourcemeta::core::HTTP_STATUS_NOT_EXTENDED);
}

TEST(HTTP_status_from_code, network_authentication_required_511) {
  EXPECT_EQ(sourcemeta::core::http_status_from_code(511),
            sourcemeta::core::HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED);
}

TEST(HTTP_status_from_code, unknown_0) {
  const auto status{sourcemeta::core::http_status_from_code(0)};
  EXPECT_EQ(status.code, 0);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_99) {
  const auto status{sourcemeta::core::http_status_from_code(99)};
  EXPECT_EQ(status.code, 99);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_199) {
  const auto status{sourcemeta::core::http_status_from_code(199)};
  EXPECT_EQ(status.code, 199);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_299) {
  const auto status{sourcemeta::core::http_status_from_code(299)};
  EXPECT_EQ(status.code, 299);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_306) {
  const auto status{sourcemeta::core::http_status_from_code(306)};
  EXPECT_EQ(status.code, 306);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_399) {
  const auto status{sourcemeta::core::http_status_from_code(399)};
  EXPECT_EQ(status.code, 399);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_420) {
  const auto status{sourcemeta::core::http_status_from_code(420)};
  EXPECT_EQ(status.code, 420);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_499) {
  const auto status{sourcemeta::core::http_status_from_code(499)};
  EXPECT_EQ(status.code, 499);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_509) {
  const auto status{sourcemeta::core::http_status_from_code(509)};
  EXPECT_EQ(status.code, 509);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_599) {
  const auto status{sourcemeta::core::http_status_from_code(599)};
  EXPECT_EQ(status.code, 599);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}

TEST(HTTP_status_from_code, unknown_999) {
  const auto status{sourcemeta::core::http_status_from_code(999)};
  EXPECT_EQ(status.code, 999);
  EXPECT_TRUE(status.phrase.empty());
  EXPECT_TRUE(status.wire.empty());
}
