#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

#include <string_view> // std::string_view

using namespace std::literals::string_view_literals;

TEST(html_block_script_with_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<script type=\"module\">\n\nconst x = 1;\n\n</script>\nafter", false)};
  EXPECT_EQ(result, "&lt;script type=\"module\">\n"
                    "\n"
                    "const x = 1;\n"
                    "\n"
                    "&lt;/script>\n"
                    "<p>after</p>\n");
}

TEST(html_block_pre_keeps_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<pre>\nline one\n\n*not emphasis*\n</pre>\nafter", false)};
  EXPECT_EQ(result, "<pre>\n"
                    "line one\n"
                    "\n"
                    "*not emphasis*\n"
                    "</pre>\n"
                    "<p>after</p>\n");
}

TEST(html_block_style_with_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<style>\n\n.x { color: red; }\n\n</style>\nafter", false)};
  EXPECT_EQ(result, "&lt;style>\n"
                    "\n"
                    ".x { color: red; }\n"
                    "\n"
                    "&lt;/style>\n"
                    "<p>after</p>\n");
}

TEST(html_block_comment_with_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<!--\n\n*hidden*\n\n-->\nafter", false)};
  EXPECT_EQ(result, "<!--\n"
                    "\n"
                    "*hidden*\n"
                    "\n"
                    "-->\n"
                    "<p>after</p>\n");
}

TEST(html_block_processing_instruction) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<?xml\n\nversion=\"1.0\"\n\n?>\nafter", false)};
  EXPECT_EQ(result, "<?xml\n"
                    "\n"
                    "version=\"1.0\"\n"
                    "\n"
                    "?>\n"
                    "<p>after</p>\n");
}

TEST(html_block_declaration) {
  const auto result{
      sourcemeta::core::markdown_to_html("<!ENTITY example \"value\">", false)};
  EXPECT_EQ(result, "<!ENTITY example \"value\">\n");
}

TEST(html_block_cdata_section) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<![CDATA[\nif (a < b) {\n\n  go();\n}\n]]>\nafter", false)};
  EXPECT_EQ(result, "<![CDATA[\n"
                    "if (a < b) {\n"
                    "\n"
                    "  go();\n"
                    "}\n"
                    "]]>\n"
                    "<p>after</p>\n");
}

TEST(html_block_ends_at_blank_line) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<div>\n*one*\n\n*two*\n</div>", false)};
  EXPECT_EQ(result, "<div>\n"
                    "*one*\n"
                    "<p><em>two</em></p>\n"
                    "</div>\n");
}

TEST(html_block_partial_opening_tag) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<section class=\"a\"\n  id=\"b\">\n</section>", false)};
  EXPECT_EQ(result, "<section class=\"a\"\n"
                    "  id=\"b\">\n"
                    "</section>\n");
}

TEST(html_block_starting_with_closing_tag) {
  const auto result{
      sourcemeta::core::markdown_to_html("</section>\n_text_", false)};
  EXPECT_EQ(result, "</section>\n"
                    "_text_\n");
}

TEST(html_block_complete_inline_tag) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<span class=\"x\">\n_text_\n</span>", false)};
  EXPECT_EQ(result, "<span class=\"x\">\n"
                    "_text_\n"
                    "</span>\n");
}

TEST(html_block_complete_tag_cannot_interrupt_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text\n<span>\nmore", false)};
  EXPECT_EQ(result, "<p>Text\n"
                    "<span>\n"
                    "more</p>\n");
}

TEST(html_block_section_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Text\n<section>\nmore\n</section>", false)};
  EXPECT_EQ(result, "<p>Text</p>\n"
                    "<section>\n"
                    "more\n"
                    "</section>\n");
}

TEST(html_block_indented_four_spaces_is_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("    <div>code</div>", false)};
  EXPECT_EQ(result, "<pre><code>&lt;div&gt;code&lt;/div&gt;\n"
                    "</code></pre>\n");
}

TEST(html_block_inside_list_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("* <section>\n* text", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<section>\n"
                    "</li>\n"
                    "<li>text</li>\n"
                    "</ul>\n");
}

TEST(inline_html_open_tags) {
  const auto result{sourcemeta::core::markdown_to_html("<x><yy1><z-z>", false)};
  EXPECT_EQ(result, "<p><x><yy1><z-z></p>\n");
}

