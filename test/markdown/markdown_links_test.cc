#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(inline_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[click here](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">click here</a></p>\n");
}

TEST(inline_link_with_title) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click](https://example.com \"My Title\")")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com\" title=\"My Title\">click</a></p>\n");
}

TEST(reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click][ref]\n\n[ref]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">click</a></p>\n");
}

TEST(reference_link_with_title) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click][ref]\n\n[ref]: https://example.com \"Title\"")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com\" title=\"Title\">click</a></p>\n");
}

TEST(collapsed_reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[example][]\n\n[example]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">example</a></p>\n");
}

TEST(shortcut_reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[example]\n\n[example]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">example</a></p>\n");
}

TEST(link_with_emphasis_inside) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[**bold link**](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\"><strong>bold "
                    "link</strong></a></p>\n");
}

TEST(code_inside_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[`code`](https://example.com)")};
  EXPECT_EQ(result,
            "<p><a href=\"https://example.com\"><code>code</code></a></p>\n");
}

TEST(link_with_special_chars_in_url) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[search](https://example.com/q?a=1&b=2)")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com/q?a=1&amp;b=2\">search</a></p>\n");
}

TEST(link_reference_definition_title_on_next_line) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]: /manual\n\"The manual\"\n\n[docs]")};
  EXPECT_EQ(result,
            "<p><a href=\"/manual\" title=\"The manual\">docs</a></p>\n");
}

TEST(link_reference_definition_spread_over_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "  [docs]:\n    /manual\n     (Manual)  \n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"/manual\" title=\"Manual\">docs</a></p>\n");
}

TEST(link_reference_definition_multiline_title) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]: /manual \"\nfirst\nsecond\n\"\n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"/manual\" title=\"\n"
                    "first\n"
                    "second\n"
                    "\">docs</a></p>\n");
}

TEST(link_reference_definition_title_with_blank_line_is_invalid) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]: /manual \"first\n\nsecond\"\n\n[docs]")};
  EXPECT_EQ(result, "<p>[docs]: /manual &quot;first</p>\n"
                    "<p>second&quot;</p>\n"
                    "<p>[docs]</p>\n");
}

TEST(link_reference_definition_destination_on_next_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]:\n  /manual\n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">docs</a></p>\n");
}

TEST(link_reference_definition_missing_destination) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]:\n\ntext [docs]")};
  EXPECT_EQ(result, "<p>[docs]:</p>\n"
                    "<p>text [docs]</p>\n");
}

TEST(link_reference_definition_empty_angle_destination) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]: <>\n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"\">docs</a></p>\n");
}

TEST(link_reference_definition_title_without_separating_space) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]: /manual\"title\"\n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"/manual%22title%22\">docs</a></p>\n");
}

TEST(link_reference_definition_backslash_escapes) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]: /a\\b\\#c 'it\\'s'\n\n[docs]")};
  EXPECT_EQ(result, "<p><a href=\"/a%5Cb#c\" title=\"it's\">docs</a></p>\n");
}

TEST(link_reference_definition_after_its_use) {
  const auto result{
      sourcemeta::core::markdown_to_html("See [docs].\n\n[docs]: /manual")};
  EXPECT_EQ(result, "<p>See <a href=\"/manual\">docs</a>.</p>\n");
}

TEST(link_reference_definition_first_one_wins) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]\n\n[docs]: /one\n\n[docs]: /two")};
  EXPECT_EQ(result, "<p><a href=\"/one\">docs</a></p>\n");
}

TEST(link_reference_definition_label_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("[DoCs]: /manual\n\n[dOcS]")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">dOcS</a></p>\n");
}

TEST(link_reference_definition_label_full_case_folding) {
  const auto result{
      sourcemeta::core::markdown_to_html("[STRASSE]: /road\n\n[straße]")};
  EXPECT_EQ(result, "<p><a href=\"/road\">stra\xc3\x9f"
                    "e</a></p>\n");
}

TEST(link_reference_definition_alone_renders_nothing) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]: /manual 'Manual'")};
  EXPECT_EQ(result, "");
}

TEST(link_reference_definition_with_trailing_text_is_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]: /manual 'Manual' trailing")};
  EXPECT_EQ(result, "<p>[docs]: /manual 'Manual' trailing</p>\n");
}

TEST(link_reference_definition_title_with_trailing_text_on_next_line) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]: /manual\n'Manual' trailing\n\n[docs]")};
  EXPECT_EQ(result, "<p>'Manual' trailing</p>\n"
                    "<p><a href=\"/manual\" title=\"Manual\">docs</a></p>\n");
}

