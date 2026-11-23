#include <sourcemeta/core/benchmark.h>

#include <cassert> // assert
#include <cstddef> // std::size_t
#include <string>  // std::string, std::to_string

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/yaml.h>

// The record count sets how much of each construct the parser walks over, so
// it is the knob to turn when stress testing this functionality
static constexpr std::size_t RECORD_COUNT{128};
static constexpr std::size_t PARAGRAPH_COUNT{256};

// A record carries one of every shape the block parser branches on: plain,
// single quoted and double quoted scalars, a literal and a folded block
// scalar, a flow sequence, a flow mapping, and a nested block sequence of
// block mappings
static auto append_record(std::string &result, const std::size_t index)
    -> void {
  const auto number{std::to_string(index)};
  result.append("  - name: record-");
  result.append(number);
  result.append("\n    identifier: ");
  result.append(number);
  result.append("\n    enabled: true\n    ratio: 0.");
  result.append(number);
  result.append("\n    title: \"Quoted title ");
  result.append(number);
  result.append("\"\n    summary: 'Single quoted summary ");
  result.append(number);
  result.append("'\n    description: |\n      A literal block scalar for "
                "record ");
  result.append(number);
  result.append(".\n"
                "      It keeps every line break it was given, and carries\n"
                "      on for several lines so that scanning one costs\n"
                "      something worth measuring.\n"
                "    abstract: >\n"
                "      A folded block scalar whose lines are joined into a\n"
                "      single paragraph unless one of them is indented\n"
                "      further than the rest.\n"
                "    tags: [ alpha, beta, gamma ]\n"
                "    limits: { soft: 10, hard: 20 }\n"
                "    history:\n"
                "      - at: 2020-01-01\n"
                "        note: created\n"
                "      - at: 2021-06-15\n"
                "        note: updated\n");
}

BENCHMARK(YAML_Parse_Records) {
  std::string input{"records:\n"};
  for (std::size_t index = 0; index < RECORD_COUNT; index += 1) {
    append_record(input, index);
  }

  for (auto iteration : state) {
    auto result{sourcemeta::core::parse_yaml(input)};
    assert(result.is_object());
    assert(result.at("records").size() == RECORD_COUNT);
    assert(result.at("records").at(0).at("history").size() == 2);
    assert(result.at("records").at(0).at("tags").size() == 3);
    assert(result.at("records").at(0).at("limits").size() == 2);
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

// A document that is almost entirely block scalars, so that what this measures
// is scanning them rather than the surrounding mapping
BENCHMARK(YAML_Parse_Block_Scalars) {
  std::string input{"paragraphs:\n"};
  for (std::size_t index = 0; index < PARAGRAPH_COUNT; index += 1) {
    const auto number{std::to_string(index)};
    input.append("  paragraph-")
        .append(number)
        .append(": |-\n    The first line of paragraph ")
        .append(number)
        .append(".\n"
                "    A second line that carries on from it.\n"
                "    A third line, indented the same as the rest.\n"
                "      A fourth line indented further than the rest.\n"
                "    A fifth line back at the original indentation.\n");
  }

  for (auto iteration : state) {
    auto result{sourcemeta::core::parse_yaml(input)};
    assert(result.is_object());
    assert(result.at("paragraphs").size() == PARAGRAPH_COUNT);
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}
