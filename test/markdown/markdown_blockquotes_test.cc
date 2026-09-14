#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(simple_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> quoted text")};
  EXPECT_EQ(result, "<blockquote>\n<p>quoted text</p>\n</blockquote>\n");
}

TEST(multiline_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("> line one\n> line two")};
  EXPECT_EQ(result, "<blockquote>\n<p>line one\nline two</p>\n</blockquote>\n");
}

TEST(nested_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> outer\n>> inner")};
  EXPECT_EQ(result, "<blockquote>\n<p>outer</p>\n<blockquote>\n<p>inner</p>\n"
                    "</blockquote>\n</blockquote>\n");
}

TEST(blockquote_with_formatting) {
  const auto result{
      sourcemeta::core::markdown_to_html("> **bold** and *italic*")};
  EXPECT_EQ(result,
            "<blockquote>\n<p><strong>bold</strong> and <em>italic</em></p>\n"
            "</blockquote>\n");
}

TEST(blockquote_with_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("> items:\n> - one\n> - two")};
  EXPECT_EQ(result,
            "<blockquote>\n<p>items:</p>\n<ul>\n<li>one</li>\n<li>two</li>\n"
            "</ul>\n</blockquote>\n");
}

TEST(blockquote_with_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("> example:\n>\n>     code here")};
  EXPECT_EQ(result, "<blockquote>\n<p>example:</p>\n"
                    "<pre><code>code here\n</code></pre>\n</blockquote>\n");
}

TEST(blockquote_with_heading) {
  const auto result{sourcemeta::core::markdown_to_html("> ## Quoted heading")};
  EXPECT_EQ(result, "<blockquote>\n<h2>Quoted heading</h2>\n</blockquote>\n");
}

TEST(deeply_nested_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> a\n>> b\n>>> c")};
  EXPECT_EQ(result, "<blockquote>\n<p>a</p>\n<blockquote>\n<p>b</p>\n"
                    "<blockquote>\n<p>c</p>\n</blockquote>\n</blockquote>\n"
                    "</blockquote>\n");
}

TEST(blockquote_followed_by_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("> quote\n\n- item one\n- item two")};
  EXPECT_EQ(result, "<blockquote>\n<p>quote</p>\n</blockquote>\n"
                    "<ul>\n<li>item one</li>\n<li>item two</li>\n</ul>\n");
}

TEST(blockquote_with_strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("> ~~deleted~~")};
  EXPECT_EQ(result, "<blockquote>\n<p><del>deleted</del></p>\n</blockquote>\n");
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
