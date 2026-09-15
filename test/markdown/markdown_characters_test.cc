#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

using namespace std::literals::string_view_literals;

TEST(ampersand_in_text) {
  const auto result{sourcemeta::core::markdown_to_html("AT&T")};
  EXPECT_EQ(result, "<p>AT&amp;T</p>\n");
}

TEST(angle_brackets_in_text) {
  const auto result{sourcemeta::core::markdown_to_html("1 < 2 and 3 > 2")};
  EXPECT_EQ(result, "<p>1 &lt; 2 and 3 &gt; 2</p>\n");
}

TEST(unicode_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("Hello \xC3\xA9\xC3\xA0\xC3\xBC")};
  EXPECT_EQ(result, "<p>Hello \xC3\xA9\xC3\xA0\xC3\xBC</p>\n");
}

TEST(cjk_characters) {
  const auto result{
      sourcemeta::core::markdown_to_html("\xE4\xBD\xA0\xE5\xA5\xBD")};
  EXPECT_EQ(result, "<p>\xE4\xBD\xA0\xE5\xA5\xBD</p>\n");
}

TEST(emoji) {
  const auto result{sourcemeta::core::markdown_to_html("\xF0\x9F\x98\x80")};
  EXPECT_EQ(result, "<p>\xF0\x9F\x98\x80</p>\n");
}

TEST(invalid_utf8_is_replaced) {
  const auto result{sourcemeta::core::markdown_to_html("hello \xFF world")};
  EXPECT_EQ(result, "<p>hello \xEF\xBF\xBD world</p>\n");
}

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

TEST(text_with_special_characters) {
  const auto result{
      sourcemeta::core::markdown_to_html("cost: $5 + 10% ; done?")};
  EXPECT_EQ(result, "<p>cost: $5 + 10% ; done?</p>\n");
}

TEST(text_with_greek_characters) {
  const auto result{sourcemeta::core::markdown_to_html("λέξη και φράση")};
  EXPECT_EQ(result,
            "<p>\xce\xbb\xce\xad\xce\xbe\xce\xb7 \xce\xba\xce\xb1\xce\xb9 "
            "\xcf\x86\xcf\x81\xce\xac\xcf\x83\xce\xb7</p>\n");
}

TEST(text_internal_spaces_preserved) {
  const auto result{sourcemeta::core::markdown_to_html("wide     gap")};
  EXPECT_EQ(result, "<p>wide     gap</p>\n");
}

TEST(invalid_utf8_truncated_two_byte_sequence_at_end) {
  const auto result{sourcemeta::core::markdown_to_html("caf\xC3")};
  EXPECT_EQ(result, "<p>caf\xef\xbf\xbd</p>\n");
}

TEST(invalid_utf8_truncated_three_byte_sequence_mid_text) {
  const auto result{sourcemeta::core::markdown_to_html("x\xE2\x82"
                                                       "b")};
  EXPECT_EQ(result, "<p>x\xef\xbf\xbd"
                    "b</p>\n");
}

TEST(invalid_utf8_truncated_four_byte_sequence_before_newline) {
  const auto result{sourcemeta::core::markdown_to_html("x\xF0\x9F\x98\ny")};
  EXPECT_EQ(result, "<p>x\xef\xbf\xbd\n"
                    "y</p>\n");
}

TEST(invalid_utf8_stray_continuation_byte) {
  const auto result{sourcemeta::core::markdown_to_html("x\x80y")};
  EXPECT_EQ(result, "<p>x\xef\xbf\xbdy</p>\n");
}

TEST(invalid_utf8_consecutive_continuation_bytes) {
  const auto result{sourcemeta::core::markdown_to_html("\x80\x81\xBF")};
  EXPECT_EQ(result, "<p>\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbd</p>\n");
}

TEST(invalid_utf8_overlong_encoded_slash) {
  const auto result{sourcemeta::core::markdown_to_html("path\xC0\xAF"
                                                       "etc")};
  EXPECT_EQ(result, "<p>path\xef\xbf\xbd\xef\xbf\xbd"
                    "etc</p>\n");
}

TEST(invalid_utf8_overlong_encoded_nul) {
  const auto result{sourcemeta::core::markdown_to_html("x\xE0\x80\x80y")};
  EXPECT_EQ(result, "<p>x\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbdy</p>\n");
}

TEST(invalid_utf8_encoded_surrogate) {
  const auto result{sourcemeta::core::markdown_to_html("x\xED\xA0\x80y")};
  EXPECT_EQ(result, "<p>x\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbdy</p>\n");
}

TEST(invalid_utf8_code_point_above_unicode_range) {
  const auto result{sourcemeta::core::markdown_to_html("x\xF4\x90\x80\x80y")};
  EXPECT_EQ(result,
            "<p>x\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbdy</p>\n");
}

TEST(invalid_utf8_five_byte_sequence) {
  const auto result{
      sourcemeta::core::markdown_to_html("x\xF8\x88\x80\x80\x80y")};
  EXPECT_EQ(result,
            "<p>x\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbdy<"
            "/p>\n");
}

