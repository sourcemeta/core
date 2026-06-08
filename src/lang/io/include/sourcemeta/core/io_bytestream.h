#ifndef SOURCEMETA_CORE_IO_BYTESTREAM_H_
#define SOURCEMETA_CORE_IO_BYTESTREAM_H_

#ifndef SOURCEMETA_CORE_IO_EXPORT
#include <sourcemeta/core/io_export.h>
#endif

#include <cstddef>          // std::byte
#include <cstdint>          // std::uint8_t
#include <initializer_list> // std::initializer_list
#include <sstream>          // std::istringstream, std::ostringstream
#include <vector>           // std::vector

namespace sourcemeta::core {

/// @ingroup io
/// An input stream constructed from an inline list of byte values. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
///
/// sourcemeta::core::InputByteStream stream{0x1f, 0x8b, 0x08, 0x00};
/// ```
class SOURCEMETA_CORE_IO_EXPORT InputByteStream : public std::istringstream {
public:
  InputByteStream(std::initializer_list<std::uint8_t> bytes);
};

/// @ingroup io
/// An output stream that exposes its accumulated bytes as a byte vector. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <cassert>
///
/// sourcemeta::core::OutputByteStream stream;
/// stream.put('A');
/// assert(stream.bytes().size() == 1);
/// ```
class SOURCEMETA_CORE_IO_EXPORT OutputByteStream : public std::ostringstream {
public:
  [[nodiscard]] auto bytes() const -> std::vector<std::byte>;
};

} // namespace sourcemeta::core

#endif
