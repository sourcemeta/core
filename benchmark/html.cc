#include <benchmark/benchmark.h>

#include <sourcemeta/core/html.h>

#include <string> // std::string, std::to_string
#include <vector> // std::vector

using namespace sourcemeta::core::html;

static auto make_table() -> sourcemeta::core::HTML {
  std::vector<sourcemeta::core::HTMLNode> row_nodes;
  row_nodes.reserve(100000);
  for (std::size_t index = 0; index < 100000; ++index) {
    auto row_str{std::to_string(index)};
    row_nodes.emplace_back(
        tr(td(a({{"href", "/file/" + row_str}},
                i({{"class", "icon icon-file"}}, std::string("")),
                span("file_" + row_str + ".json"))),
           td(span({{"class", "size"}}, small("1.2 KB"))),
           td(span({{"class", "date"}}, small("2025-01-15"))),
           td(a({{"href", "/user/owner"}}, span("owner"))),
           td(span({{"class", "type"}}, small("application/json"))),
           td(div({{"class", "progress"}},
                  div({{"class", "progress-bar"}, {"style", "width: 75%"}},
                      std::string(""))))));
  }

  return table({{"class", "file-table"}},
               thead(tr(th("Name"), th("Size"), th("Date"), th("Owner"),
                        th("Type"), th("Status"))),
               tbody({{"class", "file-list"}}, std::move(row_nodes)));
}

static void HTML_Build_Table_100000(benchmark::State &state) {
  for (auto _ : state) {
    auto result{make_table()};
    benchmark::DoNotOptimize(result);
  }
}

static void HTML_Render_Table_100000(benchmark::State &state) {
  const auto tree{make_table()};
  for (auto _ : state) {
    auto result{tree.render()};
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(HTML_Build_Table_100000);
BENCHMARK(HTML_Render_Table_100000);
