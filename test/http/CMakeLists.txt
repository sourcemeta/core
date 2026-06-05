sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME http
  SOURCES http_status_test.cc http_make_problem_details_test.cc
          http_match_accept_test.cc http_match_accept_language_test.cc
          http_negotiate_encoding_test.cc http_from_date_test.cc
          http_format_link_test.cc http_field_list_contains_any_test.cc)

target_link_libraries(sourcemeta_core_http_unit
  PRIVATE sourcemeta::core::http)