TEST(inline_html_empty_elements) {
  const auto result{sourcemeta::core::markdown_to_html("<br/><hr />", false)};
  EXPECT_EQ(result, "<p><br/><hr /></p>\n");
}

TEST(inline_html_attributes_across_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<span\n  data-x='1'\n  data-y=2>", false)};
  EXPECT_EQ(result, "<p><span\n"
                    "data-x='1'\n"
                    "data-y=2></p>\n");
}

TEST(inline_html_invalid_tag_names) {
  const auto result{sourcemeta::core::markdown_to_html("<1x> <-x>", false)};
  EXPECT_EQ(result, "<p>&lt;1x&gt; &lt;-x&gt;</p>\n");
}

TEST(inline_html_invalid_attribute_name) {
  const auto result{
      sourcemeta::core::markdown_to_html("<span 1x=\"y\">", false)};
  EXPECT_EQ(result, "<p>&lt;span 1x=&quot;y&quot;&gt;</p>\n");
}

TEST(inline_html_invalid_attribute_values) {
  const auto result{
      sourcemeta::core::markdown_to_html("<span a=\"b> <span a='b>", false)};
  EXPECT_EQ(result, "<p>&lt;span a=&quot;b&gt; &lt;span a=&#39;b&gt;</p>\n");
}

TEST(inline_html_invalid_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html(
      "< span> <span/ > <span a=b\n c!d>", false)};
  EXPECT_EQ(result, "<p>&lt; span&gt; &lt;span/ &gt; &lt;span a=b\n"
                    "c!d&gt;</p>\n");
}

TEST(inline_html_closing_tags) {
  const auto result{
      sourcemeta::core::markdown_to_html("</span></section >", false)};
  EXPECT_EQ(result, "<p></span></section ></p>\n");
}

TEST(inline_html_closing_tag_with_attributes_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("</span class=\"x\">", false)};
  EXPECT_EQ(result, "<p>&lt;/span class=&quot;x&quot;&gt;</p>\n");
}

TEST(inline_html_comment_text_cannot_contain_two_hyphens) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <!-- a -- b -->", false)};
  EXPECT_EQ(result, "<p>x &lt;!-- a -- b --&gt;</p>\n");
}

TEST(inline_html_comment_text_cannot_start_with_greater_than) {
  const auto result{sourcemeta::core::markdown_to_html(
      "x <!--> y -->\n\nx <!---> y -->", false)};
  EXPECT_EQ(result, "<p>x &lt;!--&gt; y --&gt;</p>\n"
                    "<p>x &lt;!---&gt; y --&gt;</p>\n");
}

TEST(inline_html_comment_text_cannot_end_with_hyphen) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <!-- foo--->", false)};
  EXPECT_EQ(result, "<p>foo &lt;!-- foo---&gt;</p>\n");
}

TEST(inline_html_comment_spanning_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "foo <!-- this is a\ncomment - with hyphen -->", false)};
  EXPECT_EQ(result, "<p>foo <!-- this is a\n"
                    "comment - with hyphen --></p>\n");
}

TEST(inline_html_empty_comment) {
  const auto result{sourcemeta::core::markdown_to_html("x <!----> y", false)};
  EXPECT_EQ(result, "<p>x <!----> y</p>\n");
}

TEST(inline_html_cdata_after_text_that_is_not_a_comment) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <!-- a -- b <![CDATA[c]]>", false)};
  EXPECT_EQ(result, "<p>x &lt;!-- a -- b <![CDATA[c]]></p>\n");
}

TEST(inline_html_processing_instruction) {
  const auto result{sourcemeta::core::markdown_to_html("x <?go run ?>", false)};
  EXPECT_EQ(result, "<p>x <?go run ?></p>\n");
}

TEST(inline_html_processing_instruction_with_consecutive_question_marks) {
  const auto result{sourcemeta::core::markdown_to_html("x <?go ?\?>", false)};
  EXPECT_EQ(result, "<p>x <?go ?\?></p>\n");
}

TEST(inline_html_declaration) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <!DOCTYPE svg>", false)};
  EXPECT_EQ(result, "<p>x <!DOCTYPE svg></p>\n");
}

TEST(inline_html_cdata_section) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <![CDATA[a < b & c]]>", false)};
  EXPECT_EQ(result, "<p>x <![CDATA[a < b & c]]></p>\n");
}

