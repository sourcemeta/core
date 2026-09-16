#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(example_1) {
  const auto result{
      sourcemeta::core::markdown_to_html("\tfoo\tbaz\t\tbim\n", false)};
  EXPECT_EQ(result, "<pre><code>foo\tbaz\t\tbim\n"
                    "</code></pre>\n");
}

TEST(example_2) {
  const auto result{
      sourcemeta::core::markdown_to_html("  \tfoo\tbaz\t\tbim\n", false)};
  EXPECT_EQ(result, "<pre><code>foo\tbaz\t\tbim\n"
                    "</code></pre>\n");
}

TEST(example_3) {
  const auto result{sourcemeta::core::markdown_to_html("    a\ta\n"
                                                       "    \xE1\xBD\x90\ta\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>a\ta\n"
                    "\xE1\xBD\x90\ta\n"
                    "</code></pre>\n");
}

TEST(example_4) {
  const auto result{sourcemeta::core::markdown_to_html("  - foo\n"
                                                       "\n"
                                                       "\tbar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_5) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "\n"
                                                       "\t\tbar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<pre><code>  bar\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_6) {
  const auto result{sourcemeta::core::markdown_to_html(">\t\tfoo\n", false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>  foo\n"
                    "</code></pre>\n"
                    "</blockquote>\n");
}

TEST(example_7) {
  const auto result{sourcemeta::core::markdown_to_html("-\t\tfoo\n", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<pre><code>  foo\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_8) {
  const auto result{sourcemeta::core::markdown_to_html("    foo\n"
                                                       "\tbar\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>foo\n"
                    "bar\n"
                    "</code></pre>\n");
}

TEST(example_9) {
  const auto result{sourcemeta::core::markdown_to_html(" - foo\n"
                                                       "   - bar\n"
                                                       "\t - baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo\n"
                    "<ul>\n"
                    "<li>bar\n"
                    "<ul>\n"
                    "<li>baz</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_10) {
  const auto result{sourcemeta::core::markdown_to_html("#\tFoo\n", false)};
  EXPECT_EQ(result, "<h1>Foo</h1>\n");
}

TEST(example_11) {
  const auto result{sourcemeta::core::markdown_to_html("*\t*\t*\t\n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_12) {
  const auto result{sourcemeta::core::markdown_to_html("- `one\n"
                                                       "- two`\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>`one</li>\n"
                    "<li>two`</li>\n"
                    "</ul>\n");
}

TEST(example_13) {
  const auto result{sourcemeta::core::markdown_to_html("***\n"
                                                       "---\n"
                                                       "___\n",
                                                       false)};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n"
                    "<hr />\n");
}

TEST(example_14) {
  const auto result{sourcemeta::core::markdown_to_html("+++\n", false)};
  EXPECT_EQ(result, "<p>+++</p>\n");
}

TEST(example_15) {
  const auto result{sourcemeta::core::markdown_to_html("===\n", false)};
  EXPECT_EQ(result, "<p>===</p>\n");
}

TEST(example_16) {
  const auto result{sourcemeta::core::markdown_to_html("--\n"
                                                       "**\n"
                                                       "__\n",
                                                       false)};
  EXPECT_EQ(result, "<p>--\n"
                    "**\n"
                    "__</p>\n");
}

TEST(example_17) {
  const auto result{sourcemeta::core::markdown_to_html(" ***\n"
                                                       "  ***\n"
                                                       "   ***\n",
                                                       false)};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n"
                    "<hr />\n");
}

TEST(example_18) {
  const auto result{sourcemeta::core::markdown_to_html("    ***\n", false)};
  EXPECT_EQ(result, "<pre><code>***\n"
                    "</code></pre>\n");
}

TEST(example_19) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "    ***\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "***</p>\n");
}

TEST(example_20) {
  const auto result{sourcemeta::core::markdown_to_html(
      "_____________________________________\n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_21) {
  const auto result{sourcemeta::core::markdown_to_html(" - - -\n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_22) {
  const auto result{
      sourcemeta::core::markdown_to_html(" **  * ** * ** * **\n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_23) {
  const auto result{
      sourcemeta::core::markdown_to_html("-     -      -      -\n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_24) {
  const auto result{sourcemeta::core::markdown_to_html("- - - -    \n", false)};
  EXPECT_EQ(result, "<hr />\n");
}

TEST(example_25) {
  const auto result{sourcemeta::core::markdown_to_html("_ _ _ _ a\n"
                                                       "\n"
                                                       "a------\n"
                                                       "\n"
                                                       "---a---\n",
                                                       false)};
  EXPECT_EQ(result, "<p>_ _ _ _ a</p>\n"
                    "<p>a------</p>\n"
                    "<p>---a---</p>\n");
}

TEST(example_26) {
  const auto result{sourcemeta::core::markdown_to_html(" *-*\n", false)};
  EXPECT_EQ(result, "<p><em>-</em></p>\n");
}

TEST(example_27) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "***\n"
                                                       "- bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n"
                    "<hr />\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n");
}

TEST(example_28) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "***\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo</p>\n"
                    "<hr />\n"
                    "<p>bar</p>\n");
}

TEST(example_29) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "---\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo</h2>\n"
                    "<p>bar</p>\n");
}

TEST(example_30) {
  const auto result{sourcemeta::core::markdown_to_html("* Foo\n"
                                                       "* * *\n"
                                                       "* Bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>Foo</li>\n"
                    "</ul>\n"
                    "<hr />\n"
                    "<ul>\n"
                    "<li>Bar</li>\n"
                    "</ul>\n");
}

TEST(example_31) {
  const auto result{sourcemeta::core::markdown_to_html("- Foo\n"
                                                       "- * * *\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>Foo</li>\n"
                    "<li>\n"
                    "<hr />\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_32) {
  const auto result{sourcemeta::core::markdown_to_html("# foo\n"
                                                       "## foo\n"
                                                       "### foo\n"
                                                       "#### foo\n"
                                                       "##### foo\n"
                                                       "###### foo\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>foo</h1>\n"
                    "<h2>foo</h2>\n"
                    "<h3>foo</h3>\n"
                    "<h4>foo</h4>\n"
                    "<h5>foo</h5>\n"
                    "<h6>foo</h6>\n");
}

TEST(example_33) {
  const auto result{sourcemeta::core::markdown_to_html("####### foo\n", false)};
  EXPECT_EQ(result, "<p>####### foo</p>\n");
}

TEST(example_34) {
  const auto result{sourcemeta::core::markdown_to_html("#5 bolt\n"
                                                       "\n"
                                                       "#hashtag\n",
                                                       false)};
  EXPECT_EQ(result, "<p>#5 bolt</p>\n"
                    "<p>#hashtag</p>\n");
}

TEST(example_35) {
  const auto result{sourcemeta::core::markdown_to_html("\\## foo\n", false)};
  EXPECT_EQ(result, "<p>## foo</p>\n");
}

TEST(example_36) {
  const auto result{
      sourcemeta::core::markdown_to_html("# foo *bar* \\*baz\\*\n", false)};
  EXPECT_EQ(result, "<h1>foo <em>bar</em> *baz*</h1>\n");
}

TEST(example_37) {
  const auto result{sourcemeta::core::markdown_to_html(
      "#                  foo                     \n", false)};
  EXPECT_EQ(result, "<h1>foo</h1>\n");
}

TEST(example_38) {
  const auto result{sourcemeta::core::markdown_to_html(" ### foo\n"
                                                       "  ## foo\n"
                                                       "   # foo\n",
                                                       false)};
  EXPECT_EQ(result, "<h3>foo</h3>\n"
                    "<h2>foo</h2>\n"
                    "<h1>foo</h1>\n");
}

TEST(example_39) {
  const auto result{sourcemeta::core::markdown_to_html("    # foo\n", false)};
  EXPECT_EQ(result, "<pre><code># foo\n"
                    "</code></pre>\n");
}

TEST(example_40) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "    # bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo\n"
                    "# bar</p>\n");
}

TEST(example_41) {
  const auto result{sourcemeta::core::markdown_to_html("## foo ##\n"
                                                       "  ###   bar    ###\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>foo</h2>\n"
                    "<h3>bar</h3>\n");
}

TEST(example_42) {
  const auto result{sourcemeta::core::markdown_to_html(
      "# foo ##################################\n"
      "##### foo ##\n",
      false)};
  EXPECT_EQ(result, "<h1>foo</h1>\n"
                    "<h5>foo</h5>\n");
}

TEST(example_43) {
  const auto result{
      sourcemeta::core::markdown_to_html("### foo ###     \n", false)};
  EXPECT_EQ(result, "<h3>foo</h3>\n");
}

TEST(example_44) {
  const auto result{
      sourcemeta::core::markdown_to_html("### foo ### b\n", false)};
  EXPECT_EQ(result, "<h3>foo ### b</h3>\n");
}

TEST(example_45) {
  const auto result{sourcemeta::core::markdown_to_html("# foo#\n", false)};
  EXPECT_EQ(result, "<h1>foo#</h1>\n");
}

TEST(example_46) {
  const auto result{sourcemeta::core::markdown_to_html("### foo \\###\n"
                                                       "## foo #\\##\n"
                                                       "# foo \\#\n",
                                                       false)};
  EXPECT_EQ(result, "<h3>foo ###</h3>\n"
                    "<h2>foo ###</h2>\n"
                    "<h1>foo #</h1>\n");
}

TEST(example_47) {
  const auto result{sourcemeta::core::markdown_to_html("****\n"
                                                       "## foo\n"
                                                       "****\n",
                                                       false)};
  EXPECT_EQ(result, "<hr />\n"
                    "<h2>foo</h2>\n"
                    "<hr />\n");
}

TEST(example_48) {
  const auto result{sourcemeta::core::markdown_to_html("Foo bar\n"
                                                       "# baz\n"
                                                       "Bar foo\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo bar</p>\n"
                    "<h1>baz</h1>\n"
                    "<p>Bar foo</p>\n");
}

TEST(example_49) {
  const auto result{sourcemeta::core::markdown_to_html("## \n"
                                                       "#\n"
                                                       "### ###\n",
                                                       false)};
  EXPECT_EQ(result, "<h2></h2>\n"
                    "<h1></h1>\n"
                    "<h3></h3>\n");
}

TEST(example_50) {
  const auto result{sourcemeta::core::markdown_to_html("Foo *bar*\n"
                                                       "=========\n"
                                                       "\n"
                                                       "Foo *bar*\n"
                                                       "---------\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>Foo <em>bar</em></h1>\n"
                    "<h2>Foo <em>bar</em></h2>\n");
}

TEST(example_51) {
  const auto result{sourcemeta::core::markdown_to_html("Foo *bar\n"
                                                       "baz*\n"
                                                       "====\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>Foo <em>bar\n"
                    "baz</em></h1>\n");
}

TEST(example_52) {
  const auto result{sourcemeta::core::markdown_to_html("  Foo *bar\n"
                                                       "baz*\t\n"
                                                       "====\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>Foo <em>bar\n"
                    "baz</em></h1>\n");
}

TEST(example_53) {
  const auto result{
      sourcemeta::core::markdown_to_html("Foo\n"
                                         "-------------------------\n"
                                         "\n"
                                         "Foo\n"
                                         "=\n",
                                         false)};
  EXPECT_EQ(result, "<h2>Foo</h2>\n"
                    "<h1>Foo</h1>\n");
}

TEST(example_54) {
  const auto result{sourcemeta::core::markdown_to_html("   Foo\n"
                                                       "---\n"
                                                       "\n"
                                                       "  Foo\n"
                                                       "-----\n"
                                                       "\n"
                                                       "  Foo\n"
                                                       "  ===\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo</h2>\n"
                    "<h2>Foo</h2>\n"
                    "<h1>Foo</h1>\n");
}

TEST(example_55) {
  const auto result{sourcemeta::core::markdown_to_html("    Foo\n"
                                                       "    ---\n"
                                                       "\n"
                                                       "    Foo\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>Foo\n"
                    "---\n"
                    "\n"
                    "Foo\n"
                    "</code></pre>\n"
                    "<hr />\n");
}

TEST(example_56) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "   ----      \n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo</h2>\n");
}

TEST(example_57) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "    ---\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "---</p>\n");
}

TEST(example_58) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "= =\n"
                                                       "\n"
                                                       "Foo\n"
                                                       "--- -\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "= =</p>\n"
                    "<p>Foo</p>\n"
                    "<hr />\n");
}

TEST(example_59) {
  const auto result{sourcemeta::core::markdown_to_html("Foo  \n"
                                                       "-----\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo</h2>\n");
}

TEST(example_60) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\\\n"
                                                       "----\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo\\</h2>\n");
}

TEST(example_61) {
  const auto result{sourcemeta::core::markdown_to_html("`Foo\n"
                                                       "----\n"
                                                       "`\n"
                                                       "\n"
                                                       "<a title=\"a lot\n"
                                                       "---\n"
                                                       "of dashes\"/>\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>`Foo</h2>\n"
                    "<p>`</p>\n"
                    "<h2>&lt;a title=&quot;a lot</h2>\n"
                    "<p>of dashes&quot;/&gt;</p>\n");
}

TEST(example_62) {
  const auto result{sourcemeta::core::markdown_to_html("> Foo\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>Foo</p>\n"
                    "</blockquote>\n"
                    "<hr />\n");
}

TEST(example_63) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "bar\n"
                                                       "===\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo\n"
                    "bar\n"
                    "===</p>\n"
                    "</blockquote>\n");
}

TEST(example_64) {
  const auto result{sourcemeta::core::markdown_to_html("- Foo\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>Foo</li>\n"
                    "</ul>\n"
                    "<hr />\n");
}

TEST(example_65) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "Bar\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>Foo\n"
                    "Bar</h2>\n");
}

TEST(example_66) {
  const auto result{sourcemeta::core::markdown_to_html("---\n"
                                                       "Foo\n"
                                                       "---\n"
                                                       "Bar\n"
                                                       "---\n"
                                                       "Baz\n",
                                                       false)};
  EXPECT_EQ(result, "<hr />\n"
                    "<h2>Foo</h2>\n"
                    "<h2>Bar</h2>\n"
                    "<p>Baz</p>\n");
}

TEST(example_67) {
  const auto result{sourcemeta::core::markdown_to_html("\n"
                                                       "====\n",
                                                       false)};
  EXPECT_EQ(result, "<p>====</p>\n");
}

TEST(example_68) {
  const auto result{sourcemeta::core::markdown_to_html("---\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<hr />\n"
                    "<hr />\n");
}

TEST(example_69) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "-----\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n"
                    "<hr />\n");
}

TEST(example_70) {
  const auto result{sourcemeta::core::markdown_to_html("    foo\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>foo\n"
                    "</code></pre>\n"
                    "<hr />\n");
}

TEST(example_71) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "-----\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo</p>\n"
                    "</blockquote>\n"
                    "<hr />\n");
}

TEST(example_72) {
  const auto result{sourcemeta::core::markdown_to_html("\\> foo\n"
                                                       "------\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>&gt; foo</h2>\n");
}

TEST(example_73) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "\n"
                                                       "bar\n"
                                                       "---\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo</p>\n"
                    "<h2>bar</h2>\n"
                    "<p>baz</p>\n");
}

TEST(example_74) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "bar\n"
                                                       "\n"
                                                       "---\n"
                                                       "\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "bar</p>\n"
                    "<hr />\n"
                    "<p>baz</p>\n");
}

TEST(example_75) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "bar\n"
                                                       "* * *\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "bar</p>\n"
                    "<hr />\n"
                    "<p>baz</p>\n");
}

TEST(example_76) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "bar\n"
                                                       "\\---\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "bar\n"
                    "---\n"
                    "baz</p>\n");
}

TEST(example_77) {
  const auto result{
      sourcemeta::core::markdown_to_html("    a simple\n"
                                         "      indented code block\n",
                                         false)};
  EXPECT_EQ(result, "<pre><code>a simple\n"
                    "  indented code block\n"
                    "</code></pre>\n");
}

TEST(example_78) {
  const auto result{sourcemeta::core::markdown_to_html("  - foo\n"
                                                       "\n"
                                                       "    bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_79) {
  const auto result{sourcemeta::core::markdown_to_html("1.  foo\n"
                                                       "\n"
                                                       "    - bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_80) {
  const auto result{sourcemeta::core::markdown_to_html("    <a/>\n"
                                                       "    *hi*\n"
                                                       "\n"
                                                       "    - one\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>&lt;a/&gt;\n"
                    "*hi*\n"
                    "\n"
                    "- one\n"
                    "</code></pre>\n");
}

TEST(example_81) {
  const auto result{sourcemeta::core::markdown_to_html("    chunk1\n"
                                                       "\n"
                                                       "    chunk2\n"
                                                       "  \n"
                                                       " \n"
                                                       " \n"
                                                       "    chunk3\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>chunk1\n"
                    "\n"
                    "chunk2\n"
                    "\n"
                    "\n"
                    "\n"
                    "chunk3\n"
                    "</code></pre>\n");
}

TEST(example_82) {
  const auto result{sourcemeta::core::markdown_to_html("    chunk1\n"
                                                       "      \n"
                                                       "      chunk2\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>chunk1\n"
                    "  \n"
                    "  chunk2\n"
                    "</code></pre>\n");
}

TEST(example_83) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "    bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "bar</p>\n");
}

TEST(example_84) {
  const auto result{sourcemeta::core::markdown_to_html("    foo\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>foo\n"
                    "</code></pre>\n"
                    "<p>bar</p>\n");
}

TEST(example_85) {
  const auto result{sourcemeta::core::markdown_to_html("# Heading\n"
                                                       "    foo\n"
                                                       "Heading\n"
                                                       "------\n"
                                                       "    foo\n"
                                                       "----\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>Heading</h1>\n"
                    "<pre><code>foo\n"
                    "</code></pre>\n"
                    "<h2>Heading</h2>\n"
                    "<pre><code>foo\n"
                    "</code></pre>\n"
                    "<hr />\n");
}

TEST(example_86) {
  const auto result{sourcemeta::core::markdown_to_html("        foo\n"
                                                       "    bar\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>    foo\n"
                    "bar\n"
                    "</code></pre>\n");
}

TEST(example_87) {
  const auto result{sourcemeta::core::markdown_to_html("\n"
                                                       "    \n"
                                                       "    foo\n"
                                                       "    \n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>foo\n"
                    "</code></pre>\n");
}

TEST(example_88) {
  const auto result{sourcemeta::core::markdown_to_html("    foo  \n", false)};
  EXPECT_EQ(result, "<pre><code>foo  \n"
                    "</code></pre>\n");
}

TEST(example_89) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "<\n"
                                                       " >\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>&lt;\n"
                    " &gt;\n"
                    "</code></pre>\n");
}

TEST(example_90) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\n"
                                                       "<\n"
                                                       " >\n"
                                                       "~~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>&lt;\n"
                    " &gt;\n"
                    "</code></pre>\n");
}

TEST(example_91) {
  const auto result{sourcemeta::core::markdown_to_html("``\n"
                                                       "foo\n"
                                                       "``\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>foo</code></p>\n");
}

TEST(example_92) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "aaa\n"
                                                       "~~~\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "~~~\n"
                    "</code></pre>\n");
}

TEST(example_93) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\n"
                                                       "aaa\n"
                                                       "```\n"
                                                       "~~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "```\n"
                    "</code></pre>\n");
}

TEST(example_94) {
  const auto result{sourcemeta::core::markdown_to_html("````\n"
                                                       "aaa\n"
                                                       "```\n"
                                                       "``````\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "```\n"
                    "</code></pre>\n");
}

TEST(example_95) {
  const auto result{sourcemeta::core::markdown_to_html("~~~~\n"
                                                       "aaa\n"
                                                       "~~~\n"
                                                       "~~~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "~~~\n"
                    "</code></pre>\n");
}

TEST(example_96) {
  const auto result{sourcemeta::core::markdown_to_html("```\n", false)};
  EXPECT_EQ(result, "<pre><code></code></pre>\n");
}

TEST(example_97) {
  const auto result{sourcemeta::core::markdown_to_html("`````\n"
                                                       "\n"
                                                       "```\n"
                                                       "aaa\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>\n"
                    "```\n"
                    "aaa\n"
                    "</code></pre>\n");
}

TEST(example_98) {
  const auto result{sourcemeta::core::markdown_to_html("> ```\n"
                                                       "> aaa\n"
                                                       "\n"
                                                       "bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>aaa\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<p>bbb</p>\n");
}

TEST(example_99) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "\n"
                                                       "  \n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>\n"
                    "  \n"
                    "</code></pre>\n");
}

TEST(example_100) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code></code></pre>\n");
}

TEST(example_101) {
  const auto result{sourcemeta::core::markdown_to_html(" ```\n"
                                                       " aaa\n"
                                                       "aaa\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "aaa\n"
                    "</code></pre>\n");
}

TEST(example_102) {
  const auto result{sourcemeta::core::markdown_to_html("  ```\n"
                                                       "aaa\n"
                                                       "  aaa\n"
                                                       "aaa\n"
                                                       "  ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "aaa\n"
                    "aaa\n"
                    "</code></pre>\n");
}

TEST(example_103) {
  const auto result{sourcemeta::core::markdown_to_html("   ```\n"
                                                       "   aaa\n"
                                                       "    aaa\n"
                                                       "  aaa\n"
                                                       "   ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    " aaa\n"
                    "aaa\n"
                    "</code></pre>\n");
}

TEST(example_104) {
  const auto result{sourcemeta::core::markdown_to_html("    ```\n"
                                                       "    aaa\n"
                                                       "    ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>```\n"
                    "aaa\n"
                    "```\n"
                    "</code></pre>\n");
}

TEST(example_105) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "aaa\n"
                                                       "  ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "</code></pre>\n");
}

TEST(example_106) {
  const auto result{sourcemeta::core::markdown_to_html("   ```\n"
                                                       "aaa\n"
                                                       "  ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "</code></pre>\n");
}

TEST(example_107) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "aaa\n"
                                                       "    ```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "    ```\n"
                    "</code></pre>\n");
}

TEST(example_108) {
  const auto result{sourcemeta::core::markdown_to_html("``` ```\n"
                                                       "aaa\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code> </code>\n"
                    "aaa</p>\n");
}

TEST(example_109) {
  const auto result{sourcemeta::core::markdown_to_html("~~~~~~\n"
                                                       "aaa\n"
                                                       "~~~ ~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "~~~ ~~\n"
                    "</code></pre>\n");
}

TEST(example_110) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "```\n"
                                                       "bar\n"
                                                       "```\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo</p>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "<p>baz</p>\n");
}

TEST(example_111) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "---\n"
                                                       "~~~\n"
                                                       "bar\n"
                                                       "~~~\n"
                                                       "# baz\n",
                                                       false)};
  EXPECT_EQ(result, "<h2>foo</h2>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "<h1>baz</h1>\n");
}

TEST(example_112) {
  const auto result{sourcemeta::core::markdown_to_html("```ruby\n"
                                                       "def foo(x)\n"
                                                       "  return 3\n"
                                                       "end\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code class=\"language-ruby\">def foo(x)\n"
                    "  return 3\n"
                    "end\n"
                    "</code></pre>\n");
}

TEST(example_113) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~~~    ruby startline=3 $%@#$\n"
                                         "def foo(x)\n"
                                         "  return 3\n"
                                         "end\n"
                                         "~~~~~~~\n",
                                         false)};
  EXPECT_EQ(result, "<pre><code class=\"language-ruby\">def foo(x)\n"
                    "  return 3\n"
                    "end\n"
                    "</code></pre>\n");
}

TEST(example_114) {
  const auto result{sourcemeta::core::markdown_to_html("````;\n"
                                                       "````\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code class=\"language-;\"></code></pre>\n");
}

TEST(example_115) {
  const auto result{sourcemeta::core::markdown_to_html("``` aa ```\n"
                                                       "foo\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>aa</code>\n"
                    "foo</p>\n");
}

TEST(example_116) {
  const auto result{sourcemeta::core::markdown_to_html("~~~ aa ``` ~~~\n"
                                                       "foo\n"
                                                       "~~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code class=\"language-aa\">foo\n"
                    "</code></pre>\n");
}

TEST(example_117) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "``` aaa\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>``` aaa\n"
                    "</code></pre>\n");
}

TEST(example_118) {
  const auto result{sourcemeta::core::markdown_to_html("<table><tr><td>\n"
                                                       "<pre>\n"
                                                       "**Hello**,\n"
                                                       "\n"
                                                       "_world_.\n"
                                                       "</pre>\n"
                                                       "</td></tr></table>\n",
                                                       false)};
  EXPECT_EQ(result, "<table><tr><td>\n"
                    "<pre>\n"
                    "**Hello**,\n"
                    "<p><em>world</em>.\n"
                    "</pre></p>\n"
                    "</td></tr></table>\n");
}

TEST(example_119) {
  const auto result{sourcemeta::core::markdown_to_html("<table>\n"
                                                       "  <tr>\n"
                                                       "    <td>\n"
                                                       "           hi\n"
                                                       "    </td>\n"
                                                       "  </tr>\n"
                                                       "</table>\n"
                                                       "\n"
                                                       "okay.\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "  <tr>\n"
                    "    <td>\n"
                    "           hi\n"
                    "    </td>\n"
                    "  </tr>\n"
                    "</table>\n"
                    "<p>okay.</p>\n");
}

TEST(example_120) {
  const auto result{sourcemeta::core::markdown_to_html(" <div>\n"
                                                       "  *hello*\n"
                                                       "         <foo><a>\n",
                                                       false)};
  EXPECT_EQ(result, " <div>\n"
                    "  *hello*\n"
                    "         <foo><a>\n");
}

TEST(example_121) {
  const auto result{sourcemeta::core::markdown_to_html("</div>\n"
                                                       "*foo*\n",
                                                       false)};
  EXPECT_EQ(result, "</div>\n"
                    "*foo*\n");
}

TEST(example_122) {
  const auto result{sourcemeta::core::markdown_to_html("<DIV CLASS=\"foo\">\n"
                                                       "\n"
                                                       "*Markdown*\n"
                                                       "\n"
                                                       "</DIV>\n",
                                                       false)};
  EXPECT_EQ(result, "<DIV CLASS=\"foo\">\n"
                    "<p><em>Markdown</em></p>\n"
                    "</DIV>\n");
}

TEST(example_123) {
  const auto result{sourcemeta::core::markdown_to_html("<div id=\"foo\"\n"
                                                       "  class=\"bar\">\n"
                                                       "</div>\n",
                                                       false)};
  EXPECT_EQ(result, "<div id=\"foo\"\n"
                    "  class=\"bar\">\n"
                    "</div>\n");
}

TEST(example_124) {
  const auto result{
      sourcemeta::core::markdown_to_html("<div id=\"foo\" class=\"bar\n"
                                         "  baz\">\n"
                                         "</div>\n",
                                         false)};
  EXPECT_EQ(result, "<div id=\"foo\" class=\"bar\n"
                    "  baz\">\n"
                    "</div>\n");
}

TEST(example_125) {
  const auto result{sourcemeta::core::markdown_to_html("<div>\n"
                                                       "*foo*\n"
                                                       "\n"
                                                       "*bar*\n",
                                                       false)};
  EXPECT_EQ(result, "<div>\n"
                    "*foo*\n"
                    "<p><em>bar</em></p>\n");
}

TEST(example_126) {
  const auto result{sourcemeta::core::markdown_to_html("<div id=\"foo\"\n"
                                                       "*hi*\n",
                                                       false)};
  EXPECT_EQ(result, "<div id=\"foo\"\n"
                    "*hi*\n");
}

TEST(example_127) {
  const auto result{sourcemeta::core::markdown_to_html("<div class\n"
                                                       "foo\n",
                                                       false)};
  EXPECT_EQ(result, "<div class\n"
                    "foo\n");
}

TEST(example_128) {
  const auto result{sourcemeta::core::markdown_to_html("<div *?\?\?-&&&-<---\n"
                                                       "*foo*\n",
                                                       false)};
  EXPECT_EQ(result, "<div *?\?\?-&&&-<---\n"
                    "*foo*\n");
}

TEST(example_129) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<div><a href=\"bar\">*foo*</a></div>\n", false)};
  EXPECT_EQ(result, "<div><a href=\"bar\">*foo*</a></div>\n");
}

TEST(example_130) {
  const auto result{sourcemeta::core::markdown_to_html("<table><tr><td>\n"
                                                       "foo\n"
                                                       "</td></tr></table>\n",
                                                       false)};
  EXPECT_EQ(result, "<table><tr><td>\n"
                    "foo\n"
                    "</td></tr></table>\n");
}

TEST(example_131) {
  const auto result{sourcemeta::core::markdown_to_html("<div></div>\n"
                                                       "``` c\n"
                                                       "int x = 33;\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<div></div>\n"
                    "``` c\n"
                    "int x = 33;\n"
                    "```\n");
}

TEST(example_132) {
  const auto result{sourcemeta::core::markdown_to_html("<a href=\"foo\">\n"
                                                       "*bar*\n"
                                                       "</a>\n",
                                                       false)};
  EXPECT_EQ(result, "<a href=\"foo\">\n"
                    "*bar*\n"
                    "</a>\n");
}

TEST(example_133) {
  const auto result{sourcemeta::core::markdown_to_html("<Warning>\n"
                                                       "*bar*\n"
                                                       "</Warning>\n",
                                                       false)};
  EXPECT_EQ(result, "<Warning>\n"
                    "*bar*\n"
                    "</Warning>\n");
}

TEST(example_134) {
  const auto result{sourcemeta::core::markdown_to_html("<i class=\"foo\">\n"
                                                       "*bar*\n"
                                                       "</i>\n",
                                                       false)};
  EXPECT_EQ(result, "<i class=\"foo\">\n"
                    "*bar*\n"
                    "</i>\n");
}

TEST(example_135) {
  const auto result{sourcemeta::core::markdown_to_html("</ins>\n"
                                                       "*bar*\n",
                                                       false)};
  EXPECT_EQ(result, "</ins>\n"
                    "*bar*\n");
}

TEST(example_136) {
  const auto result{sourcemeta::core::markdown_to_html("<del>\n"
                                                       "*foo*\n"
                                                       "</del>\n",
                                                       false)};
  EXPECT_EQ(result, "<del>\n"
                    "*foo*\n"
                    "</del>\n");
}

TEST(example_137) {
  const auto result{sourcemeta::core::markdown_to_html("<del>\n"
                                                       "\n"
                                                       "*foo*\n"
                                                       "\n"
                                                       "</del>\n",
                                                       false)};
  EXPECT_EQ(result, "<del>\n"
                    "<p><em>foo</em></p>\n"
                    "</del>\n");
}

TEST(example_138) {
  const auto result{
      sourcemeta::core::markdown_to_html("<del>*foo*</del>\n", false)};
  EXPECT_EQ(result, "<p><del><em>foo</em></del></p>\n");
}

TEST(example_139) {
  const auto result{
      sourcemeta::core::markdown_to_html("<pre language=\"haskell\"><code>\n"
                                         "import Text.HTML.TagSoup\n"
                                         "\n"
                                         "main :: IO ()\n"
                                         "main = print $ parseTags tags\n"
                                         "</code></pre>\n"
                                         "okay\n",
                                         false)};
  EXPECT_EQ(result, "<pre language=\"haskell\"><code>\n"
                    "import Text.HTML.TagSoup\n"
                    "\n"
                    "main :: IO ()\n"
                    "main = print $ parseTags tags\n"
                    "</code></pre>\n"
                    "<p>okay</p>\n");
}

// GFM section 6.11 filters this tag, which this example of GFM section 4.6 does
// not account for
TEST(example_140) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<script type=\"text/javascript\">\n"
      "// JavaScript example\n"
      "\n"
      "document.getElementById(\"demo\").innerHTML = \"Hello JavaScript!\";\n"
      "</script>\n"
      "okay\n",
      false)};
  EXPECT_EQ(
      result,
      "&lt;script type=\"text/javascript\">\n"
      "// JavaScript example\n"
      "\n"
      "document.getElementById(\"demo\").innerHTML = \"Hello JavaScript!\";\n"
      "&lt;/script>\n"
      "<p>okay</p>\n");
}

// GFM section 6.11 filters this tag, which this example of GFM section 4.6 does
// not account for
TEST(example_141) {
  const auto result{sourcemeta::core::markdown_to_html("<style\n"
                                                       "  type=\"text/css\">\n"
                                                       "h1 {color:red;}\n"
                                                       "\n"
                                                       "p {color:blue;}\n"
                                                       "</style>\n"
                                                       "okay\n",
                                                       false)};
  EXPECT_EQ(result, "&lt;style\n"
                    "  type=\"text/css\">\n"
                    "h1 {color:red;}\n"
                    "\n"
                    "p {color:blue;}\n"
                    "&lt;/style>\n"
                    "<p>okay</p>\n");
}

// GFM section 6.11 filters this tag, which this example of GFM section 4.6 does
// not account for
TEST(example_142) {
  const auto result{sourcemeta::core::markdown_to_html("<style\n"
                                                       "  type=\"text/css\">\n"
                                                       "\n"
                                                       "foo\n",
                                                       false)};
  EXPECT_EQ(result, "&lt;style\n"
                    "  type=\"text/css\">\n"
                    "\n"
                    "foo\n");
}

TEST(example_143) {
  const auto result{sourcemeta::core::markdown_to_html("> <div>\n"
                                                       "> foo\n"
                                                       "\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<div>\n"
                    "foo\n"
                    "</blockquote>\n"
                    "<p>bar</p>\n");
}

TEST(example_144) {
  const auto result{sourcemeta::core::markdown_to_html("- <div>\n"
                                                       "- foo\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<div>\n"
                    "</li>\n"
                    "<li>foo</li>\n"
                    "</ul>\n");
}

// GFM section 6.11 filters this tag, which this example of GFM section 4.6 does
// not account for
TEST(example_145) {
  const auto result{
      sourcemeta::core::markdown_to_html("<style>p{color:red;}</style>\n"
                                         "*foo*\n",
                                         false)};
  EXPECT_EQ(result, "&lt;style>p{color:red;}&lt;/style>\n"
                    "<p><em>foo</em></p>\n");
}

TEST(example_146) {
  const auto result{sourcemeta::core::markdown_to_html("<!-- foo -->*bar*\n"
                                                       "*baz*\n",
                                                       false)};
  EXPECT_EQ(result, "<!-- foo -->*bar*\n"
                    "<p><em>baz</em></p>\n");
}

// GFM section 6.11 filters this tag, which this example of GFM section 4.6 does
// not account for
TEST(example_147) {
  const auto result{sourcemeta::core::markdown_to_html("<script>\n"
                                                       "foo\n"
                                                       "</script>1. *bar*\n",
                                                       false)};
  EXPECT_EQ(result, "&lt;script>\n"
                    "foo\n"
                    "&lt;/script>1. *bar*\n");
}

TEST(example_148) {
  const auto result{sourcemeta::core::markdown_to_html("<!-- Foo\n"
                                                       "\n"
                                                       "bar\n"
                                                       "   baz -->\n"
                                                       "okay\n",
                                                       false)};
  EXPECT_EQ(result, "<!-- Foo\n"
                    "\n"
                    "bar\n"
                    "   baz -->\n"
                    "<p>okay</p>\n");
}

TEST(example_149) {
  const auto result{sourcemeta::core::markdown_to_html("<?php\n"
                                                       "\n"
                                                       "  echo '>';\n"
                                                       "\n"
                                                       "?>\n"
                                                       "okay\n",
                                                       false)};
  EXPECT_EQ(result, "<?php\n"
                    "\n"
                    "  echo '>';\n"
                    "\n"
                    "?>\n"
                    "<p>okay</p>\n");
}

TEST(example_150) {
  const auto result{
      sourcemeta::core::markdown_to_html("<!DOCTYPE html>\n", false)};
  EXPECT_EQ(result, "<!DOCTYPE html>\n");
}

TEST(example_151) {
  const auto result{
      sourcemeta::core::markdown_to_html("<![CDATA[\n"
                                         "function matchwo(a,b)\n"
                                         "{\n"
                                         "  if (a < b && a < 0) then {\n"
                                         "    return 1;\n"
                                         "\n"
                                         "  } else {\n"
                                         "\n"
                                         "    return 0;\n"
                                         "  }\n"
                                         "}\n"
                                         "]]>\n"
                                         "okay\n",
                                         false)};
  EXPECT_EQ(result, "<![CDATA[\n"
                    "function matchwo(a,b)\n"
                    "{\n"
                    "  if (a < b && a < 0) then {\n"
                    "    return 1;\n"
                    "\n"
                    "  } else {\n"
                    "\n"
                    "    return 0;\n"
                    "  }\n"
                    "}\n"
                    "]]>\n"
                    "<p>okay</p>\n");
}

TEST(example_152) {
  const auto result{sourcemeta::core::markdown_to_html("  <!-- foo -->\n"
                                                       "\n"
                                                       "    <!-- foo -->\n",
                                                       false)};
  EXPECT_EQ(result, "  <!-- foo -->\n"
                    "<pre><code>&lt;!-- foo --&gt;\n"
                    "</code></pre>\n");
}

TEST(example_153) {
  const auto result{sourcemeta::core::markdown_to_html("  <div>\n"
                                                       "\n"
                                                       "    <div>\n",
                                                       false)};
  EXPECT_EQ(result, "  <div>\n"
                    "<pre><code>&lt;div&gt;\n"
                    "</code></pre>\n");
}

TEST(example_154) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "<div>\n"
                                                       "bar\n"
                                                       "</div>\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo</p>\n"
                    "<div>\n"
                    "bar\n"
                    "</div>\n");
}

TEST(example_155) {
  const auto result{sourcemeta::core::markdown_to_html("<div>\n"
                                                       "bar\n"
                                                       "</div>\n"
                                                       "*foo*\n",
                                                       false)};
  EXPECT_EQ(result, "<div>\n"
                    "bar\n"
                    "</div>\n"
                    "*foo*\n");
}

TEST(example_156) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "<a href=\"bar\">\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "<a href=\"bar\">\n"
                    "baz</p>\n");
}

TEST(example_157) {
  const auto result{sourcemeta::core::markdown_to_html("<div>\n"
                                                       "\n"
                                                       "*Emphasized* text.\n"
                                                       "\n"
                                                       "</div>\n",
                                                       false)};
  EXPECT_EQ(result, "<div>\n"
                    "<p><em>Emphasized</em> text.</p>\n"
                    "</div>\n");
}

TEST(example_158) {
  const auto result{sourcemeta::core::markdown_to_html("<div>\n"
                                                       "*Emphasized* text.\n"
                                                       "</div>\n",
                                                       false)};
  EXPECT_EQ(result, "<div>\n"
                    "*Emphasized* text.\n"
                    "</div>\n");
}

TEST(example_159) {
  const auto result{sourcemeta::core::markdown_to_html("<table>\n"
                                                       "\n"
                                                       "<tr>\n"
                                                       "\n"
                                                       "<td>\n"
                                                       "Hi\n"
                                                       "</td>\n"
                                                       "\n"
                                                       "</tr>\n"
                                                       "\n"
                                                       "</table>\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<tr>\n"
                    "<td>\n"
                    "Hi\n"
                    "</td>\n"
                    "</tr>\n"
                    "</table>\n");
}

TEST(example_160) {
  const auto result{sourcemeta::core::markdown_to_html("<table>\n"
                                                       "\n"
                                                       "  <tr>\n"
                                                       "\n"
                                                       "    <td>\n"
                                                       "      Hi\n"
                                                       "    </td>\n"
                                                       "\n"
                                                       "  </tr>\n"
                                                       "\n"
                                                       "</table>\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "  <tr>\n"
                    "<pre><code>&lt;td&gt;\n"
                    "  Hi\n"
                    "&lt;/td&gt;\n"
                    "</code></pre>\n"
                    "  </tr>\n"
                    "</table>\n");
}

TEST(example_161) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url \"title\"\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_162) {
  const auto result{
      sourcemeta::core::markdown_to_html("   [foo]: \n"
                                         "      /url  \n"
                                         "           'the title'  \n"
                                         "\n"
                                         "[foo]\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"the title\">foo</a></p>\n");
}

TEST(example_163) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[Foo*bar\\]]:my_(url) 'title (with parens)'\n"
      "\n"
      "[Foo*bar\\]]\n",
      false)};
  EXPECT_EQ(result, "<p><a href=\"my_(url)\" title=\"title (with "
                    "parens)\">Foo*bar]</a></p>\n");
}

