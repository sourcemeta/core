#include <sourcemeta/core/io_binary.h>
#include <sourcemeta/core/io_error.h>

#include <cstring> // std::memcpy
#include <ios>     // std::streamsize

namespace sourcemeta::core {

BinaryWriter::BinaryWriter(std::ostream &stream) noexcept : stream_{&stream} {}

auto BinaryWriter::write_bytes(const std::byte *data, const std::size_t size)
    -> void {
  this->stream_->write(reinterpret_cast<const char *>(data),
                       static_cast<std::streamsize>(size));
  if (this->stream_->fail()) {
    throw IOStreamWriteError{};
  }
}

BinaryReader::BinaryReader(const FileView &view) noexcept : view_{&view} {}

auto BinaryReader::offset() const noexcept -> std::size_t {
  return this->offset_;
}

auto BinaryReader::seek(const std::size_t position) -> void {
  if (position > this->view_->size()) {
    throw IOReadOutOfBoundsError{};
  }

  this->offset_ = position;
}

auto BinaryReader::read_bytes(std::byte *destination, const std::size_t size)
    -> void {
  if (size > this->view_->size() - this->offset_) {
    throw IOReadOutOfBoundsError{};
  }

  if (size > 0) {
    std::memcpy(destination, this->view_->as<std::byte>(this->offset_), size);
    this->offset_ += size;
  }
}

} // namespace sourcemeta::core
