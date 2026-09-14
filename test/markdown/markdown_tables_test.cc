#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(simple_table) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\n| - | - |\n| 1 | 2 |")};
  EXPECT_EQ(
      result,
      "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</thead>\n"
      "<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(table_with_alignment) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| left | center | right |\n| :--- | :---: | ---: |\n| a | b | c |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n"
                    "<th align=\"left\">left</th>\n"
                    "<th align=\"center\">center</th>\n"
                    "<th align=\"right\">right</th>\n"
                    "</tr>\n</thead>\n<tbody>\n<tr>\n"
                    "<td align=\"left\">a</td>\n"
                    "<td align=\"center\">b</td>\n"
                    "<td align=\"right\">c</td>\n"
                    "</tr>\n</tbody>\n</table>\n");
}

TEST(table_with_formatting_in_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| **bold** |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><strong>bold</strong></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(table_with_multiple_rows) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| x | y |\n| - | - |\n| 1 | 2 |\n| 3 | 4 |\n| 5 | 6 |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>x</th>\n<th>y</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n"
            "<tr>\n<td>3</td>\n<td>4</td>\n</tr>\n"
            "<tr>\n<td>5</td>\n<td>6</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(table_with_pipe_in_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| a \\| b |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td>a | b</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(link_inside_table_cell) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| h |\n| - |\n| [click](https://example.com) |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td><a href=\"https://example.com\">click</a>"
            "</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(code_inside_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| `code` |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><code>code</code></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(empty_table_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\n| - | - |\n|   |   |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td></td>\n<td></td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(table_header_only) {
  const auto result{sourcemeta::core::markdown_to_html("| a | b |\n| - | - |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</"
                    "thead>\n</table>\n");
}

TEST(table_after_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Here is a table:\n\n| a |\n| - |\n| 1 |")};
  EXPECT_EQ(result, "<p>Here is a table:</p>\n"
                    "<table>\n<thead>\n<tr>\n<th>a</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td>1</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(table_with_inline_code_in_header) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| `key` | `value` |\n| - | - |\n| a | b |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th><code>key</code></th>\n"
                    "<th><code>value</code></th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td>a</td>\n<td>b</td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(table_escaped_pipes_inside_inline_content) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| code |\n| ---- |\n| `a\\|b` |\n| _c\\|d_ |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>code</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td><code>a|b</code></td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "<td><em>c|d</em></td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_ends_at_blank_line) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| a | b |\n|---|---|\n| 1 | 2 |\n\nafter")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "<th>b</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>1</td>\n"
                    "<td>2</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n"
                    "<p>after</p>\n");
}

TEST(table_ends_at_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a |\n|---|\n| 1 |\n> quote")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>1</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n"
                    "<blockquote>\n"
                    "<p>quote</p>\n"
                    "</blockquote>\n");
}

TEST(table_line_without_pipes_is_row) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\n|---|---|\ntext row")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "<th>b</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>text row</td>\n"
                    "<td></td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_delimiter_count_mismatch_is_not_table) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b | c |\n| - | - |")};
  EXPECT_EQ(result, "<p>| a | b | c |\n"
                    "| - | - |</p>\n");
}

TEST(table_rows_with_fewer_and_more_cells) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| a | b |\n| - | - |\n| 1 |\n| 1 | 2 | 3 |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "<th>b</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>1</td>\n"
                    "<td></td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "<td>1</td>\n"
                    "<td>2</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_without_outer_pipes) {
  const auto result{
      sourcemeta::core::markdown_to_html("x | y\n:-- | --:\n1 | 2")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th align=\"left\">x</th>\n"
                    "<th align=\"right\">y</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td align=\"left\">1</td>\n"
                    "<td align=\"right\">2</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_single_column_with_pipes) {
  const auto result{
      sourcemeta::core::markdown_to_html("| only |\n| :-: |\n| one |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th align=\"center\">only</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td align=\"center\">one</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_every_alignment) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| l | r | c | n |\n|:--|--:|:-:|---|\n| 1 | 2 | 3 | 4 |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th align=\"left\">l</th>\n"
                    "<th align=\"right\">r</th>\n"
                    "<th align=\"center\">c</th>\n"
                    "<th>n</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td align=\"left\">1</td>\n"
                    "<td align=\"right\">2</td>\n"
                    "<td align=\"center\">3</td>\n"
                    "<td>4</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_delimiter_row_with_surrounding_spaces) {
  const auto result{
      sourcemeta::core::markdown_to_html("x|y\n  --- |  :---:  ")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>x</th>\n"
                    "<th align=\"center\">y</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(table_header_cell_with_escaped_pipe) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a \\| b | c |\n| - | - |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a | b</th>\n"
                    "<th>c</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(table_cell_with_reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| [manual] |\n| --- |\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th><a href=\"/manual\">manual</a></th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(table_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("intro\na | b\n- | -")};
  EXPECT_EQ(result, "<p>intro\n"
                    "a | b</p>\n"
                    "<ul>\n"
                    "<li>| -</li>\n"
                    "</ul>\n");
}

TEST(table_inside_list_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a | b\n  - | -\n  1 | 2")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a | b\n"
                    "<ul>\n"
                    "<li>| -\n"
                    "1 | 2</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(table_inside_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("> a | b\n> - | -\n> 1 | 2")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>a | b</p>\n"
                    "<ul>\n"
                    "<li>| -\n"
                    "1 | 2</li>\n"
                    "</ul>\n"
                    "</blockquote>\n");
}

TEST(table_cell_with_entities) {
  const auto result{
      sourcemeta::core::markdown_to_html("| &lt;tag&gt; |\n| --- |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>&lt;tag&gt;</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(table_empty_header_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("|  |  |\n|--|--|\n| x | y |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th></th>\n"
                    "<th></th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>x</td>\n"
                    "<td>y</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(table_emphasis_does_not_span_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("| _a | b_ |\n|---|---|")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>_a</th>\n"
                    "<th>b_</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(table_invalid_delimiter_row_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("| a |\n| :-:x |")};
  EXPECT_EQ(result, "<p>| a |\n"
                    "| :-:x |</p>\n");
}

TEST(table_delimiter_count_mismatch_without_outer_pipes) {
  const auto result{sourcemeta::core::markdown_to_html("x | y | z\n- | -")};
  EXPECT_EQ(result, "<p>x | y | z</p>\n"
                    "<ul>\n"
                    "<li>| -</li>\n"
                    "</ul>\n");
}

TEST(table_leading_pipe_without_trailing_pipe) {
  const auto result{
      sourcemeta::core::markdown_to_html("| x | y\n|---|---\n| 1 | 2")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>x</th>\n"
                    "<th>y</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>1</td>\n"
                    "<td>2</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}
