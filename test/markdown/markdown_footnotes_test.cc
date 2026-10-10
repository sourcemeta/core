#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

#include <string> // std::string

TEST(footnote_basic) {
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

TEST(footnote_multiple) {
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

TEST(footnote_referenced_multiple_times) {
  const auto result{sourcemeta::core::markdown_to_html(
      "one[^n] two[^n] three[^n]\n\n[^n]: shared")};
  EXPECT_EQ(
      result,
      "<p>one<sup class=\"footnote-ref\"><a href=\"#fn-n\" id=\"fnref-n\" "
      "data-footnote-ref>1</a></sup> two<sup class=\"footnote-ref\"><a "
      "href=\"#fn-n\" id=\"fnref-n-2\" data-footnote-ref>1</a></sup> three<sup "
      "class=\"footnote-ref\"><a href=\"#fn-n\" id=\"fnref-n-3\" "
      "data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n"
      "<ol>\n"
      "<li id=\"fn-n\">\n"
      "<p>shared <a href=\"#fnref-n\" class=\"footnote-backref\" "
      "data-footnote-backref data-footnote-backref-idx=\"1\" aria-label=\"Back "
      "to reference 1\">\xe2\x86\xa9</a> <a href=\"#fnref-n-2\" "
      "class=\"footnote-backref\" data-footnote-backref "
      "data-footnote-backref-idx=\"1-2\" aria-label=\"Back to reference "
      "1-2\">\xe2\x86\xa9<sup class=\"footnote-ref\">2</sup></a> <a "
      "href=\"#fnref-n-3\" class=\"footnote-backref\" data-footnote-backref "
      "data-footnote-backref-idx=\"1-3\" aria-label=\"Back to reference "
      "1-3\">\xe2\x86\xa9<sup class=\"footnote-ref\">3</sup></a></p>\n"
      "</li>\n"
      "</ol>\n"
      "</section>\n");
}

TEST(footnote_unreferenced_definition_omitted) {
  const auto result{
      sourcemeta::core::markdown_to_html("body\n\n[^orphan]: unused")};
  EXPECT_EQ(result, "<p>body</p>\n");
}

TEST(footnote_reference_without_definition) {
  const auto result{sourcemeta::core::markdown_to_html("body[^ghost]")};
  EXPECT_EQ(result, "<p>body[^ghost]</p>\n");
}

TEST(footnote_label_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^LABEL]\n\n[^label]: text")};
  EXPECT_EQ(result,
            "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-label\" "
            "id=\"fnref-label\" data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-label\">\n"
            "<p>text <a href=\"#fnref-label\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_definition_with_multiple_blocks) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^m]\n\n[^m]: first\n\n    > quoted\n\n    * item")};
  EXPECT_EQ(result, "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-m\" "
                    "id=\"fnref-m\" data-footnote-ref>1</a></sup></p>\n"
                    "<section class=\"footnotes\" data-footnotes>\n"
                    "<ol>\n"
                    "<li id=\"fn-m\">\n"
                    "<p>first</p>\n"
                    "<blockquote>\n"
                    "<p>quoted</p>\n"
                    "</blockquote>\n"
                    "<ul>\n"
                    "<li>item</li>\n"
                    "</ul>\n"
                    "<a href=\"#fnref-m\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"1\" "
                    "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</section>\n");
}

TEST(footnote_numbering_follows_references) {
  const auto result{sourcemeta::core::markdown_to_html(
      "x[^z] y[^y] z[^x]\n\n[^x]: X\n[^y]: Y\n[^z]: Z")};
  EXPECT_EQ(result,
            "<p>x<sup class=\"footnote-ref\"><a href=\"#fn-z\" id=\"fnref-z\" "
            "data-footnote-ref>1</a></sup> y<sup class=\"footnote-ref\"><a "
            "href=\"#fn-y\" id=\"fnref-y\" data-footnote-ref>2</a></sup> z<sup "
            "class=\"footnote-ref\"><a href=\"#fn-x\" id=\"fnref-x\" "
            "data-footnote-ref>3</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-z\">\n"
            "<p>Z <a href=\"#fnref-z\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-y\">\n"
            "<p>Y <a href=\"#fnref-y\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"2\" "
            "aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-x\">\n"
            "<p>X <a href=\"#fnref-x\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"3\" "
            "aria-label=\"Back to reference 3\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_label_with_special_characters) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^a&b<c>]\n\n[^a&b<c>]: text")};
  EXPECT_EQ(
      result,
      "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-a&amp;b%3Cc%3E\" "
      "id=\"fnref-a&amp;b%3Cc%3E\" data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n"
      "<ol>\n"
      "<li id=\"fn-a&amp;b%3Cc%3E\">\n"
      "<p>text <a href=\"#fnref-a&amp;b%3Cc%3E\" class=\"footnote-backref\" "
      "data-footnote-backref data-footnote-backref-idx=\"1\" aria-label=\"Back "
      "to reference 1\">\xe2\x86\xa9</a></p>\n"
      "</li>\n"
      "</ol>\n"
      "</section>\n");
}

