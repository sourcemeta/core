function(sourcemeta_target_coverage)
  cmake_parse_arguments(SOURCEMETA_TARGET_COVERAGE "" "OUTPUT" "EXCLUDE" ${ARGN})

  if(NOT SOURCEMETA_TARGET_COVERAGE_OUTPUT)
    message(FATAL_ERROR "You must pass an output directory using the OUTPUT option")
  endif()

  get_property(COVERAGE_TEST_TARGETS GLOBAL PROPERTY SOURCEMETA_TEST_TARGETS)
  if(NOT COVERAGE_TEST_TARGETS)
    message(FATAL_ERROR "You must register tests before declaring the coverage target")
  endif()

  # On Apple platforms, the LLVM tools that understand the profile format
  # emitted by the system compiler are only reachable through `xcrun`
  if(APPLE)
    execute_process(COMMAND xcrun --find llvm-profdata
      OUTPUT_VARIABLE COVERAGE_LLVM_PROFDATA
      OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND xcrun --find llvm-cov
      OUTPUT_VARIABLE COVERAGE_LLVM_COV
      OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
  else()
    find_program(COVERAGE_LLVM_PROFDATA llvm-profdata REQUIRED)
    find_program(COVERAGE_LLVM_COV llvm-cov REQUIRED)
  endif()

  set(COVERAGE_OBJECTS)
  foreach(test_target IN LISTS COVERAGE_TEST_TARGETS)
    list(APPEND COVERAGE_OBJECTS "$<TARGET_FILE:${test_target}>")
  endforeach()
  # Joined with a custom separator to survive being passed as a single
  # command line argument to the report script
  list(JOIN COVERAGE_OBJECTS "|" COVERAGE_OBJECTS)

  set(COVERAGE_EXCLUDE "${PROJECT_BINARY_DIR}")
  list(APPEND COVERAGE_EXCLUDE ${SOURCEMETA_TARGET_COVERAGE_EXCLUDE})
  list(JOIN COVERAGE_EXCLUDE "|" COVERAGE_EXCLUDE)

  set(COVERAGE_PROFILE_DIRECTORY "${SOURCEMETA_TARGET_COVERAGE_OUTPUT}/profile")
  add_custom_target(coverage VERBATIM USES_TERMINAL
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E rm -R -f "${COVERAGE_PROFILE_DIRECTORY}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${COVERAGE_PROFILE_DIRECTORY}"
    COMMAND "${CMAKE_COMMAND}" -E env
      "LLVM_PROFILE_FILE=${COVERAGE_PROFILE_DIRECTORY}/%p.profraw"
      "${CMAKE_CTEST_COMMAND}" --test-dir "${PROJECT_BINARY_DIR}"
      --build-config $<CONFIG> --output-on-failure --parallel
    COMMAND "${CMAKE_COMMAND}"
      "-DCOVERAGE_LLVM_PROFDATA=${COVERAGE_LLVM_PROFDATA}"
      "-DCOVERAGE_LLVM_COV=${COVERAGE_LLVM_COV}"
      "-DCOVERAGE_PROFILE_DIRECTORY=${COVERAGE_PROFILE_DIRECTORY}"
      "-DCOVERAGE_OUTPUT_DIRECTORY=${SOURCEMETA_TARGET_COVERAGE_OUTPUT}"
      "-DCOVERAGE_OBJECTS=${COVERAGE_OBJECTS}"
      "-DCOVERAGE_EXCLUDE=${COVERAGE_EXCLUDE}"
      -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/coverage-report.cmake")
  add_dependencies(coverage ${COVERAGE_TEST_TARGETS})
endfunction()
