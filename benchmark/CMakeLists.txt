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

if(SOURCEMETA_CORE_JSONPATH)
  list(APPEND BENCHMARK_SOURCES jsonpath.cc)
endif()

if(SOURCEMETA_CORE_URITEMPLATE)
  list(APPEND BENCHMARK_SOURCES uritemplate.cc)
endif()

if(SOURCEMETA_CORE_JSONL)
  list(APPEND BENCHMARK_SOURCES jsonl.cc)
endif()

if(SOURCEMETA_CORE_JSONLD)
  list(APPEND BENCHMARK_SOURCES jsonld.cc)
endif()

if(SOURCEMETA_CORE_HTML)
  list(APPEND BENCHMARK_SOURCES html.cc)
endif()

if(SOURCEMETA_CORE_MARKDOWN)
  list(APPEND BENCHMARK_SOURCES markdown.cc)
endif()

if(SOURCEMETA_CORE_GZIP)
  list(APPEND BENCHMARK_SOURCES gzip.cc)
endif()

if(SOURCEMETA_CORE_JOSE)
  list(APPEND BENCHMARK_SOURCES jose.cc)
endif()

if(SOURCEMETA_CORE_CRYPTO)
  list(APPEND BENCHMARK_SOURCES crypto.cc)
endif()

if(BENCHMARK_SOURCES)
  sourcemeta_benchmark(NAMESPACE sourcemeta PROJECT core
    SOURCES ${BENCHMARK_SOURCES})
  target_compile_definitions(sourcemeta_core_benchmarks
    PRIVATE CURRENT_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

  if(SOURCEMETA_CORE_REGEX)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::regex)
  endif()

  if(SOURCEMETA_CORE_JSON)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::json sourcemeta::core::io)
  endif()

  if(SOURCEMETA_CORE_JSONPOINTER)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jsonpointer)
  endif()

  if(SOURCEMETA_CORE_JSONSCHEMA)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jsonschema sourcemeta::core::json
              sourcemeta::core::jsonpointer)
  endif()

  if(SOURCEMETA_CORE_JSONPATH)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jsonpath sourcemeta::core::json
              sourcemeta::core::jsonpointer)
  endif()

  if(SOURCEMETA_CORE_URITEMPLATE)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::uritemplate)
  endif()

  if(SOURCEMETA_CORE_JSONL)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jsonl)
  endif()

  if(SOURCEMETA_CORE_JSONLD)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jsonld sourcemeta::core::json
              sourcemeta::core::jsonpointer)
  endif()

  if(SOURCEMETA_CORE_HTML)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::html)
  endif()

  if(SOURCEMETA_CORE_MARKDOWN)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::markdown)
  endif()

  if(SOURCEMETA_CORE_GZIP)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::gzip sourcemeta::core::io)
  endif()

  if(SOURCEMETA_CORE_JOSE)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::jose sourcemeta::core::crypto
              sourcemeta::core::json)
  endif()

  if(SOURCEMETA_CORE_CRYPTO)
    target_link_libraries(sourcemeta_core_benchmarks
      PRIVATE sourcemeta::core::crypto sourcemeta::core::io)
  endif()

  # A single run prints the human readable report and writes the JSON file, so
  # that recording results for later comparison costs nothing extra
  add_custom_target(benchmark
    COMMAND sourcemeta_core_benchmarks
      --output "${PROJECT_BINARY_DIR}/benchmark.json"
    DEPENDS sourcemeta_core_benchmarks
    COMMENT "Running benchmark...")
else()
  add_custom_target(benchmark VERBATIM
    COMMAND "${CMAKE_COMMAND}" -E echo "Nothing to benchmark"
    COMMAND "${CMAKE_COMMAND}" -E false)
endif()
