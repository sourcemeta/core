#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(tab_indented_code_block) {
  const auto result{sourcemeta::core::markdown_to_html("\tlime\tpear\t\tplum")};
  EXPECT_EQ(result, "<pre><code>lime\tpear\t\tplum\n"
                    "</code></pre>\n");
}

TEST(tab_after_space_indented_code_block) {
  const auto result{sourcemeta::core::markdown_to_html(" \tlime\tpear")};
  EXPECT_EQ(result, "<pre><code>lime\tpear\n"
                    "</code></pre>\n");
}

TEST(tab_list_item_continuation) {
  const auto result{sourcemeta::core::markdown_to_html(" - kiwi\n\n\tmango")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>kiwi</p>\n"
                    "<p>mango</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(tab_list_item_indented_code) {
  const auto result{sourcemeta::core::markdown_to_html("* kiwi\n\n\t\tmango")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>kiwi</p>\n"
                    "<pre><code>  mango\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(tab_after_blockquote_marker) {
  const auto result{sourcemeta::core::markdown_to_html(">\t\tcedar\n>\t\toak")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>  cedar\n"
                    "  oak\n"
                    "</code></pre>\n"
                    "</blockquote>\n");
}

TEST(tab_after_list_marker) {
  const auto result{sourcemeta::core::markdown_to_html("+\t\tcedar")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>  cedar\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(tab_after_heading_marker) {
  const auto result{sourcemeta::core::markdown_to_html("##\tTitle")};
  EXPECT_EQ(result, "<h2>Title</h2>\n");
}

TEST(tab_between_thematic_break_characters) {
  const auto result{sourcemeta::core::markdown_to_html("-\t-\t-")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(list_marker_takes_precedence_over_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("* `alpha\n* beta`")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>`alpha</li>\n"
                    "<li>beta`</li>\n"
                    "</ul>\n");
}

TEST(thematic_break_with_trailing_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("***\t  ")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_plus_signs_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("++++")};
  EXPECT_EQ(result, "<p>++++</p>\n");
}

TEST(thematic_break_equals_signs_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("= = = =")};
  EXPECT_EQ(result, "<p>= = = =</p>\n");
}

TEST(thematic_break_two_characters_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("**\n\n__")};
  EXPECT_EQ(result, "<p>**</p>\n"
                    "<p>__</p>\n");
}

TEST(thematic_break_indented_one_to_three_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("   ___\n  ---\n ***")};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n"
                    "<hr />\n");
}

TEST(thematic_break_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    ---")};
  EXPECT_EQ(result, "<pre><code>---\n"
                    "</code></pre>\n");
}

TEST(thematic_break_indented_four_spaces_continues_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("Paragraph text\n    ___")};
  EXPECT_EQ(result, "<p>Paragraph text\n"
                    "___</p>\n");
}

TEST(thematic_break_many_characters) {
  const auto result{sourcemeta::core::markdown_to_html(
      "-------------------------------------------------")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_spaces_between_characters) {
  const auto result{sourcemeta::core::markdown_to_html("* * * *")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_with_other_characters_is_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("- - - x\n\nx______\n\n***x***")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>x</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "<p>x______</p>\n"
                    "<p><em><strong>x</strong></em></p>\n");
}

TEST(thematic_break_mixed_characters_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("-*-")};
  EXPECT_EQ(result, "<p>-*-</p>\n");
}

TEST(thematic_break_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("alpha\n___\nbeta")};
  EXPECT_EQ(result, "<p>alpha</p>\n"
                    "<hr />\n"
                    "<p>beta</p>\n");
}

TEST(dashes_after_paragraph_make_setext_heading) {
  const auto result{sourcemeta::core::markdown_to_html("alpha\n-----\nbeta")};
  EXPECT_EQ(result, "<h2>alpha</h2>\n"
                    "<p>beta</p>\n");
}

TEST(thematic_break_takes_precedence_over_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("- one\n- - -\n- two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<hr />\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(thematic_break_inside_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n* - - -")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>\n"
                    "<hr />\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(atx_heading_eight_hashes_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("######## title")};
  EXPECT_EQ(result, "<p>######## title</p>\n");
}

TEST(atx_heading_without_space_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("#1 fan\n\n##tag")};
  EXPECT_EQ(result, "<p>#1 fan</p>\n"
                    "<p>##tag</p>\n");
}

