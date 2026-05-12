sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME io
  PRIVATE_HEADERS binary.h error.h fileview.h stream.h temporary.h write.h
  SOURCES io.cc io_binary.cc io_fileview.cc io_temporary.cc io_write.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME io)
endif()
