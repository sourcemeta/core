if(WIN32)
  add_test(NAME core.process.spawn.e2e
    COMMAND powershell -ExecutionPolicy Bypass -File
      "${CMAKE_CURRENT_SOURCE_DIR}/process_spawn_test.ps1"
      "$<TARGET_FILE:sourcemeta_core_process_unit_spawn_main>")
  sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME process
    SOURCES process_error_test.cc process_spawn_test_windows.cc
      process_spawn_and_capture_test.cc process_spawn_input_test.cc
      process_usage_test.cc process_descriptors_test.cc
      process_start_time_test.cc)
else()
  add_test(NAME core.process.spawn.e2e
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/process_spawn_test.sh"
      "$<TARGET_FILE:sourcemeta_core_process_unit_spawn_main>")
  sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME process
    SOURCES process_error_test.cc process_spawn_test_unix.cc
      process_spawn_and_capture_test.cc process_spawn_input_test.cc
      process_usage_test.cc process_descriptors_test.cc
      process_start_time_test.cc)
endif()

target_link_libraries(sourcemeta_core_process_unit PRIVATE sourcemeta::core::process)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME process
  VARIANT unit_spawn_main SOURCES process_spawn_main.cc)
target_link_libraries(sourcemeta_core_process_unit_spawn_main
  PRIVATE sourcemeta::core::process)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME process
  VARIANT unit_capture_main SOURCES process_spawn_and_capture_main.cc)
target_link_libraries(sourcemeta_core_process_unit_capture_main
  PRIVATE sourcemeta::core::text)

add_dependencies(sourcemeta_core_process_unit
  sourcemeta_core_process_unit_capture_main)
target_compile_definitions(sourcemeta_core_process_unit
  PRIVATE HELPER_BINARY="$<TARGET_FILE:sourcemeta_core_process_unit_capture_main>")
