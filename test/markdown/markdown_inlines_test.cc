#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(backslash_escapes_every_ascii_punctuation) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a\\~b\\}c\\|d\\{e\\`f\\_g\\^h\\]i\\\\j\\[k\\@l\\?m\\>n\\=o\\<p\\;q\\:"
      "r\\/s\\.t\\-u\\,v\\+w\\*x\\)y\\(z\\'1\\&2\\%3\\$4\\#5\\\"6\\!")};
  EXPECT_EQ(result, "<p>a~b}c|d{e`f_g^h]i\\j[k@l?m&gt;n=o&lt;p;q:r/"
                    "s.t-u,v+w*x)y(z'1&amp;2%3$4#5&quot;6!</p>\n");
}

TEST(backslash_before_other_characters_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("\\Z \\9 \\é \\€ \\\t")};
  EXPECT_EQ(result, "<p>\\Z \\9 \\\xc3\xa9 \\\xe2\x82\xac \\</p>\n");
}

TEST(backslash_escaped_markup_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\\_no emphasis_\n\\<em> no tag\n\\![no image](/x.png)\n2\\) no "
      "list\n\\+ no list\n\\> no quote\n\\&amp; no entity")};
  EXPECT_EQ(result, "<p>_no emphasis_\n"
                    "&lt;em&gt; no tag\n"
                    "!<a href=\"/x.png\">no image</a>\n"
                    "2) no list\n"
                    "+ no list\n"
                    "&gt; no quote\n"
                    "&amp;amp; no entity</p>\n");
}

TEST(backslash_escaped_backslash_before_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("\\\\_emphasis_")};
  EXPECT_EQ(result, "<p>\\<em>emphasis</em></p>\n");
}

TEST(backslash_at_end_of_line_is_hard_break) {
  const auto result{sourcemeta::core::markdown_to_html("one\\\ntwo")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(backslash_inside_code_span_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("`\\*\\_`")};
  EXPECT_EQ(result, "<p><code>\\*\\_</code></p>\n");
}

TEST(backslash_inside_indented_code_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("    \\*\\_")};
  EXPECT_EQ(result, "<pre><code>\\*\\_\n"
                    "</code></pre>\n");
}

TEST(backslash_inside_fenced_code_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("```\n\\*\\_\n```")};
  EXPECT_EQ(result, "<pre><code>\\*\\_\n"
                    "</code></pre>\n");
}

TEST(backslash_inside_autolink_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/\\_\\*>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%5C_%5C*\">https://"
                    "sourcemeta.com/\\_\\*</a></p>\n");
}

TEST(backslash_escapes_in_link_destination_and_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/a\\_b 'c\\_d')")};
  EXPECT_EQ(result, "<p><a href=\"/a_b\" title=\"c_d\">docs</a></p>\n");
}

TEST(backslash_escapes_in_reference_definition) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs]\n\n[docs]: /a\\_b 'c\\_d'")};
  EXPECT_EQ(result, "<p><a href=\"/a_b\" title=\"c_d\">docs</a></p>\n");
}

TEST(entity_named_references) {
  const auto result{sourcemeta::core::markdown_to_html(
      "&lt; &gt; &quot; &apos; &euro; &hellip;\n&rightarrow; "
      "&NotNestedLessLess; &bigstar;")};
  EXPECT_EQ(result, "<p>&lt; &gt; &quot; ' \xe2\x82\xac \xe2\x80\xa6\n"
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
                    "\xef\xbf\xbd\n"
                    "\xef\xbf\xbd\n"
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

TEST(code_span_with_double_backticks) {
  const auto result{sourcemeta::core::markdown_to_html("``a`b``")};
  EXPECT_EQ(result, "<p><code>a`b</code></p>\n");
}

TEST(code_span_strips_one_surrounding_space) {
  const auto result{sourcemeta::core::markdown_to_html("`` `x` ``")};
  EXPECT_EQ(result, "<p><code>`x`</code></p>\n");
}

TEST(code_span_strips_only_one_space_per_side) {
  const auto result{sourcemeta::core::markdown_to_html("``   x   ``")};
  EXPECT_EQ(result, "<p><code>  x  </code></p>\n");
}

TEST(code_span_space_on_one_side_is_kept) {
  const auto result{sourcemeta::core::markdown_to_html("`x `")};
  EXPECT_EQ(result, "<p><code>x </code></p>\n");
}

TEST(code_span_non_breaking_space_is_kept) {
  const auto result{sourcemeta::core::markdown_to_html("`\xC2\xA0"
                                                       "x`")};
  EXPECT_EQ(result, "<p><code>\xc2\xa0x</code></p>\n");
}

TEST(code_span_of_only_spaces_is_kept) {
  const auto result{sourcemeta::core::markdown_to_html("`   `")};
  EXPECT_EQ(result, "<p><code>   </code></p>\n");
}

TEST(code_span_line_endings_become_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("`one\ntwo\nthree`")};
  EXPECT_EQ(result, "<p><code>one two three</code></p>\n");
}

TEST(code_span_interior_spaces_are_kept) {
  const auto result{sourcemeta::core::markdown_to_html("`a    b  \n  c`")};
  EXPECT_EQ(result, "<p><code>a    b   c</code></p>\n");
}

TEST(code_span_backslash_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("`a\\`b")};
  EXPECT_EQ(result, "<p><code>a\\</code>b</p>\n");
}

