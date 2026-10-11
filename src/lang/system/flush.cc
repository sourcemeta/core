#include <sourcemeta/core/system_error.h>
#include <sourcemeta/core/system_file.h>

#include <filesystem> // std::filesystem
#include <system_error> // std::error_code, std::system_category, std::generic_category

#if defined(_WIN32)
#include <windows.h> // HANDLE, CreateFileW, FlushFileBuffers, CloseHandle
#else
#include <cerrno>   // errno
#include <fcntl.h>  // open, O_RDWR, O_DIRECTORY, O_RDONLY
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
  auto descriptor = ::open(path.c_str(), O_RDWR);
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

  // After syncing a file, we should also sync the directory to ensure
  // durability
  auto directory_fd =
      ::open(path.parent_path().c_str(), O_DIRECTORY | O_RDONLY);
  if (directory_fd != -1) {
    if (::fsync(directory_fd) == -1) {
      const auto error_code = std::error_code{errno, std::generic_category()};
      ::close(directory_fd);
      throw std::filesystem::filesystem_error{
          "failed to flush the parent directory to disk", path, error_code};
    }

    ::close(directory_fd);
  }
#endif
}

} // namespace sourcemeta::core
