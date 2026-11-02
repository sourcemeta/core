#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/html.h>

#include <cstddef> // std::size_t
#include <string>  // std::string, std::to_string

static void write_table(sourcemeta::core::HTMLWriter &document) {
  document.table().attribute("class", "file-table");
  document.thead();
  document.tr();
  document.th("Name");
  document.th("Size");
  document.th("Date");
  document.th("Owner");
  document.th("Type");
  document.th("Status");
  document.close();
  document.close();
  document.tbody().attribute("class", "file-list");
  for (std::size_t index = 0; index < 100000; ++index) {
    auto row_str{std::to_string(index)};
    document.tr();
    document.td();
    document.a().attribute("href", "/file/" + row_str);
    document.i().attribute("class", "icon icon-file");
    document.close();
    document.span("file_" + row_str + ".json");
    document.close();
    document.close();
    document.td();
    document.span().attribute("class", "size");
    document.small("1.2 KB");
    document.close();
    document.close();
    document.td();
    document.span().attribute("class", "date");
    document.small("2025-01-15");
    document.close();
    document.close();
    document.td();
    document.a().attribute("href", "/user/owner");
    document.span("owner");
    document.close();
    document.close();
    document.td();
    document.span().attribute("class", "type");
    document.small("application/json");
    document.close();
    document.close();
    document.td();
    document.div().attribute("class", "progress");
    document.div()
        .attribute("class", "progress-bar")
        .attribute("style", "width: 75%");
    document.close();
    document.close();
    document.close();
    document.close();
  }
  document.close();
  document.close();
}

BENCHMARK(HTML_Build_Table_100000) {
  for (auto iteration : state) {
    sourcemeta::core::HTMLWriter document;
    document.reserve(std::size_t{100000} * 300);
    write_table(document);
    auto result{std::string(document.str())};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(HTML_Render_Table_100000) {
  sourcemeta::core::HTMLWriter document;
  document.reserve(std::size_t{100000} * 300);
  write_table(document);
  for (auto iteration : state) {
    auto result{std::string(document.str())};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}
