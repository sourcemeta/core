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
  // The spare capacity is part of the contents of the underlying string, which
  // the C++ standard forbids from holding indeterminate values, so growing
  // without initializing the new space is not an option
  this->buffer_.resize(capacity);
  this->begin_ = this->buffer_.data();
  this->cursor_ = this->begin_ + used;
  this->end_ = this->begin_ + capacity;
}

auto HTMLBuffer::write(std::ostream &stream) -> void {
  const auto used{this->size()};
  if (used > 0) {
    stream.write(this->begin_, static_cast<std::streamsize>(used));
  }
}

auto HTMLWriter::write(std::ostream &stream) -> void {
  this->buffer_.write(stream);
}

} // namespace sourcemeta::core
