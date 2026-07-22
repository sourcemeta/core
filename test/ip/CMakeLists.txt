sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME ip
  SOURCES ipv4_test.cc ipv6_test.cc
    ipv4_classify_test.cc ipv6_classify_test.cc
    ip_is_globally_routable_test.cc)

target_link_libraries(sourcemeta_core_ip_unit
  PRIVATE sourcemeta::core::ip)
