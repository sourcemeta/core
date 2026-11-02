#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(soft_line_break) {
  const auto result{sourcemeta::core::markdown_to_html("line one\nline two")};
  EXPECT_EQ(result, "<p>line one\nline two</p>\n");
}

TEST(hard_line_break_with_two_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("line one  \nline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
}

TEST(hard_line_break_with_backslash) {
  const auto result{sourcemeta::core::markdown_to_html("line one\\\nline two")};
  EXPECT_EQ(result, "<p>line one<br />\nline two</p>\n");
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
