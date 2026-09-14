#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(italic_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("*italic*")};
  EXPECT_EQ(result, "<p><em>italic</em></p>\n");
}

TEST(italic_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("_italic_")};
  EXPECT_EQ(result, "<p><em>italic</em></p>\n");
}

TEST(bold_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("**bold**")};
  EXPECT_EQ(result, "<p><strong>bold</strong></p>\n");
}

TEST(bold_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("__bold__")};
  EXPECT_EQ(result, "<p><strong>bold</strong></p>\n");
}

TEST(bold_and_italic) {
  const auto result{sourcemeta::core::markdown_to_html("***bold italic***")};
  EXPECT_EQ(result, "<p><em><strong>bold italic</strong></em></p>\n");
}

TEST(bold_inside_italic) {
  const auto result{
      sourcemeta::core::markdown_to_html("*this is **bold** inside italic*")};
  EXPECT_EQ(result,
            "<p><em>this is <strong>bold</strong> inside italic</em></p>\n");
}

TEST(italic_inside_bold) {
  const auto result{
      sourcemeta::core::markdown_to_html("**this is *italic* inside bold**")};
  EXPECT_EQ(result,
            "<p><strong>this is <em>italic</em> inside bold</strong></p>\n");
}

TEST(emphasis_across_words) {
  const auto result{
      sourcemeta::core::markdown_to_html("this is **all bold** here")};
  EXPECT_EQ(result, "<p>this is <strong>all bold</strong> here</p>\n");
}

TEST(bold_and_italic_in_list_item) {
  const auto result{
      sourcemeta::core::markdown_to_html("- ***bold italic item***\n- normal")};
  EXPECT_EQ(result,
            "<ul>\n<li><em><strong>bold italic item</strong></em></li>\n"
            "<li>normal</li>\n</ul>\n");
}

TEST(emphasis_does_not_cross_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("*start `code* end`")};
  EXPECT_EQ(result, "<p>*start <code>code* end</code></p>\n");
}

TEST(single_character_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("*a*")};
  EXPECT_EQ(result, "<p><em>a</em></p>\n");
}

TEST(emphasis_opening_followed_by_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("x _ y z_")};
  EXPECT_EQ(result, "<p>x _ y z_</p>\n");
}

TEST(emphasis_opening_between_letter_and_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("x*(y)*")};
  EXPECT_EQ(result, "<p>x*(y)*</p>\n");
}

TEST(emphasis_opening_followed_by_non_breaking_space) {
  const auto result{sourcemeta::core::markdown_to_html("_\xC2\xA0"
                                                       "x\xC2\xA0_")};
  EXPECT_EQ(result, "<p>_\xc2\xa0x\xc2\xa0_</p>\n");
}

TEST(emphasis_intraword_with_asterisks) {
  const auto result{
      sourcemeta::core::markdown_to_html("un*frigging*believable")};
  EXPECT_EQ(result, "<p>un<em>frigging</em>believable</p>\n");
}

TEST(emphasis_intraword_with_underscores_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("snake_case_name")};
  EXPECT_EQ(result, "<p>snake_case_name</p>\n");
}

TEST(emphasis_intraword_with_underscores_in_cyrillic) {
  const auto result{sourcemeta::core::markdown_to_html("река_течёт_быстро")};
  EXPECT_EQ(result, "<p>\xd1\x80\xd0\xb5\xd0\xba\xd0\xb0_"
                    "\xd1\x82\xd0\xb5\xd1\x87\xd1\x91\xd1\x82_"
                    "\xd0\xb1\xd1\x8b\xd1\x81\xd1\x82\xd1\x80\xd0\xbe</p>\n");
}

TEST(emphasis_between_digits) {
  const auto result{sourcemeta::core::markdown_to_html("1*2*3\n\n1_2_3")};
  EXPECT_EQ(result, "<p>1<em>2</em>3</p>\n"
                    "<p>1_2_3</p>\n");
}

TEST(emphasis_underscore_between_letters_and_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("x_(y)_z")};
  EXPECT_EQ(result, "<p>x_(y)_z</p>\n");
}

