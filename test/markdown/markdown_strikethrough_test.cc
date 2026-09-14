#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("~~deleted~~")};
  EXPECT_EQ(result, "<p><del>deleted</del></p>\n");
}

TEST(strikethrough_with_other_formatting) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~deleted **bold** text~~")};
  EXPECT_EQ(result, "<p><del>deleted <strong>bold</strong> text</del></p>\n");
}

TEST(strikethrough_in_sentence) {
  const auto result{
      sourcemeta::core::markdown_to_html("This is ~~wrong~~ correct")};
  EXPECT_EQ(result, "<p>This is <del>wrong</del> correct</p>\n");
}

TEST(strikethrough_inside_table_cell) {
  const auto result{
      sourcemeta::core::markdown_to_html("| h |\n| - |\n| ~~old~~ |")};
  EXPECT_EQ(result, "<table>\n<thead>\n<tr>\n<th>h</th>\n</tr>\n</thead>\n"
                    "<tbody>\n<tr>\n<td><del>old</del></td>\n</tr>\n"
                    "</tbody>\n</table>\n");
}

TEST(strikethrough_with_code) {
  const auto result{sourcemeta::core::markdown_to_html("~~use `old_func()`~~")};
  EXPECT_EQ(result, "<p><del>use <code>old_func()</code></del></p>\n");
}

TEST(strikethrough_with_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~[removed](https://example.com)~~")};
  EXPECT_EQ(result,
            "<p><del><a href=\"https://example.com\">removed</a></del></p>\n");
}

TEST(strikethrough_bold_italic_combined) {
  const auto result{sourcemeta::core::markdown_to_html("~~***all three***~~")};
  EXPECT_EQ(result, "<p><del><em><strong>all three</strong></em></del></p>\n");
}

TEST(single_tilde_strikethrough) {
  const auto result{sourcemeta::core::markdown_to_html("~not deleted~")};
  EXPECT_EQ(result, "<p><del>not deleted</del></p>\n");
}

TEST(strikethrough_spanning_lines) {
  const auto result{sourcemeta::core::markdown_to_html("~~one\ntwo~~")};
  EXPECT_EQ(result, "<p><del>one\n"
                    "two</del></p>\n");
}

TEST(strikethrough_triple_tildes_inline) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~~y~~~ z")};
  EXPECT_EQ(result, "<p>x ~~~y~~~ z</p>\n");
}

TEST(strikethrough_unclosed) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~y")};
  EXPECT_EQ(result, "<p>x ~~y</p>\n");
}

TEST(strikethrough_with_surrounding_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("x ~~ y ~~ z")};
  EXPECT_EQ(result, "<p>x ~~ y ~~ z</p>\n");
}

TEST(strikethrough_and_emphasis_overlap) {
  const auto result{sourcemeta::core::markdown_to_html("~~_x~~_")};
  EXPECT_EQ(result, "<p><del>_x</del>_</p>\n");
}

TEST(strikethrough_escaped_closing) {
  const auto result{sourcemeta::core::markdown_to_html("~~x\\~~")};
  EXPECT_EQ(result, "<p>~~x~~</p>\n");
}

TEST(strikethrough_inside_link_text) {
  const auto result{sourcemeta::core::markdown_to_html("[~~old~~ new](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\"><del>old</del> new</a></p>\n");
}

TEST(strikethrough_with_one_or_two_tildes) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~Hi~~ Hello, ~there~ world!")};
  EXPECT_EQ(result, "<p><del>Hi</del> Hello, <del>there</del> world!</p>\n");
}

TEST(strikethrough_stops_at_the_end_of_a_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("This ~~has a\n\nnew paragraph~~.")};
  EXPECT_EQ(result, "<p>This ~~has a</p>\n"
                    "<p>new paragraph~~.</p>\n");
}

TEST(strikethrough_with_three_tildes_does_not_strike) {
  const auto result{
      sourcemeta::core::markdown_to_html("This will ~~~not~~~ strike.")};
  EXPECT_EQ(result, "<p>This will ~~~not~~~ strike.</p>\n");
}

TEST(strikethrough_pairs_need_the_same_number_of_tildes) {
  const auto result{sourcemeta::core::markdown_to_html("~one~~\n\n~~two~")};
  EXPECT_EQ(result, "<p>~one~~</p>\n"
                    "<p>~~two~</p>\n");
}

TEST(strikethrough_with_single_tilde_inside_double_tildes) {
  const auto result{sourcemeta::core::markdown_to_html("~~a~b~~")};
  EXPECT_EQ(result, "<p><del>a~b</del></p>\n");
}