TEST(atx_heading_escaped_hash_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("\\# title")};
  EXPECT_EQ(result, "<p># title</p>\n");
}

TEST(atx_heading_inline_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("## plain *em* \\*raw\\*")};
  EXPECT_EQ(result, "<h2>plain <em>em</em> *raw*</h2>\n");
}

TEST(atx_heading_surrounding_spaces_ignored) {
  const auto result{
      sourcemeta::core::markdown_to_html("###      spaced out      ")};
  EXPECT_EQ(result, "<h3>spaced out</h3>\n");
}

TEST(atx_heading_indented_up_to_three_spaces) {
  const auto result{
      sourcemeta::core::markdown_to_html("   #### deep\n  # shallow")};
  EXPECT_EQ(result, "<h4>deep</h4>\n"
                    "<h1>shallow</h1>\n");
}

TEST(atx_heading_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    ## code")};
  EXPECT_EQ(result, "<pre><code>## code\n"
                    "</code></pre>\n");
}

TEST(atx_heading_indented_four_spaces_continues_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("text\n    ## more")};
  EXPECT_EQ(result, "<p>text\n"
                    "## more</p>\n");
}

TEST(atx_heading_closing_sequence_of_any_length) {
  const auto result{
      sourcemeta::core::markdown_to_html("## title #######\n#### title #")};
  EXPECT_EQ(result, "<h2>title</h2>\n"
                    "<h4>title</h4>\n");
}

TEST(atx_heading_closing_sequence_followed_by_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("## title ##   ")};
  EXPECT_EQ(result, "<h2>title</h2>\n");
}

TEST(atx_heading_hashes_followed_by_text_are_content) {
  const auto result{sourcemeta::core::markdown_to_html("## title ## tail")};
  EXPECT_EQ(result, "<h2>title ## tail</h2>\n");
}

TEST(atx_heading_closing_sequence_needs_preceding_space) {
  const auto result{sourcemeta::core::markdown_to_html("## title##")};
  EXPECT_EQ(result, "<h2>title##</h2>\n");
}

TEST(atx_heading_escaped_closing_hashes) {
  const auto result{
      sourcemeta::core::markdown_to_html("## title \\##\n# title #\\#")};
  EXPECT_EQ(result, "<h2>title ##</h2>\n"
                    "<h1>title ##</h1>\n");
}

TEST(atx_heading_between_thematic_breaks) {
  const auto result{sourcemeta::core::markdown_to_html("---\n# title\n---")};
  EXPECT_EQ(result, "<hr />\n"
                    "<h1>title</h1>\n"
                    "<hr />\n");
}

TEST(atx_heading_interrupts_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("one line\n## heading\nnext line")};
  EXPECT_EQ(result, "<p>one line</p>\n"
                    "<h2>heading</h2>\n"
                    "<p>next line</p>\n");
}

TEST(atx_heading_empty) {
  const auto result{sourcemeta::core::markdown_to_html("#\n## \n#### ####")};
  EXPECT_EQ(result, "<h1></h1>\n"
                    "<h2></h2>\n"
                    "<h4></h4>\n");
}

TEST(setext_heading_single_character_underline) {
  const auto result{sourcemeta::core::markdown_to_html("Title *em*\n=")};
  EXPECT_EQ(result, "<h1>Title <em>em</em></h1>\n");
}

TEST(setext_heading_multiline_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("First *line\nsecond* line\n---")};
  EXPECT_EQ(result, "<h2>First <em>line\n"
                    "second</em> line</h2>\n");
}

TEST(setext_heading_content_surrounding_spaces_stripped) {
  const auto result{
      sourcemeta::core::markdown_to_html(" Title *em\ntext*  \n===")};
  EXPECT_EQ(result, "<h1>Title <em>em\n"
                    "text</em></h1>\n");
}

TEST(setext_heading_indented_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("  Title\n  ===\n\n   Other\n-")};
  EXPECT_EQ(result, "<h1>Title</h1>\n"
                    "<h2>Other</h2>\n");
}

TEST(setext_heading_content_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    Title\n    ===")};
  EXPECT_EQ(result, "<pre><code>Title\n"
                    "===\n"
                    "</code></pre>\n");
}

TEST(setext_heading_underline_indented_up_to_three_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("Title\n  ===   ")};
  EXPECT_EQ(result, "<h1>Title</h1>\n");
}

