function(sourcemeta_enable_clang_tidy)
  find_program(CLANG_TIDY_BIN NAMES clang-tidy REQUIRED)
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_BIN};--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy;-header-filter=${CMAKE_CURRENT_SOURCE_DIR}/src/*" PARENT_SCOPE)
endfunction()

function(sourcemeta_disable_clang_tidy)
  unset(CMAKE_CXX_CLANG_TIDY PARENT_SCOPE)
endfunction()
