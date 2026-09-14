#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(simple_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("Hello world")};
  EXPECT_EQ(result, "<p>Hello world</p>\n");
}

TEST(multiple_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html(
      "First paragraph\n\nSecond paragraph")};
  EXPECT_EQ(result, "<p>First paragraph</p>\n<p>Second paragraph</p>\n");
}

TEST(three_paragraphs) {
  const auto result{sourcemeta::core::markdown_to_html("One\n\nTwo\n\nThree")};
  EXPECT_EQ(result, "<p>One</p>\n<p>Two</p>\n<p>Three</p>\n");
}

TEST(empty_input) {
  const auto result{sourcemeta::core::markdown_to_html("")};
  EXPECT_TRUE(result.empty());
}

TEST(whitespace_only) {
  const auto result{sourcemeta::core::markdown_to_html("   \n\n   ")};
  EXPECT_TRUE(result.empty());
}

TEST(paragraph_with_inline_code_and_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Use `foo()` from [the docs](https://docs.com).")};
  EXPECT_EQ(result, "<p>Use <code>foo()</code> from "
                    "<a href=\"https://docs.com\">the docs</a>.</p>\n");
}

TEST(paragraph_leading_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html(" one\n  two")};
  EXPECT_EQ(result, "<p>one\n"
                    "two</p>\n");
}

TEST(paragraph_continuation_leading_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html(
      "one\n          two\n                     three")};
  EXPECT_EQ(result, "<p>one\n"
                    "two\n"
                    "three</p>\n");
}

TEST(paragraph_first_line_indented_three_spaces) {
  const auto result{sourcemeta::core::markdown_to_html("  first\nsecond")};
  EXPECT_EQ(result, "<p>first\n"
                    "second</p>\n");
}

TEST(paragraph_first_line_indented_five_spaces_is_code) {
  const auto result{sourcemeta::core::markdown_to_html("     first\nsecond")};
  EXPECT_EQ(result, "<pre><code> first\n"
                    "</code></pre>\n"
                    "<p>second</p>\n");
}

TEST(paragraph_final_trailing_spaces_stripped) {
  const auto result{sourcemeta::core::markdown_to_html("one   \ntwo   ")};
  EXPECT_EQ(result, "<p>one<br />\n"
                    "two</p>\n");
}

TEST(blank_lines_at_start_and_end_ignored) {
  const auto result{
      sourcemeta::core::markdown_to_html("\n  \none\n\n \n## two\n\n\n")};
  EXPECT_EQ(result, "<p>one</p>\n"
                    "<h2>two</h2>\n");
}
