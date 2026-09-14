sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME terminal
  SOURCES terminal_test.cc)

target_link_libraries(sourcemeta_core_terminal_unit
  PRIVATE sourcemeta::core::terminal)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME terminal
  VARIANT test_helper SOURCES terminal_child_main.cc)
target_link_libraries(sourcemeta_core_terminal_test_helper
  PRIVATE sourcemeta::core::terminal
  PRIVATE sourcemeta::core::options)

macro(add_terminal_test name)
  add_test(NAME core.terminal.${name} COMMAND
    "$<TARGET_FILE:sourcemeta_core_clitest>"
    "${CMAKE_CURRENT_SOURCE_DIR}/${name}.clitest"
    --binary "$<TARGET_FILE:sourcemeta_core_terminal_test_helper>")
endmacro()

add_terminal_test(color_auto)
add_terminal_test(color_always)
add_terminal_test(color_never)
