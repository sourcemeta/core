#include <sourcemeta/core/io.h>
#include <sourcemeta/core/io_write.h>

#include <cassert>      // assert
#include <cerrno>       // EACCES, errno
#include <filesystem>   // std::filesystem
#include <ios>          // std::ios::binary, std::ios::trunc
#include <system_error> // std::error_code

namespace sourcemeta::core::detail {

AtomicFileWriter::AtomicFileWriter(const std::filesystem::path &destination)
    : destination_{destination}, staging_{destination} {
  // The staging file lives next to the destination so that
  // `std::filesystem::rename` stays on a single filesystem and remains
  // atomic. Using the system-wide temporary directory would risk `EXDEV`
  // errors on cross-filesystem builds (CI containers, NFS mounts, etc.).
  this->staging_ += ".tmp";

  if (this->destination_.has_parent_path()) {
    std::filesystem::create_directories(this->destination_.parent_path());
  }

  this->stream_.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  try {
    this->stream_.open(this->staging_, std::ios::binary | std::ios::trunc);
  } catch (...) {
    // Capture before any other syscall can clobber errno
    const auto open_errno{errno};
    std::error_code remove_error;
    std::filesystem::remove(this->staging_, remove_error);
    if (open_errno == EACCES) {
      throw IOFilePermissionError{this->destination_};
    }

    throw;
  }
}

AtomicFileWriter::~AtomicFileWriter() {
  if (!this->committed_) {
    if (this->stream_.is_open()) {
      this->stream_.close();
    }

    std::error_code error;
    std::filesystem::remove(this->staging_, error);
  }
}

auto AtomicFileWriter::stream() -> std::ostream & { return this->stream_; }

auto AtomicFileWriter::commit() -> void {
  assert(!this->committed_);
  this->stream_.flush();
  this->stream_.close();
  sourcemeta::core::flush(this->staging_);
  std::filesystem::rename(this->staging_, this->destination_);
  this->committed_ = true;
}

} // namespace sourcemeta::core::detail

namespace sourcemeta::core {

auto atomic_write_file(const std::filesystem::path &path,
                       const std::string_view contents) -> void {
  atomic_write_file(path, [contents](std::ostream &stream) -> void {
    stream.write(contents.data(),
                 static_cast<std::streamsize>(contents.size()));
  });
}

auto atomic_write_file(const std::filesystem::path &path,
                       const std::span<const std::byte> contents) -> void {
  atomic_write_file(path, [contents](std::ostream &stream) -> void {
    stream.write(reinterpret_cast<const char *>(contents.data()),
                 static_cast<std::streamsize>(contents.size()));
  });
}

} // namespace sourcemeta::core
