#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(html_entity_named) {
  const auto result{sourcemeta::core::markdown_to_html("&copy; 2025")};
  EXPECT_EQ(result, "<p>\xC2\xA9 2025</p>\n");
}

TEST(html_entity_numeric) {
  const auto result{sourcemeta::core::markdown_to_html("&#169; 2025")};
  EXPECT_EQ(result, "<p>\xC2\xA9 2025</p>\n");
}

TEST(entity_named_references) {
  const auto result{sourcemeta::core::markdown_to_html(
      "&lt; &gt; &quot; &apos; &euro; &hellip;\n&rightarrow; "
      "&NotNestedLessLess; &bigstar;")};
  EXPECT_EQ(result, "<p>&lt; &gt; &quot; &#39; \xe2\x82\xac \xe2\x80\xa6\n"
                    "\xe2\x86\x92 \xe2\xaa\xa1\xcc\xb8 \xe2\x98\x85</p>\n");
}

TEST(entity_decimal_references) {
  const auto result{
      sourcemeta::core::markdown_to_html("&#65; &#8364; &#128512;")};
  EXPECT_EQ(result, "<p>A \xe2\x82\xac \xf0\x9f\x98\x80</p>\n");
}

TEST(entity_hexadecimal_references) {
  const auto result{
      sourcemeta::core::markdown_to_html("&#x41; &#X20AC; &#x1F600;")};
  EXPECT_EQ(result, "<p>A \xe2\x82\xac \xf0\x9f\x98\x80</p>\n");
}

TEST(entity_invalid_references_are_literal) {
  const auto result{sourcemeta::core::markdown_to_html(
      "&euro &y; &#z; &#xz;\n&#12345678;\n&#x1234567;\n&NotARealEntity;")};
  EXPECT_EQ(result, "<p>&amp;euro &amp;y; &amp;#z; &amp;#xz;\n"
                    "&amp;#12345678;\n"
                    "&amp;#x1234567;\n"
                    "&amp;NotARealEntity;</p>\n");
}

TEST(entity_without_semicolon_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("&amp")};
  EXPECT_EQ(result, "<p>&amp;amp</p>\n");
}

TEST(entity_unknown_name_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("&notreal;")};
  EXPECT_EQ(result, "<p>&amp;notreal;</p>\n");
}

TEST(entity_code_point_beyond_unicode_range) {
  const auto result{sourcemeta::core::markdown_to_html("&#x110000;")};
  EXPECT_EQ(result, "<p>\xef\xbf\xbd</p>\n");
}

TEST(entity_surrogate_code_point) {
  const auto result{sourcemeta::core::markdown_to_html("&#56320;")};
  EXPECT_EQ(result, "<p>\xef\xbf\xbd</p>\n");
}

TEST(entity_in_link_destination_and_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/caf&eacute; 'caf&eacute;')")};
  EXPECT_EQ(result,
            "<p><a href=\"/caf%C3%A9\" title=\"caf\xc3\xa9\">docs</a></p>\n");
}

TEST(entity_in_reference_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs]\n\n[docs]: /caf&eacute; 'caf&eacute;'")};
  EXPECT_EQ(result,
            "<p><a href=\"/caf%C3%A9\" title=\"caf\xc3\xa9\">docs</a></p>\n");
}

TEST(entity_inside_code_span_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("`&eacute;`")};
  EXPECT_EQ(result, "<p><code>&amp;eacute;</code></p>\n");
}

TEST(entity_inside_indented_code_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("    &eacute;&amp;")};
  EXPECT_EQ(result, "<pre><code>&amp;eacute;&amp;amp;\n"
                    "</code></pre>\n");
}

TEST(entity_cannot_create_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("&#95;word&#95;")};
  EXPECT_EQ(result, "<p>_word_</p>\n");
}

TEST(entity_cannot_create_list) {
  const auto result{sourcemeta::core::markdown_to_html("&#45; item")};
  EXPECT_EQ(result, "<p>- item</p>\n");
}

TEST(entity_newline_is_not_line_break) {
  const auto result{sourcemeta::core::markdown_to_html("one&#xA;two")};
  EXPECT_EQ(result, "<p>one\n"
                    "two</p>\n");
}

TEST(entity_tabs_are_not_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("&#x9;&#x9;code")};
  EXPECT_EQ(result, "<p>\t\tcode</p>\n");
}

TEST(entity_quotes_do_not_delimit_link_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/manual &#34;Title&#34;)")};
  EXPECT_EQ(result, "<p>[docs](/manual &quot;Title&quot;)</p>\n");
}

TEST(entity_nonentities) {
  const auto result{sourcemeta::core::markdown_to_html(
      "&nbsp &x; &#; &#x;\n&#87654321;\n&#abcdef0;\n&ThisIsNotDefined; &hi?;")};
  EXPECT_EQ(result, "<p>&amp;nbsp &amp;x; &amp;#; &amp;#x;\n"
                    "&amp;#87654321;\n"
                    "&amp;#abcdef0;\n"
                    "&amp;ThisIsNotDefined; &amp;hi?;</p>\n");
}

TEST(entity_seven_decimal_digits_and_six_hexadecimal_digits) {
  const auto result{
      sourcemeta::core::markdown_to_html("&#0000065; &#x000041;")};
  EXPECT_EQ(result, "<p>A A</p>\n");
}
