sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME numeric
  PRIVATE_HEADERS decimal.h SOURCES decimal.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME numeric)
endif()

target_link_libraries(sourcemeta_core_numeric PUBLIC mpdecimal)
target_link_libraries(sourcemeta_core_numeric PUBLIC mpdecimalxx)
