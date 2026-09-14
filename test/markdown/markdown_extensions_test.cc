#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

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

TEST(strikethrough_spanning_lines) {
  const auto result{sourcemeta::core::markdown_to_html("~~one\ntwo~~")};
  EXPECT_EQ(result, "<p><del>one\n"
                    "two</del></p>\n");
}

TEST(strikethrough_triple_tildes_inline) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~~y~~~ z")};
  EXPECT_EQ(result, "<p>x ~~~y~~~ z</p>\n");
}

TEST(strikethrough_unclosed) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~y")};
  EXPECT_EQ(result, "<p>x ~~y</p>\n");
}

TEST(strikethrough_with_surrounding_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~ y ~~ z")};
  EXPECT_EQ(result, "<p>x ~~ y ~~ z</p>\n");
}

TEST(strikethrough_and_emphasis_overlap) {
  const auto result{sourcemeta::core::markdown_to_html("~~_x~~_")};
  EXPECT_EQ(result, "<p><del>_x</del>_</p>\n");
}

TEST(strikethrough_escaped_closing) {
  const auto result{sourcemeta::core::markdown_to_html("~~x\\~~")};
  EXPECT_EQ(result, "<p>~~x~~</p>\n");
}

TEST(strikethrough_inside_link_text) {
  const auto result{sourcemeta::core::markdown_to_html("[~~old~~ new](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\"><del>old</del> new</a></p>\n");
}

TEST(extended_autolink_www_with_path) {
  const auto result{
      sourcemeta::core::markdown_to_html("see www.sourcemeta.com/docs/intro")};
  EXPECT_EQ(result, "<p>see <a "
                    "href=\"http://www.sourcemeta.com/docs/"
                    "intro\">www.sourcemeta.com/docs/intro</a></p>\n");
}

TEST(extended_autolink_trailing_punctuation_excluded) {
  const auto result{
      sourcemeta::core::markdown_to_html("Go to www.sourcemeta.com/x.y!")};
  EXPECT_EQ(result, "<p>Go to <a "
                    "href=\"http://www.sourcemeta.com/x.y\">www.sourcemeta.com/"
                    "x.y</a>!</p>\n");
}

TEST(extended_autolink_balanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/wiki/Thing_(disambiguation)")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"https://sourcemeta.com/wiki/Thing_(disambiguation)\">https://"
      "sourcemeta.com/wiki/Thing_(disambiguation)</a></p>\n");
}

TEST(extended_autolink_unbalanced_closing_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("https://sourcemeta.com/a_(b)))")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/a_(b)\">https://"
                    "sourcemeta.com/a_(b)</a>))</p>\n");
}

TEST(extended_autolink_inside_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("(see https://sourcemeta.com/a_(b))")};
  EXPECT_EQ(result, "<p>(see <a "
                    "href=\"https://sourcemeta.com/a_(b)\">https://"
                    "sourcemeta.com/a_(b)</a>)</p>\n");
}

TEST(extended_autolink_entity_like_suffix_excluded) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/?x=1&amp;\n\nhttps://sourcemeta.com/?x=1&y=2")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/?x=1\">https://"
                    "sourcemeta.com/?x=1</a>&amp;</p>\n"
                    "<p><a "
                    "href=\"https://sourcemeta.com/?x=1&amp;y=2\">https://"
                    "sourcemeta.com/?x=1&amp;y=2</a></p>\n");
}

TEST(extended_autolink_ends_at_less_than) {
  const auto result{
      sourcemeta::core::markdown_to_html("https://sourcemeta.com/a<b")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/a\">https://sourcemeta.com/"
                    "a</a>&lt;b</p>\n");
}

TEST(extended_autolink_underscore_in_final_domain_segments) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.sourcemeta._com\n\nwww.source_meta.com")};
  EXPECT_EQ(result, "<p>www.sourcemeta._com</p>\n"
                    "<p>www.source_meta.com</p>\n");
}

