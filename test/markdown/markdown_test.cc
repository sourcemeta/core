#include <sourcemeta/core/markdown.h>

#include <gtest/gtest.h>

TEST(Markdown_to_html, simple_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Hello world")};
  EXPECT_EQ(result, "<p>Hello world</p>\n");
}

TEST(Markdown_to_html, multiple_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "First paragraph\n\nSecond paragraph")};
  EXPECT_EQ(result, "<p>First paragraph</p>\n<p>Second paragraph</p>\n");
}

TEST(Markdown_to_html, three_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html("One\n\nTwo\n\nThree")};
  EXPECT_EQ(result, "<p>One</p>\n<p>Two</p>\n<p>Three</p>\n");
}

TEST(Markdown_to_html, empty_input) {
  const auto result{sourcemeta::core::markdown_to_html("")};
  EXPECT_TRUE(result.empty());
}

TEST(Markdown_to_html, whitespace_only) {
  const auto result{sourcemeta::core::markdown_to_html("   \n\n   ")};
  EXPECT_TRUE(result.empty());
}

TEST(Markdown_to_html, soft_line_break) {
  const auto result{sourcemeta::core::markdown_to_html("line one\nline two")};
  EXPECT_EQ(result, "<p>line one\nline two</p>\n");
}

TEST(Markdown_to_html, hard_line_break_with_two_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("line one  \nline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
}

TEST(Markdown_to_html, hard_line_break_with_backslash) {
  const auto result{sourcemeta::core::markdown_to_html("line one\\\nline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
}

TEST(Markdown_to_html, atx_heading_level_1) {
  const auto result{sourcemeta::core::markdown_to_html("# Heading 1")};
  EXPECT_EQ(result, "<h1>Heading 1</h1>\n");
}

TEST(Markdown_to_html, atx_heading_level_2) {
  const auto result{sourcemeta::core::markdown_to_html("## Heading 2")};
  EXPECT_EQ(result, "<h2>Heading 2</h2>\n");
}

TEST(Markdown_to_html, atx_heading_level_3) {
  const auto result{sourcemeta::core::markdown_to_html("### Heading 3")};
  EXPECT_EQ(result, "<h3>Heading 3</h3>\n");
}

TEST(Markdown_to_html, atx_heading_level_4) {
  const auto result{sourcemeta::core::markdown_to_html("#### Heading 4")};
  EXPECT_EQ(result, "<h4>Heading 4</h4>\n");
}

TEST(Markdown_to_html, atx_heading_level_5) {
  const auto result{sourcemeta::core::markdown_to_html("##### Heading 5")};
  EXPECT_EQ(result, "<h5>Heading 5</h5>\n");
}

TEST(Markdown_to_html, atx_heading_level_6) {
  const auto result{sourcemeta::core::markdown_to_html("###### Heading 6")};
  EXPECT_EQ(result, "<h6>Heading 6</h6>\n");
}

TEST(Markdown_to_html, atx_heading_with_closing_hashes) {
  const auto result{sourcemeta::core::markdown_to_html("## Heading ##")};
  EXPECT_EQ(result, "<h2>Heading</h2>\n");
}

TEST(Markdown_to_html, setext_heading_level_1) {
  const auto result{sourcemeta::core::markdown_to_html("Heading 1\n=========")};
  EXPECT_EQ(result, "<h1>Heading 1</h1>\n");
}

TEST(Markdown_to_html, setext_heading_level_2) {
  const auto result{sourcemeta::core::markdown_to_html("Heading 2\n---------")};
  EXPECT_EQ(result, "<h2>Heading 2</h2>\n");
}

TEST(Markdown_to_html, italic_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("*italic*")};
  EXPECT_EQ(result, "<p><em>italic</em></p>\n");
}

TEST(Markdown_to_html, italic_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("_italic_")};
  EXPECT_EQ(result, "<p><em>italic</em></p>\n");
}

TEST(Markdown_to_html, bold_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("**bold**")};
  EXPECT_EQ(result, "<p><strong>bold</strong></p>\n");
}

