sourcemeta_googletest(NAMESPACE sourcemeta PROJECT core NAME gzip
  SOURCES gzip_test.cc gzip_tar_test.cc)

target_link_libraries(sourcemeta_core_gzip_unit
  PRIVATE sourcemeta::core::gzip)

sourcemeta_executable(NAMESPACE sourcemeta PROJECT core NAME gzip_tgz_maker
  SOURCES gzip_tgz_maker.cc)
target_link_libraries(sourcemeta_core_gzip_tgz_maker
  PRIVATE sourcemeta::core::gzip)

if(WIN32)
  set(GZIP_TGZ_TEST "${CMAKE_CURRENT_SOURCE_DIR}/gzip_tgz_test.ps1")
  set(GZIP_TGZ_TEST_COMMAND powershell -ExecutionPolicy Bypass -File "${GZIP_TGZ_TEST}")
else()
  set(GZIP_TGZ_TEST "${CMAKE_CURRENT_SOURCE_DIR}/gzip_tgz_test.sh")
  set(GZIP_TGZ_TEST_COMMAND "${GZIP_TGZ_TEST}")
endif()

add_test(NAME core.gzip.tar_e2e_1
  COMMAND ${GZIP_TGZ_TEST_COMMAND} "$<TARGET_FILE:sourcemeta_core_gzip_tgz_maker>"
    "${CMAKE_CURRENT_BINARY_DIR}/gzip_tar_e2e_1.tgz"
    file1.txt FooBar
    file2.md Test)