TEST(inline_html_entity_in_attribute_is_preserved) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <span title=\"&copy;\">", false)};
  EXPECT_EQ(result, "<p>x <span title=\"&copy;\"></p>\n");
}

TEST(inline_html_backslash_in_attribute_is_preserved) {
  const auto result{
      sourcemeta::core::markdown_to_html(R"MD(x <span title="\_">)MD", false)};
  EXPECT_EQ(result, "<p>x <span title=\"\\_\"></p>\n");
}

TEST(tagfilter_every_disallowed_tag) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<plaintext> <script> <noframes> <noembed> <iframe> <xmp> <style> "
      "<textarea> <title>",
      false)};
  EXPECT_EQ(result,
            "<p>&lt;plaintext> &lt;script> &lt;noframes> &lt;noembed> "
            "&lt;iframe> &lt;xmp> &lt;style> &lt;textarea> &lt;title></p>\n");
}

TEST(tagfilter_mixed_allowed_and_disallowed_tags) {
  const auto result{
      sourcemeta::core::markdown_to_html("<b> <iframe src=\"x\"> <i>", false)};
  EXPECT_EQ(result, "<p><b> &lt;iframe src=\"x\"> <i></p>\n");
}

TEST(tagfilter_closing_disallowed_tags) {
  const auto result{
      sourcemeta::core::markdown_to_html("</textarea> </xmp>", false)};
  EXPECT_EQ(result, "<p>&lt;/textarea> &lt;/xmp></p>\n");
}

TEST(tagfilter_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("<IfRaMe src=\"x\"></iFrAmE>", false)};
  EXPECT_EQ(result, "&lt;IfRaMe src=\"x\">&lt;/iFrAmE>\n");
}

TEST(tagfilter_inside_html_block) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<div>\n<noembed>x</noembed> <TITLE>y</TITLE>\n</div>", false)};
  EXPECT_EQ(result, "<div>\n"
                    "&lt;noembed>x&lt;/noembed> &lt;TITLE>y&lt;/TITLE>\n"
                    "</div>\n");
}

TEST(unsafe_mode_keeps_uppercase_file_scheme) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](FILE://C:/secrets.txt)", false)};
  EXPECT_EQ(result, "<p><a href=\"FILE://C:/secrets.txt\">x</a></p>\n");
}

TEST(unsafe_mode_keeps_dangerous_image_source) {
  const auto result{
      sourcemeta::core::markdown_to_html("![x](vbscript:msgbox)", false)};
  EXPECT_EQ(result, "<p><img src=\"vbscript:msgbox\" alt=\"x\" /></p>\n");
}

TEST(unsafe_mode_keeps_raw_html_inside_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html(
      "_<span title=\"_\">x</span>_", false)};
  EXPECT_EQ(result, "<p><em><span title=\"_\">x</span></em></p>\n");
}

TEST(unsafe_mode_raw_html_takes_precedence_over_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x <span data=\"](/y)\">", false)};
  EXPECT_EQ(result, "<p>[x <span data=\"](/y)\"></p>\n");
}

TEST(unsafe_mode_list_separated_by_html_comment) {
  const auto result{sourcemeta::core::markdown_to_html(
      "* one\n\n<!-- split -->\n\n* two", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<!-- split -->\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(unsafe_mode_passes_raw_html) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div onclick=\"x\">hi</div>", false)};
  EXPECT_EQ(result, "<div onclick=\"x\">hi</div>\n");
}

TEST(unsafe_mode_keeps_dangerous_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[click](javascript:alert(1))", false)};
  EXPECT_EQ(result, "<p><a href=\"javascript:alert(1)\">click</a></p>\n");
}

TEST(unsafe_mode_invalid_utf8_inside_html_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div>\xFF</div>", false)};
  EXPECT_EQ(result, "<div>\xef\xbf\xbd</div>\n");
}

TEST(unsafe_mode_invalid_utf8_inside_inline_html_attribute) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <span title=\"\xC3\">", false)};
  EXPECT_EQ(result, "<p>x <span title=\"\xef\xbf\xbd\"></p>\n");
}

TEST(unsafe_mode_nul_character_inside_inline_html) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <span title=\"a\0z\">"sv, false)};
  EXPECT_EQ(result, "<p>x <span title=\"a\xef\xbf\xbdz\"></p>\n");
}