TEST(invalid_utf8_lead_byte_followed_by_ascii) {
  const auto result{sourcemeta::core::markdown_to_html("\xE4"
                                                       "abc")};
  EXPECT_EQ(result, "<p>\xef\xbf\xbd"
                    "abc</p>\n");
}

TEST(invalid_utf8_bytes_fe_and_ff) {
  const auto result{sourcemeta::core::markdown_to_html("\xFE\xFF text")};
  EXPECT_EQ(result, "<p>\xef\xbf\xbd\xef\xbf\xbd text</p>\n");
}

TEST(invalid_utf8_in_heading) {
  const auto result{sourcemeta::core::markdown_to_html("# t\xFFitle")};
  EXPECT_EQ(result, "<h1>t\xef\xbf\xbditle</h1>\n");
}

TEST(invalid_utf8_in_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("`a\xFF"
                                                       "b`")};
  EXPECT_EQ(result, "<p><code>a\xef\xbf\xbd"
                    "b</code></p>\n");
}

TEST(invalid_utf8_in_fenced_code) {
  const auto result{sourcemeta::core::markdown_to_html("```\n\xC3\n```")};
  EXPECT_EQ(result, "<pre><code>\xef\xbf\xbd\n"
                    "</code></pre>\n");
}

TEST(invalid_utf8_in_fenced_code_info_string) {
  const auto result{sourcemeta::core::markdown_to_html("```l\xFF"
                                                       "ang\ncode\n```")};
  EXPECT_EQ(result, "<pre><code class=\"language-l\xef\xbf\xbd"
                    "ang\">code\n"
                    "</code></pre>\n");
}

TEST(invalid_utf8_in_indented_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("    \xE2\x82\n    next")};
  EXPECT_EQ(result, "<pre><code>\xef\xbf\xbd\n"
                    "next\n"
                    "</code></pre>\n");
}

TEST(invalid_utf8_in_link_destination) {
  const auto result{sourcemeta::core::markdown_to_html("[x](/a\xFF"
                                                       "b)")};
  EXPECT_EQ(result, "<p><a href=\"/a%EF%BF%BDb\">x</a></p>\n");
}

TEST(invalid_utf8_in_link_title) {
  const auto result{sourcemeta::core::markdown_to_html("[x](/a 't\xC3')")};
  EXPECT_EQ(result, "<p><a href=\"/a\" title=\"t\xef\xbf\xbd\">x</a></p>\n");
}

TEST(invalid_utf8_in_reference_label) {
  const auto result{
      sourcemeta::core::markdown_to_html("[l\xFF]\n\n[l\xFF]: /x")};
  EXPECT_EQ(result, "<p><a href=\"/x\">l\xef\xbf\xbd</a></p>\n");
}

TEST(invalid_utf8_in_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/\xFF>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%EF%BF%BD\">https://"
                    "sourcemeta.com/\xef\xbf\xbd</a></p>\n");
}

TEST(invalid_utf8_in_extended_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.sourcemeta.com/\xC3path")};
  EXPECT_EQ(result,
            "<p><a "
            "href=\"http://www.sourcemeta.com/"
            "%EF%BF%BDpath\">www.sourcemeta.com/\xef\xbf\xbdpath</a></p>\n");
}

TEST(invalid_utf8_in_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a |\n| - |\n| \xFF |")};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>\xef\xbf\xbd</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(invalid_utf8_in_image_description) {
  const auto result{sourcemeta::core::markdown_to_html("![\xC3](/i.png)")};
  EXPECT_EQ(result, "<p><img src=\"/i.png\" alt=\"\xef\xbf\xbd\" /></p>\n");
}

TEST(invalid_utf8_between_emphasis_delimiters) {
  const auto result{sourcemeta::core::markdown_to_html("*\xFF*")};
  EXPECT_EQ(result, "<p><em>\xef\xbf\xbd</em></p>\n");
}

TEST(invalid_utf8_in_footnote_label) {
  const auto result{
      sourcemeta::core::markdown_to_html("x[^\xFF]\n\n[^\xFF]: note")};
  EXPECT_EQ(result,
            "<p>x<sup class=\"footnote-ref\"><a href=\"#fn-%EF%BF%BD\" "
            "id=\"fnref-%EF%BF%BD\" data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-%EF%BF%BD\">\n"
            "<p>note <a href=\"#fnref-%EF%BF%BD\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(invalid_utf8_inside_entity_like_text) {
  const auto result{sourcemeta::core::markdown_to_html("&\xFF"
                                                       "amp;")};
  EXPECT_EQ(result, "<p>&amp;\xef\xbf\xbd"
                    "amp;</p>\n");
}

TEST(nul_character_at_start) {
  const auto result{sourcemeta::core::markdown_to_html("\0text"sv)};
  EXPECT_EQ(result, "<p>\xef\xbf\xbdtext</p>\n");
}

TEST(nul_character_in_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("`x\0y`"sv)};
  EXPECT_EQ(result, "<p><code>x\xef\xbf\xbdy</code></p>\n");
}

