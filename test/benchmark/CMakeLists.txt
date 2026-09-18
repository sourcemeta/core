sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME benchmark
  VARIANT success SOURCES benchmark_success.cc)
target_link_libraries(sourcemeta_core_benchmark_success
  PRIVATE sourcemeta::core::benchmark sourcemeta::core::benchmark_main)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME benchmark
  VARIANT empty SOURCES benchmark_empty.cc)
target_link_libraries(sourcemeta_core_benchmark_empty
  PRIVATE sourcemeta::core::benchmark sourcemeta::core::benchmark_main)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME benchmark
  VARIANT stray SOURCES benchmark_stray.cc)
target_link_libraries(sourcemeta_core_benchmark_stray
  PRIVATE sourcemeta::core::benchmark sourcemeta::core::benchmark_main)

# The framework's own command line is exercised through the script runner that
# the test module ships, so the behaviour is proven the way a user meets it
macro(add_benchmark_test name binary)
  add_test(NAME core.benchmark.${name} COMMAND
    "$<TARGET_FILE:sourcemeta_core_clitest>"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmark_${name}.clitest"
    --binary "$<TARGET_FILE:${binary}>")
endmacro()

add_benchmark_test(report sourcemeta_core_benchmark_success)
add_benchmark_test(help sourcemeta_core_benchmark_success)
add_benchmark_test(filter sourcemeta_core_benchmark_success)
add_benchmark_test(filter_none sourcemeta_core_benchmark_success)
add_benchmark_test(output sourcemeta_core_benchmark_success)
add_benchmark_test(empty sourcemeta_core_benchmark_empty)
add_benchmark_test(stray sourcemeta_core_benchmark_stray)