TEST(tagfilter_tag_name_ends_at_solidus_and_form_feed) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<div>\n<script/x>a</script> <style\f>b</style>\n</div>", false)};
  EXPECT_EQ(result, "<div>\n"
                    "&lt;script/x>a&lt;/script> &lt;style\f>b&lt;/style>\n"
                    "</div>\n");
}

TEST(tagfilter_vertical_tab_does_not_end_tag_name) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div>\n<xmp\v>a\n</div>", false)};
  EXPECT_EQ(result, "<div>\n"
                    "<xmp\v>a\n"
                    "</div>\n");
}

TEST(tagfilter_tag_name_ended_by_a_tab) {
  // The HTML tokenizer ends a tag name at a tab just as it does at a space, so
  // the tag is still one of the disallowed ones and its opening angle bracket
  // is escaped
  const auto result{
      sourcemeta::core::markdown_to_html("x <script\tsrc=y>", false)};
  EXPECT_EQ(result, "<p>x &lt;script\tsrc=y></p>\n");
}

TEST(inline_html_attribute_with_an_empty_value) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a href=>x</a>", false)};
  EXPECT_EQ(result, "<p>&lt;a href=&gt;x</a></p>\n");
}

TEST(inline_html_self_closing_tag) {
  const auto result{sourcemeta::core::markdown_to_html("<br/>x", false)};
  EXPECT_EQ(result, "<p><br/>x</p>\n");
}

TEST(inline_html_declaration_that_is_not_a_comment) {
  const auto result{sourcemeta::core::markdown_to_html("<!-x>", false)};
  EXPECT_EQ(result, "<p>&lt;!-x&gt;</p>\n");
}

// CommonMark 0.29 section 4.6 condition 4 opens an HTML block on `<!` followed
// by a letter, so this is a block rather than an inline declaration
TEST(html_block_declaration_without_a_space) {
  const auto result{sourcemeta::core::markdown_to_html("<!A>", false)};
  EXPECT_EQ(result, "<!A>\n");
}

TEST(inline_html_cdata_with_a_wrong_prefix) {
  const auto result{sourcemeta::core::markdown_to_html("<![CDAT[x]]>", false)};
  EXPECT_EQ(result, "<p>&lt;![CDAT[x]]&gt;</p>\n");
}

// Section 4.6 condition 5 ends the section at the first `]]>`, so an interior
// pair of brackets is content rather than a terminator
TEST(html_block_cdata_with_an_interior_double_bracket) {
  const auto result{
      sourcemeta::core::markdown_to_html("<![CDATA[a]]b]]>x", false)};
  EXPECT_EQ(result, "<![CDATA[a]]b]]>x\n");
}

// CommonMark section 6.6 gives a closing tag a name, so a tag with none is
// text rather than HTML
TEST(html_closing_tag_without_a_name_is_text) {
  const auto result{sourcemeta::core::markdown_to_html("a </> b"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;/&gt; b</p>\n");
}

// GFM section 4.6 start condition 6 admits a tag closed with a slash straight
// after its name, which no spec example carries
TEST(html_block_tag_closed_after_its_name) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div/>\ntext"sv, false)};
  EXPECT_EQ(result, "<div/>\ntext\n");
}

// The scanner lowercases a tag name into a sixteen byte buffer to weigh it
// against the names that open a block of their own. A seventeenth letter is
// one more than the buffer holds, so the name is abandoned rather than the
// buffer overrun, and CommonMark section 4.6 start condition 7 takes the tag
// as an ordinary one
TEST(html_block_tag_name_longer_than_the_lowercase_buffer) {
  const auto result{
      sourcemeta::core::markdown_to_html("<abcdefghijklmnopq>\n"sv, false)};
  EXPECT_EQ(result, "<abcdefghijklmnopq>\n");
}

// CommonMark section 6.6 gives a declaration an ASCII letter and then
// whitespace, so this one is neither a declaration nor HTML
TEST(html_inline_declaration_without_whitespace_is_text) {
  const auto result{sourcemeta::core::markdown_to_html("a <!X> b"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;!X&gt; b</p>\n");
}

// A CDATA section ends at the first "]]>", so a "]]" that is not followed by
// the closing angle bracket keeps the section open
TEST(html_inline_cdata_with_a_bracket_pair_inside) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <![CDATA[x]]y]]> b"sv, false)};
  EXPECT_EQ(result, "<p>a <![CDATA[x]]y]]> b</p>\n");
}