TEST(Markdown_to_html, bold_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("__bold__")};
  EXPECT_EQ(result, "<p><strong>bold</strong></p>\n");
}

TEST(Markdown_to_html, bold_and_italic) {
  const auto result{sourcemeta::core::markdown_to_html("***bold italic***")};
  EXPECT_EQ(result, "<p><em><strong>bold italic</strong></em></p>\n");
}

TEST(Markdown_to_html, bold_inside_italic) {
  const auto result{
      sourcemeta::core::markdown_to_html("*this is **bold** inside italic*")};
  EXPECT_EQ(result,
            "<p><em>this is <strong>bold</strong> inside italic</em></p>\n");
}

TEST(Markdown_to_html, italic_inside_bold) {
  const auto result{
      sourcemeta::core::markdown_to_html("**this is *italic* inside bold**")};
  EXPECT_EQ(result,
            "<p><strong>this is <em>italic</em> inside bold</strong></p>\n");
}

TEST(Markdown_to_html, inline_code) {
  const auto result{sourcemeta::core::markdown_to_html("Use `printf()`")};
  EXPECT_EQ(result, "<p>Use <code>printf()</code></p>\n");
}

TEST(Markdown_to_html, inline_code_with_backticks_inside) {
  const auto result{
      sourcemeta::core::markdown_to_html("`` there is a ` here ``")};
  EXPECT_EQ(result, "<p><code>there is a ` here</code></p>\n");
}

TEST(Markdown_to_html, inline_code_preserves_html) {
  const auto result{
      sourcemeta::core::markdown_to_html("`<div class=\"foo\">`")};
  EXPECT_EQ(result, "<p><code>&lt;div class=&quot;foo&quot;&gt;</code></p>\n");
}

TEST(Markdown_to_html, fenced_code_block_backticks) {
  const auto result{sourcemeta::core::markdown_to_html("```\ncode here\n```")};
  EXPECT_EQ(result, "<pre><code>code here\n</code></pre>\n");
}

TEST(Markdown_to_html, fenced_code_block_tildes) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\ncode here\n~~~")};
  EXPECT_EQ(result, "<pre><code>code here\n</code></pre>\n");
}

TEST(Markdown_to_html, fenced_code_block_with_language) {
  const auto result{
      sourcemeta::core::markdown_to_html("```cpp\nint x = 0;\n```")};
  EXPECT_EQ(result, "<pre lang=\"cpp\"><code>int x = 0;\n</code></pre>\n");
}

TEST(Markdown_to_html, fenced_code_block_html_escaped) {
  const auto result{
      sourcemeta::core::markdown_to_html("```\n<div>&amp;</div>\n```")};
  EXPECT_EQ(result,
            "<pre><code>&lt;div&gt;&amp;amp;&lt;/div&gt;\n</code></pre>\n");
}

TEST(Markdown_to_html, indented_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("    int x = 0;\n    return x;")};
  EXPECT_EQ(result, "<pre><code>int x = 0;\nreturn x;\n</code></pre>\n");
}

TEST(Markdown_to_html, inline_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[click here](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">click here</a></p>\n");
}

TEST(Markdown_to_html, inline_link_with_title) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click](https://example.com \"My Title\")")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com\" title=\"My Title\">click</a></p>\n");
}

TEST(Markdown_to_html, reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click][ref]\n\n[ref]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">click</a></p>\n");
}

TEST(Markdown_to_html, reference_link_with_title) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click][ref]\n\n[ref]: https://example.com \"Title\"")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com\" title=\"Title\">click</a></p>\n");
}

TEST(Markdown_to_html, collapsed_reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[example][]\n\n[example]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">example</a></p>\n");
}

TEST(Markdown_to_html, shortcut_reference_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[example]\n\n[example]: https://example.com")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">example</a></p>\n");
}

TEST(Markdown_to_html, link_with_emphasis_inside) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[**bold link**](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\"><strong>bold "
                    "link</strong></a></p>\n");
}

