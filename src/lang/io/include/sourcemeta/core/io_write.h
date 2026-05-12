#ifndef SOURCEMETA_CORE_IO_WRITE_H_
#define SOURCEMETA_CORE_IO_WRITE_H_

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

/// @ingroup io
///
/// RAII helper used by `atomic_write_file`. Stages writes to a sibling
/// `<path>.tmp` file, fsyncs and renames on `commit`, deletes the
/// staging file in the destructor if `commit` was not called. Not part
/// of the public API.
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
/// Atomically write `contents` to `path`. Stages at a sibling `<path>.tmp`,
/// fsyncs to disk (via `sourcemeta::core::flush`), and renames over `path`.
/// Creates parent directories if missing. The staging file is removed if
/// the operation fails before the rename. For example:
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
/// Atomically write a byte span to `path`. See the string-view overload
/// for semantics. For example:
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
/// Callback variant of `atomic_write_file`. Opens a staging `std::ostream`,
/// passes it to `writer`, renames on clean return, deletes the staging file
/// if `writer` throws. The staging stream is opened in binary mode so
/// callers get exact byte semantics on every platform. For example:
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

} // namespace sourcemeta::core

#endif