TEST(example_164) {
  const auto result{sourcemeta::core::markdown_to_html("[Foo bar]:\n"
                                                       "<my url>\n"
                                                       "'title'\n"
                                                       "\n"
                                                       "[Foo bar]\n",
                                                       false)};
  EXPECT_EQ(result,
            "<p><a href=\"my%20url\" title=\"title\">Foo bar</a></p>\n");
}

TEST(example_165) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url '\n"
                                                       "title\n"
                                                       "line1\n"
                                                       "line2\n"
                                                       "'\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"\n"
                    "title\n"
                    "line1\n"
                    "line2\n"
                    "\">foo</a></p>\n");
}

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_166) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url 'title\n"
                                                       "\n"
                                                       "with blank line'\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo]: /url &#39;title</p>\n"
                    "<p>with blank line&#39;</p>\n"
                    "<p>[foo]</p>\n");
}

TEST(example_167) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]:\n"
                                                       "/url\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">foo</a></p>\n");
}

TEST(example_168) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]:\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo]:</p>\n"
                    "<p>[foo]</p>\n");
}

TEST(example_169) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: <>\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"\">foo</a></p>\n");
}

TEST(example_170) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: <bar>(baz)\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo]: <bar>(baz)</p>\n"
                    "<p>[foo]</p>\n");
}