TEST(Markdown_to_html, image) {
  const auto result{
      sourcemeta::core::markdown_to_html("![alt text](image.png)")};
  EXPECT_EQ(result, "<p><img src=\"image.png\" alt=\"alt text\" /></p>\n");
}

TEST(Markdown_to_html, image_with_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("![alt](image.png \"My Image\")")};
  EXPECT_EQ(
      result,
      "<p><img src=\"image.png\" alt=\"alt\" title=\"My Image\" /></p>\n");
}

TEST(Markdown_to_html, simple_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> quoted text")};
  EXPECT_EQ(result, "<blockquote>\n<p>quoted text</p>\n</blockquote>\n");
}

TEST(Markdown_to_html, multiline_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("> line one\n> line two")};
  EXPECT_EQ(result, "<blockquote>\n<p>line one\nline two</p>\n</blockquote>\n");
}

TEST(Markdown_to_html, nested_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> outer\n>> inner")};
  EXPECT_EQ(result, "<blockquote>\n<p>outer</p>\n<blockquote>\n<p>inner</p>\n"
                    "</blockquote>\n</blockquote>\n");
}

TEST(Markdown_to_html, blockquote_with_formatting) {
  const auto result{
      sourcemeta::core::markdown_to_html("> **bold** and *italic*")};
  EXPECT_EQ(result,
            "<blockquote>\n<p><strong>bold</strong> and <em>italic</em></p>\n"
            "</blockquote>\n");
}

TEST(Markdown_to_html, unordered_list_with_dashes) {
  const auto result{
      sourcemeta::core::markdown_to_html("- one\n- two\n- three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(Markdown_to_html, unordered_list_with_asterisks) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n* two\n* three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(Markdown_to_html, unordered_list_with_plus) {
  const auto result{
      sourcemeta::core::markdown_to_html("+ one\n+ two\n+ three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(Markdown_to_html, nested_unordered_list) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- outer\n  - inner\n  - inner2\n- outer2")};
  EXPECT_EQ(result, "<ul>\n<li>outer\n<ul>\n<li>inner</li>\n<li>inner2</li>\n"
                    "</ul>\n</li>\n<li>outer2</li>\n</ul>\n");
}

TEST(Markdown_to_html, ordered_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1. one\n2. two\n3. three")};
  EXPECT_EQ(result,
            "<ol>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ol>\n");
}

TEST(Markdown_to_html, ordered_list_starting_at_nonone) {
  const auto result{
      sourcemeta::core::markdown_to_html("3. three\n4. four\n5. five")};
  EXPECT_EQ(result,
            "<ol start=\"3\">\n<li>three</li>\n<li>four</li>\n<li>five</li>\n"
            "</ol>\n");
}

TEST(Markdown_to_html, nested_ordered_in_unordered) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- item\n  1. sub one\n  2. sub two\n- item2")};
  EXPECT_EQ(result, "<ul>\n<li>item\n<ol>\n<li>sub one</li>\n<li>sub two</li>\n"
                    "</ol>\n</li>\n<li>item2</li>\n</ul>\n");
}

TEST(Markdown_to_html, loose_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("- one\n\n- two\n\n- three")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>one</p>\n</li>\n<li>\n<p>two</p>\n</li>\n"
                    "<li>\n<p>three</p>\n</li>\n</ul>\n");
}

