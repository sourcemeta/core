#include <benchmark/benchmark.h>

#include <sourcemeta/core/markdown.h>

#include <cstddef> // std::size_t
#include <string>  // std::string, std::to_string
#include <vector>  // std::vector

static auto repeat(const std::string &pattern, const std::size_t count)
    -> std::string {
  std::string result;
  result.reserve(pattern.size() * count);
  for (std::size_t index = 0; index < count; ++index) {
    result.append(pattern);
  }

  return result;
}

static auto increasing_backtick_runs(const std::size_t count) -> std::string {
  std::string result;
  for (std::size_t length = 1; length <= count; ++length) {
    result.push_back('x');
    result.append(length, '`');
  }

  return result;
}

static auto staircase_list(const std::size_t depth) -> std::string {
  std::string result;
  for (std::size_t level = 0; level < depth; ++level) {
    result.append(level * 2, ' ');
    result.append("- level\n");
  }

  return result;
}

// NOLINTNEXTLINE(readability-identifier-naming)
static void Markdown_To_HTML_Realistic_Document(benchmark::State &state) {
  std::string input;
  for (std::size_t index = 0; index < 500; ++index) {
    const auto number{std::to_string(index)};
    input.append("# Section ").append(number).append("\n\n");
    input.append("This paragraph has *emphasis*, **strong emphasis**, ")
        .append("`inline code`, a [link](https://sourcemeta.com/")
        .append(number)
        .append(" \"Title\"), an autolink to www.sourcemeta.com, and ")
        .append("~~deleted text~~ with an entity &copy; and a footnote[^")
        .append(number)
        .append("].\n\n");
    input.append("> A block quote with a [reference link][ref")
        .append(number)
        .append("]\n> spanning two lines.\n\n");
    input.append("- First item\n  - Nested *item*\n  - Another `item`\n"
                 "- [x] Completed task\n- [ ] Pending task\n\n");
    input.append("1. One\n2. Two\n3. Three\n\n");
    input.append("| Name | Value | Notes |\n| :--- | ---: | :---: |\n"
                 "| alpha | 1 | *first* |\n| beta | 2 | `second` |\n"
                 "| gamma | 3 | [third](/third) |\n\n");
    input.append("```cpp\nauto main() -> int {\n  return 0;\n}\n```\n\n");
    input.append("    indented code line\n\n---\n\n");
    input.append("[ref")
        .append(number)
        .append("]: https://sourcemeta.com/reference/")
        .append(number)
        .append("\n[^")
        .append(number)
        .append("]: Footnote ")
        .append(number)
        .append(".\n\n");
  }

  for (auto iteration : state) {
    auto result{sourcemeta::core::markdown_to_html(input)};
    benchmark::DoNotOptimize(result);
  }
}

// Inputs that make naive delimiter, bracket, and container handling
// quadratic or worse, which a linear parser renders almost instantly
// NOLINTNEXTLINE(readability-identifier-naming)
static void Markdown_To_HTML_Pathological(benchmark::State &state) {
  const std::string input{
      repeat("_x **y ", 5000) + "z" + repeat(" y** x_", 5000) + "\n\n" +
      repeat("x_. ", 10000) + "\n\n" + repeat("__y ", 10000) + "\n\n" +
      repeat("]b", 10000) + "\n\n" + repeat("![b", 10000) + "\n\n" +
      repeat("_b* ", 10000) + "\n\n" + "x***y" + repeat("z* ", 10000) + "\n\n" +
      repeat("[ b*", 10000) + "\n\n" + repeat("[ x](", 10000) + "\n\n" +
      repeat("![", 5000) + "x" + repeat("]", 5000) + "\n\n" +
      repeat("![x](<y", 5000) + "\n\nx" + repeat("<![CDATA[", 5000) + "\n\nx" +
      repeat("<?", 5000) + "\n\n" + increasing_backtick_runs(2000) + "\n\n" +
      repeat("> - ", 50) + "deep\n\n" + staircase_list(200) + "\n" +
      "| a | b | c |\n|---|---|---|\n" + repeat("| x | y | z |\n", 5000)};

  for (auto iteration : state) {
    auto result{sourcemeta::core::markdown_to_html(input)};
    benchmark::DoNotOptimize(result);
  }
}

// Many short inputs, like the descriptions that schema documentation renders,
// where the cost of every call matters more than the cost of every byte, so
// every iteration converts the next input of the corpus
// NOLINTNEXTLINE(readability-identifier-naming)
static void Markdown_To_HTML_Short_Descriptions(benchmark::State &state) {
  std::vector<std::string> inputs;
  inputs.reserve(1000);
  for (std::size_t index = 0; index < 250; ++index) {
    const auto number{std::to_string(index)};
    inputs.push_back(
        std::string{"The identifier of resource "}.append(number).append("."));
    inputs.push_back(std::string{"A `string` that must match the *pattern* "}
                         .append(number)
                         .append("."));
    inputs.push_back(
        std::string{"See [the documentation](https://sourcemeta.com/"}
            .append(number)
            .append(") for **details**."));
    inputs.push_back(
        std::string{"- First option\n- Second option "}.append(number));
  }

  std::size_t position{0};
  for (auto iteration : state) {
    auto result{sourcemeta::core::markdown_to_html(inputs[position])};
    benchmark::DoNotOptimize(result);
    position += 1;
    if (position == inputs.size()) {
      position = 0;
    }
  }
}

BENCHMARK(Markdown_To_HTML_Realistic_Document);
BENCHMARK(Markdown_To_HTML_Pathological);
BENCHMARK(Markdown_To_HTML_Short_Descriptions);