TEST(link_reference_definition_indented_four_spaces_is_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("    [docs]: /manual\n\n[docs]")};
  EXPECT_EQ(result, "<pre><code>[docs]: /manual\n"
                    "</code></pre>\n"
                    "<p>[docs]</p>\n");
}

TEST(link_reference_definition_inside_fenced_code_is_content) {
  const auto result{sourcemeta::core::markdown_to_html(
      "~~~\n[docs]: /manual\n~~~\n\n[docs]")};
  EXPECT_EQ(result, "<pre><code>[docs]: /manual\n"
                    "</code></pre>\n"
                    "<p>[docs]</p>\n");
}

TEST(link_reference_definition_cannot_interrupt_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("Intro\n[docs]: /manual\n\n[docs]")};
  EXPECT_EQ(result, "<p>Intro\n"
                    "[docs]: /manual</p>\n"
                    "<p>[docs]</p>\n");
}

TEST(link_reference_definition_between_heading_and_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html(
      "## [Docs]\n[docs]: /manual\n> quoted")};
  EXPECT_EQ(result, "<h2><a href=\"/manual\">Docs</a></h2>\n"
                    "<blockquote>\n"
                    "<p>quoted</p>\n"
                    "</blockquote>\n");
}

TEST(link_reference_definition_followed_by_dashes) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]: /manual\n---\n[docs]")};
  EXPECT_EQ(result, "<p>---\n"
                    "<a href=\"/manual\">docs</a></p>\n");
}

TEST(link_reference_definitions_consecutive) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[one]: /1 'One'\n[two]: /2\n  'Two'\n[three]: </3>\n\n[one] [two] "
      "[three]")};
  EXPECT_EQ(result, "<p><a href=\"/1\" title=\"One\">one</a> <a href=\"/2\" "
                    "title=\"Two\">two</a> <a href=\"/3\">three</a></p>\n");
}

TEST(link_reference_definition_inside_blockquote_is_global) {
  const auto result{
      sourcemeta::core::markdown_to_html("> [docs]: /manual\n\n[docs]")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "</blockquote>\n"
                    "<p><a href=\"/manual\">docs</a></p>\n");
}

TEST(link_reference_definition_label_whitespace_normalized) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[the   long\n label]: /manual\n\n[The long label]")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">The long label</a></p>\n");
}

TEST(link_reference_definition_empty_label_is_invalid) {
  const auto result{
      sourcemeta::core::markdown_to_html("[]: /manual\n\n[] text")};
  EXPECT_EQ(result, "<p>[]: /manual</p>\n"
                    "<p>[] text</p>\n");
}

TEST(link_reference_definition_whitespace_label_is_invalid) {
  const auto result{
      sourcemeta::core::markdown_to_html("[  ]: /manual\n\n[  ]")};
  EXPECT_EQ(result, "<p>[  ]: /manual</p>\n"
                    "<p>[  ]</p>\n");
}

TEST(link_destination_single_space) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]( )")};
  EXPECT_EQ(result, "<p><a href=\"\">docs</a></p>\n");
}

TEST(link_destination_empty_angle_brackets_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]( <> )")};
  EXPECT_EQ(result, "<p><a href=\"\">docs</a></p>\n");
}

TEST(link_destination_angle_brackets_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a b c>)")};
  EXPECT_EQ(result, "<p><a href=\"a%20b%20c\">docs</a></p>\n");
}

TEST(link_destination_with_space_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/a b)")};
  EXPECT_EQ(result, "<p>[docs](/a b)</p>\n");
}

TEST(link_destination_with_newline_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/a\n/b)")};
  EXPECT_EQ(result, "<p>[docs](/a\n"
                    "/b)</p>\n");
}

TEST(link_destination_angle_brackets_with_newline_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](</a\nb>)")};
  EXPECT_EQ(result, "<p>[docs](&lt;/a\n"
                    "b&gt;)</p>\n");
}

TEST(link_destination_angle_brackets_with_closing_paren) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a)b>)")};
  EXPECT_EQ(result, "<p><a href=\"a)b\">docs</a></p>\n");
}

TEST(link_destination_unmatched_angle_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a\\>)")};
  EXPECT_EQ(result, "<p>[docs](&lt;a&gt;)</p>\n");
}

TEST(link_destination_invalid_angle_bracket_forms) {
  const auto result{sourcemeta::core::markdown_to_html("[x](<y)z\n[x](<y>z)")};
  EXPECT_EQ(result, "<p>[x](&lt;y)z\n"
                    "[x](<!-- raw HTML omitted -->z)</p>\n");
}

TEST(link_destination_escaped_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](\\(a\\)b)")};
  EXPECT_EQ(result, "<p><a href=\"(a)b\">docs</a></p>\n");
}