TEST(Markdown_to_html, thematic_break_with_dashes) {
  const auto result{sourcemeta::core::markdown_to_html("---")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(Markdown_to_html, thematic_break_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("***")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(Markdown_to_html, thematic_break_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("___")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(Markdown_to_html, thematic_break_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("- - -")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(Markdown_to_html, escape_asterisk) {
  const auto result{sourcemeta::core::markdown_to_html("\\*not italic\\*")};
  EXPECT_EQ(result, "<p>*not italic*</p>\n");
}

TEST(Markdown_to_html, escape_hash) {
  const auto result{sourcemeta::core::markdown_to_html("\\# not a heading")};
  EXPECT_EQ(result, "<p># not a heading</p>\n");
}

TEST(Markdown_to_html, escape_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\[not a link\\](url)")};
  EXPECT_EQ(result, "<p>[not a link](url)</p>\n");
}

TEST(Markdown_to_html, escape_backtick) {
  const auto result{sourcemeta::core::markdown_to_html("\\`not code\\`")};
  EXPECT_EQ(result, "<p>`not code`</p>\n");
}

TEST(Markdown_to_html, html_entity_named) {
  const auto result{sourcemeta::core::markdown_to_html("&copy; 2025")};
  EXPECT_EQ(result, "<p>\u00A9 2025</p>\n");
}

TEST(Markdown_to_html, html_entity_numeric) {
  const auto result{sourcemeta::core::markdown_to_html("&#169; 2025")};
  EXPECT_EQ(result, "<p>\u00A9 2025</p>\n");
}

TEST(Markdown_to_html, ampersand_in_text) {
  const auto result{sourcemeta::core::markdown_to_html("AT&T")};
  EXPECT_EQ(result, "<p>AT&amp;T</p>\n");
}

TEST(Markdown_to_html, angle_brackets_in_text) {
  const auto result{sourcemeta::core::markdown_to_html("1 < 2 and 3 > 2")};
  EXPECT_EQ(result, "<p>1 &lt; 2 and 3 &gt; 2</p>\n");
}

TEST(Markdown_to_html, unicode_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("Hello \xC3\xA9\xC3\xA0\xC3\xBC")};
  EXPECT_EQ(result, "<p>Hello \xC3\xA9\xC3\xA0\xC3\xBC</p>\n");
}

TEST(Markdown_to_html, cjk_characters) {
  const auto result{
      sourcemeta::core::markdown_to_html("\xE4\xBD\xA0\xE5\xA5\xBD")};
  EXPECT_EQ(result, "<p>\xE4\xBD\xA0\xE5\xA5\xBD</p>\n");
}

TEST(Markdown_to_html, emoji) {
  const auto result{sourcemeta::core::markdown_to_html("\xF0\x9F\x98\x80")};
  EXPECT_EQ(result, "<p>\xF0\x9F\x98\x80</p>\n");
}

TEST(Markdown_to_html, inline_html_is_sanitized) {
  const auto result{sourcemeta::core::markdown_to_html("Hello <em>world</em>")};
  EXPECT_EQ(
      result,
      "<p>Hello <!-- raw HTML omitted -->world<!-- raw HTML omitted --></p>\n");
}

TEST(Markdown_to_html, raw_html_block_is_sanitized) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div class=\"foo\">bar</div>")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n");
}

TEST(Markdown_to_html, script_tag_is_sanitized) {
  const auto result{
      sourcemeta::core::markdown_to_html("<script>alert('xss')</script>")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n");
}

TEST(Markdown_to_html, iframe_is_sanitized) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<iframe src=\"https://evil.com\"></iframe>")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n");
}

TEST(Markdown_to_html, javascript_link_is_sanitized) {
  const auto result{
      sourcemeta::core::markdown_to_html("[click](javascript:alert('xss'))")};
  EXPECT_EQ(result, "<p><a href=\"\">click</a></p>\n");
}

TEST(Markdown_to_html, simple_table) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\n| - | - |\n| 1 | 2 |")};
  EXPECT_EQ(
      result,
      "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</thead>\n"
      "<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, table_with_alignment) {
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

TEST(Markdown_to_html, table_with_formatting_in_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| **bold** |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><strong>bold</strong></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(Markdown_to_html, table_with_multiple_rows) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| x | y |\n| - | - |\n| 1 | 2 |\n| 3 | 4 |\n| 5 | 6 |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>x</th>\n<th>y</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n"
            "<tr>\n<td>3</td>\n<td>4</td>\n</tr>\n"
            "<tr>\n<td>5</td>\n<td>6</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, table_with_pipe_in_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| a \\| b |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td>a | b</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, autolink_url) {
  const auto result{
      sourcemeta::core::markdown_to_html("Visit https://example.com today")};
  EXPECT_EQ(result,
            "<p>Visit <a href=\"https://example.com\">https://example.com</a>"
            " today</p>\n");
}

TEST(Markdown_to_html, autolink_http) {
  const auto result{sourcemeta::core::markdown_to_html("http://example.com")};
  EXPECT_EQ(result,
            "<p><a href=\"http://example.com\">http://example.com</a></p>\n");
}

TEST(Markdown_to_html, autolink_www) {
  const auto result{sourcemeta::core::markdown_to_html("www.example.com")};
  EXPECT_EQ(result,
            "<p><a href=\"http://www.example.com\">www.example.com</a></p>\n");
}

TEST(Markdown_to_html, autolink_email) {
  const auto result{
      sourcemeta::core::markdown_to_html("Contact user@example.com")};
  EXPECT_EQ(result, "<p>Contact <a href=\"mailto:user@example.com\">"
                    "user@example.com</a></p>\n");
}

TEST(Markdown_to_html, angle_bracket_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://example.com>")};
  EXPECT_EQ(result,
            "<p><a href=\"https://example.com\">https://example.com</a></p>\n");
}

TEST(Markdown_to_html, strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("~~deleted~~")};
  EXPECT_EQ(result, "<p><del>deleted</del></p>\n");
}

TEST(Markdown_to_html, strikethrough_with_other_formatting) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~deleted **bold** text~~")};
  EXPECT_EQ(result, "<p><del>deleted <strong>bold</strong> text</del></p>\n");
}

