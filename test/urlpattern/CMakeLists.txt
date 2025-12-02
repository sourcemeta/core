sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME urlpattern
  SOURCES
    urlpattern_test_utils.h
    urlpattern_parse_test.cc
    urlpattern_parse_string_test.cc
    urlpattern_parse_json_test.cc
    urlpattern_parse_input_json_test.cc
    urlpattern_match_test.cc
    urlpattern_match_pathname_test.cc
    urlpattern_parse_pathname_test.cc
    urlpattern_parse_hostname_test.cc
    urlpattern_match_hostname_test.cc
    urlpattern_parse_protocol_test.cc
    urlpattern_parse_username_test.cc
    urlpattern_parse_password_test.cc
    urlpattern_parse_port_test.cc
    urlpattern_parse_search_test.cc
    urlpattern_parse_hash_test.cc
    urlpattern_match_protocol_test.cc
    urlpattern_match_username_test.cc
    urlpattern_match_password_test.cc
    urlpattern_match_port_test.cc
    urlpattern_match_search_test.cc
    urlpattern_match_hash_test.cc
    urlpattern_compare_test.cc)

target_link_libraries(sourcemeta_core_urlpattern_unit
  PRIVATE sourcemeta::core::urlpattern)

# WPT URL Pattern Test Suite
sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME urlpattern_wpt
  SOURCES urlpattern_wpt.cc)
target_compile_definitions(sourcemeta_core_urlpattern_wpt_unit
  PRIVATE WPT_URLPATTERN_PATH="${PROJECT_SOURCE_DIR}/vendor/wpt/urlpattern")
target_link_libraries(sourcemeta_core_urlpattern_wpt_unit
  PRIVATE sourcemeta::core::urlpattern)
target_link_libraries(sourcemeta_core_urlpattern_wpt_unit
  PRIVATE sourcemeta::core::json)
target_link_libraries(sourcemeta_core_urlpattern_wpt_unit
  PRIVATE sourcemeta::core::io)
