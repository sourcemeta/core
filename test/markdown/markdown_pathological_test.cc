#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view

using namespace std::literals::string_view_literals;

static auto repeat(const std::string_view pattern, const std::size_t count)
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

static auto numbered_reference_definitions(const std::size_t count)
    -> std::string {
  std::string result;
  for (std::size_t index = 0; index < count; ++index) {
    const auto number{std::to_string(index)};
    result.append("[r").append(number).append("]: /").append(number).append(
        "\n");
  }

  return result;
}

static auto numbered_reference_uses(const std::size_t count) -> std::string {
  std::string result;
  for (std::size_t index = 0; index < count; ++index) {
    if (index > 0) {
      result.push_back(' ');
    }

    result.append("[r").append(std::to_string(index)).append("]");
  }

  return result;
}

static auto numbered_reference_anchors(const std::size_t count) -> std::string {
  std::string result;
  for (std::size_t index = 0; index < count; ++index) {
    if (index > 0) {
      result.push_back(' ');
    }

    const auto number{std::to_string(index)};
    result.append("<a href=\"/")
        .append(number)
        .append("\">r")
        .append(number)
        .append("</a>");
  }

  return result;
}

TEST(alternating_nested_emphasis) {
  const auto input{repeat("_x **y ", 5000) + "z" + repeat(" y** x_", 5000)};
  const auto result{sourcemeta::core::markdown_to_html(input)};
  EXPECT_EQ(result, "<p>" + repeat("<em>x <strong>y ", 5000) + "z" +
                        repeat(" y</strong> x</em>", 5000) + "</p>\n");
}

TEST(underscore_closers_before_punctuation_without_openers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("x_. ", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("x_. ", 9999) + "x_.</p>\n");
}

TEST(double_underscore_openers_without_closers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("__y ", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("__y ", 9999) + "__y</p>\n");
}

TEST(link_closers_without_openers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("]b", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("]b", 10000) + "</p>\n");
}

TEST(image_openers_without_closers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("![b", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("![b", 10000) + "</p>\n");
}

TEST(mismatched_underscore_openers_and_asterisk_closers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("_b* ", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("_b* ", 9999) + "_b*</p>\n");
}

TEST(intraword_triple_asterisks_followed_by_single_closers) {
  const auto result{
      sourcemeta::core::markdown_to_html("x***y" + repeat("z* ", 10000))};
  EXPECT_EQ(result, "<p>x<em><em><em>yz</em> z</em> z</em> " +
                        repeat("z* ", 9996) + "z*</p>\n");
}

TEST(link_openers_with_asterisk_closers) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("[ b*", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("[ b*", 10000) + "</p>\n");
}

TEST(unclosed_link_destinations_with_open_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html(repeat("[ x](", 10000))};
  EXPECT_EQ(result, "<p>" + repeat("[ x](", 10000) + "</p>\n");
}

TEST(nested_image_brackets) {
  const auto input{repeat("![", 5000) + "x" + repeat("]", 5000)};
  const auto result{sourcemeta::core::markdown_to_html(input)};
  EXPECT_EQ(result, "<p>" + input + "</p>\n");
}

TEST(unclosed_image_angle_bracket_destinations) {
  const auto result{
      sourcemeta::core::markdown_to_html(repeat("![x](<y", 5000))};
  EXPECT_EQ(result, "<p>" + repeat("![x](&lt;y", 5000) + "</p>\n");
}

TEST(unclosed_inline_cdata_sections) {
  const auto result{
      sourcemeta::core::markdown_to_html("x" + repeat("<![CDATA[", 5000))};
  EXPECT_EQ(result, "<p>x" + repeat("&lt;![CDATA[", 5000) + "</p>\n");
}

TEST(unclosed_inline_processing_instructions) {
  const auto result{
      sourcemeta::core::markdown_to_html("x" + repeat("<?", 5000))};
  EXPECT_EQ(result, "<p>x" + repeat("&lt;?", 5000) + "</p>\n");
}

TEST(backtick_runs_of_increasing_length) {
  const auto input{increasing_backtick_runs(2000)};
  const auto result{sourcemeta::core::markdown_to_html(input)};
  EXPECT_EQ(result, "<p>" + input + "</p>\n");
}

