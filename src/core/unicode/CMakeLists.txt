find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(SOURCEMETA_CORE_UNICODE_UCD_DIR
  "${core_SOURCE_DIR}/vendor/unicodetools/unicodetools/data/ucd/dev")

set(SOURCEMETA_CORE_UNICODE_COMBINING_CLASS_HEADER
  "${CMAKE_CURRENT_BINARY_DIR}/combining_class.h")

add_custom_command(
  OUTPUT "${SOURCEMETA_CORE_UNICODE_COMBINING_CLASS_HEADER}"
  COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/codegen.py"
    "${SOURCEMETA_CORE_UNICODE_UCD_DIR}/extracted/DerivedCombiningClass.txt"
    "${SOURCEMETA_CORE_UNICODE_COMBINING_CLASS_HEADER}"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/codegen.py"
    "${SOURCEMETA_CORE_UNICODE_UCD_DIR}/extracted/DerivedCombiningClass.txt"
  COMMENT "Generating Unicode canonical combining class table"
  VERBATIM)

sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME unicode
  SOURCES unicode.cc "${SOURCEMETA_CORE_UNICODE_COMBINING_CLASS_HEADER}")

target_include_directories(sourcemeta_core_unicode PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}")

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME unicode)
endif()
