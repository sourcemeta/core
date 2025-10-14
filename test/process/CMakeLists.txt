sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME process
  SOURCES process_spawn_test.cc)

target_link_libraries(sourcemeta_core_process_unit PRIVATE sourcemeta::core::process)

add_executable(sourcemeta_core_process_unit_spawn_main process_spawn_main.cc)
target_link_libraries(sourcemeta_core_process_unit_spawn_main
  PRIVATE sourcemeta::core::process)

if(WIN32 AND NOT MSYS AND NOT CYGWIN AND NOT MINGW)
  # Windows native (MSVC) - use PowerShell script
  add_test(NAME core.process.spawn.e2e
    COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/process_spawn_test.ps1"
      "$<TARGET_FILE:sourcemeta_core_process_unit_spawn_main>")
else()
  # POSIX systems (Linux, macOS, MSYS2, etc.) - use shell script
  add_test(NAME core.process.spawn.e2e
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/process_spawn_test.sh"
      "$<TARGET_FILE:sourcemeta_core_process_unit_spawn_main>")
endif()