TEST(Markdown_to_html, strikethrough_in_sentence) {
  const auto result{
      sourcemeta::core::markdown_to_html("This is ~~wrong~~ correct")};
  EXPECT_EQ(result, "<p>This is <del>wrong</del> correct</p>\n");
}

TEST(Markdown_to_html, task_list_unchecked) {
  const auto result{sourcemeta::core::markdown_to_html("- [ ] todo")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> todo</li>\n"
            "</ul>\n");
}

TEST(Markdown_to_html, task_list_checked) {
  const auto result{sourcemeta::core::markdown_to_html("- [x] done")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" checked=\"\" disabled=\"\" />"
            " done</li>\n</ul>\n");
}

TEST(Markdown_to_html, task_list_mixed) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- [x] done\n- [ ] todo\n- [x] also done")};
  EXPECT_EQ(
      result,
      "<ul>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> done</li>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> todo</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> also "
      "done</li>\n"
      "</ul>\n");
}

TEST(Markdown_to_html, heading_followed_by_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("# Title\n\nSome text here.")};
  EXPECT_EQ(result, "<h1>Title</h1>\n<p>Some text here.</p>\n");
}

TEST(Markdown_to_html, blockquote_with_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("> items:\n> - one\n> - two")};
  EXPECT_EQ(result,
            "<blockquote>\n<p>items:</p>\n<ul>\n<li>one</li>\n<li>two</li>\n"
            "</ul>\n</blockquote>\n");
}

TEST(Markdown_to_html, list_with_code_block) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- item:\n\n  ```\n  code\n  ```\n\n- next")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>item:</p>\n<pre><code>code\n</code></pre>\n"
                    "</li>\n<li>\n<p>next</p>\n</li>\n</ul>\n");
}

TEST(Markdown_to_html, link_inside_heading) {
  const auto result{
      sourcemeta::core::markdown_to_html("## [Link](https://example.com)")};
  EXPECT_EQ(result, "<h2><a href=\"https://example.com\">Link</a></h2>\n");
}

TEST(Markdown_to_html, emphasis_across_words) {
  const auto result{
      sourcemeta::core::markdown_to_html("this is **all bold** here")};
  EXPECT_EQ(result, "<p>this is <strong>all bold</strong> here</p>\n");
}

