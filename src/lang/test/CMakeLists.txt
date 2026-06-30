sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME test
  SOURCES test.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME test)
endif()

target_link_libraries(sourcemeta_core_test
  PRIVATE sourcemeta::core::options)
target_link_libraries(sourcemeta_core_test
  PRIVATE sourcemeta::core::stacktrace)

# An executable's entry point cannot be resolved from a shared library, so the
# runner's main() is carried as an interface source that compiles into each test
# executable with that executable's own flags
add_library(sourcemeta_core_test_main INTERFACE)
add_library(sourcemeta::core::test_main ALIAS sourcemeta_core_test_main)
target_sources(sourcemeta_core_test_main INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cc")
target_link_libraries(sourcemeta_core_test_main INTERFACE sourcemeta::core::test)
