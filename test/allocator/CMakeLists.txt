sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME allocator
  SOURCES allocator_name_test.cc)

target_link_libraries(sourcemeta_core_allocator_unit
  PRIVATE sourcemeta::core::allocator)

# Which allocator a build ends up with is not derivable from the headers, so
# the expectation comes from the same condition that selects it
if(Mimalloc_FOUND)
  target_compile_definitions(sourcemeta_core_allocator_unit
    PRIVATE SOURCEMETA_CORE_TEST_ALLOCATOR="mimalloc")
else()
  target_compile_definitions(sourcemeta_core_allocator_unit
    PRIVATE SOURCEMETA_CORE_TEST_ALLOCATOR="system")
endif()
