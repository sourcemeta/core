sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME ip
  SOURCES ipv4_test.cc ipv6_test.cc)

target_link_libraries(sourcemeta_core_ip_unit
  PRIVATE sourcemeta::core::ip)