TEST(setext_heading_underline_indented_four_spaces_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Title\n    ===")};
  EXPECT_EQ(result, "<p>Title\n"
                    "===</p>\n");
}

TEST(setext_heading_underline_with_internal_space_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Title\n== ==")};
  EXPECT_EQ(result, "<p>Title\n"
                    "== ==</p>\n");
}

TEST(setext_heading_trailing_spaces_are_not_hard_break) {
  const auto result{sourcemeta::core::markdown_to_html("Title   \n===")};
  EXPECT_EQ(result, "<h1>Title</h1>\n");
}

TEST(setext_heading_trailing_backslash_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("Title\\\n===")};
  EXPECT_EQ(result, "<h1>Title\\</h1>\n");
}

TEST(setext_heading_underline_breaks_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("`code\n===\n`")};
  EXPECT_EQ(result, "<h1>`code</h1>\n"
                    "<p>`</p>\n");
}

TEST(setext_heading_equals_underline_is_lazy_continuation) {
  const auto result{sourcemeta::core::markdown_to_html("> Title\n===")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>Title\n"
                    "===</p>\n"
                    "</blockquote>\n");
}

TEST(setext_heading_dashes_after_lazy_line_are_thematic_break) {
  const auto result{
      sourcemeta::core::markdown_to_html("> quote\ncontinued\n---")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>quote\n"
                    "continued</p>\n"
                    "</blockquote>\n"
                    "<hr />\n");
}

TEST(setext_heading_equals_after_list_item_is_lazy_continuation) {
  const auto result{sourcemeta::core::markdown_to_html("* Item\n===")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>Item\n"
                    "===</li>\n"
                    "</ul>\n");
}

TEST(setext_heading_multiline_paragraph_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("Line one\nLine two\n===")};
  EXPECT_EQ(result, "<h1>Line one\n"
                    "Line two</h1>\n");
}

TEST(setext_heading_equals_line_then_heading) {
  const auto result{
      sourcemeta::core::markdown_to_html("===\nTitle\n===\nText\n---")};
  EXPECT_EQ(result, "<h1>===\n"
                    "Title</h1>\n"
                    "<h2>Text</h2>\n");
}

TEST(setext_heading_underline_without_content_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("\n==")};
  EXPECT_EQ(result, "<p>==</p>\n");
}

TEST(setext_heading_dashes_after_thematic_break) {
  const auto result{sourcemeta::core::markdown_to_html("***\n---")};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n");
}

TEST(setext_heading_dashes_after_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("* item\n---")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>item</li>\n"
                    "</ul>\n"
                    "<hr />\n");
}

TEST(setext_heading_equals_after_indented_code) {
  const auto result{sourcemeta::core::markdown_to_html("    code\n===")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>===</p>\n");
}

TEST(setext_heading_escaped_content) {
  const auto result{sourcemeta::core::markdown_to_html("\\- not a list\n---")};
  EXPECT_EQ(result, "<h2>- not a list</h2>\n");
}

TEST(setext_heading_after_blank_line_separated_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("Intro\n\nTitle\n===\nBody")};
  EXPECT_EQ(result, "<p>Intro</p>\n"
                    "<h1>Title</h1>\n"
                    "<p>Body</p>\n");
}

TEST(setext_heading_spaced_dashes_after_blank_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("One\ntwo\n\n- - -\n\nthree")};
  EXPECT_EQ(result, "<p>One\n"
                    "two</p>\n"
                    "<hr />\n"
                    "<p>three</p>\n");
}

TEST(setext_heading_underscores_are_thematic_break) {
  const auto result{sourcemeta::core::markdown_to_html("One\ntwo\n___\nthree")};
  EXPECT_EQ(result, "<p>One\n"
                    "two</p>\n"
                    "<hr />\n"
                    "<p>three</p>\n");
}

TEST(setext_heading_escaped_equals_are_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("One\ntwo\n\\===\nthree")};
  EXPECT_EQ(result, "<p>One\n"
                    "two\n"
                    "===\n"
                    "three</p>\n");
}

TEST(indented_code_chunks_separated_by_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "    first\n\n    second\n \n  \n    third")};
  EXPECT_EQ(result, "<pre><code>first\n"
                    "\n"
                    "second\n"
                    "\n"
                    "\n"
                    "third\n"
                    "</code></pre>\n");
}

