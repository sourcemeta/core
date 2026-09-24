if(NOT SLJIT_FOUND)
  # This code generator ships inside the regular expression library that
  # brought it in, but it stands on its own and other libraries may take it
  set(SLJIT_DIR "${PROJECT_SOURCE_DIR}/vendor/pcre2/deps/sljit")
  set(SLJIT_SOURCE_DIR "${SLJIT_DIR}/sljit_src")

  # This library ships one source that includes every backend and allocator
  # that the target architecture and platform select
  # Merged into the library that uses it, so that no archive, no header and
  # no CMake package of our own build of it reaches an installed consumer
  add_library(sljit OBJECT "${SLJIT_SOURCE_DIR}/sljitLir.c")
  sourcemeta_add_default_options(PRIVATE sljit)

  # The compiler structure that this library hands out grows extra members
  # under any of the tracing, argument checking, or debugging options, so
  # every translation unit that reaches for the header has to be told the
  # same configuration that the library itself was built with
  target_compile_definitions(sljit PUBLIC SLJIT_CONFIG_AUTO=1)
  target_compile_definitions(sljit PUBLIC SLJIT_VERBOSE=0)
  target_compile_definitions(sljit PUBLIC SLJIT_DEBUG=0)

  target_include_directories(sljit PUBLIC
    "$<BUILD_INTERFACE:${SLJIT_SOURCE_DIR}>")

  add_library(SLJIT::sljit ALIAS sljit)

  set(SLJIT_FOUND ON)
endif()