// CommonMark section 6.6 gives an attribute name as
// [A-Za-z_:][A-Za-z0-9_.:-]*, so this character belongs in one
TEST(html_inline_attribute_name_with_a_colon) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a <span xml:lang=\"en\">b</span> c"sv, false)};
  EXPECT_EQ(result, "<p>a <span xml:lang=\"en\">b</span> c</p>\n");
}

// CommonMark section 6.6 gives an attribute name as
// [A-Za-z_:][A-Za-z0-9_.:-]*, so this character belongs in one
TEST(html_inline_attribute_name_with_a_dot) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a <span a.b=\"1\">b</span> c"sv, false)};
  EXPECT_EQ(result, "<p>a <span a.b=\"1\">b</span> c</p>\n");
}

// CommonMark section 6.6 gives an attribute name as
// [A-Za-z_:][A-Za-z0-9_.:-]*, so this character belongs in one
TEST(html_inline_attribute_name_with_a_hyphen) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a <span data-x=\"1\">b</span> c"sv, false)};
  EXPECT_EQ(result, "<p>a <span data-x=\"1\">b</span> c</p>\n");
}

// Section 6.6 excludes this character from an unquoted attribute value, so the
// tag is no tag and the text stands as written
TEST(html_inline_unquoted_attribute_value_with_a_single_quote) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <span x=a'b>c"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;span x=a&#39;b&gt;c</p>\n");
}

// Section 6.6 excludes this character from an unquoted attribute value, so the
// tag is no tag and the text stands as written
TEST(html_inline_unquoted_attribute_value_with_an_equals_sign) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <span x=a=b>c"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;span x=a=b&gt;c</p>\n");
}

// Section 6.6 excludes this character from an unquoted attribute value, so the
// tag is no tag and the text stands as written
TEST(html_inline_unquoted_attribute_value_with_a_less_than_sign) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <span x=a<b>c"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;span x=a<b>c</p>\n");
}

// Section 6.6 excludes this character from an unquoted attribute value, so the
// tag is no tag and the text stands as written
TEST(html_inline_unquoted_attribute_value_with_a_backtick) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <span x=a`b>c"sv, false)};
  EXPECT_EQ(result, "<p>a &lt;span x=a`b&gt;c</p>\n");
}

// GFM section 4.6 start condition 1 admits whitespace after the tag name, and
// the whitespace raw HTML allows includes the vertical tab and the form feed
TEST(html_block_pre_opened_with_a_line_tabulation) {
  const auto result{
      sourcemeta::core::markdown_to_html("<pre\vx>a</pre>\nafter", false)};
  EXPECT_EQ(result, "<pre\vx>a</pre>\n<p>after</p>\n");
}

TEST(html_block_pre_opened_with_a_form_feed) {
  const auto result{
      sourcemeta::core::markdown_to_html("<pre\fx>a</pre>\nafter", false)};
  EXPECT_EQ(result, "<pre\fx>a</pre>\n<p>after</p>\n");
}

// The end condition of start condition 1 is a closing tag of one of its four
// names, so a closing tag of another name leaves the block open, and one of
// those names that is not closed by an angle bracket does too
TEST(html_block_pre_ignores_a_closing_tag_of_another_name) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<pre>\n</div>\n</pre>\nafter", false)};
  EXPECT_EQ(result, "<pre>\n</div>\n</pre>\n<p>after</p>\n");
}

TEST(html_block_pre_ignores_a_closing_tag_without_an_angle_bracket) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<pre>\n</pre x\n</pre>\nafter", false)};
  EXPECT_EQ(result, "<pre>\n</pre x\n</pre>\n<p>after</p>\n");
}

// GFM section 6.11 reads a comment as raw HTML only when it closes, and the
// text may neither open with an angle bracket nor be a single dash
TEST(inline_html_comment_without_a_closing_is_not_raw_html) {
  const auto result{sourcemeta::core::markdown_to_html("a <!--b c", false)};
  EXPECT_EQ(result, "<p>a &lt;!--b c</p>\n");
}

TEST(inline_html_declaration_without_an_angle_bracket_is_not_raw_html) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <!DOCTYPE html", false)};
  EXPECT_EQ(result, "<p>a &lt;!DOCTYPE html</p>\n");
}

