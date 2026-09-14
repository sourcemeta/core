#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

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
  EXPECT_EQ(result, "<p>&lt;span a=&quot;b&gt; &lt;span a='b&gt;</p>\n");
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

TEST(inline_html_comment_with_double_hyphen) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <!-- a -- b -->", false)};
  EXPECT_EQ(result, "<p>x <!-- a -- b --></p>\n");
}

TEST(inline_html_comment_edge_forms) {
  const auto result{sourcemeta::core::markdown_to_html(
      "x <!--> y -->\n\nx <!---> y -->", false)};
  EXPECT_EQ(result, "<p>x <!--> y --&gt;</p>\n"
                    "<p>x <!---> y --&gt;</p>\n");
}

TEST(inline_html_processing_instruction) {
  const auto result{sourcemeta::core::markdown_to_html("x <?go run ?>", false)};
  EXPECT_EQ(result, "<p>x <?go run ?></p>\n");
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