TEST(Markdown_to_html, paragraph_with_inline_code_and_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Use `foo()` from [the docs](https://docs.com).")};
  EXPECT_EQ(result, "<p>Use <code>foo()</code> from "
                    "<a href=\"https://docs.com\">the docs</a>.</p>\n");
}

TEST(Markdown_to_html, strikethrough_inside_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| ~~old~~ |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><del>old</del></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(Markdown_to_html, link_inside_table_cell) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| h |\n| - |\n| [click](https://example.com) |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td><a href=\"https://example.com\">click</a>"
            "</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, code_inside_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| `code` |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><code>code</code></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(Markdown_to_html, empty_table_cells) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\n| - | - |\n|   |   |")};
  EXPECT_EQ(result,
            "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</thead>\n"
            "<tbody>\n<tr>\n<td></td>\n<td></td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, table_header_only) {
  const auto result{sourcemeta::core::markdown_to_html("| a | b |\n| - | - |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</"
                    "thead>\n</table>\n");
}

TEST(Markdown_to_html, task_list_with_bold) {
  const auto result{
      sourcemeta::core::markdown_to_html("- [ ] **important** task")};
  EXPECT_EQ(result, "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> "
                    "<strong>important</strong> task</li>\n</ul>\n");
}

TEST(Markdown_to_html, task_list_with_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- [x] see [docs](https://example.com)")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> "
            "see <a href=\"https://example.com\">docs</a></li>\n</ul>\n");
}

TEST(Markdown_to_html, task_list_with_code) {
  const auto result{sourcemeta::core::markdown_to_html("- [ ] fix `bug`")};
  EXPECT_EQ(result, "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> "
                    "fix <code>bug</code></li>\n</ul>\n");
}

TEST(Markdown_to_html, code_inside_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[`code`](https://example.com)")};
  EXPECT_EQ(result,
            "<p><a href=\"https://example.com\"><code>code</code></a></p>\n");
}

TEST(Markdown_to_html, inline_code_inside_heading) {
  const auto result{
      sourcemeta::core::markdown_to_html("## The `main` function")};
  EXPECT_EQ(result, "<h2>The <code>main</code> function</h2>\n");
}

TEST(Markdown_to_html, image_as_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[![logo](logo.png)](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">"
                    "<img src=\"logo.png\" alt=\"logo\" /></a></p>\n");
}

TEST(Markdown_to_html, strikethrough_with_code) {
  const auto result{sourcemeta::core::markdown_to_html("~~use `old_func()`~~")};
  EXPECT_EQ(result, "<p><del>use <code>old_func()</code></del></p>\n");
}

TEST(Markdown_to_html, strikethrough_with_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~[removed](https://example.com)~~")};
  EXPECT_EQ(result,
            "<p><del><a href=\"https://example.com\">removed</a></del></p>\n");
}

TEST(Markdown_to_html, autolink_inside_list) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- visit https://example.com\n- done")};
  EXPECT_EQ(result, "<ul>\n<li>visit <a href=\"https://example.com\">"
                    "https://example.com</a></li>\n<li>done</li>\n</ul>\n");
}

TEST(Markdown_to_html, blockquote_with_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("> example:\n>\n>     code here")};
  EXPECT_EQ(result, "<blockquote>\n<p>example:</p>\n"
                    "<pre><code>code here\n</code></pre>\n</blockquote>\n");
}

TEST(Markdown_to_html, blockquote_with_heading) {
  const auto result{sourcemeta::core::markdown_to_html("> ## Quoted heading")};
  EXPECT_EQ(result, "<blockquote>\n<h2>Quoted heading</h2>\n</blockquote>\n");
}

TEST(Markdown_to_html, deeply_nested_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("- a\n  - b\n    - c\n      - d")};
  EXPECT_EQ(result, "<ul>\n<li>a\n<ul>\n<li>b\n<ul>\n<li>c\n<ul>\n<li>d</li>\n"
                    "</ul>\n</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>\n");
}

TEST(Markdown_to_html, deeply_nested_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> a\n>> b\n>>> c")};
  EXPECT_EQ(result, "<blockquote>\n<p>a</p>\n<blockquote>\n<p>b</p>\n"
                    "<blockquote>\n<p>c</p>\n</blockquote>\n</blockquote>\n"
                    "</blockquote>\n");
}

TEST(Markdown_to_html, consecutive_headings) {
  const auto result{
      sourcemeta::core::markdown_to_html("# One\n\n## Two\n\n### Three")};
  EXPECT_EQ(result, "<h1>One</h1>\n<h2>Two</h2>\n<h3>Three</h3>\n");
}

TEST(Markdown_to_html, consecutive_thematic_breaks) {
  const auto result{sourcemeta::core::markdown_to_html("---\n\n---")};
  EXPECT_EQ(result, "<hr />\n<hr />\n");
}

TEST(Markdown_to_html, link_with_special_chars_in_url) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[search](https://example.com/q?a=1&b=2)")};
  EXPECT_EQ(
      result,
      "<p><a href=\"https://example.com/q?a=1&amp;b=2\">search</a></p>\n");
}

TEST(Markdown_to_html, image_with_empty_alt) {
  const auto result{sourcemeta::core::markdown_to_html("![](image.png)")};
  EXPECT_EQ(result, "<p><img src=\"image.png\" alt=\"\" /></p>\n");
}

TEST(Markdown_to_html, bold_and_italic_in_list_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("- ***bold italic item***\n- normal")};
  EXPECT_EQ(result,
            "<ul>\n<li><em><strong>bold italic item</strong></em></li>\n"
            "<li>normal</li>\n</ul>\n");
}