TEST(extended_autolink_underscore_in_earlier_domain_segment) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.docs_site.sourcemeta.com")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"http://"
      "www.docs_site.sourcemeta.com\">www.docs_site.sourcemeta.com</a></p>\n");
}

TEST(extended_autolink_http_with_query_and_fragment) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/search?q=json+schema#results")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"https://sourcemeta.com/search?q=json+schema#results\">https://"
      "sourcemeta.com/search?q=json+schema#results</a></p>\n");
}

TEST(extended_autolink_email_local_and_domain_symbols) {
  const auto result{sourcemeta::core::markdown_to_html(
      "team@sourcemeta.com\n\nteam@source+meta.com\n\nteam+tag@sourcemeta."
      "com")};
  EXPECT_EQ(
      result,
      "<p><a href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a></p>\n"
      "<p>team@source+meta.com</p>\n"
      "<p><a "
      "href=\"mailto:team+tag@sourcemeta.com\">team+tag@sourcemeta.com</a></"
      "p>\n");
}

TEST(extended_autolink_email_trailing_symbols) {
  const auto result{sourcemeta::core::markdown_to_html(
      "first.last@sourcemeta.com.\n\nfirst.last@sourcemeta.com-\n\nfirst.last@"
      "sourcemeta.com_")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"mailto:first.last@sourcemeta.com\">first.last@"
                    "sourcemeta.com</a>.</p>\n"
                    "<p>first.last@sourcemeta.com-</p>\n"
                    "<p>first.last@sourcemeta.com_</p>\n");
}

TEST(extended_autolink_mailto_and_xmpp) {
  const auto result{sourcemeta::core::markdown_to_html(
      "mailto:team@sourcemeta.com\n\nxmpp:team@sourcemeta.com/resource")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:team@sourcemeta.com\">mailto:team@sourcemeta.com</a></p>\n"
      "<p><a "
      "href=\"xmpp:team@sourcemeta.com/resource\">xmpp:team@sourcemeta.com/"
      "resource</a></p>\n");
}

TEST(extended_autolink_not_after_word_character) {
  const auto result{sourcemeta::core::markdown_to_html("xwww.sourcemeta.com")};
  EXPECT_EQ(result, "<p>xwww.sourcemeta.com</p>\n");
}

TEST(extended_autolink_inside_emphasis) {
  const auto result{
      sourcemeta::core::markdown_to_html("_https://sourcemeta.com_")};
  EXPECT_EQ(
      result,
      "<p><em><a "
      "href=\"https://sourcemeta.com\">https://sourcemeta.com</a></em></p>\n");
}

TEST(extended_autolink_ftp_scheme) {
  const auto result{
      sourcemeta::core::markdown_to_html("ftp://files.sourcemeta.com/archive")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"ftp://files.sourcemeta.com/archive\">ftp://"
                    "files.sourcemeta.com/archive</a></p>\n");
}