// GFM section 6.11 reads an attribute name as "a letter, _, or :, followed by
// zero or more ASCII letters, digits, _, ., :, or -"
TEST(html_inline_attribute_name_starting_with_a_colon) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <b :x=\"1\">c</b>", false)};
  EXPECT_EQ(result, "<p>a <b :x=\"1\">c</b></p>\n");
}

TEST(html_inline_attribute_name_carrying_an_underscore) {
  const auto result{
      sourcemeta::core::markdown_to_html("a <b data_x=\"1\">c</b>", false)};
  EXPECT_EQ(result, "<p>a <b data_x=\"1\">c</b></p>\n");
}

// The same section ends an unquoted attribute value at whitespace or at any of
// ", ', =, <, > and `
TEST(html_inline_unquoted_attribute_value_ends_at_a_single_quote) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y'z>c", false)};
  EXPECT_EQ(result, "<p>a &lt;b x=y&#39;z&gt;c</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_an_equals_sign) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y=z>c", false)};
  EXPECT_EQ(result, "<p>a &lt;b x=y=z&gt;c</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_a_line_tabulation) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y\vz>c", false)};
  EXPECT_EQ(result, "<p>a <b x=y\vz>c</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_a_form_feed) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y\fz>c", false)};
  EXPECT_EQ(result, "<p>a <b x=y\fz>c</p>\n");
}

// The other two names of GFM section 4.6 start condition 1
TEST(html_block_textarea) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<textarea>\na\n</textarea>\nafter", false)};
  EXPECT_EQ(result, "&lt;textarea>\na\n&lt;/textarea>\n<p>after</p>\n");
}

TEST(html_block_style) {
  const auto result{
      sourcemeta::core::markdown_to_html("<style>\na\n</style>\nafter", false)};
  EXPECT_EQ(result, "&lt;style>\na\n&lt;/style>\n<p>after</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_a_space) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y z>c", false)};
  EXPECT_EQ(result, "<p>a <b x=y z>c</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_a_tabulation) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y\tz>c", false)};
  EXPECT_EQ(result, "<p>a <b x=y\tz>c</p>\n");
}

TEST(html_inline_unquoted_attribute_value_ends_at_a_double_quote) {
  const auto result{sourcemeta::core::markdown_to_html("a <b x=y\"z>c", false)};
  EXPECT_EQ(result, "<p>a &lt;b x=y&quot;z&gt;c</p>\n");
}

// Start condition 1 needs whitespace or a right angle bracket after the tag
// name, and condition 7 leaves the raw text elements out, so a self-closing
// one of those opens no block at all
TEST(html_block_self_closing_script_opens_no_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("<script/>\ntext", false)};
  EXPECT_EQ(result, "<p>&lt;script/>\ntext</p>\n");
}

TEST(html_block_closing_script_tag_opens_the_complete_tag_condition) {
  const auto result{
      sourcemeta::core::markdown_to_html("</script>\ntext", false)};
  EXPECT_EQ(result, "&lt;/script>\ntext\n");
}

// Start condition 6 needs the same, and the slash it also admits has to be the
// one that closes the tag
TEST(html_block_block_tag_followed_by_an_equals_sign_opens_nothing) {
  const auto result{sourcemeta::core::markdown_to_html("<div=x>\ntext", false)};
  EXPECT_EQ(result, "<p>&lt;div=x&gt;\ntext</p>\n");
}

TEST(html_block_block_tag_with_a_slash_that_closes_nothing_opens_nothing) {
  const auto result{sourcemeta::core::markdown_to_html("<div/x>\ntext", false)};
  EXPECT_EQ(result, "<p>&lt;div/x&gt;\ntext</p>\n");
}

// Start condition 7 takes a complete tag followed by nothing but whitespace,
// which GFM section 2.1 counts a tabulation among
TEST(html_block_complete_tag_followed_by_a_tabulation) {
  const auto result{
      sourcemeta::core::markdown_to_html("<del>\t\ntext\n\nafter", false)};
  EXPECT_EQ(result, "<del>\t\ntext\n<p>after</p>\n");
}

TEST(html_block_complete_tag_followed_by_a_form_feed) {
  const auto result{
      sourcemeta::core::markdown_to_html("<del>\f\ntext\n\nafter", false)};
  EXPECT_EQ(result, "<del>\f\ntext\n<p>after</p>\n");
}
