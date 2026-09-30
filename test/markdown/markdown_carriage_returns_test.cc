#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

// CommonMark 0.29 section 2.1 counts a carriage return that is not followed by
// a line feed as a line ending of its own, so every construct that a line feed
// delimits a carriage return delimits too

TEST(soft_line_break) {
  const auto result{sourcemeta::core::markdown_to_html("line one\rline two")};
  EXPECT_EQ(result, "<p>line one\nline two</p>\n");
}

TEST(hard_line_break_with_two_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("line one  \rline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
}

TEST(hard_line_break_with_backslash) {
  const auto result{sourcemeta::core::markdown_to_html("line one\\\rline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
}

TEST(blank_line_separates_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html("one\r\rtwo")};
  EXPECT_EQ(result, "<p>one</p>\n<p>two</p>\n");
}

TEST(trailing_carriage_return) {
  const auto result{sourcemeta::core::markdown_to_html("one\r")};
  EXPECT_EQ(result, "<p>one</p>\n");
}

TEST(atx_heading) {
  const auto result{sourcemeta::core::markdown_to_html("# Title\rbody")};
  EXPECT_EQ(result, "<h1>Title</h1>\n<p>body</p>\n");
}

TEST(setext_heading) {
  const auto result{sourcemeta::core::markdown_to_html("Title\r=====")};
  EXPECT_EQ(result, "<h1>Title</h1>\n");
}

TEST(thematic_break) {
  const auto result{sourcemeta::core::markdown_to_html("one\r\r---\r\rtwo")};
  EXPECT_EQ(result, "<p>one</p>\n<hr />\n<p>two</p>\n");
}

TEST(bullet_list) {
  const auto result{sourcemeta::core::markdown_to_html("- one\r- two")};
  EXPECT_EQ(result, "<ul>\n<li>one</li>\n<li>two</li>\n</ul>\n");
}

TEST(block_quote) {
  const auto result{sourcemeta::core::markdown_to_html("> one\r> two")};
  EXPECT_EQ(result, "<blockquote>\n<p>one\ntwo</p>\n</blockquote>\n");
}

TEST(fenced_code_block) {
  const auto result{sourcemeta::core::markdown_to_html("```\rcode\r```")};
  EXPECT_EQ(result, "<pre><code>code\n</code></pre>\n");
}

TEST(indented_code_block) {
  const auto result{sourcemeta::core::markdown_to_html("    code\r    more")};
  EXPECT_EQ(result, "<pre><code>code\nmore\n</code></pre>\n");
}

TEST(link_reference_definition) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\r\r[foo]")};
  EXPECT_EQ(result, "<p><a href=\"/url\">foo</a></p>\n");
}

TEST(link_reference_definition_with_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]: /url\r\"Title\"\r\r[foo]")};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"Title\">foo</a></p>\n");
}

TEST(table) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a | b |\r| - | - |\r| 1 | 2 |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n"
                    "</thead>\n<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(html_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div>\rtext\r</div>", false)};
  EXPECT_EQ(result, "<div>\ntext\n</div>\n");
}

TEST(mixed_with_line_feed) {
  const auto result{sourcemeta::core::markdown_to_html("one\rtwo\nthree")};
  EXPECT_EQ(result, "<p>one\ntwo\nthree</p>\n");
}

TEST(carriage_return_line_feed_pair_is_one_break) {
  const auto result{sourcemeta::core::markdown_to_html("one\r\ntwo")};
  EXPECT_EQ(result, "<p>one\ntwo</p>\n");
}

TEST(carriage_return_line_feed_blank_line) {
  const auto result{sourcemeta::core::markdown_to_html("one\r\n\r\ntwo")};
  EXPECT_EQ(result, "<p>one</p>\n<p>two</p>\n");
}

TEST(emphasis_across_carriage_return) {
  const auto result{sourcemeta::core::markdown_to_html("*one\rtwo*")};
  EXPECT_EQ(result, "<p><em>one\ntwo</em></p>\n");
}

TEST(code_span_across_carriage_return) {
  const auto result{sourcemeta::core::markdown_to_html("`one\rtwo`")};
  EXPECT_EQ(result, "<p><code>one two</code></p>\n");
}
