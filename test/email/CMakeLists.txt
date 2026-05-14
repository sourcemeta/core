sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME email
  SOURCES email_test.cc)

target_link_libraries(sourcemeta_core_email_unit
  PRIVATE sourcemeta::core::email)
