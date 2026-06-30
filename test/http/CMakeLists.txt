sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME http
  SOURCES http_status_test.cc http_make_problem_details_test.cc
          http_match_accept_test.cc http_match_accept_language_test.cc
          http_negotiate_encoding_test.cc http_from_date_test.cc
          http_format_link_test.cc http_field_list_contains_any_test.cc
          http_accept_includes_all_test.cc http_content_type_matches_test.cc
          http_method_test.cc http_status_from_code_test.cc
          http_is_status_line_test.cc http_accumulate_header_line_test.cc
          http_parse_headers_test.cc http_serialize_headers_test.cc
          http_header_find_test.cc http_error_test.cc
          http_parse_bearer_test.cc http_cache_control_max_age_test.cc)

target_link_libraries(sourcemeta_core_http_unit
  PRIVATE sourcemeta::core::http)
