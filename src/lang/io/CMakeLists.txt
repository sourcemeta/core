sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME io
  PRIVATE_HEADERS binary.h bytestream.h error.h
  SOURCES io.cc io_binary.cc io_bytestream.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME io)
endif()

# The public interface of this module exposes the operating system primitives it
# is built on, such as the memory-mapped file a binary reader reads from
target_link_libraries(sourcemeta_core_io PUBLIC sourcemeta::core::system)
