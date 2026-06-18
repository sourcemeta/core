sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME http_ci
  SOURCES http_system_request_test.cc)

target_link_libraries(sourcemeta_core_http_ci_unit
  PRIVATE sourcemeta::core::http)
target_link_libraries(sourcemeta_core_http_ci_unit
  PRIVATE sourcemeta::core::json)
