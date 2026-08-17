# The script runner has no public headers, so it is exercised the same way the
# framework itself is: through its command line, against a golden diagnostic.
# Each case runs from the fixture directory so that the path a diagnostic names
# is the bare script name rather than wherever the checkout happens to live
add_executable(sourcemeta_core_clitest_stub stub.cc)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::options)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::gzip)
target_link_libraries(sourcemeta_core_clitest_stub
  PRIVATE sourcemeta::core::io)

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
  add_test(NAME core.clitest.run_${name} COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
    "$<TARGET_FILE:sourcemeta_core_clitest>" ${code}
    "${CMAKE_CURRENT_SOURCE_DIR}/run_${name}.txt"
    "run_${name}.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
  set_tests_properties(core.clitest.run_${name} PROPERTIES
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


add_clitest_run_test(arguments_are_expanded 0)
add_clitest_run_test(checksum 0)
add_clitest_run_test(compress_and_remove 0)
add_clitest_run_test(copy_and_tree 0)
add_clitest_run_test(drop_lines_containing 0)
add_clitest_run_test(empty_stream_is_absent 0)
add_clitest_run_test(exports_variables 0)
add_clitest_run_test(extract_and_sort 0)
add_clitest_run_test(observation_format 0)
add_clitest_run_test(replace_and_drop 0)
add_clitest_run_test(sandbox_variables 0)
add_clitest_run_test(standard_input 0)
add_clitest_run_test(stat_mtime 0)
add_clitest_run_test(working_directory 0)
add_clitest_run_test(write_creates_parents 0)
add_clitest_run_test(compare_failure_shows_a_diff 1)
add_clitest_run_test(exit_code_mismatch 1)

add_clitest_run_test(carriage_returns_are_normalised 0)
add_clitest_run_test(compress_roundtrip 0)
add_clitest_run_test(copy_overwrites 0)
add_clitest_run_test(invalid_utf8_is_replaced 0)
add_clitest_run_test(one_trailing_terminator_is_dropped 0)
add_clitest_run_test(output_without_a_terminator 0)
add_clitest_run_test(remove_a_directory 0)
add_clitest_run_test(stream_of_one_terminator_is_one_empty_line 0)
add_clitest_run_test(surrogate_utf8_is_replaced 0)
add_clitest_run_test(truncated_utf8_is_one_replacement 0)
add_clitest_run_test(well_formed_utf8_survives 0)
add_test(NAME core.clitest.error_bad_pattern COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_bad_pattern.txt"
  "error_bad_pattern.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_bad_pattern PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_compare_missing_file COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_compare_missing_file.txt"
  "error_compare_missing_file.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_compare_missing_file PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_not_an_exit_code COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_not_an_exit_code.txt"
  "error_not_an_exit_code.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_not_an_exit_code PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_reserved_checksum_name COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_reserved_checksum_name.txt"
  "error_reserved_checksum_name.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_reserved_checksum_name PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_reserved_environment_name COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_reserved_environment_name.txt"
  "error_reserved_environment_name.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_reserved_environment_name PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_run_bad_trailer COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_run_bad_trailer.txt"
  "error_run_bad_trailer.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_run_bad_trailer PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_run_too_few_operands COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_run_too_few_operands.txt"
  "error_run_too_few_operands.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_run_too_few_operands PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_sort_usage COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_sort_usage.txt"
  "error_sort_usage.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_sort_usage PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_undefined_variable COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_undefined_variable.txt"
  "error_undefined_variable.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_undefined_variable PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME core.clitest.error_unknown_command COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_clitest>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/error_unknown_command.txt"
  "error_unknown_command.clitest" --binary "$<TARGET_FILE:sourcemeta_core_clitest_stub>")
set_tests_properties(core.clitest.error_unknown_command PROPERTIES
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_clitest_run_test(braces_are_literal 0)
add_clitest_run_test(carriage_return_script 0)
add_clitest_run_test(expecting_a_variable 0)
add_clitest_run_test(cores 0)

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
