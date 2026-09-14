#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(atx_heading_level_1) {
  const auto result{sourcemeta::core::markdown_to_html("# Heading 1")};
  EXPECT_EQ(result, "<h1>Heading 1</h1>\n");
}

TEST(atx_heading_level_2) {
  const auto result{sourcemeta::core::markdown_to_html("## Heading 2")};
  EXPECT_EQ(result, "<h2>Heading 2</h2>\n");
}

TEST(atx_heading_level_3) {
  const auto result{sourcemeta::core::markdown_to_html("### Heading 3")};
  EXPECT_EQ(result, "<h3>Heading 3</h3>\n");
}

TEST(atx_heading_level_4) {
  const auto result{sourcemeta::core::markdown_to_html("#### Heading 4")};
  EXPECT_EQ(result, "<h4>Heading 4</h4>\n");
}

TEST(atx_heading_level_5) {
  const auto result{sourcemeta::core::markdown_to_html("##### Heading 5")};
  EXPECT_EQ(result, "<h5>Heading 5</h5>\n");
}

TEST(atx_heading_level_6) {
  const auto result{sourcemeta::core::markdown_to_html("###### Heading 6")};
  EXPECT_EQ(result, "<h6>Heading 6</h6>\n");
}

TEST(atx_heading_with_closing_hashes) {
  const auto result{sourcemeta::core::markdown_to_html("## Heading ##")};
  EXPECT_EQ(result, "<h2>Heading</h2>\n");
}

TEST(setext_heading_level_1) {
  const auto result{sourcemeta::core::markdown_to_html("Heading 1\n=========")};
  EXPECT_EQ(result, "<h1>Heading 1</h1>\n");
}

TEST(setext_heading_level_2) {
  const auto result{sourcemeta::core::markdown_to_html("Heading 2\n---------")};
  EXPECT_EQ(result, "<h2>Heading 2</h2>\n");
}

TEST(heading_followed_by_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("# Title\n\nSome text here.")};
  EXPECT_EQ(result, "<h1>Title</h1>\n<p>Some text here.</p>\n");
}

TEST(link_inside_heading) {
  const auto result{
      sourcemeta::core::markdown_to_html("## [Link](https://example.com)")};
  EXPECT_EQ(result, "<h2><a href=\"https://example.com\">Link</a></h2>\n");
}

TEST(inline_code_inside_heading) {
  const auto result{
      sourcemeta::core::markdown_to_html("## The `main` function")};
  EXPECT_EQ(result, "<h2>The <code>main</code> function</h2>\n");
}

TEST(consecutive_headings) {
  const auto result{
      sourcemeta::core::markdown_to_html("# One\n\n## Two\n\n### Three")};
  EXPECT_EQ(result, "<h1>One</h1>\n<h2>Two</h2>\n<h3>Three</h3>\n");
}

TEST(heading_with_thematic_break) {
  const auto result{
      sourcemeta::core::markdown_to_html("# Title\n\n---\n\nContent")};
  EXPECT_EQ(result, "<h1>Title</h1>\n<hr />\n<p>Content</p>\n");
}

TEST(heading_with_bold_and_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("## **Bold** and `code`")};
  EXPECT_EQ(result, "<h2><strong>Bold</strong> and <code>code</code></h2>\n");
}

TEST(dashes_after_paragraph_make_setext_heading) {
  const auto result{sourcemeta::core::markdown_to_html("alpha\n-----\nbeta")};
  EXPECT_EQ(result, "<h2>alpha</h2>\n"
                    "<p>beta</p>\n");
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
