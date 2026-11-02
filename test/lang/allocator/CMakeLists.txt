sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME allocator
  SOURCES allocator_test.cc)

# Named the way every executable in this build names it, as a program that
# linked the same library both ways would be asking for its archive to go in
# whole and to go in a member at a time at once
target_link_libraries(sourcemeta_core_allocator_unit
  PRIVATE ${SOURCEMETA_CORE_ALLOCATOR_TARGET})

# Which allocator a build ends up with is not derivable from the headers, so
# the expectation comes from the same condition that selects it
if(Mimalloc_FOUND)
  target_compile_definitions(sourcemeta_core_allocator_unit
    PRIVATE SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC)
endif()
