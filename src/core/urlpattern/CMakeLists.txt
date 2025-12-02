sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME urlpattern
  PRIVATE_HEADERS error.h part.h component.h
  SOURCES urlpattern.cc urlpattern_part.cc urlpattern_component.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME urlpattern)
endif()

target_link_libraries(sourcemeta_core_urlpattern PUBLIC
  sourcemeta::core::json)
target_link_libraries(sourcemeta_core_urlpattern PUBLIC
  sourcemeta::core::regex)
target_link_libraries(sourcemeta_core_urlpattern PRIVATE
  sourcemeta::core::punycode)

# TODO: Find a way to get rid of this?
target_link_libraries(sourcemeta_core_urlpattern PRIVATE PCRE2::pcre2)
