sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME argv
  SOURCES argv_program_path_test.cc)

target_link_libraries(sourcemeta_core_argv_unit
  PRIVATE sourcemeta::core::argv)

# On MSYS, the path we test against should not contain `.exe`
if(CMAKE_SYSTEM_NAME STREQUAL "MSYS")
  get_target_property(sourcemeta_core_argv_unit_path sourcemeta_core_argv_unit LOCATION)
  string(REPLACE ".exe" "" sourcemeta_core_argv_unit_path "${sourcemeta_core_argv_unit_path}")
  target_compile_definitions(sourcemeta_core_argv_unit
    PRIVATE EXECUTABLE_PATH="${sourcemeta_core_argv_unit_path}")
else()
  target_compile_definitions(sourcemeta_core_argv_unit
    PRIVATE EXECUTABLE_PATH="$<TARGET_FILE:sourcemeta_core_argv_unit>")
endif()
