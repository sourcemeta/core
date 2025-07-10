function(sourcemeta_find_clang_tidy)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_program(CLANG_TIDY_BIN NAMES clang-tidy REQUIRED)
  else()
    message(FATAL_ERROR "Incompatible compiler toolchain. Clang-tidy is only tested to be working with clang toolchain")
  endif()

endfunction()

function(sourcemeta_set_clang_tidy_config)
  cmake_parse_arguments(SOURCEMETA_SET_CLANG_TIDY_CONFIG "" "HEADER_FILTER" "" ${ARGN})
  if(NOT SOURCEMETA_SET_CLANG_TIDY_CONFIG_HEADER_FILTER)
    message(FATAL_ERROR "HEADER_FILTER is required for clang-tidy.")
  endif()

  sourcemeta_find_clang_tidy()
  set(SOURCEMETA_CXX_CLANG_TIDY "${CLANG_TIDY_BIN};--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy;-header-filter=${SOURCEMETA_SET_CLANG_TIDY_CONFIG_HEADER_FILTER}" PARENT_SCOPE)
endfunction()
