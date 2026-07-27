sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME dns
  SOURCES hostname_test.cc idn_hostname_test.cc idn_hostname_uts46_test.cc)

target_link_libraries(sourcemeta_core_dns_unit
  PRIVATE sourcemeta::core::dns)

sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME idn_hostname_uts46_suite
  SOURCES idn_hostname_uts46_suite.cc)
target_compile_definitions(sourcemeta_core_idn_hostname_uts46_suite_unit
  PRIVATE
    IDNATESTV2_PATH="${PROJECT_SOURCE_DIR}/vendor/unicodetools/unicodetools/data/idna/dev/IdnaTestV2.txt"
    IDNAMAPPINGTABLE_PATH="${PROJECT_SOURCE_DIR}/vendor/unicodetools/unicodetools/data/idna/dev/IdnaMappingTable.txt")
target_link_libraries(sourcemeta_core_idn_hostname_uts46_suite_unit
  PRIVATE
    sourcemeta::core::dns
    sourcemeta::core::unicode
    sourcemeta::core::io
    sourcemeta::core::text
    sourcemeta::core::numeric)