TEST(emphasis_underscore_after_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("x._[y]_")};
  EXPECT_EQ(result, "<p>x.<em>[y]</em></p>\n");
}

TEST(emphasis_mismatched_delimiter_characters) {
  const auto result{sourcemeta::core::markdown_to_html("*x_")};
  EXPECT_EQ(result, "<p>*x_</p>\n");
}

TEST(emphasis_closing_preceded_by_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("_x y _")};
  EXPECT_EQ(result, "<p>_x y _</p>\n");
}

TEST(emphasis_closing_on_new_line) {
  const auto result{sourcemeta::core::markdown_to_html("_x y\n_")};
  EXPECT_EQ(result, "<p>_x y\n"
                    "_</p>\n");
}

TEST(emphasis_closing_not_right_flanking) {
  const auto result{sourcemeta::core::markdown_to_html("_[_x]")};
  EXPECT_EQ(result, "<p>_[_x]</p>\n");
}

TEST(emphasis_nested_with_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("_[_x_]_")};
  EXPECT_EQ(result, "<p><em>[<em>x</em>]</em></p>\n");
}

TEST(emphasis_closing_followed_by_letter) {
  const auto result{sourcemeta::core::markdown_to_html("*x*y")};
  EXPECT_EQ(result, "<p><em>x</em>y</p>\n");
}

TEST(emphasis_underscore_closing_followed_by_letter) {
  const auto result{sourcemeta::core::markdown_to_html("_x_y\n\n_x_y_z_")};
  EXPECT_EQ(result, "<p>_x_y</p>\n"
                    "<p><em>x_y_z</em></p>\n");
}

TEST(emphasis_underscore_followed_by_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("_[x]_!")};
  EXPECT_EQ(result, "<p><em>[x]</em>!</p>\n");
}

TEST(strong_emphasis_opening_followed_by_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("__ x y__")};
  EXPECT_EQ(result, "<p>__ x y__</p>\n");
}

TEST(strong_emphasis_between_letter_and_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("x__(y)__")};
  EXPECT_EQ(result, "<p>x__(y)__</p>\n");
}

TEST(strong_emphasis_intraword_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("x**y**z")};
  EXPECT_EQ(result, "<p>x<strong>y</strong>z</p>\n");
}

TEST(strong_emphasis_nested_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("**x, **y**, z**")};
  EXPECT_EQ(result, "<p><strong>x, y, z</strong></p>\n");
}

TEST(strong_emphasis_underscore_after_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html("x.__[y]__")};
  EXPECT_EQ(result, "<p>x.<strong>[y]</strong></p>\n");
}

TEST(emphasis_rule_of_three_nested_strong) {
  const auto result{sourcemeta::core::markdown_to_html("_x__y__z_")};
  EXPECT_EQ(result, "<p><em>x__y__z</em></p>\n");
}

TEST(emphasis_rule_of_three_unmatched) {
  const auto result{sourcemeta::core::markdown_to_html("_x__y_")};
  EXPECT_EQ(result, "<p><em>x__y</em></p>\n");
}

TEST(emphasis_rule_of_three_triple_opening) {
  const auto result{sourcemeta::core::markdown_to_html("___x__ y_")};
  EXPECT_EQ(result, "<p><em><strong>x</strong> y</em></p>\n");
}

TEST(emphasis_rule_of_three_triple_closing) {
  const auto result{sourcemeta::core::markdown_to_html("_x __y___")};
  EXPECT_EQ(result, "<p><em>x <strong>y</strong></em></p>\n");
}

TEST(emphasis_rule_of_three_intraword) {
  const auto result{sourcemeta::core::markdown_to_html("a***b***c")};
  EXPECT_EQ(result, "<p>a<em><strong>b</strong></em>c</p>\n");
}

TEST(emphasis_rule_of_three_long_runs) {
  const auto result{sourcemeta::core::markdown_to_html("a****b*******c")};
  EXPECT_EQ(result, "<p>a<strong>b</strong>***c</p>\n");
}

