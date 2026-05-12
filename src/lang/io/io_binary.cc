#include <sourcemeta/core/io_binary.h>

#include <cstring>      // std::memcpy
#include <filesystem>   // std::filesystem::filesystem_error
#include <ios>          // std::streamsize
#include <system_error> // std::make_error_code, std::errc

namespace sourcemeta::core {

BinaryWriter::BinaryWriter(std::ostream &stream) noexcept : stream_{&stream} {}

auto BinaryWriter::write_bytes(const std::byte *data, const std::size_t size)
    -> void {
  this->stream_->write(reinterpret_cast<const char *>(data),
                       static_cast<std::streamsize>(size));
  if (this->stream_->fail()) {
    throw std::filesystem::filesystem_error{
        "BinaryWriter: failed to write to stream",
        std::make_error_code(std::errc::io_error)};
  }
}

BinaryReader::BinaryReader(const FileView &view) noexcept : view_{&view} {}

auto BinaryReader::offset() const noexcept -> std::size_t {
  return this->offset_;
}

auto BinaryReader::seek(const std::size_t position) -> void {
  if (position > this->view_->size()) {
    throw std::filesystem::filesystem_error{
        "BinaryReader: seek position past end of view",
        std::make_error_code(std::errc::result_out_of_range)};
  }

  this->offset_ = position;
}

auto BinaryReader::read_bytes(std::byte *destination, const std::size_t size)
    -> void {
  if (this->offset_ + size > this->view_->size()) {
    throw std::filesystem::filesystem_error{
        "BinaryReader: read past end of view",
        std::make_error_code(std::errc::result_out_of_range)};
  }

  if (size > 0) {
    std::memcpy(destination, this->view_->as<std::byte>(this->offset_), size);
    this->offset_ += size;
  }
}

} // namespace sourcemeta::core
