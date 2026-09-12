sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME terminal
  SOURCES terminal_test.cc)

target_link_libraries(sourcemeta_core_terminal_unit
  PRIVATE sourcemeta::core::terminal
  PRIVATE sourcemeta::core::process)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME terminal
  VARIANT unit_child_main SOURCES terminal_child_main.cc)
target_link_libraries(sourcemeta_core_terminal_unit_child_main
  PRIVATE sourcemeta::core::terminal)

add_dependencies(sourcemeta_core_terminal_unit
  sourcemeta_core_terminal_unit_child_main)
target_compile_definitions(sourcemeta_core_terminal_unit
  PRIVATE HELPER_BINARY="$<TARGET_FILE:sourcemeta_core_terminal_unit_child_main>")