TEST(nul_character_in_fenced_code) {
  const auto result{sourcemeta::core::markdown_to_html("```\nx\0y\n```"sv)};
  EXPECT_EQ(result, "<pre><code>x\xef\xbf\xbdy\n"
                    "</code></pre>\n");
}

TEST(nul_character_in_link_destination) {
  const auto result{sourcemeta::core::markdown_to_html("[x](/a\0z)"sv)};
  EXPECT_EQ(result, "<p><a href=\"/a%EF%BF%BDz\">x</a></p>\n");
}

TEST(nul_character_in_heading) {
  const auto result{sourcemeta::core::markdown_to_html("# a\0"sv)};
  EXPECT_EQ(result, "<h1>a\xef\xbf\xbd</h1>\n");
}

TEST(nul_character_in_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| a |\n| - |\n| \0 |"sv)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>a</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>\xef\xbf\xbd</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(start_of_heading_control_character_preserved) {
  const auto result{sourcemeta::core::markdown_to_html("x\x01y")};
  EXPECT_EQ(result, "<p>x\x01y</p>\n");
}

TEST(delete_character_preserved) {
  const auto result{sourcemeta::core::markdown_to_html("x\x7Fy")};
  EXPECT_EQ(result, "<p>x\x7fy</p>\n");
}

TEST(form_feed_between_words) {
  const auto result{sourcemeta::core::markdown_to_html("a\fz")};
  EXPECT_EQ(result, "<p>a\x0cz</p>\n");
}

TEST(vertical_tab_between_words) {
  const auto result{sourcemeta::core::markdown_to_html("a\vz")};
  EXPECT_EQ(result, "<p>a\x0bz</p>\n");
}

TEST(form_feed_between_thematic_break_characters) {
  const auto result{sourcemeta::core::markdown_to_html("-\f-\f-")};
  EXPECT_EQ(result, "<p>-\x0c-\x0c-</p>\n");
}

TEST(lone_carriage_return_at_end) {
  const auto result{sourcemeta::core::markdown_to_html("text\r")};
  EXPECT_EQ(result, "<p>text</p>\n");
}

TEST(carriage_return_before_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("a\r* b")};
  EXPECT_EQ(result, "<p>a</p>\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n");
}

TEST(crlf_after_trailing_spaces_is_hard_break) {
  const auto result{sourcemeta::core::markdown_to_html("a  \r\nb")};
  EXPECT_EQ(result, "<p>a<br />\n"
                    "b</p>\n");
}

TEST(non_breaking_space_after_list_marker_is_not_list) {
  const auto result{sourcemeta::core::markdown_to_html("*\xC2\xA0item")};
  EXPECT_EQ(result, "<p>*&nbsp;item</p>\n");
}

TEST(em_spaces_do_not_indent_code) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83\xE2\x80\x83"
      "code")};
  EXPECT_EQ(result, "<p>\xe2\x80\x83\xe2\x80\x83\xe2\x80\x83\xe2\x80\x83"
                    "code</p>\n");
}

TEST(zero_width_joiner_sequence_preserved) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x92\xBB")};
  EXPECT_EQ(result, "<p>\xf0\x9f\x91\xa8\xe2\x80\x8d\xf0\x9f\x92\xbb</p>\n");
}

TEST(combining_accent_preserved) {
  const auto result{sourcemeta::core::markdown_to_html("e\xCC\x81")};
  EXPECT_EQ(result, "<p>e\xcc\x81</p>\n");
}

TEST(right_to_left_text_preserved) {
  const auto result{
      sourcemeta::core::markdown_to_html("\xD7\xA9\xD7\x9C\xD7\x95\xD7\x9D")};
  EXPECT_EQ(result, "<p>\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d</p>\n");
}

TEST(byte_order_mark_after_first_line) {
  const auto result{sourcemeta::core::markdown_to_html("a\n\xEF\xBB\xBF"
                                                       "b")};
  EXPECT_EQ(result, "<p>a\n"
                    "\xef\xbb\xbf"
                    "b</p>\n");
}

TEST(four_byte_character_in_link_destination) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](/\xF0\x9F\x98\x80)")};
  EXPECT_EQ(result, "<p><a href=\"/%F0%9F%98%80\">x</a></p>\n");
}

TEST(non_ascii_link_destination_is_percent_encoded) {
  const auto result{sourcemeta::core::markdown_to_html("[x](/caf\xC3\xA9)")};
  EXPECT_EQ(result, "<p><a href=\"/caf%C3%A9\">x</a></p>\n");
}

TEST(quotes_in_text_are_escaped) {
  const auto result{sourcemeta::core::markdown_to_html("say \"hi\" and 'bye'")};
  EXPECT_EQ(result, "<p>say &quot;hi&quot; and &#39;bye&#39;</p>\n");
}

TEST(nul_characters_are_replaced) {
  const auto result{sourcemeta::core::markdown_to_html("one\0two\0"sv)};
  EXPECT_EQ(result, "<p>one\xef\xbf\xbdtwo\xef\xbf\xbd</p>\n");
}