TEST(extended_autolink_inside_link_text) {
  const auto result{
      sourcemeta::core::markdown_to_html("[https://sourcemeta.com](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">https://sourcemeta.com</a></p>\n");
}

TEST(extended_autolink_inside_code_span) {
  const auto result{
      sourcemeta::core::markdown_to_html("`https://sourcemeta.com`")};
  EXPECT_EQ(result, "<p><code>https://sourcemeta.com</code></p>\n");
}

TEST(task_list_uppercase_marker) {
  const auto result{sourcemeta::core::markdown_to_html("* [X] finished")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> finished</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_needs_following_space) {
  const auto result{sourcemeta::core::markdown_to_html("* [ ]pending")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[ ]pending</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_with_other_character) {
  const auto result{
      sourcemeta::core::markdown_to_html("* [-] partial\n* [x] complete")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[-] partial</li>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> complete</li>\n"
                    "</ul>\n");
}

TEST(task_list_nested) {
  const auto result{sourcemeta::core::markdown_to_html(
      "* [ ] parent\n  * [x] child\n    * [ ] grandchild")};
  EXPECT_EQ(result,
            "<ul>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> parent\n"
            "<ul>\n"
            "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> child\n"
            "<ul>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> grandchild</li>\n"
            "</ul>\n"
            "</li>\n"
            "</ul>\n"
            "</li>\n"
            "</ul>\n");
}

TEST(task_list_in_ordered_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) [ ] first\n2) [X] second")};
  EXPECT_EQ(
      result,
      "<ol>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> first</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> second</li>\n"
      "</ol>\n");
}

TEST(task_list_marker_not_at_item_start) {
  const auto result{sourcemeta::core::markdown_to_html("* text [ ] later")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>text [ ] later</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_alone) {
  const auto result{sourcemeta::core::markdown_to_html("* [x]")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[x]</li>\n"
                    "</ul>\n");
}

TEST(task_list_in_loose_list) {
  const auto result{sourcemeta::core::markdown_to_html("* [x] a\n\n* [ ] b")};
  EXPECT_EQ(result,
            "<ul>\n"
            "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> \n"
            "<p>a</p>\n"
            "</li>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> \n"
            "<p>b</p>\n"
            "</li>\n"
            "</ul>\n");
}

TEST(footnote_referenced_multiple_times) {
  const auto result{sourcemeta::core::markdown_to_html(
      "one[^n] two[^n] three[^n]\n\n[^n]: shared")};
  EXPECT_EQ(
      result,
      "<p>one<sup class=\"footnote-ref\"><a href=\"#fn-n\" id=\"fnref-n\" "
      "data-footnote-ref>1</a></sup> two<sup class=\"footnote-ref\"><a "
      "href=\"#fn-n\" id=\"fnref-n-2\" data-footnote-ref>1</a></sup> three<sup "
      "class=\"footnote-ref\"><a href=\"#fn-n\" id=\"fnref-n-3\" "
      "data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n"
      "<ol>\n"
      "<li id=\"fn-n\">\n"
      "<p>shared <a href=\"#fnref-n\" class=\"footnote-backref\" "
      "data-footnote-backref data-footnote-backref-idx=\"1\" aria-label=\"Back "
      "to reference 1\">\xe2\x86\xa9</a> <a href=\"#fnref-n-2\" "
      "class=\"footnote-backref\" data-footnote-backref "
      "data-footnote-backref-idx=\"1-2\" aria-label=\"Back to reference "
      "1-2\">\xe2\x86\xa9<sup class=\"footnote-ref\">2</sup></a> <a "
      "href=\"#fnref-n-3\" class=\"footnote-backref\" data-footnote-backref "
      "data-footnote-backref-idx=\"1-3\" aria-label=\"Back to reference "
      "1-3\">\xe2\x86\xa9<sup class=\"footnote-ref\">3</sup></a></p>\n"
      "</li>\n"
      "</ol>\n"
      "</section>\n");
}

TEST(footnote_unreferenced_definition_omitted) {
  const auto result{
      sourcemeta::core::markdown_to_html("body\n\n[^orphan]: unused")};
  EXPECT_EQ(result, "<p>body</p>\n");
}

TEST(footnote_reference_without_definition) {
  const auto result{sourcemeta::core::markdown_to_html("body[^ghost]")};
  EXPECT_EQ(result, "<p>body[^ghost]</p>\n");
}

TEST(footnote_label_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^LABEL]\n\n[^label]: text")};
  EXPECT_EQ(result,
            "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-label\" "
            "id=\"fnref-label\" data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-label\">\n"
            "<p>text <a href=\"#fnref-label\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_definition_with_multiple_blocks) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^m]\n\n[^m]: first\n\n    > quoted\n\n    * item")};
  EXPECT_EQ(result, "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-m\" "
                    "id=\"fnref-m\" data-footnote-ref>1</a></sup></p>\n"
                    "<section class=\"footnotes\" data-footnotes>\n"
                    "<ol>\n"
                    "<li id=\"fn-m\">\n"
                    "<p>first</p>\n"
                    "<blockquote>\n"
                    "<p>quoted</p>\n"
                    "</blockquote>\n"
                    "<ul>\n"
                    "<li>item</li>\n"
                    "</ul>\n"
                    "<a href=\"#fnref-m\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"1\" "
                    "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</section>\n");
}

TEST(footnote_numbering_follows_references) {
  const auto result{sourcemeta::core::markdown_to_html(
      "x[^z] y[^y] z[^x]\n\n[^x]: X\n[^y]: Y\n[^z]: Z")};
  EXPECT_EQ(result,
            "<p>x<sup class=\"footnote-ref\"><a href=\"#fn-z\" id=\"fnref-z\" "
            "data-footnote-ref>1</a></sup> y<sup class=\"footnote-ref\"><a "
            "href=\"#fn-y\" id=\"fnref-y\" data-footnote-ref>2</a></sup> z<sup "
            "class=\"footnote-ref\"><a href=\"#fn-x\" id=\"fnref-x\" "
            "data-footnote-ref>3</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-z\">\n"
            "<p>Z <a href=\"#fnref-z\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-y\">\n"
            "<p>Y <a href=\"#fnref-y\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"2\" "
            "aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-x\">\n"
            "<p>X <a href=\"#fnref-x\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"3\" "
            "aria-label=\"Back to reference 3\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_label_with_special_characters) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^a&b<c>]\n\n[^a&b<c>]: text")};
  EXPECT_EQ(
      result,
      "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-a&amp;b%3Cc%3E\" "
      "id=\"fnref-a&amp;b%3Cc%3E\" data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n"
      "<ol>\n"
      "<li id=\"fn-a&amp;b%3Cc%3E\">\n"
      "<p>text <a href=\"#fnref-a&amp;b%3Cc%3E\" class=\"footnote-backref\" "
      "data-footnote-backref data-footnote-backref-idx=\"1\" aria-label=\"Back "
      "to reference 1\">\xe2\x86\xa9</a></p>\n"
      "</li>\n"
      "</ol>\n"
      "</section>\n");
}

