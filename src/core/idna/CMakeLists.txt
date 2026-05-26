sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME idna
  SOURCES idna.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME idna)
endif()

target_link_libraries(sourcemeta_core_idna PRIVATE sourcemeta::core::unicode)