TEST(emphasis_containing_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("_see [docs](/manual)_")};
  EXPECT_EQ(result, "<p><em>see <a href=\"/manual\">docs</a></em></p>\n");
}

TEST(emphasis_empty_delimiters_are_literal) {
  const auto result{sourcemeta::core::markdown_to_html(
      "__ empty strong\n\n____ empty double")};
  EXPECT_EQ(result, "<p>__ empty strong</p>\n"
                    "<p>____ empty double</p>\n");
}

TEST(emphasis_spanning_lines) {
  const auto result{sourcemeta::core::markdown_to_html("_one\ntwo_")};
  EXPECT_EQ(result, "<p><em>one\n"
                    "two</em></p>\n");
}

TEST(strong_emphasis_nested_inside_itself) {
  const auto result{sourcemeta::core::markdown_to_html("**x **y** z**")};
  EXPECT_EQ(result, "<p><strong>x y z</strong></p>\n");
}

TEST(strong_emphasis_quadruple_delimiters) {
  const auto result{sourcemeta::core::markdown_to_html("****x****")};
  EXPECT_EQ(result, "<p><strong>x</strong></p>\n");
}

TEST(strong_emphasis_quintuple_delimiters) {
  const auto result{sourcemeta::core::markdown_to_html("*****x*****")};
  EXPECT_EQ(result, "<p><em><strong>x</strong></em></p>\n");
}

TEST(emphasis_with_escaped_delimiters) {
  const auto result{sourcemeta::core::markdown_to_html("x _\\__\n\nx __\\___")};
  EXPECT_EQ(result, "<p>x <em>_</em></p>\n"
                    "<p>x <strong>_</strong></p>\n");
}

TEST(emphasis_with_excess_delimiters) {
  const auto result{
      sourcemeta::core::markdown_to_html("_x__\n\n__x_\n\n_x____")};
  EXPECT_EQ(result, "<p><em>x</em>_</p>\n"
                    "<p>_<em>x</em></p>\n"
                    "<p><em>x</em>___</p>\n");
}

TEST(emphasis_overlapping_delimiters) {
  const auto result{sourcemeta::core::markdown_to_html("_x *y_ z*")};
  EXPECT_EQ(result, "<p><em>x *y</em> z*</p>\n");
}

TEST(emphasis_overlapping_strong_and_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("_x **y _z w** v_")};
  EXPECT_EQ(result, "<p><em>x <strong>y _z w</strong> v</em></p>\n");
}

TEST(emphasis_unclosed_outer_strong) {
  const auto result{sourcemeta::core::markdown_to_html("__x __y z__")};
  EXPECT_EQ(result, "<p>__x <strong>y z</strong></p>\n");
}

TEST(emphasis_unclosed_outer_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("_x _y z_")};
  EXPECT_EQ(result, "<p>_x <em>y z</em></p>\n");
}

TEST(emphasis_does_not_span_link_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("_[x_](/url)")};
  EXPECT_EQ(result, "<p>_<a href=\"/url\">x_</a></p>\n");
}

TEST(emphasis_asterisk_does_not_span_link_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("*x [y*](/url)")};
  EXPECT_EQ(result, "<p>*x <a href=\"/url\">y*</a></p>\n");
}

TEST(strong_emphasis_does_not_span_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("**x `**` y**")};
  EXPECT_EQ(result, "<p><strong>x <code>**</code> y</strong></p>\n");
}

TEST(emphasis_does_not_span_autolink) {
  const auto result{sourcemeta::core::markdown_to_html(
      "_x<https://sourcemeta.com/?a=_>\n\n**x<https://sourcemeta.com/?b=**>")};
  EXPECT_EQ(result, "<p>_x<a "
                    "href=\"https://sourcemeta.com/?a=_\">https://"
                    "sourcemeta.com/?a=_</a></p>\n"
                    "<p>**x<a "
                    "href=\"https://sourcemeta.com/?b=**\">https://"
                    "sourcemeta.com/?b=**</a></p>\n");
}

TEST(emphasis_does_not_span_link_text_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("_a [b_ c]")};
  EXPECT_EQ(result, "<p><em>a [b</em> c]</p>\n");
}
