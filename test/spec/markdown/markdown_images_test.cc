#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(image) {
  const auto result{
      sourcemeta::core::markdown_to_html("![alt text](image.png)")};
  EXPECT_EQ(result, "<p><img src=\"image.png\" alt=\"alt text\" /></p>\n");
}

TEST(image_with_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("![alt](image.png \"My Image\")")};
  EXPECT_EQ(
      result,
      "<p><img src=\"image.png\" alt=\"alt\" title=\"My Image\" /></p>\n");
}

TEST(image_as_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[![logo](logo.png)](https://example.com)")};
  EXPECT_EQ(result, "<p><a href=\"https://example.com\">"
                    "<img src=\"logo.png\" alt=\"logo\" /></a></p>\n");
}

TEST(image_with_empty_alt) {
  const auto result{sourcemeta::core::markdown_to_html("![](image.png)")};
  EXPECT_EQ(result, "<p><img src=\"image.png\" alt=\"\" /></p>\n");
}

TEST(image_description_containing_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("![a [b](/inner)](/image.png)")};
  EXPECT_EQ(result, "<p><img src=\"/image.png\" alt=\"a b\" /></p>\n");
}

TEST(image_alt_text_is_plain_text) {
  const auto result{
      sourcemeta::core::markdown_to_html("![_italic_ and `code`](/i.png)")};
  EXPECT_EQ(result, "<p><img src=\"/i.png\" alt=\"italic and code\" /></p>\n");
}

TEST(image_alt_text_from_nested_image) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![outer ![inner](/in.png)](/out.png)")};
  EXPECT_EQ(result, "<p><img src=\"/out.png\" alt=\"outer inner\" /></p>\n");
}

TEST(image_alt_text_from_nested_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("![outer [inner](/in)](/out.png)")};
  EXPECT_EQ(result, "<p><img src=\"/out.png\" alt=\"outer inner\" /></p>\n");
}

TEST(image_collapsed_and_shortcut_references) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![logo][]\n\n![LOGO]\n\n[logo]: /logo.png 'Logo'")};
  EXPECT_EQ(result,
            "<p><img src=\"/logo.png\" alt=\"logo\" title=\"Logo\" /></p>\n"
            "<p><img src=\"/logo.png\" alt=\"LOGO\" title=\"Logo\" /></p>\n");
}

TEST(image_escaped_bracket_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("!\\[logo]\n\n[logo]: /logo.png")};
  EXPECT_EQ(result, "<p>![logo]</p>\n");
}

TEST(image_escaped_exclamation_is_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\![logo]\n\n[logo]: /logo.png")};
  EXPECT_EQ(result, "<p>!<a href=\"/logo.png\">logo</a></p>\n");
}

TEST(image_angle_bracket_destination_with_space) {
  const auto result{sourcemeta::core::markdown_to_html("![logo](</a b.png>)")};
  EXPECT_EQ(result, "<p><img src=\"/a%20b.png\" alt=\"logo\" /></p>\n");
}

TEST(image_title_with_escaped_quote) {
  const auto result{
      sourcemeta::core::markdown_to_html("![logo](/l.png 'it\\'s')")};
  EXPECT_EQ(result,
            "<p><img src=\"/l.png\" alt=\"logo\" title=\"it&#39;s\" /></p>\n");
}

TEST(image_description_spanning_a_soft_break) {
  // The description becomes the alt attribute, which cannot hold a line break,
  // so the break is flattened into a single space
  const auto result{sourcemeta::core::markdown_to_html("![a\nb](/u)")};
  EXPECT_EQ(result, "<p><img src=\"/u\" alt=\"a b\" /></p>\n");
}

// An exclamation mark opens an image only before a link label, and a label
// that opens with a caret belongs to a footnote reference instead
TEST(image_bang_before_a_footnote_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text![^1]\n\n[^1]: Note")};
  EXPECT_EQ(
      result,
      "<p>Text!<sup class=\"footnote-ref\"><a href=\"#fn-1\" id=\"fnref-1\""
      " data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
      "<li id=\"fn-1\">\n<p>Note "
      "<a href=\"#fnref-1\" class=\"footnote-backref\""
      " data-footnote-backref data-footnote-backref-idx=\"1\""
      " aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
      "</li>\n</ol>\n</section>\n");
}
