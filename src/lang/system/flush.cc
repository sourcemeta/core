#include <sourcemeta/core/system_error.h>
#include <sourcemeta/core/system_file.h>

#include <filesystem> // std::filesystem
#include <system_error> // std::error_code, std::system_category, std::generic_category

#if defined(_WIN32)
#include <windows.h> // HANDLE, CreateFileW, FlushFileBuffers, CloseHandle
#else
#include <cerrno>   // errno, EINVAL, ENOTSUP, EOPNOTSUPP
#include <fcntl.h>  // open, O_RDWR, O_DIRECTORY, O_RDONLY, O_CLOEXEC
#include <unistd.h> // close, fsync
#endif

namespace sourcemeta::core {

auto flush(const std::filesystem::path &path) -> void {
#if defined(_WIN32)
  HANDLE hFile =
      CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (hFile == INVALID_HANDLE_VALUE) {
    const auto error_code = std::error_code{static_cast<int>(GetLastError()),
                                            std::system_category()};
    if (error_code == std::errc::no_such_file_or_directory) {
      throw FileNotFoundError{path};
    }
    if (error_code == std::errc::permission_denied) {
      throw FilePermissionError{path};
    }

    throw std::filesystem::filesystem_error{"failed to open file for flushing",
                                            path, error_code};
  }

  if (!FlushFileBuffers(hFile)) {
    const auto error_code = std::error_code{static_cast<int>(GetLastError()),
                                            std::system_category()};
    CloseHandle(hFile);
    throw std::filesystem::filesystem_error{"failed to flush the file to disk",
                                            path, error_code};
  }

  CloseHandle(hFile);

#else
  auto descriptor = ::open(path.c_str(), O_RDWR | O_CLOEXEC);
  if (descriptor == -1) {
    const auto error_code = std::error_code{errno, std::generic_category()};
    if (error_code == std::errc::no_such_file_or_directory) {
      throw FileNotFoundError{path};
    }
    if (error_code == std::errc::permission_denied) {
      throw FilePermissionError{path};
    }

    throw std::filesystem::filesystem_error{"failed to open file for flushing",
                                            path, error_code};
  }

  if (::fsync(descriptor) == -1) {
    const auto error_code = std::error_code{errno, std::generic_category()};
    ::close(descriptor);
    throw std::filesystem::filesystem_error{"failed to flush the file to disk",
                                            path, error_code};
  }

  ::close(descriptor);

  // Syncing the file alone leaves its directory entry in the cache, so a
  // crash can lose the name of a file whose contents are on the disk. A
  // relative path without a directory component names an entry of the working
  // directory rather than of no directory at all
  const auto parent{path.has_parent_path() ? path.parent_path()
                                           : std::filesystem::path{"."}};
  const auto directory{
      ::open(parent.c_str(), O_DIRECTORY | O_RDONLY | O_CLOEXEC)};
  if (directory == -1) {
    const auto error_code = std::error_code{errno, std::generic_category()};
    throw std::filesystem::filesystem_error{
        "failed to open the parent directory for flushing", path, error_code};
  }

  // A filesystem that holds no directory entries to flush reports the request
  // as invalid or unsupported, which leaves nothing for this call to do rather
  // than anything for it to report
  if (::fsync(directory) == -1 && errno != EINVAL && errno != ENOTSUP &&
      errno != EOPNOTSUPP) {
    const auto error_code = std::error_code{errno, std::generic_category()};
    ::close(directory);
    throw std::filesystem::filesystem_error{
        "failed to flush the parent directory to disk", path, error_code};
  }

  ::close(directory);
#endif
}

} // namespace sourcemeta::core
