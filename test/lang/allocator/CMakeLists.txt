sourcemeta_test(NAMESPACE sourcemeta PROJECT core NAME allocator
  SOURCES allocator_test.cc)

# Which allocator a build ends up with is not derivable from the headers, so
# the expectation comes from the same condition that selects it
if(Mimalloc_FOUND)
  target_compile_definitions(sourcemeta_core_allocator_unit
    PRIVATE SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC)

  # Asking the allocator whether a pointer is one of its own is the only way of
  # telling where an allocation landed, and the library that carries it keeps
  # that interface to itself, so borrow the headers without the objects
  target_include_directories(sourcemeta_core_allocator_unit
    PRIVATE $<TARGET_PROPERTY:Mimalloc::Mimalloc,INTERFACE_INCLUDE_DIRECTORIES>)
endif()
