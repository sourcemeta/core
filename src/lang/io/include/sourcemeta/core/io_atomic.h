#ifndef SOURCEMETA_CORE_IO_ATOMIC_H_
#define SOURCEMETA_CORE_IO_ATOMIC_H_

#ifndef SOURCEMETA_CORE_IO_EXPORT
#include <sourcemeta/core/io_export.h>
#endif

#include <cstddef>     // std::byte
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream
#include <ostream>     // std::ostream
#include <span>        // std::span
#include <string_view> // std::string_view
#include <type_traits> // std::is_invocable_v
#include <utility>     // std::forward

namespace sourcemeta::core {

namespace detail {

class SOURCEMETA_CORE_IO_EXPORT AtomicFileWriter {
public:
  AtomicFileWriter(const std::filesystem::path &destination);
  ~AtomicFileWriter();

  AtomicFileWriter(const AtomicFileWriter &) = delete;
  AtomicFileWriter(AtomicFileWriter &&) = delete;
  auto operator=(const AtomicFileWriter &) -> AtomicFileWriter & = delete;
  auto operator=(AtomicFileWriter &&) -> AtomicFileWriter & = delete;

  [[nodiscard]] auto stream() -> std::ostream &;
  auto commit() -> void;

private:
// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif
  std::filesystem::path destination_;
  std::filesystem::path staging_;
  std::ofstream stream_;
#if defined(_MSC_VER)
#pragma warning(default : 4251)
#endif
  bool committed_{false};
};

} // namespace detail

/// @ingroup io
///
/// Atomically write `contents` to `path`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
///
/// sourcemeta::core::atomic_write_file("/tmp/foo.json", "{\"a\":1}");
/// ```
SOURCEMETA_CORE_IO_EXPORT
auto atomic_write_file(const std::filesystem::path &path,
                       const std::string_view contents) -> void;

/// @ingroup io
///
/// Atomically write a byte span to `path`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <array>
///
/// constexpr std::array<std::byte, 3> bytes{
///     std::byte{0x41}, std::byte{0x42}, std::byte{0x43}};
/// sourcemeta::core::atomic_write_file("/tmp/foo.bin", std::span{bytes});
/// ```
SOURCEMETA_CORE_IO_EXPORT
auto atomic_write_file(const std::filesystem::path &path,
                       const std::span<const std::byte> contents) -> void;

/// @ingroup io
///
/// Callback variant of `atomic_write_file`. For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <sourcemeta/core/json.h>
///
/// sourcemeta::core::atomic_write_file("/tmp/foo.json",
///     [&](std::ostream &stream) {
///       sourcemeta::core::prettify(document, stream);
///       stream << "\n";
///     });
/// ```
template <typename Writer>
  requires std::is_invocable_v<Writer, std::ostream &>
auto atomic_write_file(const std::filesystem::path &path, Writer &&writer)
    -> void {
  detail::AtomicFileWriter file{path};
  std::forward<Writer>(writer)(file.stream());
  file.commit();
}

/// @ingroup io
///
/// Atomically swap two directories. Both directories must reside on the same
/// filesystem and the original path must not be a bare filename (it must have
/// a parent component). After the call, the original path holds the contents
/// of the replacement and the replacement path holds the former contents of
/// the original. If the original does not exist, the replacement is simply
/// renamed into place and the replacement path will no longer exist.
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
///
/// sourcemeta::core::atomic_directory_swap("/output", "/staging");
/// ```
SOURCEMETA_CORE_IO_EXPORT
auto atomic_directory_swap(const std::filesystem::path &original,
                           const std::filesystem::path &replacement) -> void;

} // namespace sourcemeta::core

#endif
