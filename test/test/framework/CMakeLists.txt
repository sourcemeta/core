add_executable(sourcemeta_core_test_success test_success.cc)
target_link_libraries(sourcemeta_core_test_success
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_failure test_failure.cc)
target_link_libraries(sourcemeta_core_test_failure
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_diff test_diff.cc)
target_link_libraries(sourcemeta_core_test_diff
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_throw test_throw.cc)
target_link_libraries(sourcemeta_core_test_throw
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_filter test_filter.cc)
target_link_libraries(sourcemeta_core_test_filter
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_fixture test_fixture.cc)
target_link_libraries(sourcemeta_core_test_fixture
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_dynamic test_dynamic.cc)
target_link_libraries(sourcemeta_core_test_dynamic
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

add_executable(sourcemeta_core_test_empty test_empty.cc)
target_link_libraries(sourcemeta_core_test_empty
  PRIVATE sourcemeta::core::test sourcemeta::core::test_main)

# The framework's own command line is exercised through the script runner that
# this module ships, so the language is proven against the framework hosting it
macro(add_framework_test name binary)
  add_test(NAME core.test.${name} COMMAND
    "$<TARGET_FILE:sourcemeta_core_clitest>"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_${name}.clitest"
    --binary "$<TARGET_FILE:${binary}>")
endmacro()

add_framework_test(success sourcemeta_core_test_success)
add_framework_test(failure sourcemeta_core_test_failure)
add_framework_test(diff sourcemeta_core_test_diff)
add_framework_test(throw sourcemeta_core_test_throw)
add_framework_test(help sourcemeta_core_test_success)
add_framework_test(filter_prefix sourcemeta_core_test_filter)
add_framework_test(filter_short sourcemeta_core_test_filter)
add_framework_test(filter_none sourcemeta_core_test_filter)
add_framework_test(fixture sourcemeta_core_test_fixture)
add_framework_test(dynamic sourcemeta_core_test_dynamic)
add_framework_test(empty sourcemeta_core_test_empty)