TEST(footnote_reference_in_table_cell) {
  const auto result{sourcemeta::core::markdown_to_html(
      "| h |\n| - |\n| c[^t] |\n\n[^t]: table note")};
  EXPECT_EQ(result,
            "<table>\n"
            "<thead>\n"
            "<tr>\n"
            "<th>h</th>\n"
            "</tr>\n"
            "</thead>\n"
            "<tbody>\n"
            "<tr>\n"
            "<td>c<sup class=\"footnote-ref\"><a href=\"#fn-t\" id=\"fnref-t\" "
            "data-footnote-ref>1</a></sup></td>\n"
            "</tr>\n"
            "</tbody>\n"
            "</table>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-t\">\n"
            "<p>table note <a href=\"#fnref-t\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_reference_inside_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^a]\n\n[^a]: refers[^b]\n\n[^b]: nested")};
  EXPECT_EQ(result, "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-a\" "
                    "id=\"fnref-a\" data-footnote-ref>1</a></sup></p>\n"
                    "<section class=\"footnotes\" data-footnotes>\n"
                    "<ol>\n"
                    "<li id=\"fn-a\">\n"
                    "<p>refers<sup class=\"footnote-ref\"><a href=\"#fn-b\" "
                    "id=\"fnref-b\" data-footnote-ref>2</a></sup> <a "
                    "href=\"#fnref-a\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"1\" "
                    "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
                    "</li>\n"
                    "<li id=\"fn-b\">\n"
                    "<p>nested <a href=\"#fnref-b\" class=\"footnote-backref\" "
                    "data-footnote-backref data-footnote-backref-idx=\"2\" "
                    "aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</section>\n");
}

TEST(footnote_definition_inside_blockquote) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^q]\n\n> [^q]: inside quote")};
  EXPECT_EQ(result,
            "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-q\" "
            "id=\"fnref-q\" data-footnote-ref>1</a></sup></p>\n"
            "<blockquote>\n"
            "</blockquote>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-q\">\n"
            "<p>inside quote <a href=\"#fnref-q\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

TEST(footnote_label_with_space_is_not_footnote) {
  const auto result{sourcemeta::core::markdown_to_html(
      "body[^two words]\n\n[^two words]: text")};
  EXPECT_EQ(result, "<p>body<a href=\"text\">^two words</a></p>\n");
}

TEST(footnote_definition_nested_inside_another_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[^a]\n\n[^a]: outer\n    [^b]: inner\n\n[^b]")};
  EXPECT_EQ(result,
            "<p><sup class=\"footnote-ref\"><a href=\"#fn-a\" id=\"fnref-a\" "
            "data-footnote-ref>1</a></sup></p>\n"
            "<p><sup class=\"footnote-ref\"><a href=\"#fn-b\" id=\"fnref-b\" "
            "data-footnote-ref>2</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
            "<li id=\"fn-a\">\n"
            "<p>outer <a href=\"#fnref-a\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "<li id=\"fn-b\">\n"
            "<p>inner <a href=\"#fnref-b\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"2\" "
            "aria-label=\"Back to reference 2\">\xe2\x86\xa9</a></p>\n"
            "</li>\n</ol>\n</section>\n");
}

TEST(footnote_nested_definition_referenced_without_its_parent) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[^b]\n\n[^a]: outer\n    [^b]: inner")};
  EXPECT_EQ(result,
            "<p><sup class=\"footnote-ref\"><a href=\"#fn-b\" id=\"fnref-b\" "
            "data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
            "<li id=\"fn-b\">\n"
            "<p>inner <a href=\"#fnref-b\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n</ol>\n</section>\n");
}

TEST(footnote_nested_definition_with_the_same_label_takes_precedence) {
  const auto result{
      sourcemeta::core::markdown_to_html("[^x]\n\n[^x]: one\n    [^x]: two")};
  EXPECT_EQ(result,
            "<p><sup class=\"footnote-ref\"><a href=\"#fn-x\" id=\"fnref-x\" "
            "data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
            "<li id=\"fn-x\">\n"
            "<p>two <a href=\"#fnref-x\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n</ol>\n</section>\n");
}

