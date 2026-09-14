#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(unordered_list_with_dashes) {
  const auto result{
      sourcemeta::core::markdown_to_html("- one\n- two\n- three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(unordered_list_with_asterisks) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n* two\n* three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(unordered_list_with_plus) {
  const auto result{
      sourcemeta::core::markdown_to_html("+ one\n+ two\n+ three")};
  EXPECT_EQ(result,
            "<ul>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ul>\n");
}

TEST(nested_unordered_list) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- outer\n  - inner\n  - inner2\n- outer2")};
  EXPECT_EQ(result, "<ul>\n<li>outer\n<ul>\n<li>inner</li>\n<li>inner2</li>\n"
                    "</ul>\n</li>\n<li>outer2</li>\n</ul>\n");
}

TEST(ordered_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1. one\n2. two\n3. three")};
  EXPECT_EQ(result,
            "<ol>\n<li>one</li>\n<li>two</li>\n<li>three</li>\n</ol>\n");
}

TEST(ordered_list_starting_at_nonone) {
  const auto result{
      sourcemeta::core::markdown_to_html("3. three\n4. four\n5. five")};
  EXPECT_EQ(result,
            "<ol start=\"3\">\n<li>three</li>\n<li>four</li>\n<li>five</li>\n"
            "</ol>\n");
}

TEST(nested_ordered_in_unordered) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- item\n  1. sub one\n  2. sub two\n- item2")};
  EXPECT_EQ(result, "<ul>\n<li>item\n<ol>\n<li>sub one</li>\n<li>sub two</li>\n"
                    "</ol>\n</li>\n<li>item2</li>\n</ul>\n");
}

TEST(loose_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("- one\n\n- two\n\n- three")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>one</p>\n</li>\n<li>\n<p>two</p>\n</li>\n"
                    "<li>\n<p>three</p>\n</li>\n</ul>\n");
}

TEST(list_with_code_block) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- item:\n\n  ```\n  code\n  ```\n\n- next")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>item:</p>\n<pre><code>code\n</code></pre>\n"
                    "</li>\n<li>\n<p>next</p>\n</li>\n</ul>\n");
}

TEST(deeply_nested_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("- a\n  - b\n    - c\n      - d")};
  EXPECT_EQ(result, "<ul>\n<li>a\n<ul>\n<li>b\n<ul>\n<li>c\n<ul>\n<li>d</li>\n"
                    "</ul>\n</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>\n");
}

TEST(ordered_list_with_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "1. first\n\n   more text\n\n2. second")};
  EXPECT_EQ(result, "<ol>\n<li>\n<p>first</p>\n<p>more text</p>\n</li>\n"
                    "<li>\n<p>second</p>\n</li>\n</ol>\n");
}

TEST(list_item_with_multiple_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- paragraph one\n\n  paragraph two\n\n  paragraph three")};
  EXPECT_EQ(result, "<ul>\n<li>\n<p>paragraph one</p>\n<p>paragraph two</p>\n"
                    "<p>paragraph three</p>\n</li>\n</ul>\n");
}

TEST(list_marker_takes_precedence_over_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("* `alpha\n* beta`")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>`alpha</li>\n"
                    "<li>beta`</li>\n"
                    "</ul>\n");
}

TEST(list_item_content_needs_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n");
}

