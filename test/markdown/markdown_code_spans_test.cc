#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(inline_code) {
  const auto result{sourcemeta::core::markdown_to_html("Use `printf()`")};
  EXPECT_EQ(result, "<p>Use <code>printf()</code></p>\n");
}

TEST(inline_code_with_backticks_inside) {
  const auto result{
      sourcemeta::core::markdown_to_html("`` there is a ` here ``")};
  EXPECT_EQ(result, "<p><code>there is a ` here</code></p>\n");
}

TEST(inline_code_preserves_html) {
  const auto result{
      sourcemeta::core::markdown_to_html("`<div class=\"foo\">`")};
  EXPECT_EQ(result, "<p><code>&lt;div class=&quot;foo&quot;&gt;</code></p>\n");
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
