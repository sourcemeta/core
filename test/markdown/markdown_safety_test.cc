#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(safe_mode_strips_mixed_case_javascript_scheme) {
  const auto result{
      sourcemeta::core::markdown_to_html("[click](JaVaScRiPt:void(0))")};
  EXPECT_EQ(result, "<p><a href=\"\">click</a></p>\n");
}

TEST(safe_mode_strips_uppercase_file_scheme) {
  const auto result{
      sourcemeta::core::markdown_to_html("[local](FILE://C:/secrets.txt)")};
  EXPECT_EQ(result, "<p><a href=\"\">local</a></p>\n");
}

TEST(safe_mode_strips_data_text_plain) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](data:text/plain,hello)")};
  EXPECT_EQ(result, "<p><a href=\"\">x</a></p>\n");
}

TEST(safe_mode_keeps_data_image_gif) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![pixel](data:image/gif;base64,R0lGODlhAQABAAAAACw=)")};
  EXPECT_EQ(result,
            "<p><img src=\"data:image/gif;base64,R0lGODlhAQABAAAAACw=\" "
            "alt=\"pixel\" /></p>\n");
}

TEST(safe_mode_keeps_uppercase_data_image_jpeg_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](DATA:IMAGE/JPEG;base64,/9j/)")};
  EXPECT_EQ(result, "<p><a href=\"DATA:IMAGE/JPEG;base64,/9j/\">x</a></p>\n");
}

TEST(safe_mode_strips_data_image_bmp) {
  const auto result{
      sourcemeta::core::markdown_to_html("![x](data:image/bmp;base64,Qk0=)")};
  EXPECT_EQ(result, "<p><img src=\"\" alt=\"x\" /></p>\n");
}

TEST(safe_mode_strips_entity_encoded_javascript) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](&#x6A;avascript:void(0))")};
  EXPECT_EQ(result, "<p><a href=\"\">x</a></p>\n");
}

TEST(safe_mode_keeps_percent_encoded_colon) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x](vbscript%3Amsgbox)")};
  EXPECT_EQ(result, "<p><a href=\"vbscript%3Amsgbox\">x</a></p>\n");
}

TEST(safe_mode_strips_dangerous_image_source) {
  const auto result{
      sourcemeta::core::markdown_to_html("![x](vbscript:msgbox)")};
  EXPECT_EQ(result, "<p><img src=\"\" alt=\"x\" /></p>\n");
}

TEST(safe_mode_strips_dangerous_reference_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x][r]\n\n[r]: file:///tmp/a")};
  EXPECT_EQ(result, "<p><a href=\"\">x</a></p>\n");
}

TEST(safe_mode_strips_dangerous_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("<vbscript:msgbox>")};
  EXPECT_EQ(result, "<p><a href=\"\">vbscript:msgbox</a></p>\n");
}

TEST(safe_mode_keeps_telephone_relative_and_fragment_links) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a](tel:+123) [b](../up) [c](#anchor)")};
  EXPECT_EQ(result, "<p><a href=\"tel:+123\">a</a> <a href=\"../up\">b</a> <a "
                    "href=\"#anchor\">c</a></p>\n");
}

TEST(safe_mode_omits_inline_html_comment) {
  const auto result{
      sourcemeta::core::markdown_to_html("before <!-- hidden --> after")};
  EXPECT_EQ(result, "<p>before <!-- raw HTML omitted --> after</p>\n");
}

TEST(safe_mode_omits_processing_instruction) {
  const auto result{sourcemeta::core::markdown_to_html(
      "before <?xml version=\"1.0\"?> after")};
  EXPECT_EQ(result, "<p>before <!-- raw HTML omitted --> after</p>\n");
}

TEST(safe_mode_omits_cdata_section) {
  const auto result{
      sourcemeta::core::markdown_to_html("before <![CDATA[ data ]]> after")};
  EXPECT_EQ(result, "<p>before <!-- raw HTML omitted --> after</p>\n");
}

TEST(safe_mode_omits_declaration_block) {
  const auto result{sourcemeta::core::markdown_to_html("<!ENTITY x \"y\">")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n");
}

TEST(safe_mode_omits_html_block_but_parses_markdown_between_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<section>\n\n__strong__\n\n</section>")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n"
                    "<p><strong>strong</strong></p>\n"
                    "<!-- raw HTML omitted -->\n");
}

TEST(safe_mode_omits_html_block_with_blank_line_inside) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<div>\n<p>\n__text__\n\n_more_\n</p>\n</div>")};
  EXPECT_EQ(result, "<!-- raw HTML omitted -->\n"
                    "<p><em>more</em></p>\n"
                    "<!-- raw HTML omitted -->\n");
}

TEST(safe_mode_omits_inline_tag_with_attributes) {
  const auto result{
      sourcemeta::core::markdown_to_html("x <span style=\"color:red\"> y")};
  EXPECT_EQ(result, "<p>x <!-- raw HTML omitted --> y</p>\n");
}

TEST(safe_mode_escapes_markup_in_link_title) {
  const auto result{sourcemeta::core::markdown_to_html("[x](/y '<b>&\"</b>')")};
  EXPECT_EQ(
      result,
      "<p><a href=\"/y\" title=\"&lt;b&gt;&amp;&quot;&lt;/b&gt;\">x</a></p>\n");
}

TEST(safe_mode_escapes_quotes_in_image_title) {
  const auto result{
      sourcemeta::core::markdown_to_html(R"MD(![x](/y.png "say \"hi\""))MD")};
  EXPECT_EQ(
      result,
      "<p><img src=\"/y.png\" alt=\"x\" title=\"say &quot;hi&quot;\" /></p>\n");
}

TEST(safe_mode_omits_raw_html_inside_emphasis) {
  const auto result{
      sourcemeta::core::markdown_to_html("_<span title=\"_\">x</span>_")};
  EXPECT_EQ(
      result,
      "<p><em><!-- raw HTML omitted -->x<!-- raw HTML omitted --></em></p>\n");
}

TEST(safe_mode_omits_raw_html_taking_precedence_over_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[x <span data=\"](/y)\">")};
  EXPECT_EQ(result, "<p>[x <!-- raw HTML omitted --></p>\n");
}
