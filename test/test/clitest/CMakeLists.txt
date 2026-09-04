# A comparison implemented outside the program under test, so that a runner
# broken enough to report success is still caught here
if(WIN32)
  set(SOURCEMETA_CORE_TEST_RUNNER powershell -ExecutionPolicy Bypass -File
    "${CMAKE_CURRENT_SOURCE_DIR}/runner.ps1")
else()
  set(SOURCEMETA_CORE_TEST_RUNNER "${CMAKE_CURRENT_SOURCE_DIR}/runner.sh")
endif()

# The script runner has no public headers, so it is exercised the same way the
# framework itself is: through its command line, against a golden diagnostic.
# Each case runs from the fixture directory so that the path a diagnostic names
# is the bare script name rather than wherever the checkout happens to live
sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME clitest
  VARIANT stub SOURCES stub.cc)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::options)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::gzip)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::io)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::numeric)

macro(add_clitest_test name code)
  add_test(NAME core.clitest.${name} COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
    "$<TARGET_FILE:sourcemeta_core_clitest>" ${code}
    "${CMAKE_CURRENT_SOURCE_DIR}/${name}.txt"
    "${name}.clitest" --check)
  set_tests_properties(core.clitest.${name} PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endmacro()

# A case that runs its script rather than only checking it, against a program
# this project builds for the purpose so that no real tool has to be present
macro(add_clitest_run_test name code)
  add_test(NAME core.clitest.${name} COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
    "$<TARGET_FILE:sourcemeta_core_clitest>" ${code}
    "${CMAKE_CURRENT_SOURCE_DIR}/${name}.txt"
    "${name}.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
  set_tests_properties(core.clitest.${name} PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endmacro()

add_clitest_test(argument_aliasing_is_not_an_assertion 1)
add_clitest_test(accepts_chains 0)
add_clitest_test(accepts_comments_and_blanks 0)
add_clitest_test(accepts_comparison 0)
add_clitest_test(accepts_paths 0)
add_clitest_test(accepts_patterns 0)
add_clitest_test(accepts_quoting 0)
add_clitest_test(accepts_runtime_paths 0)
add_clitest_test(accepts_variables 0)
add_clitest_test(body_is_not_an_expansion 1)
add_clitest_test(class_pattern_is_not_an_expansion 1)
add_clitest_test(drop_pattern_holds_a_variable 1)
add_clitest_test(filter_is_not_a_read 1)
add_clitest_test(pattern_holds_a_variable 1)
add_clitest_test(pattern_is_not_an_expansion 1)
add_clitest_test(problems_in_line_order 1)
add_clitest_test(rebound_checksum 1)
add_clitest_test(repeated_observation 1)
add_clitest_test(trailing_backslash 1)
add_clitest_test(unasserted_observation 1)
add_clitest_test(unbalanced_quote 1)
add_clitest_test(unexpanded_checksum 1)
add_clitest_test(unread_compress 1)
add_clitest_test(unread_copy 1)
add_clitest_test(unread_extract 1)
add_clitest_test(unread_stat 1)
add_clitest_test(unread_tree 1)
add_clitest_test(unterminated_body 1)
add_clitest_test(write_without_until 1)


add_clitest_run_test(run_arguments_are_expanded 0)
add_clitest_run_test(run_checksum 0)
add_clitest_run_test(run_compress_and_remove 0)
add_clitest_run_test(run_copy_and_tree 0)
add_clitest_run_test(run_drop_lines_containing 0)
add_clitest_run_test(run_empty_stream_is_absent 0)
add_clitest_run_test(run_exports_variables 0)
add_clitest_run_test(run_extract_and_sort 0)
add_clitest_run_test(run_observation_format 0)
add_clitest_run_test(run_replace_and_drop 0)
add_clitest_run_test(run_sandbox_variables 0)
add_clitest_run_test(run_sort_keeps_an_open_file_open 0)
add_clitest_run_test(run_standard_input 0)
add_clitest_run_test(run_stat_mtime 0)
add_clitest_run_test(run_working_directory 0)
add_clitest_run_test(run_write_creates_parents 0)
add_clitest_run_test(run_compare_failure_shows_a_diff 1)
add_clitest_run_test(run_exit_code_mismatch 1)

add_clitest_run_test(run_carriage_returns_are_normalised 0)
add_clitest_run_test(run_compress_roundtrip 0)
add_clitest_run_test(run_copy_overwrites 0)
add_clitest_run_test(run_invalid_utf8_is_replaced 0)
add_clitest_run_test(run_one_trailing_terminator_is_dropped 0)
add_clitest_run_test(run_output_without_a_terminator 0)
add_clitest_run_test(run_remove_a_directory 0)
add_clitest_run_test(run_stream_of_one_terminator_is_one_empty_line 0)
add_clitest_run_test(run_surrogate_utf8_is_replaced 0)
add_clitest_run_test(run_truncated_utf8_is_one_replacement 0)
add_clitest_run_test(run_well_formed_utf8_survives 0)
add_clitest_run_test(error_bad_pattern 1)
add_clitest_run_test(error_compare_missing_file 1)
add_clitest_run_test(error_not_an_exit_code 1)
add_clitest_run_test(error_reserved_checksum_name 1)
add_clitest_run_test(error_reserved_environment_name 1)
add_clitest_run_test(error_run_bad_trailer 1)
add_clitest_run_test(error_run_too_few_operands 1)
add_clitest_run_test(error_sort_usage 1)
add_clitest_run_test(error_undefined_variable 1)
add_clitest_run_test(error_unknown_command 1)
add_clitest_run_test(run_braces_are_literal 0)
add_clitest_run_test(run_carriage_return_script 0)
add_clitest_run_test(run_expecting_a_variable 0)
add_clitest_run_test(run_cores 0)

# Bindings injected on the command line are ordinary ones, and reach the
# program that a script starts like any other
add_test(NAME core.clitest.injected_environment COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/injected_environment.txt"
  "injected_environment.clitest"
  --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>"
  --environment INJECTED=from-cmake)
set_tests_properties(core.clitest.injected_environment PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# The runner reports a script it cannot read against the file rather than
# against a line, since there is no line to blame yet
add_test(NAME core.clitest.missing_script COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/missing_script.txt"
  "nonexistent.clitest" --check)
set_tests_properties(core.clitest.missing_script PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Running a script needs something to run, and saying so is more use than
# failing later on a program that was never named
add_test(NAME core.clitest.missing_binary COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/missing_binary.txt"
  "accepts_comparison.clitest")
set_tests_properties(core.clitest.missing_binary PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_test(NAME core.clitest.no_script COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/no_script.txt")

# Several scripts at once are each judged and each reported, so that one failing
# early does not hide what the rest of them would have said
add_test(NAME core.clitest.many_scripts COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/many_scripts.txt"
  "accepts_comparison.clitest" "unread_tree.clitest" "accepts_paths.clitest"
  --check)
set_tests_properties(core.clitest.many_scripts PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_test(NAME core.clitest.many_scripts_all_valid COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/many_scripts_all_valid.txt"
  "accepts_comparison.clitest" "accepts_paths.clitest" --check)
set_tests_properties(core.clitest.many_scripts_all_valid PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# A binding given on the command line is known to the check, so a destination
# spelled through one is not left until the script runs
add_test(NAME core.clitest.injected_binding_is_known COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/injected_binding_is_known.txt"
  "injected_binding_is_known.clitest" --check --environment OUT=result.txt)
set_tests_properties(core.clitest.injected_binding_is_known PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# The language has no home outside this program, so asking for help prints it
add_test(NAME core.clitest.help COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/help.txt" --help)
set_tests_properties(core.clitest.help PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
