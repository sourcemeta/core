function(sourcemeta_find_clang_tidy)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_program(CLANG_TIDY_BIN NAMES clang-tidy REQUIRED)
    set(SOURCEMETA_CXX_CLANG_TIDY "${CLANG_TIDY_BIN};--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy;-header-filter=${CMAKE_CURRENT_SOURCE_DIR}/src/*" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Incompatible compiler toolchain. Clang-tidy is only tested to be working with clang toolchain")
  endif()

endfunction()
