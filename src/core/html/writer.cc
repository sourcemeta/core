#include <sourcemeta/core/html_writer.h>

#include <algorithm> // std::max
#include <cstddef>   // std::size_t
#include <iostream>  // std::ostream

namespace sourcemeta::core {

auto HTMLBuffer::grow(const std::size_t needed) -> void {
  auto new_capacity{std::max(1024UZ, this->capacity() * 2)};
  while (new_capacity < this->size() + needed) {
    new_capacity *= 2;
  }

  this->reallocate(new_capacity);
}

auto HTMLBuffer::reallocate(const std::size_t capacity) -> void {
  const auto used{this->size()};
  // Every byte past the accumulated contents is written before it is read, so
  // there is no point in zero filling the new capacity
  this->buffer_.resize_and_overwrite(
      capacity, [](const char *, const std::size_t size) { return size; });
  this->begin_ = this->buffer_.data();
  this->cursor_ = this->begin_ + used;
  this->end_ = this->begin_ + capacity;
}

auto HTMLBuffer::write(std::ostream &stream) -> void {
  stream.write(this->begin_, static_cast<std::streamsize>(this->size()));
}

auto HTMLWriter::write(std::ostream &stream) -> void {
  this->buffer_.write(stream);
}

} // namespace sourcemeta::core