TEST(code_span_backtick_run_lengths_must_match) {
  const auto result{sourcemeta::core::markdown_to_html("``x```")};
  EXPECT_EQ(result, "<p>``x```</p>\n");
}

TEST(code_span_unmatched_opening_is_literal) {
  const auto result{sourcemeta::core::markdown_to_html("``x")};
  EXPECT_EQ(result, "<p>``x</p>\n");
}

TEST(code_span_longer_closing_run_does_not_close) {
  const auto result{sourcemeta::core::markdown_to_html("``x```y``")};
  EXPECT_EQ(result, "<p><code>x```y</code></p>\n");
}

TEST(code_span_takes_precedence_over_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("_x`_`")};
  EXPECT_EQ(result, "<p>_x<code>_</code></p>\n");
}

TEST(code_span_takes_precedence_over_link) {
  const auto result{sourcemeta::core::markdown_to_html("[text `code](/url`)")};
  EXPECT_EQ(result, "<p>[text <code>code](/url</code>)</p>\n");
}

TEST(code_span_takes_precedence_over_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("`<https://sourcemeta.com/`x>`")};
  EXPECT_EQ(result, "<p><code>&lt;https://sourcemeta.com/</code>x&gt;`</p>\n");
}

TEST(autolink_takes_precedence_over_code_span) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/`x>`")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%60x\">https://"
                    "sourcemeta.com/`x</a>`</p>\n");
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

TEST(link_destination_single_space) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]( )")};
  EXPECT_EQ(result, "<p><a href=\"\">docs</a></p>\n");
}

TEST(link_destination_empty_angle_brackets_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]( <> )")};
  EXPECT_EQ(result, "<p><a href=\"\">docs</a></p>\n");
}

TEST(link_destination_angle_brackets_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a b c>)")};
  EXPECT_EQ(result, "<p><a href=\"a%20b%20c\">docs</a></p>\n");
}

TEST(link_destination_with_space_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/a b)")};
  EXPECT_EQ(result, "<p>[docs](/a b)</p>\n");
}

TEST(link_destination_with_newline_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/a\n/b)")};
  EXPECT_EQ(result, "<p>[docs](/a\n"
                    "/b)</p>\n");
}

TEST(link_destination_angle_brackets_with_newline_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](</a\nb>)")};
  EXPECT_EQ(result, "<p>[docs](&lt;/a\n"
                    "b&gt;)</p>\n");
}

TEST(link_destination_angle_brackets_with_closing_paren) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a)b>)")};
  EXPECT_EQ(result, "<p><a href=\"a)b\">docs</a></p>\n");
}

TEST(link_destination_unmatched_angle_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a\\>)")};
  EXPECT_EQ(result, "<p>[docs](&lt;a&gt;)</p>\n");
}

TEST(link_destination_invalid_angle_bracket_forms) {
  const auto result{sourcemeta::core::markdown_to_html("[x](<y)z\n[x](<y>z)")};
  EXPECT_EQ(result, "<p>[x](&lt;y)z\n"
                    "[x](<!-- raw HTML omitted -->z)</p>\n");
}

TEST(link_destination_escaped_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](\\(a\\)b)")};
  EXPECT_EQ(result, "<p><a href=\"(a)b\">docs</a></p>\n");
}

TEST(link_destination_balanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a(b(c)d)e)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c)d)e\">docs</a></p>\n");
}

TEST(link_destination_unbalanced_parentheses_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a(b(c)d)")};
  EXPECT_EQ(result, "<p>[docs](a(b(c)d)</p>\n");
}

TEST(link_destination_escaped_unbalanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\(b\\(c)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c\">docs</a></p>\n");
}

