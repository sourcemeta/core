#ifndef SOURCEMETA_CORE_SYSTEM_ERROR_H_
#define SOURCEMETA_CORE_SYSTEM_ERROR_H_

#ifndef SOURCEMETA_CORE_SYSTEM_EXPORT
#include <sourcemeta/core/system_export.h>
#endif

#include <exception>        // std::exception
#include <filesystem>       // std::filesystem::path
#include <initializer_list> // std::initializer_list
#include <span>             // std::span
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <utility>          // std::move
#include <vector>           // std::vector

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup system
/// An executable program could not be found
class SOURCEMETA_CORE_SYSTEM_EXPORT ProcessProgramNotFoundError
    : public std::exception {
public:
  /// Construct the error given the offending program.
  ProcessProgramNotFoundError(const std::string_view program)
      : program_{program} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Could not locate the requested program";
  }

  /// The offending program.
  [[nodiscard]] auto program() const noexcept -> std::string_view {
    return this->program_;
  }

private:
  std::string program_;
};

/// @ingroup system
/// A spawned process terminated abnormally
class SOURCEMETA_CORE_SYSTEM_EXPORT ProcessSpawnError : public std::exception {
public:
  /// Construct the error given the offending program and its arguments.
  ProcessSpawnError(const std::string_view program,
                    std::initializer_list<std::string_view> arguments)
      : program_{program}, arguments_{arguments.begin(), arguments.end()} {}

  /// Construct the error given the offending program and its arguments.
  ProcessSpawnError(const std::string_view program,
                    std::span<const std::string_view> arguments)
      : program_{program}, arguments_{arguments.begin(), arguments.end()} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Process terminated abnormally";
  }

  /// The offending program.
  [[nodiscard]] auto program() const noexcept -> std::string_view {
    return this->program_;
  }

  /// The arguments the program was spawned with.
  [[nodiscard]] auto arguments() const noexcept
      -> const std::vector<std::string> & {
    return this->arguments_;
  }

private:
  std::string program_;
  std::vector<std::string> arguments_;
};

/// @ingroup system
/// An error that represents a failure to memory-map a file
class SOURCEMETA_CORE_SYSTEM_EXPORT FileViewError : public std::exception {
public:
  /// Construct the error given the offending path and a message. The message is
  /// copied, so that it may name a buffer that does not outlive the call.
  FileViewError(std::filesystem::path path, std::string message)
      : path_{std::move(path)}, message_{std::move(message)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

  /// The offending path.
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
  std::string message_;
};

/// @ingroup system
/// The requested file does not exist.
class SOURCEMETA_CORE_SYSTEM_EXPORT FileNotFoundError : public std::exception {
public:
  /// Construct the error given the offending path.
  FileNotFoundError(std::filesystem::path path) : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "File not found";
  }

  /// The offending path.
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

/// @ingroup system
/// The current process lacks permission to access the requested path.
class SOURCEMETA_CORE_SYSTEM_EXPORT FilePermissionError
    : public std::exception {
public:
  /// Construct the error given the offending path.
  FilePermissionError(std::filesystem::path path) : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Permission denied";
  }

  /// The offending path.
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

/// @ingroup system
/// The path resolves to a regular file where a directory was expected.
class SOURCEMETA_CORE_SYSTEM_EXPORT NotADirectoryError : public std::exception {
public:
  /// Construct the error given the offending path.
  NotADirectoryError(std::filesystem::path path) : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Expected a directory but got a file";
  }

  /// The offending path.
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

/// @ingroup system
/// The destination path already exists and cannot be replaced.
class SOURCEMETA_CORE_SYSTEM_EXPORT FileAlreadyExistsError
    : public std::exception {
public:
  /// Construct the error given the offending path.
  FileAlreadyExistsError(std::filesystem::path path) : path_{std::move(path)} {}

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "File already exists";
  }

  /// The offending path.
  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

private:
  std::filesystem::path path_;
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

} // namespace sourcemeta::core

#endif