TEST(link_destination_balanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a(b(c)d)e)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c)d)e\">docs</a></p>\n");
}

TEST(link_destination_unbalanced_parentheses_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a(b(c)d)")};
  EXPECT_EQ(result, "<p>[docs](a(b(c)d)</p>\n");
}

TEST(link_destination_escaped_unbalanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\(b\\(c)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c\">docs</a></p>\n");
}

TEST(link_destination_angle_brackets_unbalanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a(b(c>)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c\">docs</a></p>\n");
}

TEST(link_destination_backslash_escapes) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\]\\!)")};
  EXPECT_EQ(result, "<p><a href=\"a%5D!\">docs</a></p>\n");
}

TEST(link_destination_fragments_and_queries) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[x](#top)\n\n[y](https://sourcemeta.com/?q=1#top)")};
  EXPECT_EQ(result,
            "<p><a href=\"#top\">x</a></p>\n"
            "<p><a href=\"https://sourcemeta.com/?q=1#top\">y</a></p>\n");
}

TEST(link_destination_backslash_before_letter) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\zb)")};
  EXPECT_EQ(result, "<p><a href=\"a%5Czb\">docs</a></p>\n");
}

TEST(link_destination_percent_encoding_and_entities) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](caf%C3%A9&eacute;)")};
  EXPECT_EQ(result, "<p><a href=\"caf%C3%A9%C3%A9\">docs</a></p>\n");
}

TEST(link_destination_that_looks_like_title) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]('Title')")};
  EXPECT_EQ(result, "<p><a href=\"&#x27;Title&#x27;\">docs</a></p>\n");
}

TEST(link_title_quote_styles) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a](/x 'one') [b](/y (two)) [c](/z \"three\")")};
  EXPECT_EQ(result,
            "<p><a href=\"/x\" title=\"one\">a</a> <a href=\"/y\" "
            "title=\"two\">b</a> <a href=\"/z\" title=\"three\">c</a></p>\n");
}

TEST(link_title_escaped_quotes_and_entities) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/x 'it\\'s &amp; more')")};
  EXPECT_EQ(result,
            "<p><a href=\"/x\" title=\"it's &amp; more\">docs</a></p>\n");
}

TEST(link_title_separated_by_non_breaking_space) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/x\xC2\xA0'title')")};
  EXPECT_EQ(result, "<p><a href=\"/x%C2%A0&#x27;title&#x27;\">docs</a></p>\n");
}

TEST(link_title_with_unescaped_inner_quote_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/x 'a'b')")};
  EXPECT_EQ(result, "<p>[docs](/x 'a'b')</p>\n");
}

TEST(link_title_parentheses_with_quotes_inside) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs](/x (with 'single' and \"double\"))")};
  EXPECT_EQ(result, "<p><a href=\"/x\" title=\"with 'single' and "
                    "&quot;double&quot;\">docs</a></p>\n");
}

TEST(link_whitespace_around_destination_and_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](\n  /x\n  'title'\n)")};
  EXPECT_EQ(result, "<p><a href=\"/x\" title=\"title\">docs</a></p>\n");
}

TEST(link_spaces_before_parentheses_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]  (/x)")};
  EXPECT_EQ(result, "<p>[docs]  (/x)</p>\n");
}

TEST(link_text_with_balanced_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("[a [b] [c [d]]](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a [b] [c [d]]</a></p>\n");
}

TEST(link_text_with_unbalanced_closing_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a] b](/x)")};
  EXPECT_EQ(result, "<p>[a] b](/x)</p>\n");
}

TEST(link_text_with_unbalanced_opening_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a [b](/x)")};
  EXPECT_EQ(result, "<p>[a <a href=\"/x\">b</a></p>\n");
}

TEST(link_text_with_escaped_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a \\] b](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a ] b</a></p>\n");
}

TEST(link_text_with_inline_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("[_a_ __b__ `c` ~~d~~](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\"><em>a</em> <strong>b</strong> "
                    "<code>c</code> <del>d</del></a></p>\n");
}

TEST(link_text_with_image) {
  const auto result{
      sourcemeta::core::markdown_to_html("[![logo](/logo.png 'Logo')](/home)")};
  EXPECT_EQ(result, "<p><a href=\"/home\"><img src=\"/logo.png\" alt=\"logo\" "
                    "title=\"Logo\" /></a></p>\n");
}

TEST(link_inside_link_text_is_not_allowed) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a [b](/inner) c](/outer)")};
  EXPECT_EQ(result, "<p>[a <a href=\"/inner\">b</a> c](/outer)</p>\n");
}

TEST(link_inside_emphasis_inside_link_text) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a _[b](/inner)_](/outer)")};
  EXPECT_EQ(result, "<p>[a <em><a href=\"/inner\">b</a></em>](/outer)</p>\n");
}