TEST(indented_code_keeps_extra_indentation_on_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "    first\n        \n        second")};
  EXPECT_EQ(result, "<pre><code>first\n"
                    "    \n"
                    "    second\n"
                    "</code></pre>\n");
}

TEST(indented_code_cannot_interrupt_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Text\n        code")};
  EXPECT_EQ(result, "<p>Text\n"
                    "code</p>\n");
}

TEST(indented_code_ends_at_unindented_line) {
  const auto result{sourcemeta::core::markdown_to_html("    code\ntext")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>text</p>\n");
}

TEST(indented_code_between_headings) {
  const auto result{sourcemeta::core::markdown_to_html(
      "## Top\n    code\nBottom\n===\n    code\n***")};
  EXPECT_EQ(result, "<h2>Top</h2>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<h1>Bottom</h1>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<hr />\n");
}

TEST(indented_code_first_line_extra_indentation) {
  const auto result{
      sourcemeta::core::markdown_to_html("      alpha\n    beta")};
  EXPECT_EQ(result, "<pre><code>  alpha\n"
                    "beta\n"
                    "</code></pre>\n");
}

TEST(indented_code_surrounding_blank_lines_dropped) {
  const auto result{
      sourcemeta::core::markdown_to_html("    \n\n    code\n\n    \n")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "</code></pre>\n");
}

TEST(indented_code_trailing_whitespace_kept) {
  const auto result{sourcemeta::core::markdown_to_html("    code\t  ")};
  EXPECT_EQ(result, "<pre><code>code\t  \n"
                    "</code></pre>\n");
}

TEST(indented_code_content_is_not_parsed) {
  const auto result{sourcemeta::core::markdown_to_html(
      "    **bold**\n    [link](/x)\n\n    1. item")};
  EXPECT_EQ(result, "<pre><code>**bold**\n"
                    "[link](/x)\n"
                    "\n"
                    "1. item\n"
                    "</code></pre>\n");
}

TEST(fenced_code_closing_fence_longer_than_opening) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\ncode\n~~~~~")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "</code></pre>\n");
}

TEST(fenced_code_tildes_not_closed_by_backticks) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~~~\ncode\n````\n~~~~")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "````\n"
                    "</code></pre>\n");
}

TEST(fenced_code_backticks_not_closed_by_tildes) {
  const auto result{
      sourcemeta::core::markdown_to_html("````\ncode\n~~~~\n````")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "~~~~\n"
                    "</code></pre>\n");
}

TEST(fenced_code_unclosed_empty) {
  const auto result{sourcemeta::core::markdown_to_html("~~~")};
  EXPECT_EQ(result, "<pre><code></code></pre>\n");
}

TEST(fenced_code_unclosed_runs_to_end_of_document) {
  const auto result{sourcemeta::core::markdown_to_html("~~~~\n\n~~~\ntext")};
  EXPECT_EQ(result, "<pre><code>\n"
                    "~~~\n"
                    "text\n"
                    "</code></pre>\n");
}

TEST(fenced_code_unclosed_ends_with_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("> ~~~\n> code\n\ntext")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<p>text</p>\n");
}

TEST(fenced_code_only_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\n \n\n~~~")};
  EXPECT_EQ(result, "<pre><code> \n"
                    "\n"
                    "</code></pre>\n");
}

TEST(fenced_code_empty) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\n~~~")};
  EXPECT_EQ(result, "<pre><code></code></pre>\n");
}

TEST(fenced_code_indentation_removed_from_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("  ~~~\n  one\n two\nthree\n  ~~~")};
  EXPECT_EQ(result, "<pre><code>one\n"
                    "two\n"
                    "three\n"
                    "</code></pre>\n");
}

TEST(fenced_code_indentation_removed_up_to_fence_indentation) {
  const auto result{
      sourcemeta::core::markdown_to_html(" ```\n   one\n  two\n```")};
  EXPECT_EQ(result, "<pre><code>  one\n"
                    " two\n"
                    "</code></pre>\n");
}

TEST(fenced_code_fence_indented_four_spaces_is_indented_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("    ~~~\n    code\n    ~~~")};
  EXPECT_EQ(result, "<pre><code>~~~\n"
                    "code\n"
                    "~~~\n"
                    "</code></pre>\n");
}

