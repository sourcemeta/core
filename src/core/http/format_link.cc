#include <sourcemeta/core/http.h>

#include <span>   // std::span
#include <string> // std::string

namespace sourcemeta::core {

auto http_format_link(const HTTPLink &link, std::string &out) -> void {
  out.push_back('<');
  out.append(link.target);
  out.append(">; rel=\"");
  out.append(link.rel);
  out.push_back('"');
  for (const auto &[name, value] : link.parameters) {
    out.append("; ");
    out.append(name);
    out.append("=\"");
    out.append(value);
    out.push_back('"');
  }
}

auto http_format_link(const HTTPLink &link) -> std::string {
  std::string out;
  http_format_link(link, out);
  return out;
}

auto http_format_links(std::span<const HTTPLink> links, std::string &out)
    -> void {
  bool first{true};
  for (const auto &link : links) {
    if (!first) {
      out.append(", ");
    }
    http_format_link(link, out);
    first = false;
  }
}

auto http_format_links(std::span<const HTTPLink> links) -> std::string {
  std::string out;
  http_format_links(links, out);
  return out;
}

} // namespace sourcemeta::core
