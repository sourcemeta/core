macro(add_stacktrace_test_unix scenario debug_flag)
  if(UNIX)
    set(target sourcemeta_core_stacktrace_unit_${scenario}_main_${debug_flag})
    add_executable(${target} stacktrace_${scenario}_main.cc)
    target_link_libraries(${target} PRIVATE sourcemeta::core::stacktrace)
    # These programs set their own diagnostics and their own visibility, so
    # they are not built through the usual helper, but they are still programs
    # of this project and run on the allocator that this project ships
    if(SOURCEMETA_CORE_ALLOCATOR_TARGET)
      target_link_libraries(${target} PRIVATE ${SOURCEMETA_CORE_ALLOCATOR_TARGET})
    endif()
    # We don't want consumers to be bothered with this
    if(PROJECT_IS_TOP_LEVEL)
      sourcemeta_clang_tidy_attempt_enable(TARGET ${target})
    endif()
    if("${debug_flag}" STREQUAL "none")
      target_compile_options(${target} PRIVATE -g0 -fvisibility=default)
    else()
      target_compile_options(${target} PRIVATE -${debug_flag} -fvisibility=default)
    endif()
    add_test(NAME core.stacktrace.${scenario}.${debug_flag}.e2e
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/stacktrace_${scenario}_test.sh"
        "$<TARGET_FILE:${target}>"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "$<TARGET_FILE:sourcemeta_core_stacktrace>")
  endif()
endmacro()

macro(add_stacktrace_test_windows scenario)
  if(WIN32)
    set(target sourcemeta_core_stacktrace_unit_${scenario}_main)
    add_executable(${target} stacktrace_${scenario}_main.cc)
    target_link_libraries(${target} PRIVATE sourcemeta::core::stacktrace)
    # These programs set their own diagnostics and their own visibility, so
    # they are not built through the usual helper, but they are still programs
    # of this project and run on the allocator that this project ships
    if(SOURCEMETA_CORE_ALLOCATOR_TARGET)
      target_link_libraries(${target} PRIVATE ${SOURCEMETA_CORE_ALLOCATOR_TARGET})
    endif()
    # We don't want consumers to be bothered with this
    if(PROJECT_IS_TOP_LEVEL)
      sourcemeta_clang_tidy_attempt_enable(TARGET ${target})
    endif()
    target_compile_options(${target} PRIVATE /Zi)
    target_link_options(${target} PRIVATE /DEBUG)
    add_test(NAME core.stacktrace.${scenario}.e2e
      COMMAND powershell -ExecutionPolicy Bypass -File
        "${CMAKE_CURRENT_SOURCE_DIR}/stacktrace_${scenario}_test.ps1"
        "$<TARGET_FILE:${target}>"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "$<TARGET_FILE:sourcemeta_core_stacktrace>")
  endif()
endmacro()

macro(add_stacktrace_test_windows_no_debug scenario)
  if(WIN32)
    set(target sourcemeta_core_stacktrace_unit_${scenario}_main_no_debug)
    add_executable(${target} stacktrace_${scenario}_main.cc)
    target_link_libraries(${target} PRIVATE sourcemeta::core::stacktrace)
    # These programs set their own diagnostics and their own visibility, so
    # they are not built through the usual helper, but they are still programs
    # of this project and run on the allocator that this project ships
    if(SOURCEMETA_CORE_ALLOCATOR_TARGET)
      target_link_libraries(${target} PRIVATE ${SOURCEMETA_CORE_ALLOCATOR_TARGET})
    endif()
    # We don't want consumers to be bothered with this
    if(PROJECT_IS_TOP_LEVEL)
      sourcemeta_clang_tidy_attempt_enable(TARGET ${target})
    endif()
    add_test(NAME core.stacktrace.${scenario}.no_debug.e2e
      COMMAND powershell -ExecutionPolicy Bypass -File
        "${CMAKE_CURRENT_SOURCE_DIR}/stacktrace_${scenario}_no_debug_test.ps1"
        "$<TARGET_FILE:${target}>"
        "${CMAKE_CURRENT_BINARY_DIR}")
  endif()
endmacro()

add_stacktrace_test_unix(segfault g)
add_stacktrace_test_unix(segfault g1)
add_stacktrace_test_unix(segfault none)
add_stacktrace_test_unix(abort g)
add_stacktrace_test_unix(abort g1)
add_stacktrace_test_unix(abort none)
add_stacktrace_test_unix(on_demand g)
add_stacktrace_test_unix(on_demand g1)
add_stacktrace_test_unix(on_demand none)

add_stacktrace_test_windows(segfault)
add_stacktrace_test_windows(on_demand)
add_stacktrace_test_windows_no_debug(segfault)
add_stacktrace_test_windows_no_debug(on_demand)