TEST(footnote_label_with_number_sign) {
  const auto result{
      sourcemeta::core::markdown_to_html("body[^a#b]\n\n[^a#b]: text")};
  EXPECT_EQ(result,
            "<p>body<sup class=\"footnote-ref\"><a href=\"#fn-a%23b\" "
            "id=\"fnref-a%23b\" data-footnote-ref>1</a></sup></p>\n"
            "<section class=\"footnotes\" data-footnotes>\n"
            "<ol>\n"
            "<li id=\"fn-a%23b\">\n"
            "<p>text <a href=\"#fnref-a%23b\" class=\"footnote-backref\" "
            "data-footnote-backref data-footnote-backref-idx=\"1\" "
            "aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
            "</li>\n"
            "</ol>\n"
            "</section>\n");
}

// GFM section 2.1 counts line tabulation and form feed as whitespace, so a
// label made only of those normalises away. Such a definition can never be
// referenced, and leaving it in the tree opens a footnotes section that
// nothing closes until the end of the document.
TEST(footnote_definition_with_a_label_that_normalises_away_is_dropped) {
  const auto result{
      sourcemeta::core::markdown_to_html("[^\v]: note\n\ntext\n")};
  EXPECT_EQ(result, "<p>text</p>\n");
}

TEST(footnote_definition_with_a_form_feed_label_is_dropped) {
  const auto result{
      sourcemeta::core::markdown_to_html("[^\f]: note\n\ntext\n")};
  EXPECT_EQ(result, "<p>text</p>\n");
}

// Forming a footnote reference out of an unmatched bracket drops everything
// between its brackets, so a reference nested in another one is created first
// and then leaves the document. It never resolves, and the outer label keeps
// the text the inner brackets were written as
TEST(footnote_reference_nested_in_another_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text[^a[^b]c]\n\n[^a[^b]c]: Note")};
  EXPECT_EQ(result, "<p>Text[^a[^b]c]</p>\n<p>[^a[^b]c]: Note</p>\n");
}

// A label of more codepoints than GFM section 6.6 admits for a link label
// cannot name a definition, so the reference stays the text it was written as
// even when a definition of exactly that label is present
TEST(footnote_reference_label_over_the_length_limit) {
  const std::string label(1000, 'a');
  const auto result{sourcemeta::core::markdown_to_html(
      "Text[^" + label + "]\n\n[^" + label + "]: Note")};
  EXPECT_EQ(result, "<p>Text[^" + label + "]</p>\n");
}

// A footnote reference needs a label, so empty brackets are not one
TEST(footnote_reference_without_a_label) {
  const auto result{sourcemeta::core::markdown_to_html("Text[^] rest")};
  EXPECT_EQ(result, "<p>Text[^] rest</p>\n");
}

// A label of nothing between the caret and the closing bracket is no footnote
// definition, which leaves the line to GFM section 6.6 as a link reference
// definition whose label is the caret
TEST(footnote_definition_without_a_label_is_a_link_reference_definition) {
  const auto result{sourcemeta::core::markdown_to_html("Text[^]\n\n[^]: Note")};
  EXPECT_EQ(result, "<p>Text<a href=\"Note\">^</a></p>\n");
}

// A label that carries whitespace never reaches its closing bracket, so the
// line is no footnote definition either
TEST(footnote_definition_with_whitespace_in_the_label) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text[^a b]\n\n[^a b]: Note")};
  EXPECT_EQ(result, "<p>Text<a href=\"Note\">^a b</a></p>\n");
}

// A definition admits whitespace after its colon, which includes a tabulation
TEST(footnote_definition_with_a_tabulation_after_the_colon) {
  const auto result{
      sourcemeta::core::markdown_to_html("Text[^1]\n\n[^1]:\tNote")};
  EXPECT_EQ(
      result,
      "<p>Text<sup class=\"footnote-ref\"><a href=\"#fn-1\" id=\"fnref-1\""
      " data-footnote-ref>1</a></sup></p>\n"
      "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
      "<li id=\"fn-1\">\n<p>Note "
      "<a href=\"#fnref-1\" class=\"footnote-backref\""
      " data-footnote-backref data-footnote-backref-idx=\"1\""
      " aria-label=\"Back to reference 1\">\xe2\x86\xa9</a></p>\n"
      "</li>\n</ol>\n</section>\n");
}
