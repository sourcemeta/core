set(BENCHMARK_SOURCES)

if(SOURCEMETA_CORE_REGEX)
  list(APPEND BENCHMARK_SOURCES regex.cc)
endif()

if(SOURCEMETA_CORE_JSON)
  list(APPEND BENCHMARK_SOURCES json.cc)
endif()

if(SOURCEMETA_CORE_JSONPOINTER)
  list(APPEND BENCHMARK_SOURCES jsonpointer.cc)
endif()

if(SOURCEMETA_CORE_JSONSCHEMA)
  list(APPEND BENCHMARK_SOURCES jsonschema.cc)
endif()

if(SOURCEMETA_CORE_EXTENSION_ALTERSCHEMA)
  list(APPEND BENCHMARK_SOURCES alterschema.cc)
endif()

if(BENCHMARK_SOURCES)
  sourcemeta_googlebenchmark(NAMESPACE sourcemeta PROJECT core
    SOURCES ${BENCHMARK_SOURCES})
  target_compile_definitions(sourcemeta_core_benchmark
    PRIVATE CURRENT_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

  if(SOURCEMETA_CORE_REGEX)
    target_link_libraries(sourcemeta_core_benchmark
      PRIVATE sourcemeta::core::regex)
  endif()

  if(SOURCEMETA_CORE_JSON)
    target_link_libraries(sourcemeta_core_benchmark
      PRIVATE sourcemeta::core::json)
  endif()

  if(SOURCEMETA_CORE_JSONPOINTER)
    target_link_libraries(sourcemeta_core_benchmark
      PRIVATE sourcemeta::core::jsonpointer)
  endif()

  if(SOURCEMETA_CORE_JSONSCHEMA)
    target_link_libraries(sourcemeta_core_benchmark
      PRIVATE sourcemeta::core::jsonschema)
  endif()

  if(SOURCEMETA_CORE_EXTENSION_ALTERSCHEMA)
    target_link_libraries(sourcemeta_core_benchmark
      PRIVATE sourcemeta::core::alterschema)
  endif()

  add_custom_target(benchmark_all
    COMMAND sourcemeta_core_benchmark
    DEPENDS sourcemeta_core_benchmark
    COMMENT "Running benchmark...")
  add_custom_target(benchmark_json
    COMMAND sourcemeta_core_benchmark
      --benchmark_format=json
      --benchmark_out="${PROJECT_BINARY_DIR}/benchmark.json"
    DEPENDS sourcemeta_core_benchmark
    COMMENT "Running benchmark...")
else()
  add_custom_target(benchmark_all VERBATIM
    COMMAND "${CMAKE_COMMAND}" -E echo "Nothing to benchmark"
    COMMAND "${CMAKE_COMMAND}" -E false)
  add_custom_target(benchmark_json VERBATIM
    COMMAND "${CMAKE_COMMAND}" -E echo "Nothing to benchmark"
    COMMAND "${CMAKE_COMMAND}" -E false)
endif()
