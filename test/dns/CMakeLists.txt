sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME dns
  SOURCES hostname_test.cc)

target_link_libraries(sourcemeta_core_dns_unit
  PRIVATE sourcemeta::core::dns)
