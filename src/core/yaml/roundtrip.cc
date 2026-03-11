#include <sourcemeta/core/yaml.h>

namespace sourcemeta::core {

auto YAMLRoundTrip::clear() -> void {
  this->styles.clear();
  this->aliases.clear();
  this->key_styles.clear();
  this->explicit_document_start = false;
  this->explicit_document_end = false;
  this->document_start_comment.reset();
  this->document_end_comment.reset();
  this->leading_comments.clear();
  this->post_start_comments.clear();
  this->pre_end_comments.clear();
  this->trailing_comments.clear();
  this->indent_width = 2;
}

} // namespace sourcemeta::core
