sourcemeta_test_main(SOURCEMETA_CORE_TEST_DRIVER_MAIN)

add_executable(sourcemeta_core_test_success
  test_success.cc "${SOURCEMETA_CORE_TEST_DRIVER_MAIN}")
target_link_libraries(sourcemeta_core_test_success
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_failure
  test_failure.cc "${SOURCEMETA_CORE_TEST_DRIVER_MAIN}")
target_link_libraries(sourcemeta_core_test_failure
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_throw
  test_throw.cc "${SOURCEMETA_CORE_TEST_DRIVER_MAIN}")
target_link_libraries(sourcemeta_core_test_throw
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_filter
  test_filter.cc "${SOURCEMETA_CORE_TEST_DRIVER_MAIN}")
target_link_libraries(sourcemeta_core_test_filter
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_fixture
  test_fixture.cc "${SOURCEMETA_CORE_TEST_DRIVER_MAIN}")
target_link_libraries(sourcemeta_core_test_fixture
  PRIVATE sourcemeta::core::test)

# This suite registers its cases at runtime, so it provides its own entry point
# rather than the generated one
add_executable(sourcemeta_core_test_dynamic
  test_dynamic.cc)
target_link_libraries(sourcemeta_core_test_dynamic
  PRIVATE sourcemeta::core::test)

if(WIN32)
  set(SOURCEMETA_CORE_TEST_RUNNER powershell -ExecutionPolicy Bypass -File
    "${CMAKE_CURRENT_SOURCE_DIR}/test_runner.ps1")
else()
  set(SOURCEMETA_CORE_TEST_RUNNER "${CMAKE_CURRENT_SOURCE_DIR}/test_runner.sh")
endif()

add_test(NAME core.test.success COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_success>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_success.txt")

add_test(NAME core.test.failure COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_failure>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/test_failure.txt")

add_test(NAME core.test.throw COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_throw>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/test_throw.txt")

add_test(NAME core.test.help COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_success>" 1
  "${CMAKE_CURRENT_SOURCE_DIR}/test_help.txt" --help)

add_test(NAME core.test.filter_prefix COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_filter>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_filter_prefix.txt" --filter test_filter.alpha)

add_test(NAME core.test.filter_short COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_filter>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_filter_short.txt" -f test_filter.beta)

add_test(NAME core.test.filter_none COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_filter>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_filter_none.txt" --filter nonexistent)

add_test(NAME core.test.fixture COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_fixture>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_fixture.txt")

add_test(NAME core.test.dynamic COMMAND ${SOURCEMETA_CORE_TEST_RUNNER}
  "$<TARGET_FILE:sourcemeta_core_test_dynamic>" 0
  "${CMAKE_CURRENT_SOURCE_DIR}/test_dynamic.txt")
