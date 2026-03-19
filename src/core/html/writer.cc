#include <sourcemeta/core/html_writer.h>

#include <iostream> // std::ostream

namespace sourcemeta::core {

auto HTMLWriter::flush_open_tag() -> void {
  if (this->tag_open_) {
    if (this->tag_open_is_void_) {
      this->output_ += " />";
    } else {
      this->output_ += ">";
    }

    this->tag_open_ = false;
    this->tag_open_is_void_ = false;
  }
}

auto HTMLWriter::write(std::ostream &stream) -> void {
  this->flush_open_tag();
  stream << this->output_;
}

} // namespace sourcemeta::core