TEST(fenced_code_closing_fence_indented_differently) {
  const auto result{sourcemeta::core::markdown_to_html("  ~~~\ncode\n   ~~~")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "</code></pre>\n");
}

TEST(fenced_code_closing_fence_indented_four_spaces_is_content) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\ncode\n    ~~~")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "    ~~~\n"
                    "</code></pre>\n");
}

TEST(fenced_code_closing_fence_with_internal_space_is_content) {
  const auto result{sourcemeta::core::markdown_to_html("```\ncode\n`` ``")};
  EXPECT_EQ(result, "<pre><code>code\n"
                    "`` ``\n"
                    "</code></pre>\n");
}

TEST(fenced_code_backtick_info_with_backticks_is_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("``` x ```\ncode")};
  EXPECT_EQ(result, "<p><code>x</code>\n"
                    "code</p>\n");
}

TEST(fenced_code_interrupts_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("text\n~~~\ncode\n~~~\nmore")};
  EXPECT_EQ(result, "<p>text</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>more</p>\n");
}

TEST(fenced_code_info_string_first_word_is_language) {
  const auto result{sourcemeta::core::markdown_to_html(
      "~~~ python extra words here\nprint(1)\n~~~")};
  EXPECT_EQ(result, "<pre lang=\"python\"><code>print(1)\n"
                    "</code></pre>\n");
}

TEST(fenced_code_info_string_of_symbols) {
  const auto result{sourcemeta::core::markdown_to_html("```#!\n```")};
  EXPECT_EQ(result, "<pre lang=\"#!\"><code></code></pre>\n");
}

TEST(fenced_code_backtick_fence_info_cannot_contain_backticks) {
  const auto result{sourcemeta::core::markdown_to_html("```js`\ncode")};
  EXPECT_EQ(result, "<p>```js`\n"
                    "code</p>\n");
}

TEST(fenced_code_tilde_fence_info_can_contain_backticks) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~~ `js` ~~~\ncode\n~~~")};
  EXPECT_EQ(result, "<pre lang=\"`js`\"><code>code\n"
                    "</code></pre>\n");
}

TEST(fenced_code_closing_fence_cannot_have_info) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~~\n~~~ closing\n~~~")};
  EXPECT_EQ(result, "<pre><code>~~~ closing\n"
                    "</code></pre>\n");
}

TEST(fenced_code_info_string_entities_decoded) {
  const auto result{
      sourcemeta::core::markdown_to_html("```c&#43;&#43;\ncode\n```")};
  EXPECT_EQ(result, "<pre lang=\"c++\"><code>code\n"
                    "</code></pre>\n");
}

TEST(fenced_code_info_string_backslash_escapes) {
  const auto result{sourcemeta::core::markdown_to_html("```c\\#\ncode\n```")};
  EXPECT_EQ(result, "<pre lang=\"c#\"><code>code\n"
                    "</code></pre>\n");
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

TEST(paragraph_leading_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html(" one\n  two")};
  EXPECT_EQ(result, "<p>one\n"
                    "two</p>\n");
}

TEST(paragraph_continuation_leading_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html(
      "one\n          two\n                     three")};
  EXPECT_EQ(result, "<p>one\n"
                    "two\n"
                    "three</p>\n");
}

TEST(paragraph_first_line_indented_three_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("  first\nsecond")};
  EXPECT_EQ(result, "<p>first\n"
                    "second</p>\n");
}

TEST(paragraph_first_line_indented_five_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("     first\nsecond")};
  EXPECT_EQ(result, "<pre><code> first\n"
                    "</code></pre>\n"
                    "<p>second</p>\n");
}