TEST(Markdown_to_html, fenced_code_block_with_empty_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html("```\nline one\n\nline three\n```")};
  EXPECT_EQ(result, "<pre><code>line one\n\nline three\n</code></pre>\n");
}

TEST(Markdown_to_html, ordered_list_with_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "1. first\n\n   more text\n\n2. second")};
  EXPECT_EQ(result, "<ol>\n<li>\n<p>first</p>\n<p>more text</p>\n</li>\n"
                    "<li>\n<p>second</p>\n</li>\n</ol>\n");
}

TEST(Markdown_to_html, heading_with_thematic_break) {
  const auto result{
      sourcemeta::core::markdown_to_html("# Title\n\n---\n\nContent")};
  EXPECT_EQ(result, "<h1>Title</h1>\n<hr />\n<p>Content</p>\n");
}

TEST(Markdown_to_html, table_after_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Here is a table:\n\n| a |\n| - |\n| 1 |")};
  EXPECT_EQ(result, "<p>Here is a table:</p>\n"
                    "<table>\n<thead>\n<tr>\n<th>a</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td>1</td>\n</tr>\n</tbody>\n</table>\n");
}

TEST(Markdown_to_html, blockquote_followed_by_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("> quote\n\n- item one\n- item two")};
  EXPECT_EQ(result, "<blockquote>\n<p>quote</p>\n</blockquote>\n"
                    "<ul>\n<li>item one</li>\n<li>item two</li>\n</ul>\n");
}

TEST(Markdown_to_html, multiple_autolinks_in_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html(
      "See https://one.com and https://two.com")};
  EXPECT_EQ(result,
            "<p>See <a href=\"https://one.com\">https://one.com</a> and "
            "<a href=\"https://two.com\">https://two.com</a></p>\n");
}

TEST(Markdown_to_html, emphasis_does_not_cross_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("*start `code* end`")};
  EXPECT_EQ(result, "<p>*start <code>code* end</code></p>\n");
}

TEST(Markdown_to_html, vbscript_link_is_sanitized) {
  const auto result{
      sourcemeta::core::markdown_to_html("[click](vbscript:MsgBox)")};
  EXPECT_EQ(result, "<p><a href=\"\">click</a></p>\n");
}

TEST(Markdown_to_html, data_uri_link_is_sanitized) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click](data:text/html,<h1>hi</h1>)")};
  EXPECT_EQ(result, "<p><a href=\"\">click</a></p>\n");
}

TEST(Markdown_to_html, single_character_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("*a*")};
  EXPECT_EQ(result, "<p><em>a</em></p>\n");
}

