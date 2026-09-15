#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(task_list_unchecked) {
  const auto result{sourcemeta::core::markdown_to_html("- [ ] todo")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> todo</li>\n"
            "</ul>\n");
}

TEST(task_list_checked) {
  const auto result{sourcemeta::core::markdown_to_html("- [x] done")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" checked=\"\" disabled=\"\" />"
            " done</li>\n</ul>\n");
}

TEST(task_list_mixed) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- [x] done\n- [ ] todo\n- [x] also done")};
  EXPECT_EQ(
      result,
      "<ul>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> done</li>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> todo</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> also "
      "done</li>\n"
      "</ul>\n");
}

TEST(task_list_with_bold) {
  const auto result{
      sourcemeta::core::markdown_to_html("- [ ] **important** task")};
  EXPECT_EQ(result, "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> "
                    "<strong>important</strong> task</li>\n</ul>\n");
}

TEST(task_list_with_link) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- [x] see [docs](https://example.com)")};
  EXPECT_EQ(result,
            "<ul>\n<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> "
            "see <a href=\"https://example.com\">docs</a></li>\n</ul>\n");
}

TEST(task_list_with_code) {
  const auto result{sourcemeta::core::markdown_to_html("- [ ] fix `bug`")};
  EXPECT_EQ(result, "<ul>\n<li><input type=\"checkbox\" disabled=\"\" /> "
                    "fix <code>bug</code></li>\n</ul>\n");
}

TEST(task_list_uppercase_marker) {
  const auto result{sourcemeta::core::markdown_to_html("* [X] finished")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> finished</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_needs_following_space) {
  const auto result{sourcemeta::core::markdown_to_html("* [ ]pending")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[ ]pending</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_with_other_character) {
  const auto result{
      sourcemeta::core::markdown_to_html("* [-] partial\n* [x] complete")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[-] partial</li>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> complete</li>\n"
                    "</ul>\n");
}

TEST(task_list_nested) {
  const auto result{sourcemeta::core::markdown_to_html(
      "* [ ] parent\n  * [x] child\n    * [ ] grandchild")};
  EXPECT_EQ(result,
            "<ul>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> parent\n"
            "<ul>\n"
            "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> child\n"
            "<ul>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> grandchild</li>\n"
            "</ul>\n"
            "</li>\n"
            "</ul>\n"
            "</li>\n"
            "</ul>\n");
}

TEST(task_list_in_ordered_list) {
  const auto result{
      sourcemeta::core::markdown_to_html("1) [ ] first\n2) [X] second")};
  EXPECT_EQ(
      result,
      "<ol>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> first</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> second</li>\n"
      "</ol>\n");
}

TEST(task_list_marker_not_at_item_start) {
  const auto result{sourcemeta::core::markdown_to_html("* text [ ] later")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>text [ ] later</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_alone) {
  const auto result{sourcemeta::core::markdown_to_html("* [x]")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>[x]</li>\n"
                    "</ul>\n");
}

TEST(task_list_in_loose_list) {
  const auto result{sourcemeta::core::markdown_to_html("* [x] a\n\n* [ ] b")};
  EXPECT_EQ(result,
            "<ul>\n"
            "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> \n"
            "<p>a</p>\n"
            "</li>\n"
            "<li><input type=\"checkbox\" disabled=\"\" /> \n"
            "<p>b</p>\n"
            "</li>\n"
            "</ul>\n");
}

TEST(task_list_checked_state_comes_from_the_marker) {
  const auto result{
      sourcemeta::core::markdown_to_html("- [ ] investigate [x]")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li><input type=\"checkbox\" disabled=\"\" /> "
                    "investigate [x]</li>\n"
                    "</ul>\n");
}

TEST(task_list_tab_between_brackets_is_unchecked) {
  const auto result{sourcemeta::core::markdown_to_html("- [\t] tab")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li><input type=\"checkbox\" disabled=\"\" /> tab</li>\n"
                    "</ul>\n");
}

TEST(task_list_inside_blockquote) {
  const auto result{sourcemeta::core::markdown_to_html("> - [x] quoted")};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ul>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> quoted</li>\n"
                    "</ul>\n"
                    "</blockquote>\n");
}

TEST(task_list_marker_after_blank_item_start) {
  const auto result{sourcemeta::core::markdown_to_html("-\n  [x] later")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" "
                    "/> later</li>\n"
                    "</ul>\n");
}

TEST(task_list_marker_in_second_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n\n  [x] b")};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<p>[x] b</p>\n"
                    "</li>\n"
                    "</ul>\n");
}