TEST(example_171) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo]: /url\\bar\\*baz \"foo\\\"bar\\baz\"\n"
      "\n"
      "[foo]\n",
      false)};
  EXPECT_EQ(result, "<p><a href=\"/url%5Cbar*baz\" "
                    "title=\"foo&quot;bar\\baz\">foo</a></p>\n");
}

TEST(example_172) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]\n"
                                                       "\n"
                                                       "[foo]: url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"url\">foo</a></p>\n");
}

TEST(example_173) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]\n"
                                                       "\n"
                                                       "[foo]: first\n"
                                                       "[foo]: second\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"first\">foo</a></p>\n");
}

TEST(example_174) {
  const auto result{sourcemeta::core::markdown_to_html("[FOO]: /url\n"
                                                       "\n"
                                                       "[Foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">Foo</a></p>\n");
}

TEST(example_175) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[\xCE\x91\xCE\x93\xCE\xA9]: /\xCF\x86\xCE\xBF\xCF\x85\n"
      "\n"
      "[\xCE\xB1\xCE\xB3\xCF\x89]\n",
      false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"/%CF%86%CE%BF%CF%85\">\xCE\xB1\xCE\xB3\xCF\x89</a></p>\n");
}

TEST(example_176) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\n", false)};
  EXPECT_EQ(result, "");
}