TEST(footnote_reference_in_table_cell) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| h |\n| - |\n| c[^t] |\n\n[^t]: table note")};
  EXPECT_EQ(result,
            "<table>\n"
            "<thead>\n"
            "<tr>\n"
            "<th>h</th>\n"
            "</tr>\n"
            "</thead>\n"
            "<tbody>\n"
            "<tr>\n"
            "<td>c<sup class=\"footnote-ref\"><a href=\"#fn-t\" id=\"fnref-t\" "
            "data-footnote-ref>1</a></sup></td>\n"
            "</tr>\n"
            "</tbody>\n"
            "</table>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-t\">\n"
            "<p>table note <a href=\"#fnref-t\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_reference_inside_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^a]\n\n[^a]: refers[^b]\n\n[^b]: nested")};
  EXPECT_EQ(result, "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-a\" "
                    "id=\"fnref-a\" data-footnote-ref>1</a></sup></p>\n"
                    "<section class=\"footnotes\" data-footnotes>\n"
                    "<ol>\n"
                    "<li id=\"fn-a\">\n"
                    "<p>refers<sup class=\"footnote-ref\"><a href=\"#fn-b\" "
                    "id=\"fnref-b\" data-footnote-ref>2</a></sup> <a "
                    "href=\"#fnref-a\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"1\" "
                    "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
                    "</li>\n"
                    "<li id=\"fn-b\">\n"
                    "<p>nested <a href=\"#fnref-b\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"2\" "
                    "aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</section>\n");
}

TEST(footnote_definition_inside_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^q]\n\n> [^q]: inside quote")};
  EXPECT_EQ(result,
            "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-q\" "
            "id=\"fnref-q\" data-footnote-ref>1</a></sup></p>\n"
            "<blockquote>\n"
            "</blockquote>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-q\">\n"
            "<p>inside quote <a href=\"#fnref-q\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_label_with_space_is_not_footnote) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^two words]\n\n[^two words]: text")};
  EXPECT_EQ(result, "<p>body<a href=\"text\">^two words</a></p>\n");
}