TEST(Markdown_to_html, list_item_with_multiple_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- paragraph one\n\n  paragraph two\n\n  paragraph three")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>paragraph one</p>\n<p>paragraph two</p>\n"
                    "<p>paragraph three</p>\n</li>\n</ul>\n");
}

TEST(Markdown_to_html, table_with_inline_code_in_header) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| `key` | `value` |\n| - | - |\n| a | b |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th><code>key</code></th>\n"
                    "<th><code>value</code></th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td>a</td>\n<td>b</td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(Markdown_to_html, strikethrough_bold_italic_combined) {
  const auto result{sourcemeta::core::markdown_to_html("~~***all three***~~")};
  EXPECT_EQ(result, "<p><del><em><strong>all three</strong></em></del></p>\n");
}

TEST(Markdown_to_html, autolink_with_trailing_punctuation) {
  const auto result{
      sourcemeta::core::markdown_to_html("See https://example.com.")};
  EXPECT_EQ(
      result,
      "<p>See <a href=\"https://example.com\">https://example.com</a>.</p>\n");
}

TEST(Markdown_to_html, autolink_in_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("(see https://example.com)")};
  EXPECT_EQ(
      result,
      "<p>(see <a href=\"https://example.com\">https://example.com</a>)</p>\n");
}

TEST(Markdown_to_html, blockquote_with_strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("> ~~deleted~~")};
  EXPECT_EQ(result, "<blockquote>\n<p><del>deleted</del></p>\n</blockquote>\n");
}

TEST(Markdown_to_html, heading_with_bold_and_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("## **Bold** and `code`")};
  EXPECT_EQ(result, "<h2><strong>Bold</strong> and <code>code</code></h2>\n");
}

TEST(Markdown_to_html, footnote_basic) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text[^1]\n\n[^1]: Footnote content")};
  EXPECT_EQ(
      result,
      "<p>Text<sup class=\"footnote-ref\"><a href=\"#fn-1\" id=\"fnref-1\""
      " data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
      "<li id=\"fn-1\">\n<p>Footnote content "
      "<a href=\"#fnref-1\" class=\"footnote-backref\""
      " data-footnote-backref data-footnote-backref-idx=\"1\""
      " aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
      "</li>\n</ol>\n</section>\n");
}

TEST(Markdown_to_html, footnote_multiple) {
  const auto result{sourcemeta::core::markdown_to_html(
      "A[^1] and B[^2]\n\n[^1]: First\n\n[^2]: Second")};
  EXPECT_EQ(result,
            "<p>A<sup class=\"footnote-ref\"><a href=\"#fn-1\" id=\"fnref-1\""
            " data-footnote-ref>1</a></sup> and "
            "B<sup class=\"footnote-ref\"><a href=\"#fn-2\" id=\"fnref-2\""
            " data-footnote-ref>2</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
            "<li id=\"fn-1\">\n<p>First "
            "<a href=\"#fnref-1\" class=\"footnote-backref\""
            " data-footnote-backref data-footnote-backref-idx=\"1\""
            " aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-2\">\n<p>Second "
            "<a href=\"#fnref-2\" class=\"footnote-backref\""
            " data-footnote-backref data-footnote-backref-idx=\"2\""
            " aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
            "</li>\n</ol>\n</section>\n");
}

TEST(Markdown_to_html, single_tilde_is_not_strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("~not deleted~")};
  EXPECT_EQ(result, "<p>~not deleted~</p>\n");
}

TEST(Markdown_to_html, github_pre_lang_format) {
  const auto result{
      sourcemeta::core::markdown_to_html("```python\nprint()\n```")};
  EXPECT_EQ(result, "<pre lang=\"python\"><code>print()\n</code></pre>\n");
}

TEST(Markdown_to_html, invalid_utf8_is_replaced) {
  const auto result{sourcemeta::core::markdown_to_html("hello \xFF world")};
  EXPECT_EQ(result, "<p>hello \xEF\xBF\xBD world</p>\n");
}
