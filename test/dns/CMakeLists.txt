sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME dns
  SOURCES hostname_test.cc idn_hostname_test.cc)

target_link_libraries(sourcemeta_core_dns_unit
  PRIVATE sourcemeta::core::dns)
