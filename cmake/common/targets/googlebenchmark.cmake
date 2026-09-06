function(sourcemeta_googlebenchmark)
  cmake_parse_arguments(SOURCEMETA_GOOGLEBENCHMARK ""
    "NAMESPACE;PROJECT" "SOURCES" ${ARGN})

  sourcemeta_executable(
    NAMESPACE "${SOURCEMETA_GOOGLEBENCHMARK_NAMESPACE}"
    PROJECT "${SOURCEMETA_GOOGLEBENCHMARK_PROJECT}"
    NAME benchmark
    SOURCES "${SOURCEMETA_GOOGLEBENCHMARK_SOURCES}"
    # The names of these functions are the labels GoogleBenchmark reports, which
    # tools that track results over time key their history on, and the loop
    # variable that the framework's iteration idiom asks for carries no meaning
    CLANG_TIDY_DISABLE readability-identifier-naming readability-identifier-length
    OUTPUT TARGET_NAME)

  target_link_libraries("${TARGET_NAME}" PRIVATE benchmark::benchmark)
  target_link_libraries("${TARGET_NAME}" PRIVATE benchmark::benchmark_main)
endfunction()