TEST(example_177) {
  const auto result{sourcemeta::core::markdown_to_html("[\n"
                                                       "foo\n"
                                                       "]: /url\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>bar</p>\n");
}

TEST(example_178) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]: /url \"title\" ok\n", false)};
  EXPECT_EQ(result, "<p>[foo]: /url &quot;title&quot; ok</p>\n");
}

TEST(example_179) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\n"
                                                       "\"title\" ok\n",
                                                       false)};
  EXPECT_EQ(result, "<p>&quot;title&quot; ok</p>\n");
}

TEST(example_180) {
  const auto result{
      sourcemeta::core::markdown_to_html("    [foo]: /url \"title\"\n"
                                         "\n"
                                         "[foo]\n",
                                         false)};
  EXPECT_EQ(result, "<pre><code>[foo]: /url &quot;title&quot;\n"
                    "</code></pre>\n"
                    "<p>[foo]</p>\n");
}

TEST(example_181) {
  const auto result{sourcemeta::core::markdown_to_html("```\n"
                                                       "[foo]: /url\n"
                                                       "```\n"
                                                       "\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>[foo]: /url\n"
                    "</code></pre>\n"
                    "<p>[foo]</p>\n");
}

TEST(example_182) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "[bar]: /baz\n"
                                                       "\n"
                                                       "[bar]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo\n"
                    "[bar]: /baz</p>\n"
                    "<p>[bar]</p>\n");
}

