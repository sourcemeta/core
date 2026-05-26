sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME idna
  SOURCES
    idna_property_test.cc
    idna_passes_contexto_test.cc
    idna_passes_contextj_test.cc
    idna_passes_bidi_rule_test.cc)

target_link_libraries(sourcemeta_core_idna_unit
  PRIVATE sourcemeta::core::idna)
