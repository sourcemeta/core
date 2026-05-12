#ifndef SOURCEMETA_CORE_IO_BINARY_H_
#define SOURCEMETA_CORE_IO_BINARY_H_

#ifndef SOURCEMETA_CORE_IO_EXPORT
#include <sourcemeta/core/io_export.h>
#endif

#include <sourcemeta/core/io_fileview.h>

#include <cstddef>     // std::byte, std::size_t
#include <ostream>     // std::ostream
#include <span>        // std::span
#include <type_traits> // std::is_trivially_copyable_v

namespace sourcemeta::core {

/// @ingroup io
///
/// Typed wrapper over an output stream. rituals that surround every raw binary
/// write.
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <fstream>
/// #include <cstdint>
///
/// std::ofstream raw{"/tmp/out.bin", std::ios::binary};
/// sourcemeta::core::BinaryWriter writer{raw};
/// writer.write(std::uint32_t{0x12345678});
/// ```
class SOURCEMETA_CORE_IO_EXPORT BinaryWriter {
public:
  BinaryWriter(std::ostream &stream) noexcept;

  // Prevent copying, as this class is tied to a stream resource
  BinaryWriter(const BinaryWriter &) = delete;
  BinaryWriter(BinaryWriter &&) = delete;
  auto operator=(const BinaryWriter &) -> BinaryWriter & = delete;
  auto operator=(BinaryWriter &&) -> BinaryWriter & = delete;

  /// Write a trivially-copyable value as its raw bytes.
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  auto write(const T &value) -> void {
    this->write_bytes(reinterpret_cast<const std::byte *>(&value), sizeof(T));
  }

  /// Write a span of trivially-copyable values as their raw bytes.
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  auto write(const std::span<const T> values) -> void {
    this->write_bytes(reinterpret_cast<const std::byte *>(values.data()),
                      values.size() * sizeof(T));
  }

  auto write_bytes(const std::byte *data, const std::size_t size) -> void;

private:
  std::ostream *stream_;
};

/// @ingroup io
///
/// Cursor-tracking reader over a `FileView`.
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <cstdint>
///
/// sourcemeta::core::FileView view{"/tmp/out.bin"};
/// sourcemeta::core::BinaryReader reader{view};
/// const auto value{reader.read<std::uint32_t>()};
/// ```
class SOURCEMETA_CORE_IO_EXPORT BinaryReader {
public:
  BinaryReader(const FileView &view) noexcept;

  // Prevent copying, as this class is tied to an input resource
  BinaryReader(const BinaryReader &) = delete;
  BinaryReader(BinaryReader &&) = delete;
  auto operator=(const BinaryReader &) -> BinaryReader & = delete;
  auto operator=(BinaryReader &&) -> BinaryReader & = delete;

  /// Read a trivially-copyable value from the current cursor position
  /// and advance the cursor by `sizeof(T)`.
  template <typename T>
    requires std::is_trivially_copyable_v<T>
  [[nodiscard]] auto read() -> T {
    T value;
    this->read_bytes(reinterpret_cast<std::byte *>(&value), sizeof(T));
    return value;
  }

  [[nodiscard]] auto offset() const noexcept -> std::size_t;

  /// Move the cursor to `position`.
  auto seek(const std::size_t position) -> void;

  auto read_bytes(std::byte *destination, const std::size_t size) -> void;

private:
  const FileView *view_;
  std::size_t offset_{0};
};

} // namespace sourcemeta::core

#endif