TEST(code_span_with_eighty_backticks) {
  const auto result{sourcemeta::core::markdown_to_html(
      std::string(80, '`') + "x" + std::string(80, '`'))};
  EXPECT_EQ(result, "<p><code>x</code></p>\n");
}

TEST(code_span_with_eighty_one_backticks) {
  const auto input{std::string(81, '`') + "x" + std::string(81, '`')};
  const auto result{sourcemeta::core::markdown_to_html(input)};
  EXPECT_EQ(result, "<p>" + input + "</p>\n");
}

TEST(alternating_blockquote_and_list_nesting_on_one_line) {
  const auto result{
      sourcemeta::core::markdown_to_html(repeat("> - ", 30) + "deep")};
  EXPECT_EQ(result, repeat("<blockquote>\n<ul>\n<li>\n", 29) +
                        "<blockquote>\n<ul>\n<li>deep</li>\n</ul>\n"
                        "</blockquote>\n" +
                        repeat("</li>\n</ul>\n</blockquote>\n", 29));
}

TEST(alternating_blockquote_and_list_nesting_on_one_line_beyond_depth_limit) {
  const auto result{
      sourcemeta::core::markdown_to_html(repeat("> - ", 50) + "deep")};
  EXPECT_EQ(result, repeat("<blockquote>\n<ul>\n<li>\n", 49) +
                        "<blockquote>\n<p>- deep</p>\n</blockquote>\n" +
                        repeat("</li>\n</ul>\n</blockquote>\n", 49));
}

TEST(staircase_list_of_one_hundred_fifty_levels) {
  const auto result{sourcemeta::core::markdown_to_html(staircase_list(150))};
  EXPECT_EQ(result, repeat("<ul>\n<li>level\n", 149) +
                        "<ul>\n<li>level</li>\n</ul>\n" +
                        repeat("</li>\n</ul>\n", 149));
}

TEST(table_with_many_rows) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| a | b | c |\n|---|---|---|\n" + repeat("| x | y | z |\n", 5000))};
  EXPECT_EQ(
      result,
      "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n<th>c</th>\n"
      "</tr>\n</thead>\n<tbody>\n" +
          repeat("<tr>\n<td>x</td>\n<td>y</td>\n<td>z</td>\n</tr>\n", 5000) +
          "</tbody>\n</table>\n");
}

TEST(table_with_many_columns) {
  const auto result{sourcemeta::core::markdown_to_html(
      "|" + repeat(" h |", 1000) + "\n|" + repeat("-|", 1000) + "\n|" +
      repeat(" c |", 1000))};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n" + repeat("<th>h</th>\n", 1000) +
                        "</tr>\n</thead>\n<tbody>\n<tr>\n" +
                        repeat("<td>c</td>\n", 1000) +
                        "</tr>\n</tbody>\n</table>\n");
}

TEST(nul_characters_are_replaced) {
  const auto result{sourcemeta::core::markdown_to_html("one\0two\0"sv)};
  EXPECT_EQ(result, "<p>one\xef\xbf\xbdtwo\xef\xbf\xbd</p>\n");
}

TEST(link_reference_label_of_one_thousand_characters) {
  const std::string label(1000, 'y');
  const auto result{sourcemeta::core::markdown_to_html(
      "[" + label + "]: /x\n\n[" + label + "]")};
  EXPECT_EQ(result, "<p><a href=\"/x\">" + label + "</a></p>\n");
}

TEST(link_reference_label_of_one_thousand_one_characters) {
  const std::string label(1001, 'y');
  const auto result{sourcemeta::core::markdown_to_html(
      "[" + label + "]: /x\n\n[" + label + "]")};
  EXPECT_EQ(result, "<p>[" + label + "]: /x</p>\n<p>[" + label + "]</p>\n");
}

TEST(single_paragraph_line_of_two_hundred_thousand_characters) {
  const std::string input(200000, 'w');
  const auto result{sourcemeta::core::markdown_to_html(input)};
  EXPECT_EQ(result, "<p>" + input + "</p>\n");
}

TEST(two_thousand_reference_definitions_and_uses) {
  const auto result{
      sourcemeta::core::markdown_to_html(numbered_reference_definitions(2000) +
                                         "\n" + numbered_reference_uses(2000))};
  EXPECT_EQ(result, "<p>" + numbered_reference_anchors(2000) + "</p>\n");
}
