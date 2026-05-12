#ifndef SOURCEMETA_CORE_IO_STREAM_H_
#define SOURCEMETA_CORE_IO_STREAM_H_

#ifndef SOURCEMETA_CORE_IO_EXPORT
#include <sourcemeta/core/io_export.h>
#endif

#include <sourcemeta/core/io_error.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem
#include <fstream>    // std::basic_ifstream
#include <iostream>   // std::cin
#include <istream>    // std::basic_istream
#include <sstream>    // std::basic_ostringstream
#include <string>     // std::basic_string, std::char_traits, std::string

namespace sourcemeta::core {

// Forward declaration of the FIFO-aware canonicalisation utility.
// The definition lives in `io.h` and is the same function that
// `sourcemeta::core::read_file` uses internally.
SOURCEMETA_CORE_IO_EXPORT
auto canonical(const std::filesystem::path &path) -> std::filesystem::path;

/// @ingroup io
///
/// Drain an input stream into a string. The stream is left positioned at
/// the end of what it consumed. For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::istringstream stream{"hello"};
/// const auto contents{sourcemeta::core::read_to_string(stream)};
/// assert(contents == "hello");
/// ```
template <typename CharT = char, typename Traits = std::char_traits<CharT>>
auto read_to_string(std::basic_istream<CharT, Traits> &stream)
    -> std::basic_string<CharT, Traits> {
  std::basic_ostringstream<CharT, Traits> buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

/// @ingroup io
///
/// Read an entire file into a string in a single call. Internally uses
/// `sourcemeta::core::canonical`, so it inherits the FIFO-on-Linux fix.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
/// #include <cassert>
///
/// const auto contents{sourcemeta::core::read_file_to_string("/tmp/foo.json")};
/// assert(!contents.empty());
/// ```
template <typename CharT = char, typename Traits = std::char_traits<CharT>>
auto read_file_to_string(const std::filesystem::path &path)
    -> std::basic_string<CharT, Traits> {
  if (std::filesystem::is_directory(path)) {
    throw IOIsADirectoryError{path};
  }

  std::basic_ifstream<CharT, Traits> stream{sourcemeta::core::canonical(path)};
  stream.exceptions(std::basic_ifstream<CharT, Traits>::badbit);
  assert(!stream.fail());
  assert(stream.is_open());
  return read_to_string(stream);
}

/// @ingroup io
///
/// Drain `std::cin` fully into a string. A thin convenience over
/// `read_to_string(std::cin)` that reads better at call sites. For example:
///
/// ```cpp
/// #include <sourcemeta/core/io.h>
///
/// const auto input{sourcemeta::core::read_stdin()};
/// ```
inline auto read_stdin() -> std::string { return read_to_string(std::cin); }

} // namespace sourcemeta::core

#endif
