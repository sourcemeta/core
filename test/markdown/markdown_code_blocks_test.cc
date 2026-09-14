#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(fenced_code_block_backticks) {
  const auto result{sourcemeta::core::markdown_to_html("```\ncode here\n```")};
  EXPECT_EQ(result, "<pre><code>code here\n</code></pre>\n");
}

TEST(fenced_code_block_tildes) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\ncode here\n~~~")};
  EXPECT_EQ(result, "<pre><code>code here\n</code></pre>\n");
}

TEST(fenced_code_block_with_language) {
  const auto result{
      sourcemeta::core::markdown_to_html("```cpp\nint x = 0;\n```")};
  EXPECT_EQ(result, "<pre lang=\"cpp\"><code>int x = 0;\n</code></pre>\n");
}

TEST(fenced_code_block_html_escaped) {
  const auto result{
      sourcemeta::core::markdown_to_html("```\n<div>&amp;</div>\n```")};
  EXPECT_EQ(result,
            "<pre><code>&lt;div&gt;&amp;amp;&lt;/div&gt;\n</code></pre>\n");
}

TEST(indented_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("    int x = 0;\n    return x;")};
  EXPECT_EQ(result, "<pre><code>int x = 0;\nreturn x;\n</code></pre>\n");
}

TEST(fenced_code_block_with_empty_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html("```\nline one\n\nline three\n```")};
  EXPECT_EQ(result, "<pre><code>line one\n\nline three\n</code></pre>\n");
}

TEST(github_pre_lang_format) {
  const auto result{
      sourcemeta::core::markdown_to_html("```python\nprint()\n```")};
  EXPECT_EQ(result, "<pre lang=\"python\"><code>print()\n</code></pre>\n");
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
