sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME email
  SOURCES email_test.cc idn_email_test.cc)

target_link_libraries(sourcemeta_core_email_unit
  PRIVATE sourcemeta::core::email)