TEST(paragraph_final_trailing_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html("one   \ntwo   ")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(blank_lines_at_start_and_end_ignored) {
  const auto result{
      sourcemeta::core::markdown_to_html("\n  \none\n\n \n## two\n\n\n")};
  EXPECT_EQ(result, "<p>one</p>\n"
                    "<h2>two</h2>\n");
}

TEST(blockquote_marker_without_following_space) {
  const auto result{
      sourcemeta::core::markdown_to_html(">## Title\n>text\n> more")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<h2>Title</h2>\n"
                    "<p>text\n"
                    "more</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_marker_indented_up_to_three_spaces) {
  const auto result{
      sourcemeta::core::markdown_to_html("  > one\n > two\n   > three")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one\n"
                    "two\n"
                    "three</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_marker_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    > one\n    > two")};
  EXPECT_EQ(result, "<pre><code>&gt; one\n"
                    "&gt; two\n"
                    "</code></pre>\n");
}

TEST(blockquote_lazy_paragraph_continuation) {
  const auto result{sourcemeta::core::markdown_to_html("> one\ntwo")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_lazy_continuation_between_marked_lines) {
  const auto result{sourcemeta::core::markdown_to_html("> one\ntwo\n> three")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one\n"
                    "two\n"
                    "three</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_laziness_does_not_apply_to_thematic_break) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n***")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one</p>\n"
                    "</blockquote>\n"
                    "<hr />\n");
}

TEST(blockquote_laziness_does_not_apply_to_list) {
  const auto result{sourcemeta::core::markdown_to_html("> * one\n* two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "</blockquote>\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(blockquote_laziness_does_not_apply_to_indented_code) {
  const auto result{
      sourcemeta::core::markdown_to_html(">     code\n     more")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<pre><code> more\n"
                    "</code></pre>\n");
}

TEST(blockquote_laziness_does_not_apply_to_fenced_code) {
  const auto result{sourcemeta::core::markdown_to_html("> ~~~\ncode\n~~~")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code></code></pre>\n"
                    "</blockquote>\n"
                    "<p>code</p>\n"
                    "<pre><code></code></pre>\n");
}

TEST(blockquote_lazy_line_that_looks_like_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n    * two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one\n"
                    "* two</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_empty_indented_marker) {
  const auto result{sourcemeta::core::markdown_to_html(" >")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "</blockquote>\n");
}

TEST(blockquote_empty_multiple_lines) {
  const auto result{sourcemeta::core::markdown_to_html(">\n>\n>   ")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "</blockquote>\n");
}

TEST(blockquote_blank_first_and_last_lines) {
  const auto result{sourcemeta::core::markdown_to_html(">  \n> text\n>")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>text</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_blank_line_separates_blockquotes) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n\n> two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one</p>\n"
                    "</blockquote>\n"
                    "<blockquote>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_empty_marker_line_joins_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n> \n> two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("text\n>quote")};
  EXPECT_EQ(result, "<p>text</p>\n"
                    "<blockquote>\n"
                    "<p>quote</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_ended_by_thematic_break) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n- - -\n> two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one</p>\n"
                    "</blockquote>\n"
                    "<hr />\n"
                    "<blockquote>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n");
}

TEST(blockquote_lazy_line_after_blank_marker_line) {
  const auto result{sourcemeta::core::markdown_to_html("> one\n>\ntwo")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>one</p>\n"
                    "</blockquote>\n"
                    "<p>two</p>\n");
}

TEST(blockquote_nested_lazy_continuation) {
  const auto result{sourcemeta::core::markdown_to_html(">> one\ntwo")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(blockquote_nested_markers_without_spaces) {
  const auto result{
      sourcemeta::core::markdown_to_html(">>one\n>two\n>>>three")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "<blockquote>\n"
                    "<p>three</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(blockquote_indented_code_needs_five_spaces) {
  const auto result{
      sourcemeta::core::markdown_to_html(">      code\n\n>   text")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code> code\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<blockquote>\n"
                    "<p>text</p>\n"
                    "</blockquote>\n");
}

TEST(list_item_content_needs_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n");
}

TEST(list_item_content_with_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n  two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_wide_marker_content_not_indented_enough) {
  const auto result{sourcemeta::core::markdown_to_html("1.   one\n\n    two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "</ol>\n"
                    "<pre><code>two\n"
                    "</code></pre>\n");
}

TEST(list_item_wide_marker_content_indented_enough) {
  const auto result{sourcemeta::core::markdown_to_html("1.   one\n\n     two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_inside_nested_blockquotes) {
  const auto result{
      sourcemeta::core::markdown_to_html("> > - one\n> >\n> >   two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(list_item_inside_nested_blockquotes_not_indented_enough) {
  const auto result{
      sourcemeta::core::markdown_to_html("> > - one\n> >\n> > two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(list_item_marker_needs_following_space) {
  const auto result{sourcemeta::core::markdown_to_html("*one\n\n1)two")};
  EXPECT_EQ(result, "<p>*one</p>\n"
                    "<p>1)two</p>\n");
}

TEST(list_item_multiple_blank_lines_between_blocks) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n\n\n  two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_with_several_block_types) {
  const auto result{sourcemeta::core::markdown_to_html(
      "2. one\n\n   ~~~\n   code\n   ~~~\n\n   two\n\n   > quote")};
  EXPECT_EQ(result, "<ol start=\"2\">\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>two</p>\n"
                    "<blockquote>\n"
                    "<p>quote</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_code_block_with_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "* one\n\n      code\n\n\n      more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<pre><code>code\n"
                    "\n"
                    "\n"
                    "more\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_nine_digit_number) {
  const auto result{sourcemeta::core::markdown_to_html("999999999) max")};
  EXPECT_EQ(result, "<ol start=\"999999999\">\n"
                    "<li>max</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_ten_digit_number_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("9999999999) over")};
  EXPECT_EQ(result, "<p>9999999999) over</p>\n");
}

TEST(ordered_list_starting_at_zero) {
  const auto result{sourcemeta::core::markdown_to_html("0) zero")};
  EXPECT_EQ(result, "<ol start=\"0\">\n"
                    "<li>zero</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_number_with_leading_zeros) {
  const auto result{sourcemeta::core::markdown_to_html("0007. seven")};
  EXPECT_EQ(result, "<ol start=\"7\">\n"
                    "<li>seven</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_negative_number_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("-5) negative")};
  EXPECT_EQ(result, "<p>-5) negative</p>\n");
}

TEST(list_item_indented_code_after_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("* text\n\n      code")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>text</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_indented_code_after_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html(" 100. text\n\n          code")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>\n"
                    "<p>text</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_starting_with_indented_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("*     code\n\n  text\n\n      more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>text</p>\n"
                    "<pre><code>more\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_indented_code_and_extra_space) {
  const auto result{
      sourcemeta::core::markdown_to_html("*      code\n\n  text")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code> code\n"
                    "</code></pre>\n"
                    "<p>text</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_blank_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("*\n  one\n*\n  ~~~\n  code\n  ~~~")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_blank_line_and_trailing_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("1.  \n   one")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "</ol>\n");
}

TEST(list_item_at_most_one_starting_blank_line) {
  const auto result{sourcemeta::core::markdown_to_html("*\n\n  one")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n"
                    "<p>one</p>\n");
}

TEST(list_item_empty_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n*\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(list_item_empty_with_trailing_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n*  \n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_empty_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("1) one\n2)\n3) two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ol>\n");
}

TEST(list_consisting_of_single_empty_item) {
  const auto result{sourcemeta::core::markdown_to_html("-")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n");
}

TEST(list_item_empty_cannot_interrupt_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("text\n-\n\ntext\n1)")};
  EXPECT_EQ(result, "<h2>text</h2>\n"
                    "<p>text\n"
                    "1)</p>\n");
}

TEST(list_item_indented_one_space) {
  const auto result{sourcemeta::core::markdown_to_html(
      "  1. one\n     two\n\n         code")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    - one\n      two")};
  EXPECT_EQ(result, "<pre><code>- one\n"
                    "  two\n"
                    "</code></pre>\n");
}

TEST(list_item_lazy_continuation_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html(" * one\ntwo\n\n       code")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_partially_lazy_continuation) {
  const auto result{sourcemeta::core::markdown_to_html(" * one\n  two\nthree")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one\n"
                    "two\n"
                    "three</li>\n"
                    "</ul>\n");
}

TEST(list_item_laziness_inside_nested_containers) {
  const auto result{
      sourcemeta::core::markdown_to_html("> * > quoted\nlazy line")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<blockquote>\n"
                    "<p>quoted\n"
                    "lazy line</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</blockquote>\n");
}

TEST(list_nested_with_sufficient_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n  * b\n    * c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>b\n"
                    "<ul>\n"
                    "<li>c</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_not_nested_with_one_space_increments) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n * b\n  * c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_nested_needs_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("100. a\n     * b")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_not_nested_with_insufficient_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("100. a\n    * b")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>a\n"
                    "* b</li>\n"
                    "</ol>\n");
}

TEST(list_item_containing_only_list) {
  const auto result{sourcemeta::core::markdown_to_html("+ + one")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_containing_nested_ordered_list) {
  const auto result{sourcemeta::core::markdown_to_html("3) * 4) deep")};
  EXPECT_EQ(result, "<ol start=\"3\">\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<ol start=\"4\">\n"
                    "<li>deep</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_containing_headings) {
  const auto result{
      sourcemeta::core::markdown_to_html("* ## Title\n* Text\n  ===\n  more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<h2>Title</h2>\n"
                    "</li>\n"
                    "<li>\n"
                    "<h1>Text</h1>\n"
                    "more</li>\n"
                    "</ul>\n");
}

TEST(list_changing_bullet_character_starts_new_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n* two\n- three")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ul>\n"
                    "<ul>\n"
                    "<li>three</li>\n"
                    "</ul>\n");
}

TEST(list_changing_delimiter_starts_new_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) one\n2) two\n3. three")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ol>\n"
                    "<ol start=\"3\">\n"
                    "<li>three</li>\n"
                    "</ol>\n");
}

TEST(list_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Text\n* one\n* two")};
  EXPECT_EQ(result, "<p>Text</p>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_not_starting_at_one_cannot_interrupt_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("We counted\n42. items in total")};
  EXPECT_EQ(result, "<p>We counted\n"
                    "42. items in total</p>\n");
}

TEST(ordered_list_starting_at_one_interrupts_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("We counted\n1. items in total")};
  EXPECT_EQ(result, "<p>We counted</p>\n"
                    "<ol>\n"
                    "<li>items in total</li>\n"
                    "</ol>\n");
}

TEST(list_items_separated_by_multiple_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_nested_item_with_blank_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  * b\n\n\n    c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_separated_by_html_comment) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n\n<!-- split -->\n\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<!-- raw HTML omitted -->\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(list_items_with_varying_marker_indentation) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n * b\n  * c\n * d\n* e")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "<li>d</li>\n"
                    "<li>e</li>\n"
                    "</ul>\n");
}

TEST(list_item_indented_four_spaces_continues_previous_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n * b\n  * c\n    * d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c\n"
                    "<ul>\n"
                    "<li>d</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_indented_five_spaces_is_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) a\n\n 2) b\n\n     3) c")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<ol start=\"3\">\n"
                    "<li>c</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_loose_with_blank_line_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n\n* b\n* c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_empty_item) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n-\n\n- b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li></li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_blank_line_inside_item) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n\n  b\n* c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_blank_line_before_reference_definition) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n\n  [r]: /r\n* b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blank_lines_inside_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n* ~~~\n  x\n\n  ~~~\n* b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>\n"
                    "<pre><code>x\n"
                    "\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>b</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_loose_sublist) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  * b\n\n  * c\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blank_blockquote_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("- a\n  > b\n  >\n  > c\n- d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<blockquote>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blockquote_and_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  > b\n  ~~~\n  c\n  ~~~\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<blockquote>\n"
                    "<p>b</p>\n"
                    "</blockquote>\n"
                    "<pre><code>c\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_single_tight_nested_ordered_item) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n   1. b")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>a\n"
                    "<ol>\n"
                    "<li>b</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_loose_single_item_with_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("* ~~~\n  x\n  ~~~\n\n  y")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>x\n"
                    "</code></pre>\n"
                    "<p>y</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_outer_loose_inner_tight) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n   - b\n\n   c")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_tight_sublist_inside_loose_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  1. b\n  2. c\n\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<ol>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>d</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(crlf_line_endings) {
  const auto result{
      sourcemeta::core::markdown_to_html("## Title\r\ntext\r\n\r\nmore")};
  EXPECT_EQ(result, "<h2>Title</h2>\n"
                    "<p>text</p>\n"
                    "<p>more</p>\n");
}

TEST(cr_line_endings) {
  const auto result{
      sourcemeta::core::markdown_to_html("## Title\rtext\r\rmore")};
  EXPECT_EQ(result, "<h2>Title</h2>\n"
                    "<p>text</p>\n"
                    "<p>more</p>\n");
}

TEST(mixed_line_endings) {
  const auto result{
      sourcemeta::core::markdown_to_html("one\rtwo\r\nthree\nfour")};
  EXPECT_EQ(result, "<p>one\n"
                    "two\n"
                    "three\n"
                    "four</p>\n");
}

TEST(byte_order_mark_before_list) {
  const auto result{sourcemeta::core::markdown_to_html("\xEF\xBB\xBF"
                                                       "* item")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>item</li>\n"
                    "</ul>\n");
}