TEST(list_item_content_with_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n  two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_wide_marker_content_not_indented_enough) {
  const auto result{sourcemeta::core::markdown_to_html("1.   one\n\n    two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "</ol>\n"
                    "<pre><code>two\n"
                    "</code></pre>\n");
}

TEST(list_item_wide_marker_content_indented_enough) {
  const auto result{sourcemeta::core::markdown_to_html("1.   one\n\n     two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_inside_nested_blockquotes) {
  const auto result{
      sourcemeta::core::markdown_to_html("> > - one\n> >\n> >   two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(list_item_inside_nested_blockquotes_not_indented_enough) {
  const auto result{
      sourcemeta::core::markdown_to_html("> > - one\n> >\n> > two")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(list_item_marker_needs_following_space) {
  const auto result{sourcemeta::core::markdown_to_html("*one\n\n1)two")};
  EXPECT_EQ(result, "<p>*one</p>\n"
                    "<p>1)two</p>\n");
}

TEST(list_item_multiple_blank_lines_between_blocks) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n\n\n  two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_with_several_block_types) {
  const auto result{sourcemeta::core::markdown_to_html(
      "2. one\n\n   ~~~\n   code\n   ~~~\n\n   two\n\n   > quote")};
  EXPECT_EQ(result, "<ol start=\"2\">\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>two</p>\n"
                    "<blockquote>\n"
                    "<p>quote</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_code_block_with_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html(
      "* one\n\n      code\n\n\n      more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<pre><code>code\n"
                    "\n"
                    "\n"
                    "more\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_nine_digit_number) {
  const auto result{sourcemeta::core::markdown_to_html("999999999) max")};
  EXPECT_EQ(result, "<ol start=\"999999999\">\n"
                    "<li>max</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_ten_digit_number_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("9999999999) over")};
  EXPECT_EQ(result, "<p>9999999999) over</p>\n");
}

TEST(ordered_list_starting_at_zero) {
  const auto result{sourcemeta::core::markdown_to_html("0) zero")};
  EXPECT_EQ(result, "<ol start=\"0\">\n"
                    "<li>zero</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_number_with_leading_zeros) {
  const auto result{sourcemeta::core::markdown_to_html("0007. seven")};
  EXPECT_EQ(result, "<ol start=\"7\">\n"
                    "<li>seven</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_negative_number_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("-5) negative")};
  EXPECT_EQ(result, "<p>-5) negative</p>\n");
}

TEST(list_item_indented_code_after_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("* text\n\n      code")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>text</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_indented_code_after_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html(" 100. text\n\n          code")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>\n"
                    "<p>text</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_starting_with_indented_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("*     code\n\n  text\n\n      more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "<p>text</p>\n"
                    "<pre><code>more\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_indented_code_and_extra_space) {
  const auto result{
      sourcemeta::core::markdown_to_html("*      code\n\n  text")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code> code\n"
                    "</code></pre>\n"
                    "<p>text</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_blank_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("*\n  one\n*\n  ~~~\n  code\n  ~~~")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_starting_with_blank_line_and_trailing_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("1.  \n   one")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "</ol>\n");
}

TEST(list_item_at_most_one_starting_blank_line) {
  const auto result{sourcemeta::core::markdown_to_html("*\n\n  one")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n"
                    "<p>one</p>\n");
}

TEST(list_item_empty_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n*\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(list_item_empty_with_trailing_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n*  \n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_empty_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("1) one\n2)\n3) two")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "<li></li>\n"
                    "<li>two</li>\n"
                    "</ol>\n");
}

TEST(list_consisting_of_single_empty_item) {
  const auto result{sourcemeta::core::markdown_to_html("-")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n");
}

TEST(list_item_empty_cannot_interrupt_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("text\n-\n\ntext\n1)")};
  EXPECT_EQ(result, "<h2>text</h2>\n"
                    "<p>text\n"
                    "1)</p>\n");
}

TEST(list_item_indented_one_space) {
  const auto result{sourcemeta::core::markdown_to_html(
      "  1. one\n     two\n\n         code")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    - one\n      two")};
  EXPECT_EQ(result, "<pre><code>- one\n"
                    "  two\n"
                    "</code></pre>\n");
}

TEST(list_item_lazy_continuation_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html(" * one\ntwo\n\n       code")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one\n"
                    "two</p>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_partially_lazy_continuation) {
  const auto result{sourcemeta::core::markdown_to_html(" * one\n  two\nthree")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one\n"
                    "two\n"
                    "three</li>\n"
                    "</ul>\n");
}

TEST(list_item_laziness_inside_nested_containers) {
  const auto result{
      sourcemeta::core::markdown_to_html("> * > quoted\nlazy line")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<blockquote>\n"
                    "<p>quoted\n"
                    "lazy line</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</blockquote>\n");
}

TEST(list_nested_with_sufficient_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n  * b\n    * c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>b\n"
                    "<ul>\n"
                    "<li>c</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_not_nested_with_one_space_increments) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n * b\n  * c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_nested_needs_marker_width_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("100. a\n     * b")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(ordered_list_not_nested_with_insufficient_indentation) {
  const auto result{sourcemeta::core::markdown_to_html("100. a\n    * b")};
  EXPECT_EQ(result, "<ol start=\"100\">\n"
                    "<li>a\n"
                    "* b</li>\n"
                    "</ol>\n");
}

TEST(list_item_containing_only_list) {
  const auto result{sourcemeta::core::markdown_to_html("+ + one")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_item_containing_nested_ordered_list) {
  const auto result{sourcemeta::core::markdown_to_html("3) * 4) deep")};
  EXPECT_EQ(result, "<ol start=\"3\">\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<ol start=\"4\">\n"
                    "<li>deep</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_item_containing_headings) {
  const auto result{
      sourcemeta::core::markdown_to_html("* ## Title\n* Text\n  ===\n  more")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<h2>Title</h2>\n"
                    "</li>\n"
                    "<li>\n"
                    "<h1>Text</h1>\n"
                    "more</li>\n"
                    "</ul>\n");
}

TEST(list_changing_bullet_character_starts_new_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n* two\n- three")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ul>\n"
                    "<ul>\n"
                    "<li>three</li>\n"
                    "</ul>\n");
}

TEST(list_changing_delimiter_starts_new_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) one\n2) two\n3. three")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ol>\n"
                    "<ol start=\"3\">\n"
                    "<li>three</li>\n"
                    "</ol>\n");
}

TEST(list_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Text\n* one\n* two")};
  EXPECT_EQ(result, "<p>Text</p>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_not_starting_at_one_cannot_interrupt_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("We counted\n42. items in total")};
  EXPECT_EQ(result, "<p>We counted\n"
                    "42. items in total</p>\n");
}

TEST(ordered_list_starting_at_one_interrupts_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("We counted\n1. items in total")};
  EXPECT_EQ(result, "<p>We counted</p>\n"
                    "<ol>\n"
                    "<li>items in total</li>\n"
                    "</ol>\n");
}

TEST(list_items_separated_by_multiple_blank_lines) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n\n\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_nested_item_with_blank_lines) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  * b\n\n\n    c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_separated_by_html_comment) {
  const auto result{
      sourcemeta::core::markdown_to_html("* one\n\n<!-- split -->\n\n* two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<!-- raw HTML omitted -->\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(list_items_with_varying_marker_indentation) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n * b\n  * c\n * d\n* e")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "<li>d</li>\n"
                    "<li>e</li>\n"
                    "</ul>\n");
}

TEST(list_item_indented_four_spaces_continues_previous_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n * b\n  * c\n    * d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c\n"
                    "<ul>\n"
                    "<li>d</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(ordered_list_item_indented_five_spaces_is_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) a\n\n 2) b\n\n     3) c")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<ol start=\"3\">\n"
                    "<li>c</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_loose_with_blank_line_between_items) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n\n* b\n* c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_empty_item) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n-\n\n- b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li></li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_blank_line_inside_item) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n\n  b\n* c")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_loose_with_blank_line_before_reference_definition) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n\n  [r]: /r\n* b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blank_lines_inside_code_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n* ~~~\n  x\n\n  ~~~\n* b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>\n"
                    "<pre><code>x\n"
                    "\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>b</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_loose_sublist) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  * b\n\n  * c\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blank_blockquote_line) {
  const auto result{
      sourcemeta::core::markdown_to_html("- a\n  > b\n  >\n  > c\n- d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<blockquote>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_tight_with_blockquote_and_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  > b\n  ~~~\n  c\n  ~~~\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<blockquote>\n"
                    "<p>b</p>\n"
                    "</blockquote>\n"
                    "<pre><code>c\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(list_single_tight_nested_ordered_item) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n   1. b")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>a\n"
                    "<ol>\n"
                    "<li>b</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_loose_single_item_with_code) {
  const auto result{
      sourcemeta::core::markdown_to_html("* ~~~\n  x\n  ~~~\n\n  y")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>x\n"
                    "</code></pre>\n"
                    "<p>y</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(list_outer_loose_inner_tight) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n   - b\n\n   c")};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(list_tight_sublist_inside_loose_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("* a\n  1. b\n  2. c\n\n* d")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<ol>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>d</p>\n"
                    "</li>\n"
                    "</ul>\n");
}
