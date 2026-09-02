sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME parallel
  PRIVATE_HEADERS for_each.h)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME parallel)
endif()

target_link_libraries(sourcemeta_core_parallel INTERFACE Threads::Threads)
if(CMAKE_USE_PTHREADS_INIT)
  sourcemeta_pkgconfig_declare(TARGET Threads::Threads LIBS "-pthread")
else()
  sourcemeta_pkgconfig_declare(TARGET Threads::Threads LIBS)
endif()