TEST(link_destination_angle_brackets_unbalanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](<a(b(c>)")};
  EXPECT_EQ(result, "<p><a href=\"a(b(c\">docs</a></p>\n");
}

TEST(link_destination_backslash_escapes) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\]\\!)")};
  EXPECT_EQ(result, "<p><a href=\"a%5D!\">docs</a></p>\n");
}

TEST(link_destination_fragments_and_queries) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[x](#top)\n\n[y](https://sourcemeta.com/?q=1#top)")};
  EXPECT_EQ(result,
            "<p><a href=\"#top\">x</a></p>\n"
            "<p><a href=\"https://sourcemeta.com/?q=1#top\">y</a></p>\n");
}

TEST(link_destination_backslash_before_letter) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](a\\zb)")};
  EXPECT_EQ(result, "<p><a href=\"a%5Czb\">docs</a></p>\n");
}

TEST(link_destination_percent_encoding_and_entities) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](caf%C3%A9&eacute;)")};
  EXPECT_EQ(result, "<p><a href=\"caf%C3%A9%C3%A9\">docs</a></p>\n");
}

TEST(link_destination_that_looks_like_title) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]('Title')")};
  EXPECT_EQ(result, "<p><a href=\"&#x27;Title&#x27;\">docs</a></p>\n");
}

TEST(link_title_quote_styles) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a](/x 'one') [b](/y (two)) [c](/z \"three\")")};
  EXPECT_EQ(result,
            "<p><a href=\"/x\" title=\"one\">a</a> <a href=\"/y\" "
            "title=\"two\">b</a> <a href=\"/z\" title=\"three\">c</a></p>\n");
}

TEST(link_title_escaped_quotes_and_entities) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/x 'it\\'s &amp; more')")};
  EXPECT_EQ(result,
            "<p><a href=\"/x\" title=\"it's &amp; more\">docs</a></p>\n");
}

TEST(link_title_separated_by_non_breaking_space) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](/x\xC2\xA0'title')")};
  EXPECT_EQ(result, "<p><a href=\"/x%C2%A0&#x27;title&#x27;\">docs</a></p>\n");
}

TEST(link_title_with_unescaped_inner_quote_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs](/x 'a'b')")};
  EXPECT_EQ(result, "<p>[docs](/x 'a'b')</p>\n");
}

TEST(link_title_parentheses_with_quotes_inside) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[docs](/x (with 'single' and \"double\"))")};
  EXPECT_EQ(result, "<p><a href=\"/x\" title=\"with 'single' and "
                    "&quot;double&quot;\">docs</a></p>\n");
}

TEST(link_whitespace_around_destination_and_title) {
  const auto result{
      sourcemeta::core::markdown_to_html("[docs](\n  /x\n  'title'\n)")};
  EXPECT_EQ(result, "<p><a href=\"/x\" title=\"title\">docs</a></p>\n");
}

TEST(link_spaces_before_parentheses_is_not_link) {
  const auto result{sourcemeta::core::markdown_to_html("[docs]  (/x)")};
  EXPECT_EQ(result, "<p>[docs]  (/x)</p>\n");
}

TEST(link_text_with_balanced_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("[a [b] [c [d]]](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a [b] [c [d]]</a></p>\n");
}

TEST(link_text_with_unbalanced_closing_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a] b](/x)")};
  EXPECT_EQ(result, "<p>[a] b](/x)</p>\n");
}

TEST(link_text_with_unbalanced_opening_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a [b](/x)")};
  EXPECT_EQ(result, "<p>[a <a href=\"/x\">b</a></p>\n");
}

TEST(link_text_with_escaped_bracket) {
  const auto result{sourcemeta::core::markdown_to_html("[a \\] b](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a ] b</a></p>\n");
}

TEST(link_text_with_inline_content) {
  const auto result{
      sourcemeta::core::markdown_to_html("[_a_ __b__ `c` ~~d~~](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\"><em>a</em> <strong>b</strong> "
                    "<code>c</code> <del>d</del></a></p>\n");
}

TEST(link_text_with_image) {
  const auto result{
      sourcemeta::core::markdown_to_html("[![logo](/logo.png 'Logo')](/home)")};
  EXPECT_EQ(result, "<p><a href=\"/home\"><img src=\"/logo.png\" alt=\"logo\" "
                    "title=\"Logo\" /></a></p>\n");
}

TEST(link_inside_link_text_is_not_allowed) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a [b](/inner) c](/outer)")};
  EXPECT_EQ(result, "<p>[a <a href=\"/inner\">b</a> c](/outer)</p>\n");
}

