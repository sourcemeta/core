#include <benchmark/benchmark.h>

#include <cassert> // assert
#include <cstddef> // std::size_t

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpath.h>

static void JSONPath_Descendant_Filter_Nested(benchmark::State &state) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "store": {
      "departments": [
        {
          "name": "literature",
          "shelves": [
            {
              "books": [
                { "category": "fiction", "title": "Sayings of the Century", "price": 8.95 },
                { "category": "fiction", "title": "Sword of Honour", "price": 12.99 },
                { "category": "reference", "title": "Moby Dick", "price": 8.99 },
                { "category": "fiction", "title": "The Lord of the Rings", "price": 22.99 }
              ]
            },
            {
              "books": [
                { "category": "reference", "title": "Atlas of the World", "price": 45.5 },
                { "category": "fictional biography", "title": "The Quixote", "price": 6.15 },
                { "category": "fiction", "title": "The Aleph", "price": 9.99 },
                { "category": "poetry", "title": "Leaves of Grass", "price": 5.75 }
              ]
            }
          ]
        },
        {
          "name": "science",
          "shelves": [
            {
              "books": [
                { "category": "fiction", "title": "Solaris", "price": 7.25 },
                { "category": "reference", "title": "Relativity", "price": 11.5 },
                { "category": "fiction", "title": "Foundation", "price": 3.99 },
                { "category": "fiction", "title": "Dune", "price": 10.0 }
              ]
            }
          ],
          "archive": {
            "books": [
              { "category": "fiction", "title": "The Time Machine", "price": 2.5 },
              { "category": "essay", "title": "On the Motion of Bodies", "price": 4.05 }
            ]
          }
        }
      ]
    }
  })JSON")};

  const sourcemeta::core::JSONPath path{
      "$..books[?@.price < 10 && match(@.category, 'fic.*')]"};

  for (auto _ : state) {
    std::size_t count{0};
    path.evaluate(document,
                  [&count](const auto &, const auto &) { count += 1; });
    assert(count == 6);
    benchmark::DoNotOptimize(count);
  }
}

BENCHMARK(JSONPath_Descendant_Filter_Nested);
