#include <sourcemeta/core/html_encoder.h>

#include <iostream> // std::ostream
#include <string>   // std::string

namespace sourcemeta::core {

auto HTML::render_to(std::string &output) const -> void {
  output += "<";
  output += this->tag_name;

  for (const auto &[attribute_name, attribute_value] : this->attributes) {
    std::string escaped_value{attribute_value};
    html_escape(escaped_value);
    output += " ";
    output += attribute_name;
    output += "=\"";
    output += escaped_value;
    output += "\"";
  }

  if (this->self_closing) {
    output += " />";
    return;
  }

  output += ">";

  for (const auto &child_element : this->child_elements) {
    this->render_to(output, child_element);
  }

  output += "</";
  output += this->tag_name;
  output += ">";
}

auto HTML::render_to(std::string &output, const HTMLNode &child_element) const
    -> void {
  if (const auto *text = std::get_if<std::string>(&child_element)) {
    std::string escaped_text{*text};
    html_escape(escaped_text);
    output += escaped_text;
  } else if (const auto *raw_html = std::get_if<HTMLRaw>(&child_element)) {
    output += raw_html->content;
  } else if (const auto *html_element = std::get_if<HTML>(&child_element)) {
    html_element->render_to(output);
  }
}

auto HTML::render_to(std::ostream &output_stream) const -> void {
  std::string output;
  this->render_to(output);
  output_stream << output;
}

auto HTML::render_to(std::ostream &output_stream,
                     const HTMLNode &child_element) const -> void {
  std::string output;
  this->render_to(output, child_element);
  output_stream << output;
}

auto HTML::render() const -> std::string {
  std::string output;
  this->render_to(output);
  return output;
}

auto HTML::push_back(const HTMLNode &child) -> HTML & {
  this->child_elements.push_back(child);
  return *this;
}

auto HTML::push_back(HTMLNode &&child) -> HTML & {
  this->child_elements.push_back(std::move(child));
  return *this;
}

auto operator<<(std::ostream &output_stream, const HTML &html_element)
    -> std::ostream & {
  html_element.render_to(output_stream);
  return output_stream;
}

} // namespace sourcemeta::core