TEST(link_inside_emphasis_inside_link_text) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a _[b](/inner)_](/outer)")};
  EXPECT_EQ(result, "<p>[a <em><a href=\"/inner\">b</a></em>](/outer)</p>\n");
}

TEST(image_description_containing_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("![a [b](/inner)](/image.png)")};
  EXPECT_EQ(result, "<p><img src=\"/image.png\" alt=\"a b\" /></p>\n");
}

TEST(link_text_brackets_take_precedence_over_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("_[a_](/x)")};
  EXPECT_EQ(result, "<p>_<a href=\"/x\">a_</a></p>\n");
}

TEST(link_brackets_take_precedence_over_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html("[a _b](/x_)")};
  EXPECT_EQ(result, "<p><a href=\"/x_\">a _b</a></p>\n");
}

TEST(emphasis_does_not_span_link_text_brackets) {
  const auto result{sourcemeta::core::markdown_to_html("_a [b_ c]")};
  EXPECT_EQ(result, "<p><em>a [b</em> c]</p>\n");
}

TEST(link_code_span_takes_precedence) {
  const auto result{sourcemeta::core::markdown_to_html("[a`](/x)`")};
  EXPECT_EQ(result, "<p>[a<code>](/x)</code></p>\n");
}

TEST(link_autolink_takes_precedence) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a<https://sourcemeta.com/?q=](/x)>")};
  EXPECT_EQ(result, "<p>[a<a "
                    "href=\"https://sourcemeta.com/?q=%5D(/x)\">https://"
                    "sourcemeta.com/?q=](/x)</a></p>\n");
}

TEST(reference_link_full) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[the docs][manual]\n\n[manual]: /manual 'Manual'")};
  EXPECT_EQ(result,
            "<p><a href=\"/manual\" title=\"Manual\">the docs</a></p>\n");
}

TEST(reference_link_text_with_nested_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a [b] c][ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p><a href=\"/ref\">a [b] c</a></p>\n");
}

TEST(reference_link_label_case_folding) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[text][MaNuAl]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">text</a></p>\n");
}

TEST(reference_link_label_titlecase_digraph_folding) {
  const auto result{sourcemeta::core::markdown_to_html("[ǅ]\n\n[ǆ]: /digraph")};
  EXPECT_EQ(result, "<p><a href=\"/digraph\">\xc7\x85</a></p>\n");
}

TEST(reference_link_space_between_labels_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a] [ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p>[a] <a href=\"/ref\">ref</a></p>\n");
}

TEST(reference_link_newline_between_labels_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a]\n[ref]\n\n[ref]: /ref")};
  EXPECT_EQ(result, "<p>[a]\n"
                    "<a href=\"/ref\">ref</a></p>\n");
}

TEST(reference_link_uses_first_definition) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[a][ref]\n\n[ref]: /first\n[ref]: /second")};
  EXPECT_EQ(result, "<p><a href=\"/first\">a</a></p>\n");
}

TEST(reference_link_label_escapes_are_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][ref\\*]\n\n[ref*]: /ref")};
  EXPECT_EQ(result, "<p>[a][ref*]</p>\n");
}

TEST(reference_link_label_with_unescaped_bracket_is_invalid) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b]c]\n\n[b]c]: /x")};
  EXPECT_EQ(result, "<p>[a][b]c]</p>\n"
                    "<p>[b]c]: /x</p>\n");
}

TEST(reference_link_label_with_escaped_bracket) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b\\]c]\n\n[b\\]c]: /x")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a</a></p>\n");
}

TEST(reference_link_label_with_escaped_backslash) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a\\\\b]: /x\n\n[a\\\\b]")};
  EXPECT_EQ(result, "<p><a href=\"/x\">a\\b</a></p>\n");
}

TEST(reference_link_empty_brackets_are_literal) {
  const auto result{sourcemeta::core::markdown_to_html("[][]\n\n[]: /x")};
  EXPECT_EQ(result, "<p>[][]</p>\n"
                    "<p>[]: /x</p>\n");
}

TEST(collapsed_reference_link_case_insensitive) {
  const auto result{
      sourcemeta::core::markdown_to_html("[MANUAL][]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">MANUAL</a></p>\n");
}

TEST(collapsed_reference_link_across_lines_is_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual]\n[]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">manual</a>\n"
                    "[]</p>\n");
}

TEST(shortcut_reference_link_with_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[_the_ manual]\n\n[_the_ manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\"><em>the</em> manual</a></p>\n");
}

TEST(shortcut_reference_link_escaped) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\[manual\\]\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p>[manual]</p>\n");
}