TEST(link_text_brackets_take_precedence_over_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("_[a_](/x)")};
  EXPECT_EQ(result, "<p>_<a href=\"/x\">a_</a></p>\n");
}

TEST(link_brackets_take_precedence_over_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("[a _b](/x_)")};
  EXPECT_EQ(result, "<p><a href=\"/x_\">a _b</a></p>\n");
}

TEST(link_code_span_takes_precedence) {
  const auto result{sourcemeta::core::markdown_to_html("[a`](/x)`")};
  EXPECT_EQ(result, "<p>[a<code>](/x)</code></p>\n");
}

TEST(link_autolink_takes_precedence) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a<https://sourcemeta.com/?q=](/x)>")};
  EXPECT_EQ(result, "<p>[a<a "
                    "href=\"https://sourcemeta.com/?q=%5D(/x)\">https://"
                    "sourcemeta.com/?q=](/x)</a></p>\n");
}

TEST(reference_link_full) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[the docs][manual]\n\n[manual]: /manual 'Manual'")};
  EXPECT_EQ(result,
            "<p><a href=\"/manual\" title=\"Manual\">the docs</a></p>\n");
}

TEST(reference_link_text_with_nested_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a [b] c][ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p><a href=\"/ref\">a [b] c</a></p>\n");
}

TEST(reference_link_label_case_folding) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[text][MaNuAl]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">text</a></p>\n");
}

TEST(reference_link_label_titlecase_digraph_folding) {
  const auto result{sourcemeta::core::markdown_to_html("[ǅ]\n\n[ǆ]: /digraph")};
  EXPECT_EQ(result, "<p><a href=\"/digraph\">\xc7\x85</a></p>\n");
}

TEST(reference_link_space_between_labels_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a] [ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p>[a] <a href=\"/ref\">ref</a></p>\n");
}

TEST(reference_link_newline_between_labels_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a]\n[ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p>[a]\n"
                    "<a href=\"/ref\">ref</a></p>\n");
}

TEST(reference_link_uses_first_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a][ref]\n\n[ref]: /first\n[ref]: /second")};
  EXPECT_EQ(result, "<p><a href=\"/first\">a</a></p>\n");
}

TEST(reference_link_label_escapes_are_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][ref\\*]\n\n[ref*]: /ref")};
  EXPECT_EQ(result, "<p>[a][ref*]</p>\n");
}

TEST(reference_link_label_with_unescaped_bracket_is_invalid) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b]c]\n\n[b]c]: /x")};
  EXPECT_EQ(result, "<p>[a][b]c]</p>\n"
                    "<p>[b]c]: /x</p>\n");
}

TEST(reference_link_label_with_escaped_bracket) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b\\]c]\n\n[b\\]c]: /x")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a</a></p>\n");
}

TEST(reference_link_label_with_escaped_backslash) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a\\\\b]: /x\n\n[a\\\\b]")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a\\b</a></p>\n");
}

TEST(reference_link_empty_brackets_are_literal) {
  const auto result{sourcemeta::core::markdown_to_html("[][]\n\n[]: /x")};
  EXPECT_EQ(result, "<p>[][]</p>\n"
                    "<p>[]: /x</p>\n");
}

TEST(collapsed_reference_link_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("[MANUAL][]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">MANUAL</a></p>\n");
}

TEST(collapsed_reference_link_across_lines_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual]\n[]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">manual</a>\n"
                    "[]</p>\n");
}

TEST(shortcut_reference_link_with_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[_the_ manual]\n\n[_the_ manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\"><em>the</em> manual</a></p>\n");
}

TEST(shortcut_reference_link_escaped) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\[manual\\]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p>[manual]</p>\n");
}

TEST(shortcut_reference_link_label_with_underscore) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual_]: /manual\n\n_[manual_]")};
  EXPECT_EQ(result, "<p>_<a href=\"/manual\">manual_</a></p>\n");
}

TEST(reference_link_full_takes_precedence_over_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b][c]\n\n[b]: /b\n[c]: /c")};
  EXPECT_EQ(result, "<p><a href=\"/b\">a</a><a href=\"/c\">c</a></p>\n");
}

TEST(reference_link_undefined_label_falls_back) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b][c]\n\n[a]: /a\n[c]: /c")};
  EXPECT_EQ(result, "<p>[a]<a href=\"/c\">b</a></p>\n");
}

TEST(inline_link_takes_precedence_over_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual]()\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"\">manual</a></p>\n");
}

TEST(inline_link_invalid_falls_back_to_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual](a b)\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">manual</a>(a b)</p>\n");
}
