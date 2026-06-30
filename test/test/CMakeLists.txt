add_executable(sourcemeta_core_test_success test_success.cc)
target_link_libraries(sourcemeta_core_test_success
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_failure test_failure.cc)
target_link_libraries(sourcemeta_core_test_failure
  PRIVATE sourcemeta::core::test)

add_executable(sourcemeta_core_test_throw test_throw.cc)
target_link_libraries(sourcemeta_core_test_throw
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