TEST(shortcut_reference_link_label_with_underscore) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual_]: /manual\n\n_[manual_]")};
  EXPECT_EQ(result, "<p>_<a href=\"/manual\">manual_</a></p>\n");
}

TEST(reference_link_full_takes_precedence_over_shortcut) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b][c]\n\n[b]: /b\n[c]: /c")};
  EXPECT_EQ(result, "<p><a href=\"/b\">a</a><a href=\"/c\">c</a></p>\n");
}

TEST(reference_link_undefined_label_falls_back) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a][b][c]\n\n[a]: /a\n[c]: /c")};
  EXPECT_EQ(result, "<p>[a]<a href=\"/c\">b</a></p>\n");
}

TEST(inline_link_takes_precedence_over_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual]()\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"\">manual</a></p>\n");
}

TEST(inline_link_invalid_falls_back_to_reference) {
  const auto result{
      sourcemeta::core::markdown_to_html("[manual](a b)\n\n[manual]: /manual")};
  EXPECT_EQ(result, "<p><a href=\"/manual\">manual</a>(a b)</p>\n");
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
            "<p><img src=\"/l.png\" alt=\"logo\" title=\"it's\" /></p>\n");
}

TEST(autolink_uri_schemes) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<irc://chat.example>\n\n<x-custom+scheme.1:thing>\n\n<ab:c>")};
  EXPECT_EQ(
      result,
      "<p><a href=\"irc://chat.example\">irc://chat.example</a></p>\n"
      "<p><a href=\"x-custom+scheme.1:thing\">x-custom+scheme.1:thing</a></p>\n"
      "<p><a href=\"ab:c\">ab:c</a></p>\n");
}

TEST(autolink_uri_scheme_too_short_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("<z:thing>")};
  EXPECT_EQ(result, "<p>&lt;z:thing&gt;</p>\n");
}

TEST(autolink_uri_with_space_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/a b>")};
  EXPECT_EQ(result, "<p>&lt;<a "
                    "href=\"https://sourcemeta.com/a\">https://sourcemeta.com/"
                    "a</a> b&gt;</p>\n");
}

TEST(autolink_uri_backslash_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/\\_>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%5C_\">https://"
                    "sourcemeta.com/\\_</a></p>\n");
}

TEST(autolink_email) {
  const auto result{
      sourcemeta::core::markdown_to_html("<team@sourcemeta.com>")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a></p>\n");
}

TEST(autolink_email_with_dots_plus_and_hyphen) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<first.last+tag@sub-domain.sourcemeta.com>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"mailto:first.last+tag@sub-domain.sourcemeta.com\">"
                    "first.last+tag@sub-domain.sourcemeta.com</a></p>\n");
}

TEST(autolink_email_with_backslash_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<team\\@sourcemeta.com>")};
  EXPECT_EQ(
      result,
      "<p>&lt;<a "
      "href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a>&gt;</p>\n");
}

TEST(autolink_blank_or_trailing_space_angle_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("< >\n\n<https://sourcemeta.com >")};
  EXPECT_EQ(
      result,
      "<p>&lt; &gt;</p>\n"
      "<p>&lt;<a href=\"https://sourcemeta.com\">https://sourcemeta.com</a> "
      "&gt;</p>\n");
}

TEST(autolink_without_scheme_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("<sourcemeta.com>")};
  EXPECT_EQ(result, "<p>&lt;sourcemeta.com&gt;</p>\n");
}

TEST(hard_break_with_many_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("one     \ntwo")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(hard_break_leading_spaces_on_next_line_ignored) {
  const auto result{sourcemeta::core::markdown_to_html("one  \n        two")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(hard_break_inside_emphasis) {
  const auto result{
      sourcemeta::core::markdown_to_html("_one  \ntwo_\n\n__one\\\ntwo__")};
  EXPECT_EQ(result, "<p><em>one<br />\n"
                    "two</em></p>\n"
                    "<p><strong>one<br />\n"
                    "two</strong></p>\n");
}

TEST(hard_break_not_inside_code_span) {
  const auto result{sourcemeta::core::markdown_to_html("`one  \ntwo`")};
  EXPECT_EQ(result, "<p><code>one   two</code></p>\n");
}

TEST(hard_break_not_at_end_of_block) {
  const auto result{
      sourcemeta::core::markdown_to_html("text\\\n\ntext   \n\n## title\\")};
  EXPECT_EQ(result, "<p>text\\</p>\n"
                    "<p>text</p>\n"
                    "<h2>title\\</h2>\n");
}

TEST(soft_break_surrounding_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html("one \n  two")};
  EXPECT_EQ(result, "<p>one\n"
                    "two</p>\n");
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
