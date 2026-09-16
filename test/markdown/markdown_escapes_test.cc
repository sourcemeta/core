#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(escape_asterisk) {
  const auto result{sourcemeta::core::markdown_to_html("\\*not italic\\*")};
  EXPECT_EQ(result, "<p>*not italic*</p>\n");
}

TEST(escape_hash) {
  const auto result{sourcemeta::core::markdown_to_html("\\# not a heading")};
  EXPECT_EQ(result, "<p># not a heading</p>\n");
}

TEST(escape_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\[not a link\\](url)")};
  EXPECT_EQ(result, "<p>[not a link](url)</p>\n");
}

TEST(escape_backtick) {
  const auto result{sourcemeta::core::markdown_to_html("\\`not code\\`")};
  EXPECT_EQ(result, "<p>`not code`</p>\n");
}

TEST(backslash_escapes_every_ascii_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a\\~b\\}c\\|d\\{e\\`f\\_g\\^h\\]i\\\\j\\[k\\@l\\?m\\>n\\=o\\<p\\;q\\:"
      "r\\/s\\.t\\-u\\,v\\+w\\*x\\)y\\(z\\'1\\&2\\%3\\$4\\#5\\\"6\\!")};
  EXPECT_EQ(result, "<p>a~b}c|d{e`f_g^h]i\\j[k@l?m&gt;n=o&lt;p;q:r/"
                    "s.t-u,v+w*x)y(z&#39;1&amp;2%3$4#5&quot;6!</p>\n");
}

TEST(backslash_before_other_characters_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("\\Z \\9 \\é \\€ \\\t")};
  EXPECT_EQ(result, "<p>\\Z \\9 \\\xc3\xa9 \\\xe2\x82\xac \\</p>\n");
}

TEST(backslash_escaped_markup_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\\_no emphasis_\n\\<em> no tag\n\\![no image](/x.png)\n2\\) no "
      "list\n\\+ no list\n\\> no quote\n\\&amp; no entity")};
  EXPECT_EQ(result, "<p>_no emphasis_\n"
                    "&lt;em&gt; no tag\n"
                    "!<a href=\"/x.png\">no image</a>\n"
                    "2) no list\n"
                    "+ no list\n"
                    "&gt; no quote\n"
                    "&amp;amp; no entity</p>\n");
}

TEST(backslash_escaped_backslash_before_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("\\\\_emphasis_")};
  EXPECT_EQ(result, "<p>\\<em>emphasis</em></p>\n");
}

TEST(backslash_at_end_of_line_is_hard_break) {
  const auto result{sourcemeta::core::markdown_to_html("one\\\ntwo")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(backslash_inside_code_span_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("`\\*\\_`")};
  EXPECT_EQ(result, "<p><code>\\*\\_</code></p>\n");
}

TEST(backslash_inside_indented_code_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("    \\*\\_")};
  EXPECT_EQ(result, "<pre><code>\\*\\_\n"
                    "</code></pre>\n");
}

TEST(backslash_inside_fenced_code_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("```\n\\*\\_\n```")};
  EXPECT_EQ(result, "<pre><code>\\*\\_\n"
                    "</code></pre>\n");
}

TEST(backslash_inside_autolink_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/\\_\\*>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%5C_%5C*\">https://"
                    "sourcemeta.com/\\_\\*</a></p>\n");
}

TEST(backslash_escapes_in_link_destination_and_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/a\\_b 'c\\_d')")};
  EXPECT_EQ(result, "<p><a href=\"/a_b\" title=\"c_d\">docs</a></p>\n");
}

TEST(backslash_escapes_in_reference_definition) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]\n\n[docs]: /a\\_b 'c\\_d'")};
  EXPECT_EQ(result, "<p><a href=\"/a_b\" title=\"c_d\">docs</a></p>\n");
}
