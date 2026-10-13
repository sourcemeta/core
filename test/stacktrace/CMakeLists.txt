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
    # These programs are the ones that die on a fatal signal, so they carry the
    # counters that stay up to date as they change, as the library they drive
    # does under the same condition
    if(CMAKE_CXX_FLAGS MATCHES "-fprofile-instr-generate")
      target_compile_options(${target}
        PRIVATE "SHELL:-mllvm -runtime-counter-relocation")
    endif()
    add_test(NAME core.stacktrace.${scenario}.${debug_flag}.e2e
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/stacktrace_${scenario}_test.sh"
        "$<TARGET_FILE:${target}>"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "$<TARGET_FILE:sourcemeta_core_stacktrace>")
  endif()
endmacro()

# These variants assert symbolized frames, which dbghelp can only resolve from
# the PDB that MSVC emits
macro(add_stacktrace_test_windows scenario)
  if(MSVC)
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

# The handler installs itself for five signals, of which the scenarios above
# drive two by faulting for real. The rest are raised by name instead, as what
# makes a system deliver them is particular to the architecture, while what is
# read back is the name the handler maps the signal to
macro(add_stacktrace_signal_test_unix signal_name debug_flag)
  if(UNIX)
    set(target sourcemeta_core_stacktrace_unit_signal_${signal_name}_main_${debug_flag})
    add_executable(${target} stacktrace_signal_main.cc)
    target_link_libraries(${target} PRIVATE sourcemeta::core::stacktrace)
    target_compile_definitions(${target}
      PRIVATE STACKTRACE_TEST_SIGNAL=${signal_name})
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
    # These programs are the ones that die on a fatal signal, so they carry the
    # counters that stay up to date as they change, as the library they drive
    # does under the same condition
    if(CMAKE_CXX_FLAGS MATCHES "-fprofile-instr-generate")
      target_compile_options(${target}
        PRIVATE "SHELL:-mllvm -runtime-counter-relocation")
    endif()
    add_test(NAME core.stacktrace.signal.${signal_name}.${debug_flag}.e2e
      COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/stacktrace_signal_test.sh"
        "$<TARGET_FILE:${target}>"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "$<TARGET_FILE:sourcemeta_core_stacktrace>"
        "${signal_name}")
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

add_stacktrace_signal_test_unix(SIGFPE g)
add_stacktrace_signal_test_unix(SIGBUS g)
add_stacktrace_signal_test_unix(SIGILL g)

add_stacktrace_test_windows(segfault)
add_stacktrace_test_windows(on_demand)
add_stacktrace_test_windows_no_debug(segfault)
add_stacktrace_test_windows_no_debug(on_demand)
