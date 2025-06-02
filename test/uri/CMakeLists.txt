sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME uri
  SOURCES
    uri_test.cc
    uri_fragment_test.cc
    uri_empty_test.cc
    uri_host_test.cc
    uri_path_test.cc
    uri_parse_test.cc
    uri_port_test.cc
    uri_scheme_test.cc
    uri_query_test.cc
    uri_is_absolute_test.cc
    uri_is_relative_test.cc
    uri_is_urn_test.cc
    uri_is_tag_test.cc
    uri_is_mailto_test.cc
    uri_is_ipv6_test.cc
    uri_rebase_test.cc
    uri_recompose_test.cc
    uri_recompose_without_fragment_test.cc
    uri_normalize_test.cc
    uri_canonicalize_test.cc
    uri_resolve_from_test.cc
    uri_relative_to_test.cc
    uri_escape_test.cc
    uri_unescape_test.cc
    uri_extension_test.cc
    uri_user_info_test.cc)

target_link_libraries(sourcemeta_core_uri_unit
  PRIVATE sourcemeta::core::uri)
