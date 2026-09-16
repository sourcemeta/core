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
