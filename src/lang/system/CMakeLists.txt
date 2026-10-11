sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME system
  PRIVATE_HEADERS atomic.h error.h file.h fileview.h parallel.h process.h
    stacktrace.h temporary.h usage.h
  SOURCES spawn.cc usage.cc command_line.h stacktrace.cc stacktrace_posix.h
    stacktrace_windows.h atomic.cc fileview.cc temporary.cc flush.cc
    parallel.cc)

if(UNIX)
  target_link_libraries(sourcemeta_core_system PRIVATE ${CMAKE_DL_LIBS})
  target_link_options(sourcemeta_core_system INTERFACE
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,NetBSD>:-rdynamic>"
    "$<$<PLATFORM_ID:Darwin>:-Wl,-export_dynamic>")
elseif(WIN32)
  target_link_libraries(sourcemeta_core_system PRIVATE dbghelp)
endif()

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME system)
endif()

target_link_libraries(sourcemeta_core_system PRIVATE sourcemeta::core::text)
target_link_libraries(sourcemeta_core_system PRIVATE sourcemeta::core::numeric)

# The parallel primitives are header-only, so every consumer that instantiates
# them needs the threading library too
target_link_libraries(sourcemeta_core_system PUBLIC Threads::Threads)

if(WIN32)
  target_link_libraries(sourcemeta_core_system PRIVATE psapi)
endif()