TEST(example_183) {
  const auto result{sourcemeta::core::markdown_to_html("# [Foo]\n"
                                                       "[foo]: /url\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<h1><a href=\"/url\">Foo</a></h1>\n"
                    "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_184) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\n"
                                                       "bar\n"
                                                       "===\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<h1>bar</h1>\n"
                    "<p><a href=\"/url\">foo</a></p>\n");
}

TEST(example_185) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\n"
                                                       "===\n"
                                                       "[foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>===\n"
                    "<a href=\"/url\">foo</a></p>\n");
}

TEST(example_186) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]: /foo-url \"foo\"\n"
                                         "[bar]: /bar-url\n"
                                         "  \"bar\"\n"
                                         "[baz]: /baz-url\n"
                                         "\n"
                                         "[foo],\n"
                                         "[bar],\n"
                                         "[baz]\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/foo-url\" title=\"foo\">foo</a>,\n"
                    "<a href=\"/bar-url\" title=\"bar\">bar</a>,\n"
                    "<a href=\"/baz-url\">baz</a></p>\n");
}

TEST(example_187) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]\n"
                                                       "\n"
                                                       "> [foo]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">foo</a></p>\n"
                    "<blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_188) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url\n", false)};
  EXPECT_EQ(result, "");
}

TEST(example_189) {
  const auto result{sourcemeta::core::markdown_to_html("aaa\n"
                                                       "\n"
                                                       "bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa</p>\n"
                    "<p>bbb</p>\n");
}

TEST(example_190) {
  const auto result{sourcemeta::core::markdown_to_html("aaa\n"
                                                       "bbb\n"
                                                       "\n"
                                                       "ccc\n"
                                                       "ddd\n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa\n"
                    "bbb</p>\n"
                    "<p>ccc\n"
                    "ddd</p>\n");
}

TEST(example_191) {
  const auto result{sourcemeta::core::markdown_to_html("aaa\n"
                                                       "\n"
                                                       "\n"
                                                       "bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa</p>\n"
                    "<p>bbb</p>\n");
}

TEST(example_192) {
  const auto result{sourcemeta::core::markdown_to_html("  aaa\n"
                                                       " bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa\n"
                    "bbb</p>\n");
}

TEST(example_193) {
  const auto result{sourcemeta::core::markdown_to_html(
      "aaa\n"
      "             bbb\n"
      "                                       ccc\n",
      false)};
  EXPECT_EQ(result, "<p>aaa\n"
                    "bbb\n"
                    "ccc</p>\n");
}

TEST(example_194) {
  const auto result{sourcemeta::core::markdown_to_html("   aaa\n"
                                                       "bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa\n"
                    "bbb</p>\n");
}

TEST(example_195) {
  const auto result{sourcemeta::core::markdown_to_html("    aaa\n"
                                                       "bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>aaa\n"
                    "</code></pre>\n"
                    "<p>bbb</p>\n");
}

TEST(example_196) {
  const auto result{sourcemeta::core::markdown_to_html("aaa     \n"
                                                       "bbb     \n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa<br />\n"
                    "bbb</p>\n");
}

TEST(example_197) {
  const auto result{sourcemeta::core::markdown_to_html("  \n"
                                                       "\n"
                                                       "aaa\n"
                                                       "  \n"
                                                       "\n"
                                                       "# aaa\n"
                                                       "\n"
                                                       "  \n",
                                                       false)};
  EXPECT_EQ(result, "<p>aaa</p>\n"
                    "<h1>aaa</h1>\n");
}

TEST(example_198) {
  const auto result{sourcemeta::core::markdown_to_html("| foo | bar |\n"
                                                       "| --- | --- |\n"
                                                       "| baz | bim |\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>foo</th>\n"
                    "<th>bar</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>baz</td>\n"
                    "<td>bim</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(example_199) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | defghi |\n"
                                                       ":-: | -----------:\n"
                                                       "bar | baz\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th align=\"center\">abc</th>\n"
                    "<th align=\"right\">defghi</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td align=\"center\">bar</td>\n"
                    "<td align=\"right\">baz</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(example_200) {
  const auto result{sourcemeta::core::markdown_to_html("| f\\|oo  |\n"
                                                       "| ------ |\n"
                                                       "| b `\\|` az |\n"
                                                       "| b **\\|** im |\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>f|oo</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>b <code>|</code> az</td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "<td>b <strong>|</strong> im</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(example_201) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | def |\n"
                                                       "| --- | --- |\n"
                                                       "| bar | baz |\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>abc</th>\n"
                    "<th>def</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>bar</td>\n"
                    "<td>baz</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n"
                    "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_202) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | def |\n"
                                                       "| --- | --- |\n"
                                                       "| bar | baz |\n"
                                                       "bar\n"
                                                       "\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>abc</th>\n"
                    "<th>def</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>bar</td>\n"
                    "<td>baz</td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "<td>bar</td>\n"
                    "<td></td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n"
                    "<p>bar</p>\n");
}

TEST(example_203) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | def |\n"
                                                       "| --- |\n"
                                                       "| bar |\n",
                                                       false)};
  EXPECT_EQ(result, "<p>| abc | def |\n"
                    "| --- |\n"
                    "| bar |</p>\n");
}

TEST(example_204) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | def |\n"
                                                       "| --- | --- |\n"
                                                       "| bar |\n"
                                                       "| bar | baz | boo |\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>abc</th>\n"
                    "<th>def</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "<tbody>\n"
                    "<tr>\n"
                    "<td>bar</td>\n"
                    "<td></td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "<td>bar</td>\n"
                    "<td>baz</td>\n"
                    "</tr>\n"
                    "</tbody>\n"
                    "</table>\n");
}

TEST(example_205) {
  const auto result{sourcemeta::core::markdown_to_html("| abc | def |\n"
                                                       "| --- | --- |\n",
                                                       false)};
  EXPECT_EQ(result, "<table>\n"
                    "<thead>\n"
                    "<tr>\n"
                    "<th>abc</th>\n"
                    "<th>def</th>\n"
                    "</tr>\n"
                    "</thead>\n"
                    "</table>\n");
}

TEST(example_206) {
  const auto result{sourcemeta::core::markdown_to_html("> # Foo\n"
                                                       "> bar\n"
                                                       "> baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<h1>Foo</h1>\n"
                    "<p>bar\n"
                    "baz</p>\n"
                    "</blockquote>\n");
}

TEST(example_207) {
  const auto result{sourcemeta::core::markdown_to_html("># Foo\n"
                                                       ">bar\n"
                                                       "> baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<h1>Foo</h1>\n"
                    "<p>bar\n"
                    "baz</p>\n"
                    "</blockquote>\n");
}

TEST(example_208) {
  const auto result{sourcemeta::core::markdown_to_html("   > # Foo\n"
                                                       "   > bar\n"
                                                       " > baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<h1>Foo</h1>\n"
                    "<p>bar\n"
                    "baz</p>\n"
                    "</blockquote>\n");
}

TEST(example_209) {
  const auto result{sourcemeta::core::markdown_to_html("    > # Foo\n"
                                                       "    > bar\n"
                                                       "    > baz\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>&gt; # Foo\n"
                    "&gt; bar\n"
                    "&gt; baz\n"
                    "</code></pre>\n");
}

TEST(example_210) {
  const auto result{sourcemeta::core::markdown_to_html("> # Foo\n"
                                                       "> bar\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<h1>Foo</h1>\n"
                    "<p>bar\n"
                    "baz</p>\n"
                    "</blockquote>\n");
}

TEST(example_211) {
  const auto result{sourcemeta::core::markdown_to_html("> bar\n"
                                                       "baz\n"
                                                       "> foo\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>bar\n"
                    "baz\n"
                    "foo</p>\n"
                    "</blockquote>\n");
}

TEST(example_212) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "---\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo</p>\n"
                    "</blockquote>\n"
                    "<hr />\n");
}

TEST(example_213) {
  const auto result{sourcemeta::core::markdown_to_html("> - foo\n"
                                                       "- bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n"
                    "</blockquote>\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n");
}

TEST(example_214) {
  const auto result{sourcemeta::core::markdown_to_html(">     foo\n"
                                                       "    bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>foo\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n");
}

TEST(example_215) {
  const auto result{sourcemeta::core::markdown_to_html("> ```\n"
                                                       "foo\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code></code></pre>\n"
                    "</blockquote>\n"
                    "<p>foo</p>\n"
                    "<pre><code></code></pre>\n");
}

TEST(example_216) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "    - bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo\n"
                    "- bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_217) {
  const auto result{sourcemeta::core::markdown_to_html(">\n", false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_218) {
  const auto result{sourcemeta::core::markdown_to_html(">\n"
                                                       ">  \n"
                                                       "> \n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_219) {
  const auto result{sourcemeta::core::markdown_to_html(">\n"
                                                       "> foo\n"
                                                       ">  \n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo</p>\n"
                    "</blockquote>\n");
}

TEST(example_220) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo</p>\n"
                    "</blockquote>\n"
                    "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_221) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo\n"
                    "bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_222) {
  const auto result{sourcemeta::core::markdown_to_html("> foo\n"
                                                       ">\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>foo</p>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_223) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "> bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo</p>\n"
                    "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n");
}

TEST(example_224) {
  const auto result{sourcemeta::core::markdown_to_html("> aaa\n"
                                                       "***\n"
                                                       "> bbb\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>aaa</p>\n"
                    "</blockquote>\n"
                    "<hr />\n"
                    "<blockquote>\n"
                    "<p>bbb</p>\n"
                    "</blockquote>\n");
}

TEST(example_225) {
  const auto result{sourcemeta::core::markdown_to_html("> bar\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>bar\n"
                    "baz</p>\n"
                    "</blockquote>\n");
}

TEST(example_226) {
  const auto result{sourcemeta::core::markdown_to_html("> bar\n"
                                                       "\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n"
                    "<p>baz</p>\n");
}

TEST(example_227) {
  const auto result{sourcemeta::core::markdown_to_html("> bar\n"
                                                       ">\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<p>bar</p>\n"
                    "</blockquote>\n"
                    "<p>baz</p>\n");
}

TEST(example_228) {
  const auto result{sourcemeta::core::markdown_to_html("> > > foo\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<blockquote>\n"
                    "<p>foo\n"
                    "bar</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_229) {
  const auto result{sourcemeta::core::markdown_to_html(">>> foo\n"
                                                       "> bar\n"
                                                       ">>baz\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<blockquote>\n"
                    "<p>foo\n"
                    "bar\n"
                    "baz</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_230) {
  const auto result{sourcemeta::core::markdown_to_html(">     code\n"
                                                       "\n"
                                                       ">    not code\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<pre><code>code\n"
                    "</code></pre>\n"
                    "</blockquote>\n"
                    "<blockquote>\n"
                    "<p>not code</p>\n"
                    "</blockquote>\n");
}

TEST(example_231) {
  const auto result{sourcemeta::core::markdown_to_html("A paragraph\n"
                                                       "with two lines.\n"
                                                       "\n"
                                                       "    indented code\n"
                                                       "\n"
                                                       "> A block quote.\n",
                                                       false)};
  EXPECT_EQ(result, "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n");
}

TEST(example_232) {
  const auto result{sourcemeta::core::markdown_to_html("1.  A paragraph\n"
                                                       "    with two lines.\n"
                                                       "\n"
                                                       "        indented code\n"
                                                       "\n"
                                                       "    > A block quote.\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_233) {
  const auto result{sourcemeta::core::markdown_to_html("- one\n"
                                                       "\n"
                                                       " two\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n");
}

TEST(example_234) {
  const auto result{sourcemeta::core::markdown_to_html("- one\n"
                                                       "\n"
                                                       "  two\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_235) {
  const auto result{sourcemeta::core::markdown_to_html(" -    one\n"
                                                       "\n"
                                                       "     two\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<pre><code> two\n"
                    "</code></pre>\n");
}

TEST(example_236) {
  const auto result{sourcemeta::core::markdown_to_html(" -    one\n"
                                                       "\n"
                                                       "      two\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_237) {
  const auto result{sourcemeta::core::markdown_to_html("   > > 1.  one\n"
                                                       ">>\n"
                                                       ">>     two\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ol>\n"
                    "<li>\n"
                    "<p>one</p>\n"
                    "<p>two</p>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_238) {
  const auto result{sourcemeta::core::markdown_to_html(">>- one\n"
                                                       ">>\n"
                                                       "  >  > two\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<blockquote>\n"
                    "<ul>\n"
                    "<li>one</li>\n"
                    "</ul>\n"
                    "<p>two</p>\n"
                    "</blockquote>\n"
                    "</blockquote>\n");
}

TEST(example_239) {
  const auto result{sourcemeta::core::markdown_to_html("-one\n"
                                                       "\n"
                                                       "2.two\n",
                                                       false)};
  EXPECT_EQ(result, "<p>-one</p>\n"
                    "<p>2.two</p>\n");
}

TEST(example_240) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "\n"
                                                       "\n"
                                                       "  bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_241) {
  const auto result{sourcemeta::core::markdown_to_html("1.  foo\n"
                                                       "\n"
                                                       "    ```\n"
                                                       "    bar\n"
                                                       "    ```\n"
                                                       "\n"
                                                       "    baz\n"
                                                       "\n"
                                                       "    > bam\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "<p>baz</p>\n"
                    "<blockquote>\n"
                    "<p>bam</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_242) {
  const auto result{sourcemeta::core::markdown_to_html("- Foo\n"
                                                       "\n"
                                                       "      bar\n"
                                                       "\n"
                                                       "\n"
                                                       "      baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>Foo</p>\n"
                    "<pre><code>bar\n"
                    "\n"
                    "\n"
                    "baz\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_243) {
  const auto result{
      sourcemeta::core::markdown_to_html("123456789. ok\n", false)};
  EXPECT_EQ(result, "<ol start=\"123456789\">\n"
                    "<li>ok</li>\n"
                    "</ol>\n");
}

TEST(example_244) {
  const auto result{
      sourcemeta::core::markdown_to_html("1234567890. not ok\n", false)};
  EXPECT_EQ(result, "<p>1234567890. not ok</p>\n");
}

TEST(example_245) {
  const auto result{sourcemeta::core::markdown_to_html("0. ok\n", false)};
  EXPECT_EQ(result, "<ol start=\"0\">\n"
                    "<li>ok</li>\n"
                    "</ol>\n");
}

TEST(example_246) {
  const auto result{sourcemeta::core::markdown_to_html("003. ok\n", false)};
  EXPECT_EQ(result, "<ol start=\"3\">\n"
                    "<li>ok</li>\n"
                    "</ol>\n");
}

TEST(example_247) {
  const auto result{sourcemeta::core::markdown_to_html("-1. not ok\n", false)};
  EXPECT_EQ(result, "<p>-1. not ok</p>\n");
}

TEST(example_248) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "\n"
                                                       "      bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_249) {
  const auto result{sourcemeta::core::markdown_to_html("  10.  foo\n"
                                                       "\n"
                                                       "           bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol start=\"10\">\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_250) {
  const auto result{sourcemeta::core::markdown_to_html("    indented code\n"
                                                       "\n"
                                                       "paragraph\n"
                                                       "\n"
                                                       "    more code\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<p>paragraph</p>\n"
                    "<pre><code>more code\n"
                    "</code></pre>\n");
}

TEST(example_251) {
  const auto result{sourcemeta::core::markdown_to_html("1.     indented code\n"
                                                       "\n"
                                                       "   paragraph\n"
                                                       "\n"
                                                       "       more code\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<p>paragraph</p>\n"
                    "<pre><code>more code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_252) {
  const auto result{sourcemeta::core::markdown_to_html("1.      indented code\n"
                                                       "\n"
                                                       "   paragraph\n"
                                                       "\n"
                                                       "       more code\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<pre><code> indented code\n"
                    "</code></pre>\n"
                    "<p>paragraph</p>\n"
                    "<pre><code>more code\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_253) {
  const auto result{sourcemeta::core::markdown_to_html("   foo\n"
                                                       "\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo</p>\n"
                    "<p>bar</p>\n");
}

TEST(example_254) {
  const auto result{sourcemeta::core::markdown_to_html("-    foo\n"
                                                       "\n"
                                                       "  bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n"
                    "<p>bar</p>\n");
}

TEST(example_255) {
  const auto result{sourcemeta::core::markdown_to_html("-  foo\n"
                                                       "\n"
                                                       "   bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_256) {
  const auto result{sourcemeta::core::markdown_to_html("-\n"
                                                       "  foo\n"
                                                       "-\n"
                                                       "  ```\n"
                                                       "  bar\n"
                                                       "  ```\n"
                                                       "-\n"
                                                       "      baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li>\n"
                    "<pre><code>bar\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>\n"
                    "<pre><code>baz\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_257) {
  const auto result{sourcemeta::core::markdown_to_html("-   \n"
                                                       "  foo\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n");
}

TEST(example_258) {
  const auto result{sourcemeta::core::markdown_to_html("-\n"
                                                       "\n"
                                                       "  foo\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n"
                    "<p>foo</p>\n");
}

TEST(example_259) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "-\n"
                                                       "- bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li></li>\n"
                    "<li>bar</li>\n"
                    "</ul>\n");
}

TEST(example_260) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "-   \n"
                                                       "- bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li></li>\n"
                    "<li>bar</li>\n"
                    "</ul>\n");
}

TEST(example_261) {
  const auto result{sourcemeta::core::markdown_to_html("1. foo\n"
                                                       "2.\n"
                                                       "3. bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>foo</li>\n"
                    "<li></li>\n"
                    "<li>bar</li>\n"
                    "</ol>\n");
}

TEST(example_262) {
  const auto result{sourcemeta::core::markdown_to_html("*\n", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li></li>\n"
                    "</ul>\n");
}

TEST(example_263) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "*\n"
                                                       "\n"
                                                       "foo\n"
                                                       "1.\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo\n"
                    "*</p>\n"
                    "<p>foo\n"
                    "1.</p>\n");
}

TEST(example_264) {
  const auto result{
      sourcemeta::core::markdown_to_html(" 1.  A paragraph\n"
                                         "     with two lines.\n"
                                         "\n"
                                         "         indented code\n"
                                         "\n"
                                         "     > A block quote.\n",
                                         false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_265) {
  const auto result{
      sourcemeta::core::markdown_to_html("  1.  A paragraph\n"
                                         "      with two lines.\n"
                                         "\n"
                                         "          indented code\n"
                                         "\n"
                                         "      > A block quote.\n",
                                         false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_266) {
  const auto result{
      sourcemeta::core::markdown_to_html("   1.  A paragraph\n"
                                         "       with two lines.\n"
                                         "\n"
                                         "           indented code\n"
                                         "\n"
                                         "       > A block quote.\n",
                                         false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_267) {
  const auto result{
      sourcemeta::core::markdown_to_html("    1.  A paragraph\n"
                                         "        with two lines.\n"
                                         "\n"
                                         "            indented code\n"
                                         "\n"
                                         "        > A block quote.\n",
                                         false)};
  EXPECT_EQ(result, "<pre><code>1.  A paragraph\n"
                    "    with two lines.\n"
                    "\n"
                    "        indented code\n"
                    "\n"
                    "    &gt; A block quote.\n"
                    "</code></pre>\n");
}

TEST(example_268) {
  const auto result{
      sourcemeta::core::markdown_to_html("  1.  A paragraph\n"
                                         "with two lines.\n"
                                         "\n"
                                         "          indented code\n"
                                         "\n"
                                         "      > A block quote.\n",
                                         false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>A paragraph\n"
                    "with two lines.</p>\n"
                    "<pre><code>indented code\n"
                    "</code></pre>\n"
                    "<blockquote>\n"
                    "<p>A block quote.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_269) {
  const auto result{sourcemeta::core::markdown_to_html("  1.  A paragraph\n"
                                                       "    with two lines.\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>A paragraph\n"
                    "with two lines.</li>\n"
                    "</ol>\n");
}

TEST(example_270) {
  const auto result{sourcemeta::core::markdown_to_html("> 1. > Blockquote\n"
                                                       "continued here.\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ol>\n"
                    "<li>\n"
                    "<blockquote>\n"
                    "<p>Blockquote\n"
                    "continued here.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</blockquote>\n");
}

TEST(example_271) {
  const auto result{sourcemeta::core::markdown_to_html("> 1. > Blockquote\n"
                                                       "> continued here.\n",
                                                       false)};
  EXPECT_EQ(result, "<blockquote>\n"
                    "<ol>\n"
                    "<li>\n"
                    "<blockquote>\n"
                    "<p>Blockquote\n"
                    "continued here.</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "</ol>\n"
                    "</blockquote>\n");
}

TEST(example_272) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "  - bar\n"
                                                       "    - baz\n"
                                                       "      - boo\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo\n"
                    "<ul>\n"
                    "<li>bar\n"
                    "<ul>\n"
                    "<li>baz\n"
                    "<ul>\n"
                    "<li>boo</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_273) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       " - bar\n"
                                                       "  - baz\n"
                                                       "   - boo\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li>bar</li>\n"
                    "<li>baz</li>\n"
                    "<li>boo</li>\n"
                    "</ul>\n");
}

TEST(example_274) {
  const auto result{sourcemeta::core::markdown_to_html("10) foo\n"
                                                       "    - bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol start=\"10\">\n"
                    "<li>foo\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_275) {
  const auto result{sourcemeta::core::markdown_to_html("10) foo\n"
                                                       "   - bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol start=\"10\">\n"
                    "<li>foo</li>\n"
                    "</ol>\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n");
}

TEST(example_276) {
  const auto result{sourcemeta::core::markdown_to_html("- - foo\n", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_277) {
  const auto result{sourcemeta::core::markdown_to_html("1. - 2. foo\n", false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<ul>\n"
                    "<li>\n"
                    "<ol start=\"2\">\n"
                    "<li>foo</li>\n"
                    "</ol>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_278) {
  const auto result{sourcemeta::core::markdown_to_html("- # Foo\n"
                                                       "- Bar\n"
                                                       "  ---\n"
                                                       "  baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<h1>Foo</h1>\n"
                    "</li>\n"
                    "<li>\n"
                    "<h2>Bar</h2>\n"
                    "baz</li>\n"
                    "</ul>\n");
}

// The specification example renders the same checkbox with its attributes in
// another order and without the slash of a void element
TEST(example_279) {
  const auto result{sourcemeta::core::markdown_to_html("- [ ] foo\n"
                                                       "- [x] bar\n",
                                                       false)};
  EXPECT_EQ(
      result,
      "<ul>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> foo</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> bar</li>\n"
      "</ul>\n");
}

// The specification example renders the same checkbox with its attributes in
// another order and without the slash of a void element
TEST(example_280) {
  const auto result{sourcemeta::core::markdown_to_html("- [x] foo\n"
                                                       "  - [ ] bar\n"
                                                       "  - [x] baz\n"
                                                       "- [ ] bim\n",
                                                       false)};
  EXPECT_EQ(
      result,
      "<ul>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> foo\n"
      "<ul>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> bar</li>\n"
      "<li><input type=\"checkbox\" checked=\"\" disabled=\"\" /> baz</li>\n"
      "</ul>\n"
      "</li>\n"
      "<li><input type=\"checkbox\" disabled=\"\" /> bim</li>\n"
      "</ul>\n");
}

TEST(example_281) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "- bar\n"
                                                       "+ baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li>bar</li>\n"
                    "</ul>\n"
                    "<ul>\n"
                    "<li>baz</li>\n"
                    "</ul>\n");
}

TEST(example_282) {
  const auto result{sourcemeta::core::markdown_to_html("1. foo\n"
                                                       "2. bar\n"
                                                       "3) baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>foo</li>\n"
                    "<li>bar</li>\n"
                    "</ol>\n"
                    "<ol start=\"3\">\n"
                    "<li>baz</li>\n"
                    "</ol>\n");
}

TEST(example_283) {
  const auto result{sourcemeta::core::markdown_to_html("Foo\n"
                                                       "- bar\n"
                                                       "- baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>Foo</p>\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "<li>baz</li>\n"
                    "</ul>\n");
}

TEST(example_284) {
  const auto result{sourcemeta::core::markdown_to_html(
      "The number of windows in my house is\n"
      "14.  The number of doors is 6.\n",
      false)};
  EXPECT_EQ(result, "<p>The number of windows in my house is\n"
                    "14.  The number of doors is 6.</p>\n");
}

TEST(example_285) {
  const auto result{sourcemeta::core::markdown_to_html(
      "The number of windows in my house is\n"
      "1.  The number of doors is 6.\n",
      false)};
  EXPECT_EQ(result, "<p>The number of windows in my house is</p>\n"
                    "<ol>\n"
                    "<li>The number of doors is 6.</li>\n"
                    "</ol>\n");
}

TEST(example_286) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "\n"
                                                       "- bar\n"
                                                       "\n"
                                                       "\n"
                                                       "- baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>baz</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_287) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "  - bar\n"
                                                       "    - baz\n"
                                                       "\n"
                                                       "\n"
                                                       "      bim\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo\n"
                    "<ul>\n"
                    "<li>bar\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>baz</p>\n"
                    "<p>bim</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_288) {
  const auto result{sourcemeta::core::markdown_to_html("- foo\n"
                                                       "- bar\n"
                                                       "\n"
                                                       "<!-- -->\n"
                                                       "\n"
                                                       "- baz\n"
                                                       "- bim\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>foo</li>\n"
                    "<li>bar</li>\n"
                    "</ul>\n"
                    "<!-- -->\n"
                    "<ul>\n"
                    "<li>baz</li>\n"
                    "<li>bim</li>\n"
                    "</ul>\n");
}

TEST(example_289) {
  const auto result{sourcemeta::core::markdown_to_html("-   foo\n"
                                                       "\n"
                                                       "    notcode\n"
                                                       "\n"
                                                       "-   foo\n"
                                                       "\n"
                                                       "<!-- -->\n"
                                                       "\n"
                                                       "    code\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<p>notcode</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "<!-- -->\n"
                    "<pre><code>code\n"
                    "</code></pre>\n");
}

TEST(example_290) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       " - b\n"
                                                       "  - c\n"
                                                       "   - d\n"
                                                       "  - e\n"
                                                       " - f\n"
                                                       "- g\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "<li>d</li>\n"
                    "<li>e</li>\n"
                    "<li>f</li>\n"
                    "<li>g</li>\n"
                    "</ul>\n");
}

TEST(example_291) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n"
                                                       "\n"
                                                       "  2. b\n"
                                                       "\n"
                                                       "   3. c\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_292) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       " - b\n"
                                                       "  - c\n"
                                                       "   - d\n"
                                                       "    - e\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "<li>d\n"
                    "- e</li>\n"
                    "</ul>\n");
}

TEST(example_293) {
  const auto result{sourcemeta::core::markdown_to_html("1. a\n"
                                                       "\n"
                                                       "  2. b\n"
                                                       "\n"
                                                       "    3. c\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "</ol>\n"
                    "<pre><code>3. c\n"
                    "</code></pre>\n");
}

TEST(example_294) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "- b\n"
                                                       "\n"
                                                       "- c\n",
                                                       false)};
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

TEST(example_295) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n"
                                                       "*\n"
                                                       "\n"
                                                       "* c\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li></li>\n"
                    "<li>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_296) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "- b\n"
                                                       "\n"
                                                       "  c\n"
                                                       "- d\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>d</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_297) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "- b\n"
                                                       "\n"
                                                       "  [ref]: /url\n"
                                                       "- d\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>d</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_298) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "- ```\n"
                                                       "  b\n"
                                                       "\n"
                                                       "\n"
                                                       "  ```\n"
                                                       "- c\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "<li>\n"
                    "<pre><code>b\n"
                    "\n"
                    "\n"
                    "</code></pre>\n"
                    "</li>\n"
                    "<li>c</li>\n"
                    "</ul>\n");
}

TEST(example_299) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "  - b\n"
                                                       "\n"
                                                       "    c\n"
                                                       "- d\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>\n"
                    "<p>b</p>\n"
                    "<p>c</p>\n"
                    "</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "<li>d</li>\n"
                    "</ul>\n");
}

TEST(example_300) {
  const auto result{sourcemeta::core::markdown_to_html("* a\n"
                                                       "  > b\n"
                                                       "  >\n"
                                                       "* c\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<blockquote>\n"
                    "<p>b</p>\n"
                    "</blockquote>\n"
                    "</li>\n"
                    "<li>c</li>\n"
                    "</ul>\n");
}

TEST(example_301) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "  > b\n"
                                                       "  ```\n"
                                                       "  c\n"
                                                       "  ```\n"
                                                       "- d\n",
                                                       false)};
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

TEST(example_302) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n", false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a</li>\n"
                    "</ul>\n");
}

TEST(example_303) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "  - b\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>a\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_304) {
  const auto result{sourcemeta::core::markdown_to_html("1. ```\n"
                                                       "   foo\n"
                                                       "   ```\n"
                                                       "\n"
                                                       "   bar\n",
                                                       false)};
  EXPECT_EQ(result, "<ol>\n"
                    "<li>\n"
                    "<pre><code>foo\n"
                    "</code></pre>\n"
                    "<p>bar</p>\n"
                    "</li>\n"
                    "</ol>\n");
}

TEST(example_305) {
  const auto result{sourcemeta::core::markdown_to_html("* foo\n"
                                                       "  * bar\n"
                                                       "\n"
                                                       "  baz\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>foo</p>\n"
                    "<ul>\n"
                    "<li>bar</li>\n"
                    "</ul>\n"
                    "<p>baz</p>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_306) {
  const auto result{sourcemeta::core::markdown_to_html("- a\n"
                                                       "  - b\n"
                                                       "  - c\n"
                                                       "\n"
                                                       "- d\n"
                                                       "  - e\n"
                                                       "  - f\n",
                                                       false)};
  EXPECT_EQ(result, "<ul>\n"
                    "<li>\n"
                    "<p>a</p>\n"
                    "<ul>\n"
                    "<li>b</li>\n"
                    "<li>c</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "<li>\n"
                    "<p>d</p>\n"
                    "<ul>\n"
                    "<li>e</li>\n"
                    "<li>f</li>\n"
                    "</ul>\n"
                    "</li>\n"
                    "</ul>\n");
}

TEST(example_307) {
  const auto result{sourcemeta::core::markdown_to_html("`hi`lo`\n", false)};
  EXPECT_EQ(result, "<p><code>hi</code>lo`</p>\n");
}
