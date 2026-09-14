#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(thematic_break_with_dashes) {
  const auto result{sourcemeta::core::markdown_to_html("---")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_with_asterisks) {
  const auto result{sourcemeta::core::markdown_to_html("***")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_with_underscores) {
  const auto result{sourcemeta::core::markdown_to_html("___")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_with_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("- - -")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(consecutive_thematic_breaks) {
  const auto result{sourcemeta::core::markdown_to_html("---\n\n---")};
  EXPECT_EQ(result, "<hr />\n<hr />\n");
}

TEST(thematic_break_with_trailing_whitespace) {
  const auto result{sourcemeta::core::markdown_to_html("***\t  ")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_plus_signs_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("++++")};
  EXPECT_EQ(result, "<p>++++</p>\n");
}

TEST(thematic_break_equals_signs_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("= = = =")};
  EXPECT_EQ(result, "<p>= = = =</p>\n");
}

TEST(thematic_break_two_characters_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("**\n\n__")};
  EXPECT_EQ(result, "<p>**</p>\n"
                    "<p>__</p>\n");
}

TEST(thematic_break_indented_one_to_three_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("   ___\n  ---\n ***")};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n"
                    "<hr />\n");
}

TEST(thematic_break_indented_four_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("    ---")};
  EXPECT_EQ(result, "<pre><code>---\n"
                    "</code></pre>\n");
}

TEST(thematic_break_indented_four_spaces_continues_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("Paragraph text\n    ___")};
  EXPECT_EQ(result, "<p>Paragraph text\n"
                    "___</p>\n");
}

TEST(thematic_break_many_characters) {
  const auto result{sourcemeta::core::markdown_to_html(
      "-------------------------------------------------")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_spaces_between_characters) {
  const auto result{sourcemeta::core::markdown_to_html("* * * *")};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(thematic_break_with_other_characters_is_paragraph) {
  const auto result{
      sourcemeta::core::markdown_to_html("- - - x\n\nx______\n\n***x***")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>x</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "<p>x______</p>\n"
                    "<p><em><strong>x</strong></em></p>\n");
}

TEST(thematic_break_mixed_characters_is_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("-*-")};
  EXPECT_EQ(result, "<p>-*-</p>\n");
}

TEST(thematic_break_interrupts_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("alpha\n___\nbeta")};
  EXPECT_EQ(result, "<p>alpha</p>\n"
                    "<hr />\n"
                    "<p>beta</p>\n");
}

TEST(thematic_break_takes_precedence_over_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("- one\n- - -\n- two")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<hr />\n"
                    "<ul>\n"
                    "<li>two</li>\n"
                    "</ul>\n");
}

TEST(thematic_break_inside_list_item) {
  const auto result{sourcemeta::core::markdown_to_html("* one\n* - - -")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "<li>\n"
                    "<hr />\n"
                    "</li>\n"
                    "</ul>\n");
}
