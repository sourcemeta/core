#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_308) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\\!\\\"\\#\\$\\%\\&\\'\\(\\)\\*\\+\\,\\-\\.\\/"
      "\\:\\;\\<\\=\\>\\?\\@\\[\\\\\\]\\^\\_\\`\\{\\|\\}\\~\n",
      false)};
  EXPECT_EQ(result,
            "<p>!&quot;#$%&amp;&#39;()*+,-./:;&lt;=&gt;?@[\\]^_`{|}~</p>\n");
}

TEST(example_309) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\\\t\\A\\a\\ \\3\\\xCF\x86\\\xC2\xAB\n", false)};
  EXPECT_EQ(result, "<p>\\\t\\A\\a\\ \\3\\\xCF\x86\\\xC2\xAB</p>\n");
}

TEST(example_310) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\*not emphasized*\n"
                                         "\\<br/> not a tag\n"
                                         "\\[not a link](/foo)\n"
                                         "\\`not code`\n"
                                         "1\\. not a list\n"
                                         "\\* not a list\n"
                                         "\\# not a heading\n"
                                         "\\[foo]: /url \"not a reference\"\n"
                                         "\\&ouml; not a character entity\n",
                                         false)};
  EXPECT_EQ(result, "<p>*not emphasized*\n"
                    "&lt;br/&gt; not a tag\n"
                    "[not a link](/foo)\n"
                    "`not code`\n"
                    "1. not a list\n"
                    "* not a list\n"
                    "# not a heading\n"
                    "[foo]: /url &quot;not a reference&quot;\n"
                    "&amp;ouml; not a character entity</p>\n");
}

TEST(example_311) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\\\*emphasis*\n", false)};
  EXPECT_EQ(result, "<p>\\<em>emphasis</em></p>\n");
}

TEST(example_312) {
  const auto result{sourcemeta::core::markdown_to_html("foo\\\n"
                                                       "bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "bar</p>\n");
}

TEST(example_313) {
  const auto result{
      sourcemeta::core::markdown_to_html("`` \\[\\` ``\n", false)};
  EXPECT_EQ(result, "<p><code>\\[\\`</code></p>\n");
}

TEST(example_314) {
  const auto result{sourcemeta::core::markdown_to_html("    \\[\\]\n", false)};
  EXPECT_EQ(result, "<pre><code>\\[\\]\n"
                    "</code></pre>\n");
}

TEST(example_315) {
  const auto result{sourcemeta::core::markdown_to_html("~~~\n"
                                                       "\\[\\]\n"
                                                       "~~~\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code>\\[\\]\n"
                    "</code></pre>\n");
}

TEST(example_316) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<http://example.com?find=\\*>\n", false)};
  EXPECT_EQ(result, "<p><a "
                    "href=\"http://example.com?find=%5C*\">http://"
                    "example.com?find=\\*</a></p>\n");
}

TEST(example_317) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a href=\"/bar\\/)\">\n", false)};
  EXPECT_EQ(result, "<a href=\"/bar\\/)\">\n");
}

TEST(example_318) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo](/bar\\* \"ti\\*tle\")\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/bar*\" title=\"ti*tle\">foo</a></p>\n");
}

TEST(example_319) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]\n"
                                         "\n"
                                         "[foo]: /bar\\* \"ti\\*tle\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/bar*\" title=\"ti*tle\">foo</a></p>\n");
}

TEST(example_320) {
  const auto result{sourcemeta::core::markdown_to_html("``` foo\\+bar\n"
                                                       "foo\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code class=\"language-foo+bar\">foo\n"
                    "</code></pre>\n");
}

// The specification example renders the no-break space as is, which is the same
// HTML
TEST(example_321) {
  const auto result{sourcemeta::core::markdown_to_html(
      "&nbsp; &amp; &copy; &AElig; &Dcaron;\n"
      "&frac34; &HilbertSpace; &DifferentialD;\n"
      "&ClockwiseContourIntegral; &ngE;\n",
      false)};
  EXPECT_EQ(result, "<p>&nbsp; &amp; \xC2\xA9 \xC3\x86 \xC4\x8E\n"
                    "\xC2\xBE \xE2\x84\x8B \xE2\x85\x86\n"
                    "\xE2\x88\xB2 \xE2\x89\xA7\xCC\xB8</p>\n");
}

TEST(example_322) {
  const auto result{
      sourcemeta::core::markdown_to_html("&#35; &#1234; &#992; &#0;\n", false)};
  EXPECT_EQ(result, "<p># \xD3\x92 \xCF\xA0 \xEF\xBF\xBD</p>\n");
}

TEST(example_323) {
  const auto result{
      sourcemeta::core::markdown_to_html("&#X22; &#XD06; &#xcab;\n", false)};
  EXPECT_EQ(result, "<p>&quot; \xE0\xB4\x86 \xE0\xB2\xAB</p>\n");
}

TEST(example_324) {
  const auto result{
      sourcemeta::core::markdown_to_html("&nbsp &x; &#; &#x;\n"
                                         "&#87654321;\n"
                                         "&#abcdef0;\n"
                                         "&ThisIsNotDefined; &hi?;\n",
                                         false)};
  EXPECT_EQ(result, "<p>&amp;nbsp &amp;x; &amp;#; &amp;#x;\n"
                    "&amp;#87654321;\n"
                    "&amp;#abcdef0;\n"
                    "&amp;ThisIsNotDefined; &amp;hi?;</p>\n");
}

TEST(example_325) {
  const auto result{sourcemeta::core::markdown_to_html("&copy\n", false)};
  EXPECT_EQ(result, "<p>&amp;copy</p>\n");
}

TEST(example_326) {
  const auto result{
      sourcemeta::core::markdown_to_html("&MadeUpEntity;\n", false)};
  EXPECT_EQ(result, "<p>&amp;MadeUpEntity;</p>\n");
}

TEST(example_327) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<a href=\"&ouml;&ouml;.html\">\n", false)};
  EXPECT_EQ(result, "<a href=\"&ouml;&ouml;.html\">\n");
}

TEST(example_328) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo](/f&ouml;&ouml; \"f&ouml;&ouml;\")\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/f%C3%B6%C3%B6\" "
                    "title=\"f\xC3\xB6\xC3\xB6\">foo</a></p>\n");
}

TEST(example_329) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo]\n"
      "\n"
      "[foo]: /f&ouml;&ouml; \"f&ouml;&ouml;\"\n",
      false)};
  EXPECT_EQ(result, "<p><a href=\"/f%C3%B6%C3%B6\" "
                    "title=\"f\xC3\xB6\xC3\xB6\">foo</a></p>\n");
}

TEST(example_330) {
  const auto result{sourcemeta::core::markdown_to_html("``` f&ouml;&ouml;\n"
                                                       "foo\n"
                                                       "```\n",
                                                       false)};
  EXPECT_EQ(result, "<pre><code class=\"language-f\xC3\xB6\xC3\xB6\">foo\n"
                    "</code></pre>\n");
}

TEST(example_331) {
  const auto result{
      sourcemeta::core::markdown_to_html("`f&ouml;&ouml;`\n", false)};
  EXPECT_EQ(result, "<p><code>f&amp;ouml;&amp;ouml;</code></p>\n");
}

TEST(example_332) {
  const auto result{
      sourcemeta::core::markdown_to_html("    f&ouml;f&ouml;\n", false)};
  EXPECT_EQ(result, "<pre><code>f&amp;ouml;f&amp;ouml;\n"
                    "</code></pre>\n");
}

TEST(example_333) {
  const auto result{sourcemeta::core::markdown_to_html("&#42;foo&#42;\n"
                                                       "*foo*\n",
                                                       false)};
  EXPECT_EQ(result, "<p>*foo*\n"
                    "<em>foo</em></p>\n");
}

TEST(example_334) {
  const auto result{sourcemeta::core::markdown_to_html("&#42; foo\n"
                                                       "\n"
                                                       "* foo\n",
                                                       false)};
  EXPECT_EQ(result, "<p>* foo</p>\n"
                    "<ul>\n"
                    "<li>foo</li>\n"
                    "</ul>\n");
}

TEST(example_335) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo&#10;&#10;bar\n", false)};
  EXPECT_EQ(result, "<p>foo\n"
                    "\n"
                    "bar</p>\n");
}

TEST(example_336) {
  const auto result{sourcemeta::core::markdown_to_html("&#9;foo\n", false)};
  EXPECT_EQ(result, "<p>\tfoo</p>\n");
}

TEST(example_337) {
  const auto result{
      sourcemeta::core::markdown_to_html("[a](url &quot;tit&quot;)\n", false)};
  EXPECT_EQ(result, "<p>[a](url &quot;tit&quot;)</p>\n");
}

TEST(example_338) {
  const auto result{sourcemeta::core::markdown_to_html("`foo`\n", false)};
  EXPECT_EQ(result, "<p><code>foo</code></p>\n");
}

TEST(example_339) {
  const auto result{
      sourcemeta::core::markdown_to_html("`` foo ` bar ``\n", false)};
  EXPECT_EQ(result, "<p><code>foo ` bar</code></p>\n");
}

TEST(example_340) {
  const auto result{sourcemeta::core::markdown_to_html("` `` `\n", false)};
  EXPECT_EQ(result, "<p><code>``</code></p>\n");
}

TEST(example_341) {
  const auto result{sourcemeta::core::markdown_to_html("`  ``  `\n", false)};
  EXPECT_EQ(result, "<p><code> `` </code></p>\n");
}

TEST(example_342) {
  const auto result{sourcemeta::core::markdown_to_html("` a`\n", false)};
  EXPECT_EQ(result, "<p><code> a</code></p>\n");
}

// The specification example renders the no-break space as is, which is the same
// HTML
TEST(example_343) {
  const auto result{sourcemeta::core::markdown_to_html("`\xC2\xA0"
                                                       "b\xC2\xA0`\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>&nbsp;b&nbsp;</code></p>\n");
}

// The specification example renders the no-break space as is, which is the same
// HTML
TEST(example_344) {
  const auto result{sourcemeta::core::markdown_to_html("`\xC2\xA0`\n"
                                                       "`  `\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>&nbsp;</code>\n"
                    "<code>  </code></p>\n");
}

TEST(example_345) {
  const auto result{sourcemeta::core::markdown_to_html("``\n"
                                                       "foo\n"
                                                       "bar  \n"
                                                       "baz\n"
                                                       "``\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>foo bar   baz</code></p>\n");
}

TEST(example_346) {
  const auto result{sourcemeta::core::markdown_to_html("``\n"
                                                       "foo \n"
                                                       "``\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>foo </code></p>\n");
}

TEST(example_347) {
  const auto result{sourcemeta::core::markdown_to_html("`foo   bar \n"
                                                       "baz`\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>foo   bar  baz</code></p>\n");
}

TEST(example_348) {
  const auto result{sourcemeta::core::markdown_to_html("`foo\\`bar`\n", false)};
  EXPECT_EQ(result, "<p><code>foo\\</code>bar`</p>\n");
}

TEST(example_349) {
  const auto result{sourcemeta::core::markdown_to_html("``foo`bar``\n", false)};
  EXPECT_EQ(result, "<p><code>foo`bar</code></p>\n");
}

TEST(example_350) {
  const auto result{
      sourcemeta::core::markdown_to_html("` foo `` bar `\n", false)};
  EXPECT_EQ(result, "<p><code>foo `` bar</code></p>\n");
}

TEST(example_351) {
  const auto result{sourcemeta::core::markdown_to_html("*foo`*`\n", false)};
  EXPECT_EQ(result, "<p>*foo<code>*</code></p>\n");
}

TEST(example_352) {
  const auto result{
      sourcemeta::core::markdown_to_html("[not a `link](/foo`)\n", false)};
  EXPECT_EQ(result, "<p>[not a <code>link](/foo</code>)</p>\n");
}

TEST(example_353) {
  const auto result{
      sourcemeta::core::markdown_to_html("`<a href=\"`\">`\n", false)};
  EXPECT_EQ(result, "<p><code>&lt;a href=&quot;</code>&quot;&gt;`</p>\n");
}

TEST(example_354) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a href=\"`\">`\n", false)};
  EXPECT_EQ(result, "<p><a href=\"`\">`</p>\n");
}

TEST(example_355) {
  const auto result{
      sourcemeta::core::markdown_to_html("`<http://foo.bar.`baz>`\n", false)};
  EXPECT_EQ(result, "<p><code>&lt;http://foo.bar.</code>baz&gt;`</p>\n");
}

TEST(example_356) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://foo.bar.`baz>`\n", false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"http://foo.bar.%60baz\">http://foo.bar.`baz</a>`</p>\n");
}

TEST(example_357) {
  const auto result{sourcemeta::core::markdown_to_html("```foo``\n", false)};
  EXPECT_EQ(result, "<p>```foo``</p>\n");
}

TEST(example_358) {
  const auto result{sourcemeta::core::markdown_to_html("`foo\n", false)};
  EXPECT_EQ(result, "<p>`foo</p>\n");
}

TEST(example_359) {
  const auto result{sourcemeta::core::markdown_to_html("`foo``bar``\n", false)};
  EXPECT_EQ(result, "<p>`foo<code>bar</code></p>\n");
}

TEST(example_360) {
  const auto result{sourcemeta::core::markdown_to_html("*foo bar*\n", false)};
  EXPECT_EQ(result, "<p><em>foo bar</em></p>\n");
}

TEST(example_361) {
  const auto result{
      sourcemeta::core::markdown_to_html("a * foo bar*\n", false)};
  EXPECT_EQ(result, "<p>a * foo bar*</p>\n");
}

TEST(example_362) {
  const auto result{sourcemeta::core::markdown_to_html("a*\"foo\"*\n", false)};
  EXPECT_EQ(result, "<p>a*&quot;foo&quot;*</p>\n");
}

// The specification example renders the no-break space as is, which is the same
// HTML
TEST(example_363) {
  const auto result{sourcemeta::core::markdown_to_html("*\xC2\xA0"
                                                       "a\xC2\xA0*\n",
                                                       false)};
  EXPECT_EQ(result, "<p>*&nbsp;a&nbsp;*</p>\n");
}

TEST(example_364) {
  const auto result{sourcemeta::core::markdown_to_html("foo*bar*\n", false)};
  EXPECT_EQ(result, "<p>foo<em>bar</em></p>\n");
}

TEST(example_365) {
  const auto result{sourcemeta::core::markdown_to_html("5*6*78\n", false)};
  EXPECT_EQ(result, "<p>5<em>6</em>78</p>\n");
}

TEST(example_366) {
  const auto result{sourcemeta::core::markdown_to_html("_foo bar_\n", false)};
  EXPECT_EQ(result, "<p><em>foo bar</em></p>\n");
}

TEST(example_367) {
  const auto result{sourcemeta::core::markdown_to_html("_ foo bar_\n", false)};
  EXPECT_EQ(result, "<p>_ foo bar_</p>\n");
}

TEST(example_368) {
  const auto result{sourcemeta::core::markdown_to_html("a_\"foo\"_\n", false)};
  EXPECT_EQ(result, "<p>a_&quot;foo&quot;_</p>\n");
}

TEST(example_369) {
  const auto result{sourcemeta::core::markdown_to_html("foo_bar_\n", false)};
  EXPECT_EQ(result, "<p>foo_bar_</p>\n");
}

TEST(example_370) {
  const auto result{sourcemeta::core::markdown_to_html("5_6_78\n", false)};
  EXPECT_EQ(result, "<p>5_6_78</p>\n");
}

TEST(example_371) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0\xBC"
      "_\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1"
      "\x8F_\n",
      false)};
  EXPECT_EQ(
      result,
      "<p>"
      "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0\xBC"
      "_\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1"
      "\x8F_</p>\n");
}

TEST(example_372) {
  const auto result{
      sourcemeta::core::markdown_to_html("aa_\"bb\"_cc\n", false)};
  EXPECT_EQ(result, "<p>aa_&quot;bb&quot;_cc</p>\n");
}

TEST(example_373) {
  const auto result{sourcemeta::core::markdown_to_html("foo-_(bar)_\n", false)};
  EXPECT_EQ(result, "<p>foo-<em>(bar)</em></p>\n");
}

TEST(example_374) {
  const auto result{sourcemeta::core::markdown_to_html("_foo*\n", false)};
  EXPECT_EQ(result, "<p>_foo*</p>\n");
}

TEST(example_375) {
  const auto result{sourcemeta::core::markdown_to_html("*foo bar *\n", false)};
  EXPECT_EQ(result, "<p>*foo bar *</p>\n");
}

TEST(example_376) {
  const auto result{sourcemeta::core::markdown_to_html("*foo bar\n"
                                                       "*\n",
                                                       false)};
  EXPECT_EQ(result, "<p>*foo bar\n"
                    "*</p>\n");
}

TEST(example_377) {
  const auto result{sourcemeta::core::markdown_to_html("*(*foo)\n", false)};
  EXPECT_EQ(result, "<p>*(*foo)</p>\n");
}

TEST(example_378) {
  const auto result{sourcemeta::core::markdown_to_html("*(*foo*)*\n", false)};
  EXPECT_EQ(result, "<p><em>(<em>foo</em>)</em></p>\n");
}

TEST(example_379) {
  const auto result{sourcemeta::core::markdown_to_html("*foo*bar\n", false)};
  EXPECT_EQ(result, "<p><em>foo</em>bar</p>\n");
}

TEST(example_380) {
  const auto result{sourcemeta::core::markdown_to_html("_foo bar _\n", false)};
  EXPECT_EQ(result, "<p>_foo bar _</p>\n");
}

TEST(example_381) {
  const auto result{sourcemeta::core::markdown_to_html("_(_foo)\n", false)};
  EXPECT_EQ(result, "<p>_(_foo)</p>\n");
}

TEST(example_382) {
  const auto result{sourcemeta::core::markdown_to_html("_(_foo_)_\n", false)};
  EXPECT_EQ(result, "<p><em>(<em>foo</em>)</em></p>\n");
}

TEST(example_383) {
  const auto result{sourcemeta::core::markdown_to_html("_foo_bar\n", false)};
  EXPECT_EQ(result, "<p>_foo_bar</p>\n");
}

TEST(example_384) {
  const auto result{sourcemeta::core::markdown_to_html(
      "_\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0"
      "\xBC_"
      "\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1\x8F"
      "\n",
      false)};
  EXPECT_EQ(
      result,
      "<p>_"
      "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0\xBC"
      "_\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1"
      "\x8F</p>\n");
}

TEST(example_385) {
  const auto result{
      sourcemeta::core::markdown_to_html("_foo_bar_baz_\n", false)};
  EXPECT_EQ(result, "<p><em>foo_bar_baz</em></p>\n");
}

TEST(example_386) {
  const auto result{sourcemeta::core::markdown_to_html("_(bar)_.\n", false)};
  EXPECT_EQ(result, "<p><em>(bar)</em>.</p>\n");
}

TEST(example_387) {
  const auto result{sourcemeta::core::markdown_to_html("**foo bar**\n", false)};
  EXPECT_EQ(result, "<p><strong>foo bar</strong></p>\n");
}

TEST(example_388) {
  const auto result{
      sourcemeta::core::markdown_to_html("** foo bar**\n", false)};
  EXPECT_EQ(result, "<p>** foo bar**</p>\n");
}

TEST(example_389) {
  const auto result{
      sourcemeta::core::markdown_to_html("a**\"foo\"**\n", false)};
  EXPECT_EQ(result, "<p>a**&quot;foo&quot;**</p>\n");
}

TEST(example_390) {
  const auto result{sourcemeta::core::markdown_to_html("foo**bar**\n", false)};
  EXPECT_EQ(result, "<p>foo<strong>bar</strong></p>\n");
}

TEST(example_391) {
  const auto result{sourcemeta::core::markdown_to_html("__foo bar__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo bar</strong></p>\n");
}

TEST(example_392) {
  const auto result{
      sourcemeta::core::markdown_to_html("__ foo bar__\n", false)};
  EXPECT_EQ(result, "<p>__ foo bar__</p>\n");
}

TEST(example_393) {
  const auto result{sourcemeta::core::markdown_to_html("__\n"
                                                       "foo bar__\n",
                                                       false)};
  EXPECT_EQ(result, "<p>__\n"
                    "foo bar__</p>\n");
}

TEST(example_394) {
  const auto result{
      sourcemeta::core::markdown_to_html("a__\"foo\"__\n", false)};
  EXPECT_EQ(result, "<p>a__&quot;foo&quot;__</p>\n");
}

TEST(example_395) {
  const auto result{sourcemeta::core::markdown_to_html("foo__bar__\n", false)};
  EXPECT_EQ(result, "<p>foo__bar__</p>\n");
}

TEST(example_396) {
  const auto result{sourcemeta::core::markdown_to_html("5__6__78\n", false)};
  EXPECT_EQ(result, "<p>5__6__78</p>\n");
}

TEST(example_397) {
  const auto result{sourcemeta::core::markdown_to_html(
      "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0\xBC"
      "__"
      "\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1\x8F"
      "__\n",
      false)};
  EXPECT_EQ(result, "<p>"
                    "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD"
                    "\xD1\x8F\xD0\xBC__"
                    "\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82"
                    "\xD1\x81\xD1\x8F__</p>\n");
}

TEST(example_398) {
  const auto result{
      sourcemeta::core::markdown_to_html("__foo, __bar__, baz__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo, <strong>bar</strong>, baz</strong></p>\n");
}

TEST(example_399) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo-__(bar)__\n", false)};
  EXPECT_EQ(result, "<p>foo-<strong>(bar)</strong></p>\n");
}

TEST(example_400) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo bar **\n", false)};
  EXPECT_EQ(result, "<p>**foo bar **</p>\n");
}

TEST(example_401) {
  const auto result{sourcemeta::core::markdown_to_html("**(**foo)\n", false)};
  EXPECT_EQ(result, "<p>**(**foo)</p>\n");
}

TEST(example_402) {
  const auto result{sourcemeta::core::markdown_to_html("*(**foo**)*\n", false)};
  EXPECT_EQ(result, "<p><em>(<strong>foo</strong>)</em></p>\n");
}

TEST(example_403) {
  const auto result{sourcemeta::core::markdown_to_html(
      "**Gomphocarpus (*Gomphocarpus physocarpus*, syn.\n"
      "*Asclepias physocarpa*)**\n",
      false)};
  EXPECT_EQ(result,
            "<p><strong>Gomphocarpus (<em>Gomphocarpus physocarpus</em>, syn.\n"
            "<em>Asclepias physocarpa</em>)</strong></p>\n");
}

TEST(example_404) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo \"*bar*\" foo**\n", false)};
  EXPECT_EQ(result,
            "<p><strong>foo &quot;<em>bar</em>&quot; foo</strong></p>\n");
}

TEST(example_405) {
  const auto result{sourcemeta::core::markdown_to_html("**foo**bar\n", false)};
  EXPECT_EQ(result, "<p><strong>foo</strong>bar</p>\n");
}

TEST(example_406) {
  const auto result{
      sourcemeta::core::markdown_to_html("__foo bar __\n", false)};
  EXPECT_EQ(result, "<p>__foo bar __</p>\n");
}

TEST(example_407) {
  const auto result{sourcemeta::core::markdown_to_html("__(__foo)\n", false)};
  EXPECT_EQ(result, "<p>__(__foo)</p>\n");
}

TEST(example_408) {
  const auto result{sourcemeta::core::markdown_to_html("_(__foo__)_\n", false)};
  EXPECT_EQ(result, "<p><em>(<strong>foo</strong>)</em></p>\n");
}

TEST(example_409) {
  const auto result{sourcemeta::core::markdown_to_html("__foo__bar\n", false)};
  EXPECT_EQ(result, "<p>__foo__bar</p>\n");
}

TEST(example_410) {
  const auto result{sourcemeta::core::markdown_to_html(
      "__"
      "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD\xD1\x8F\xD0\xBC"
      "__"
      "\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82\xD1\x81\xD1\x8F"
      "\n",
      false)};
  EXPECT_EQ(result, "<p>__"
                    "\xD0\xBF\xD1\x80\xD0\xB8\xD1\x81\xD1\x82\xD0\xB0\xD0\xBD"
                    "\xD1\x8F\xD0\xBC__"
                    "\xD1\x81\xD1\x82\xD1\x80\xD0\xB5\xD0\xBC\xD1\x8F\xD1\x82"
                    "\xD1\x81\xD1\x8F</p>\n");
}

TEST(example_411) {
  const auto result{
      sourcemeta::core::markdown_to_html("__foo__bar__baz__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo__bar__baz</strong></p>\n");
}

TEST(example_412) {
  const auto result{sourcemeta::core::markdown_to_html("__(bar)__.\n", false)};
  EXPECT_EQ(result, "<p><strong>(bar)</strong>.</p>\n");
}

TEST(example_413) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo [bar](/url)*\n", false)};
  EXPECT_EQ(result, "<p><em>foo <a href=\"/url\">bar</a></em></p>\n");
}

TEST(example_414) {
  const auto result{sourcemeta::core::markdown_to_html("*foo\n"
                                                       "bar*\n",
                                                       false)};
  EXPECT_EQ(result, "<p><em>foo\n"
                    "bar</em></p>\n");
}

TEST(example_415) {
  const auto result{
      sourcemeta::core::markdown_to_html("_foo __bar__ baz_\n", false)};
  EXPECT_EQ(result, "<p><em>foo <strong>bar</strong> baz</em></p>\n");
}

TEST(example_416) {
  const auto result{
      sourcemeta::core::markdown_to_html("_foo _bar_ baz_\n", false)};
  EXPECT_EQ(result, "<p><em>foo <em>bar</em> baz</em></p>\n");
}

TEST(example_417) {
  const auto result{sourcemeta::core::markdown_to_html("__foo_ bar_\n", false)};
  EXPECT_EQ(result, "<p><em><em>foo</em> bar</em></p>\n");
}

TEST(example_418) {
  const auto result{sourcemeta::core::markdown_to_html("*foo *bar**\n", false)};
  EXPECT_EQ(result, "<p><em>foo <em>bar</em></em></p>\n");
}

TEST(example_419) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo **bar** baz*\n", false)};
  EXPECT_EQ(result, "<p><em>foo <strong>bar</strong> baz</em></p>\n");
}

TEST(example_420) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo**bar**baz*\n", false)};
  EXPECT_EQ(result, "<p><em>foo<strong>bar</strong>baz</em></p>\n");
}

TEST(example_421) {
  const auto result{sourcemeta::core::markdown_to_html("*foo**bar*\n", false)};
  EXPECT_EQ(result, "<p><em>foo**bar</em></p>\n");
}

TEST(example_422) {
  const auto result{
      sourcemeta::core::markdown_to_html("***foo** bar*\n", false)};
  EXPECT_EQ(result, "<p><em><strong>foo</strong> bar</em></p>\n");
}

TEST(example_423) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo **bar***\n", false)};
  EXPECT_EQ(result, "<p><em>foo <strong>bar</strong></em></p>\n");
}

TEST(example_424) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo**bar***\n", false)};
  EXPECT_EQ(result, "<p><em>foo<strong>bar</strong></em></p>\n");
}

TEST(example_425) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo***bar***baz\n", false)};
  EXPECT_EQ(result, "<p>foo<em><strong>bar</strong></em>baz</p>\n");
}

TEST(example_426) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo******bar*********baz\n", false)};
  EXPECT_EQ(result, "<p>foo<strong><strong><strong>bar</strong></strong></"
                    "strong>***baz</p>\n");
}

TEST(example_427) {
  const auto result{sourcemeta::core::markdown_to_html(
      "*foo **bar *baz* bim** bop*\n", false)};
  EXPECT_EQ(result,
            "<p><em>foo <strong>bar <em>baz</em> bim</strong> bop</em></p>\n");
}

TEST(example_428) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo [*bar*](/url)*\n", false)};
  EXPECT_EQ(result, "<p><em>foo <a href=\"/url\"><em>bar</em></a></em></p>\n");
}

TEST(example_429) {
  const auto result{sourcemeta::core::markdown_to_html(
      "** is not an empty emphasis\n", false)};
  EXPECT_EQ(result, "<p>** is not an empty emphasis</p>\n");
}

TEST(example_430) {
  const auto result{sourcemeta::core::markdown_to_html(
      "**** is not an empty strong emphasis\n", false)};
  EXPECT_EQ(result, "<p>**** is not an empty strong emphasis</p>\n");
}

TEST(example_431) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo [bar](/url)**\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <a href=\"/url\">bar</a></strong></p>\n");
}

TEST(example_432) {
  const auto result{sourcemeta::core::markdown_to_html("**foo\n"
                                                       "bar**\n",
                                                       false)};
  EXPECT_EQ(result, "<p><strong>foo\n"
                    "bar</strong></p>\n");
}

TEST(example_433) {
  const auto result{
      sourcemeta::core::markdown_to_html("__foo _bar_ baz__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <em>bar</em> baz</strong></p>\n");
}

TEST(example_434) {
  const auto result{
      sourcemeta::core::markdown_to_html("__foo __bar__ baz__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <strong>bar</strong> baz</strong></p>\n");
}

TEST(example_435) {
  const auto result{
      sourcemeta::core::markdown_to_html("____foo__ bar__\n", false)};
  EXPECT_EQ(result, "<p><strong><strong>foo</strong> bar</strong></p>\n");
}

TEST(example_436) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo **bar****\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <strong>bar</strong></strong></p>\n");
}

TEST(example_437) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo *bar* baz**\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <em>bar</em> baz</strong></p>\n");
}

TEST(example_438) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo*bar*baz**\n", false)};
  EXPECT_EQ(result, "<p><strong>foo<em>bar</em>baz</strong></p>\n");
}

TEST(example_439) {
  const auto result{
      sourcemeta::core::markdown_to_html("***foo* bar**\n", false)};
  EXPECT_EQ(result, "<p><strong><em>foo</em> bar</strong></p>\n");
}

TEST(example_440) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo *bar***\n", false)};
  EXPECT_EQ(result, "<p><strong>foo <em>bar</em></strong></p>\n");
}

TEST(example_441) {
  const auto result{sourcemeta::core::markdown_to_html("**foo *bar **baz**\n"
                                                       "bim* bop**\n",
                                                       false)};
  EXPECT_EQ(result, "<p><strong>foo <em>bar <strong>baz</strong>\n"
                    "bim</em> bop</strong></p>\n");
}

TEST(example_442) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo [*bar*](/url)**\n", false)};
  EXPECT_EQ(result,
            "<p><strong>foo <a href=\"/url\"><em>bar</em></a></strong></p>\n");
}

TEST(example_443) {
  const auto result{sourcemeta::core::markdown_to_html(
      "__ is not an empty emphasis\n", false)};
  EXPECT_EQ(result, "<p>__ is not an empty emphasis</p>\n");
}

TEST(example_444) {
  const auto result{sourcemeta::core::markdown_to_html(
      "____ is not an empty strong emphasis\n", false)};
  EXPECT_EQ(result, "<p>____ is not an empty strong emphasis</p>\n");
}

TEST(example_445) {
  const auto result{sourcemeta::core::markdown_to_html("foo ***\n", false)};
  EXPECT_EQ(result, "<p>foo ***</p>\n");
}

TEST(example_446) {
  const auto result{sourcemeta::core::markdown_to_html("foo *\\**\n", false)};
  EXPECT_EQ(result, "<p>foo <em>*</em></p>\n");
}

TEST(example_447) {
  const auto result{sourcemeta::core::markdown_to_html("foo *_*\n", false)};
  EXPECT_EQ(result, "<p>foo <em>_</em></p>\n");
}

TEST(example_448) {
  const auto result{sourcemeta::core::markdown_to_html("foo *****\n", false)};
  EXPECT_EQ(result, "<p>foo *****</p>\n");
}

TEST(example_449) {
  const auto result{sourcemeta::core::markdown_to_html("foo **\\***\n", false)};
  EXPECT_EQ(result, "<p>foo <strong>*</strong></p>\n");
}

TEST(example_450) {
  const auto result{sourcemeta::core::markdown_to_html("foo **_**\n", false)};
  EXPECT_EQ(result, "<p>foo <strong>_</strong></p>\n");
}

TEST(example_451) {
  const auto result{sourcemeta::core::markdown_to_html("**foo*\n", false)};
  EXPECT_EQ(result, "<p>*<em>foo</em></p>\n");
}

TEST(example_452) {
  const auto result{sourcemeta::core::markdown_to_html("*foo**\n", false)};
  EXPECT_EQ(result, "<p><em>foo</em>*</p>\n");
}

TEST(example_453) {
  const auto result{sourcemeta::core::markdown_to_html("***foo**\n", false)};
  EXPECT_EQ(result, "<p>*<strong>foo</strong></p>\n");
}

TEST(example_454) {
  const auto result{sourcemeta::core::markdown_to_html("****foo*\n", false)};
  EXPECT_EQ(result, "<p>***<em>foo</em></p>\n");
}

TEST(example_455) {
  const auto result{sourcemeta::core::markdown_to_html("**foo***\n", false)};
  EXPECT_EQ(result, "<p><strong>foo</strong>*</p>\n");
}

TEST(example_456) {
  const auto result{sourcemeta::core::markdown_to_html("*foo****\n", false)};
  EXPECT_EQ(result, "<p><em>foo</em>***</p>\n");
}

TEST(example_457) {
  const auto result{sourcemeta::core::markdown_to_html("foo ___\n", false)};
  EXPECT_EQ(result, "<p>foo ___</p>\n");
}

TEST(example_458) {
  const auto result{sourcemeta::core::markdown_to_html("foo _\\__\n", false)};
  EXPECT_EQ(result, "<p>foo <em>_</em></p>\n");
}

TEST(example_459) {
  const auto result{sourcemeta::core::markdown_to_html("foo _*_\n", false)};
  EXPECT_EQ(result, "<p>foo <em>*</em></p>\n");
}

TEST(example_460) {
  const auto result{sourcemeta::core::markdown_to_html("foo _____\n", false)};
  EXPECT_EQ(result, "<p>foo _____</p>\n");
}

TEST(example_461) {
  const auto result{sourcemeta::core::markdown_to_html("foo __\\___\n", false)};
  EXPECT_EQ(result, "<p>foo <strong>_</strong></p>\n");
}

TEST(example_462) {
  const auto result{sourcemeta::core::markdown_to_html("foo __*__\n", false)};
  EXPECT_EQ(result, "<p>foo <strong>*</strong></p>\n");
}

TEST(example_463) {
  const auto result{sourcemeta::core::markdown_to_html("__foo_\n", false)};
  EXPECT_EQ(result, "<p>_<em>foo</em></p>\n");
}

TEST(example_464) {
  const auto result{sourcemeta::core::markdown_to_html("_foo__\n", false)};
  EXPECT_EQ(result, "<p><em>foo</em>_</p>\n");
}

TEST(example_465) {
  const auto result{sourcemeta::core::markdown_to_html("___foo__\n", false)};
  EXPECT_EQ(result, "<p>_<strong>foo</strong></p>\n");
}

TEST(example_466) {
  const auto result{sourcemeta::core::markdown_to_html("____foo_\n", false)};
  EXPECT_EQ(result, "<p>___<em>foo</em></p>\n");
}

TEST(example_467) {
  const auto result{sourcemeta::core::markdown_to_html("__foo___\n", false)};
  EXPECT_EQ(result, "<p><strong>foo</strong>_</p>\n");
}

TEST(example_468) {
  const auto result{sourcemeta::core::markdown_to_html("_foo____\n", false)};
  EXPECT_EQ(result, "<p><em>foo</em>___</p>\n");
}

TEST(example_469) {
  const auto result{sourcemeta::core::markdown_to_html("**foo**\n", false)};
  EXPECT_EQ(result, "<p><strong>foo</strong></p>\n");
}

TEST(example_470) {
  const auto result{sourcemeta::core::markdown_to_html("*_foo_*\n", false)};
  EXPECT_EQ(result, "<p><em><em>foo</em></em></p>\n");
}

TEST(example_471) {
  const auto result{sourcemeta::core::markdown_to_html("__foo__\n", false)};
  EXPECT_EQ(result, "<p><strong>foo</strong></p>\n");
}

TEST(example_472) {
  const auto result{sourcemeta::core::markdown_to_html("_*foo*_\n", false)};
  EXPECT_EQ(result, "<p><em><em>foo</em></em></p>\n");
}

TEST(example_473) {
  const auto result{sourcemeta::core::markdown_to_html("****foo****\n", false)};
  EXPECT_EQ(result, "<p><strong><strong>foo</strong></strong></p>\n");
}

TEST(example_474) {
  const auto result{sourcemeta::core::markdown_to_html("____foo____\n", false)};
  EXPECT_EQ(result, "<p><strong><strong>foo</strong></strong></p>\n");
}

TEST(example_475) {
  const auto result{
      sourcemeta::core::markdown_to_html("******foo******\n", false)};
  EXPECT_EQ(result,
            "<p><strong><strong><strong>foo</strong></strong></strong></p>\n");
}

TEST(example_476) {
  const auto result{sourcemeta::core::markdown_to_html("***foo***\n", false)};
  EXPECT_EQ(result, "<p><em><strong>foo</strong></em></p>\n");
}

TEST(example_477) {
  const auto result{
      sourcemeta::core::markdown_to_html("_____foo_____\n", false)};
  EXPECT_EQ(result, "<p><em><strong><strong>foo</strong></strong></em></p>\n");
}

TEST(example_478) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo _bar* baz_\n", false)};
  EXPECT_EQ(result, "<p><em>foo _bar</em> baz_</p>\n");
}

TEST(example_479) {
  const auto result{sourcemeta::core::markdown_to_html(
      "*foo __bar *baz bim__ bam*\n", false)};
  EXPECT_EQ(result, "<p><em>foo <strong>bar *baz bim</strong> bam</em></p>\n");
}

TEST(example_480) {
  const auto result{
      sourcemeta::core::markdown_to_html("**foo **bar baz**\n", false)};
  EXPECT_EQ(result, "<p>**foo <strong>bar baz</strong></p>\n");
}

TEST(example_481) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo *bar baz*\n", false)};
  EXPECT_EQ(result, "<p>*foo <em>bar baz</em></p>\n");
}

TEST(example_482) {
  const auto result{
      sourcemeta::core::markdown_to_html("*[bar*](/url)\n", false)};
  EXPECT_EQ(result, "<p>*<a href=\"/url\">bar*</a></p>\n");
}

TEST(example_483) {
  const auto result{
      sourcemeta::core::markdown_to_html("_foo [bar_](/url)\n", false)};
  EXPECT_EQ(result, "<p>_foo <a href=\"/url\">bar_</a></p>\n");
}

TEST(example_484) {
  const auto result{sourcemeta::core::markdown_to_html(
      "*<img src=\"foo\" title=\"*\"/>\n", false)};
  EXPECT_EQ(result, "<p>*<img src=\"foo\" title=\"*\"/></p>\n");
}

TEST(example_485) {
  const auto result{
      sourcemeta::core::markdown_to_html("**<a href=\"**\">\n", false)};
  EXPECT_EQ(result, "<p>**<a href=\"**\"></p>\n");
}

TEST(example_486) {
  const auto result{
      sourcemeta::core::markdown_to_html("__<a href=\"__\">\n", false)};
  EXPECT_EQ(result, "<p>__<a href=\"__\"></p>\n");
}

TEST(example_487) {
  const auto result{sourcemeta::core::markdown_to_html("*a `*`*\n", false)};
  EXPECT_EQ(result, "<p><em>a <code>*</code></em></p>\n");
}

TEST(example_488) {
  const auto result{sourcemeta::core::markdown_to_html("_a `_`_\n", false)};
  EXPECT_EQ(result, "<p><em>a <code>_</code></em></p>\n");
}

TEST(example_489) {
  const auto result{
      sourcemeta::core::markdown_to_html("**a<http://foo.bar/?q=**>\n", false)};
  EXPECT_EQ(
      result,
      "<p>**a<a href=\"http://foo.bar/?q=**\">http://foo.bar/?q=**</a></p>\n");
}

TEST(example_490) {
  const auto result{
      sourcemeta::core::markdown_to_html("__a<http://foo.bar/?q=__>\n", false)};
  EXPECT_EQ(
      result,
      "<p>__a<a href=\"http://foo.bar/?q=__\">http://foo.bar/?q=__</a></p>\n");
}

TEST(example_491) {
  const auto result{sourcemeta::core::markdown_to_html(
      "~~Hi~~ Hello, ~there~ world!\n", false)};
  EXPECT_EQ(result, "<p><del>Hi</del> Hello, <del>there</del> world!</p>\n");
}

TEST(example_492) {
  const auto result{sourcemeta::core::markdown_to_html("This ~~has a\n"
                                                       "\n"
                                                       "new paragraph~~.\n",
                                                       false)};
  EXPECT_EQ(result, "<p>This ~~has a</p>\n"
                    "<p>new paragraph~~.</p>\n");
}

TEST(example_493) {
  const auto result{sourcemeta::core::markdown_to_html(
      "This will ~~~not~~~ strike.\n", false)};
  EXPECT_EQ(result, "<p>This will ~~~not~~~ strike.</p>\n");
}

TEST(example_494) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](/uri \"title\")\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\" title=\"title\">link</a></p>\n");
}

TEST(example_495) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](/uri)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link</a></p>\n");
}

TEST(example_496) {
  const auto result{sourcemeta::core::markdown_to_html("[link]()\n", false)};
  EXPECT_EQ(result, "<p><a href=\"\">link</a></p>\n");
}

TEST(example_497) {
  const auto result{sourcemeta::core::markdown_to_html("[link](<>)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"\">link</a></p>\n");
}

TEST(example_498) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](/my uri)\n", false)};
  EXPECT_EQ(result, "<p>[link](/my uri)</p>\n");
}

TEST(example_499) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](</my uri>)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/my%20uri\">link</a></p>\n");
}

TEST(example_500) {
  const auto result{sourcemeta::core::markdown_to_html("[link](foo\n"
                                                       "bar)\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[link](foo\n"
                    "bar)</p>\n");
}

TEST(example_501) {
  const auto result{sourcemeta::core::markdown_to_html("[link](<foo\n"
                                                       "bar>)\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[link](<foo\n"
                    "bar>)</p>\n");
}

TEST(example_502) {
  const auto result{sourcemeta::core::markdown_to_html("[a](<b)c>)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"b)c\">a</a></p>\n");
}

TEST(example_503) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](<foo\\>)\n", false)};
  EXPECT_EQ(result, "<p>[link](&lt;foo&gt;)</p>\n");
}

TEST(example_504) {
  const auto result{sourcemeta::core::markdown_to_html("[a](<b)c\n"
                                                       "[a](<b)c>\n"
                                                       "[a](<b>c)\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[a](&lt;b)c\n"
                    "[a](&lt;b)c&gt;\n"
                    "[a](<b>c)</p>\n");
}

TEST(example_505) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](\\(foo\\))\n", false)};
  EXPECT_EQ(result, "<p><a href=\"(foo)\">link</a></p>\n");
}

TEST(example_506) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](foo(and(bar)))\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo(and(bar))\">link</a></p>\n");
}

TEST(example_507) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](foo\\(and\\(bar\\))\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo(and(bar)\">link</a></p>\n");
}

TEST(example_508) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](<foo(and(bar)>)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo(and(bar)\">link</a></p>\n");
}

TEST(example_509) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](foo\\)\\:)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo):\">link</a></p>\n");
}

TEST(example_510) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](#fragment)\n"
      "\n"
      "[link](http://example.com#fragment)\n"
      "\n"
      "[link](http://example.com?foo=3#frag)\n",
      false)};
  EXPECT_EQ(result,
            "<p><a href=\"#fragment\">link</a></p>\n"
            "<p><a href=\"http://example.com#fragment\">link</a></p>\n"
            "<p><a href=\"http://example.com?foo=3#frag\">link</a></p>\n");
}

TEST(example_511) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](foo\\bar)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo%5Cbar\">link</a></p>\n");
}

TEST(example_512) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](foo%20b&auml;)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"foo%20b%C3%A4\">link</a></p>\n");
}

TEST(example_513) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](\"title\")\n", false)};
  EXPECT_EQ(result, "<p><a href=\"%22title%22\">link</a></p>\n");
}

TEST(example_514) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link](/url \"title\")\n"
                                         "[link](/url 'title')\n"
                                         "[link](/url (title))\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">link</a>\n"
                    "<a href=\"/url\" title=\"title\">link</a>\n"
                    "<a href=\"/url\" title=\"title\">link</a></p>\n");
}

TEST(example_515) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](/url \"title \\\"&quot;\")\n", false)};
  EXPECT_EQ(result,
            "<p><a href=\"/url\" title=\"title &quot;&quot;\">link</a></p>\n");
}

TEST(example_516) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](/url\xC2\xA0\"title\")\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/url%C2%A0%22title%22\">link</a></p>\n");
}

TEST(example_517) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](/url \"title \"and\" title\")\n", false)};
  EXPECT_EQ(result,
            "<p>[link](/url &quot;title &quot;and&quot; title&quot;)</p>\n");
}

TEST(example_518) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link](/url 'title \"and\" title')\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title &quot;and&quot; "
                    "title\">link</a></p>\n");
}

TEST(example_519) {
  const auto result{sourcemeta::core::markdown_to_html("[link](   /uri\n"
                                                       "  \"title\"  )\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\" title=\"title\">link</a></p>\n");
}

TEST(example_520) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link] (/uri)\n", false)};
  EXPECT_EQ(result, "<p>[link] (/uri)</p>\n");
}

TEST(example_521) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link [foo [bar]]](/uri)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link [foo [bar]]</a></p>\n");
}

TEST(example_522) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link] bar](/uri)\n", false)};
  EXPECT_EQ(result, "<p>[link] bar](/uri)</p>\n");
}

TEST(example_523) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link [bar](/uri)\n", false)};
  EXPECT_EQ(result, "<p>[link <a href=\"/uri\">bar</a></p>\n");
}

TEST(example_524) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link \\[bar](/uri)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link [bar</a></p>\n");
}

TEST(example_525) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[link *foo **bar** `#`*](/uri)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link <em>foo <strong>bar</strong> "
                    "<code>#</code></em></a></p>\n");
}

TEST(example_526) {
  const auto result{
      sourcemeta::core::markdown_to_html("[![moon](moon.jpg)](/uri)\n", false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"/uri\"><img src=\"moon.jpg\" alt=\"moon\" /></a></p>\n");
}

TEST(example_527) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo [bar](/uri)](/uri)\n", false)};
  EXPECT_EQ(result, "<p>[foo <a href=\"/uri\">bar</a>](/uri)</p>\n");
}

TEST(example_528) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo *[bar [baz](/uri)](/uri)*](/uri)\n", false)};
  EXPECT_EQ(
      result,
      "<p>[foo <em>[bar <a href=\"/uri\">baz</a>](/uri)</em>](/uri)</p>\n");
}

TEST(example_529) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![[[foo](uri1)](uri2)](uri3)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"uri3\" alt=\"[foo](uri2)\" /></p>\n");
}

TEST(example_530) {
  const auto result{
      sourcemeta::core::markdown_to_html("*[foo*](/uri)\n", false)};
  EXPECT_EQ(result, "<p>*<a href=\"/uri\">foo*</a></p>\n");
}

TEST(example_531) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo *bar](baz*)\n", false)};
  EXPECT_EQ(result, "<p><a href=\"baz*\">foo *bar</a></p>\n");
}

TEST(example_532) {
  const auto result{
      sourcemeta::core::markdown_to_html("*foo [bar* baz]\n", false)};
  EXPECT_EQ(result, "<p><em>foo [bar</em> baz]</p>\n");
}

TEST(example_533) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo <bar attr=\"](baz)\">\n", false)};
  EXPECT_EQ(result, "<p>[foo <bar attr=\"](baz)\"></p>\n");
}

TEST(example_534) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo`](/uri)`\n", false)};
  EXPECT_EQ(result, "<p>[foo<code>](/uri)</code></p>\n");
}

TEST(example_535) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo<http://example.com/?search=](uri)>\n", false)};
  EXPECT_EQ(result, "<p>[foo<a "
                    "href=\"http://example.com/?search=%5D(uri)\">http://"
                    "example.com/?search=](uri)</a></p>\n");
}

TEST(example_536) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo][bar]\n"
                                         "\n"
                                         "[bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_537) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link [foo [bar]]][ref]\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link [foo [bar]]</a></p>\n");
}

TEST(example_538) {
  const auto result{sourcemeta::core::markdown_to_html("[link \\[bar][ref]\n"
                                                       "\n"
                                                       "[ref]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link [bar</a></p>\n");
}

TEST(example_539) {
  const auto result{
      sourcemeta::core::markdown_to_html("[link *foo **bar** `#`*][ref]\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">link <em>foo <strong>bar</strong> "
                    "<code>#</code></em></a></p>\n");
}

TEST(example_540) {
  const auto result{
      sourcemeta::core::markdown_to_html("[![moon](moon.jpg)][ref]\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"/uri\"><img src=\"moon.jpg\" alt=\"moon\" /></a></p>\n");
}

TEST(example_541) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo [bar](/uri)][ref]\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(result,
            "<p>[foo <a href=\"/uri\">bar</a>]<a href=\"/uri\">ref</a></p>\n");
}

TEST(example_542) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo *bar [baz][ref]*][ref]\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(result, "<p>[foo <em>bar <a href=\"/uri\">baz</a></em>]<a "
                    "href=\"/uri\">ref</a></p>\n");
}

TEST(example_543) {
  const auto result{sourcemeta::core::markdown_to_html("*[foo*][ref]\n"
                                                       "\n"
                                                       "[ref]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>*<a href=\"/uri\">foo*</a></p>\n");
}

TEST(example_544) {
  const auto result{sourcemeta::core::markdown_to_html("[foo *bar][ref]*\n"
                                                       "\n"
                                                       "[ref]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">foo *bar</a>*</p>\n");
}

TEST(example_545) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo <bar attr=\"][ref]\">\n"
                                         "\n"
                                         "[ref]: /uri\n",
                                         false)};
  EXPECT_EQ(result, "<p>[foo <bar attr=\"][ref]\"></p>\n");
}

TEST(example_546) {
  const auto result{sourcemeta::core::markdown_to_html("[foo`][ref]`\n"
                                                       "\n"
                                                       "[ref]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo<code>][ref]</code></p>\n");
}

TEST(example_547) {
  const auto result{sourcemeta::core::markdown_to_html(
      "[foo<http://example.com/?search=][ref]>\n"
      "\n"
      "[ref]: /uri\n",
      false)};
  EXPECT_EQ(result, "<p>[foo<a "
                    "href=\"http://example.com/?search=%5D%5Bref%5D\">http://"
                    "example.com/?search=][ref]</a></p>\n");
}

TEST(example_548) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo][BaR]\n"
                                         "\n"
                                         "[bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_549) {
  const auto result{sourcemeta::core::markdown_to_html("[\xE1\xBA\x9E]\n"
                                                       "\n"
                                                       "[SS]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">\xE1\xBA\x9E</a></p>\n");
}

TEST(example_550) {
  const auto result{sourcemeta::core::markdown_to_html("[Foo\n"
                                                       "  bar]: /url\n"
                                                       "\n"
                                                       "[Baz][Foo bar]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">Baz</a></p>\n");
}

TEST(example_551) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo] [bar]\n"
                                         "\n"
                                         "[bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>[foo] <a href=\"/url\" title=\"title\">bar</a></p>\n");
}

TEST(example_552) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]\n"
                                         "[bar]\n"
                                         "\n"
                                         "[bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>[foo]\n"
                    "<a href=\"/url\" title=\"title\">bar</a></p>\n");
}

TEST(example_553) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]: /url1\n"
                                                       "\n"
                                                       "[foo]: /url2\n"
                                                       "\n"
                                                       "[bar][foo]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url1\">bar</a></p>\n");
}

TEST(example_554) {
  const auto result{sourcemeta::core::markdown_to_html("[bar][foo\\!]\n"
                                                       "\n"
                                                       "[foo!]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[bar][foo!]</p>\n");
}

TEST(example_555) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][ref[]\n"
                                                       "\n"
                                                       "[ref[]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo][ref[]</p>\n"
                    "<p>[ref[]: /uri</p>\n");
}

TEST(example_556) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][ref[bar]]\n"
                                                       "\n"
                                                       "[ref[bar]]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo][ref[bar]]</p>\n"
                    "<p>[ref[bar]]: /uri</p>\n");
}

TEST(example_557) {
  const auto result{sourcemeta::core::markdown_to_html("[[[foo]]]\n"
                                                       "\n"
                                                       "[[[foo]]]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[[[foo]]]</p>\n"
                    "<p>[[[foo]]]: /url</p>\n");
}

TEST(example_558) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][ref\\[]\n"
                                                       "\n"
                                                       "[ref\\[]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">foo</a></p>\n");
}

TEST(example_559) {
  const auto result{sourcemeta::core::markdown_to_html("[bar\\\\]: /uri\n"
                                                       "\n"
                                                       "[bar\\\\]\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/uri\">bar\\</a></p>\n");
}

TEST(example_560) {
  const auto result{sourcemeta::core::markdown_to_html("[]\n"
                                                       "\n"
                                                       "[]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[]</p>\n"
                    "<p>[]: /uri</p>\n");
}

TEST(example_561) {
  const auto result{sourcemeta::core::markdown_to_html("[\n"
                                                       " ]\n"
                                                       "\n"
                                                       "[\n"
                                                       " ]: /uri\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[\n"
                    "]</p>\n"
                    "<p>[\n"
                    "]: /uri</p>\n");
}

TEST(example_562) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo][]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_563) {
  const auto result{
      sourcemeta::core::markdown_to_html("[*foo* bar][]\n"
                                         "\n"
                                         "[*foo* bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><a href=\"/url\" title=\"title\"><em>foo</em> bar</a></p>\n");
}

TEST(example_564) {
  const auto result{
      sourcemeta::core::markdown_to_html("[Foo][]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">Foo</a></p>\n");
}

TEST(example_565) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo] \n"
                                         "[]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a>\n"
                    "[]</p>\n");
}

TEST(example_566) {
  const auto result{
      sourcemeta::core::markdown_to_html("[foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_567) {
  const auto result{
      sourcemeta::core::markdown_to_html("[*foo* bar]\n"
                                         "\n"
                                         "[*foo* bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><a href=\"/url\" title=\"title\"><em>foo</em> bar</a></p>\n");
}

TEST(example_568) {
  const auto result{
      sourcemeta::core::markdown_to_html("[[*foo* bar]]\n"
                                         "\n"
                                         "[*foo* bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p>[<a href=\"/url\" title=\"title\"><em>foo</em> bar</a>]</p>\n");
}

TEST(example_569) {
  const auto result{sourcemeta::core::markdown_to_html("[[bar [foo]\n"
                                                       "\n"
                                                       "[foo]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[[bar <a href=\"/url\">foo</a></p>\n");
}

TEST(example_570) {
  const auto result{
      sourcemeta::core::markdown_to_html("[Foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><a href=\"/url\" title=\"title\">Foo</a></p>\n");
}

TEST(example_571) {
  const auto result{sourcemeta::core::markdown_to_html("[foo] bar\n"
                                                       "\n"
                                                       "[foo]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url\">foo</a> bar</p>\n");
}

TEST(example_572) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\[foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>[foo]</p>\n");
}

TEST(example_573) {
  const auto result{sourcemeta::core::markdown_to_html("[foo*]: /url\n"
                                                       "\n"
                                                       "*[foo*]\n",
                                                       false)};
  EXPECT_EQ(result, "<p>*<a href=\"/url\">foo*</a></p>\n");
}

TEST(example_574) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][bar]\n"
                                                       "\n"
                                                       "[foo]: /url1\n"
                                                       "[bar]: /url2\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url2\">foo</a></p>\n");
}

TEST(example_575) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][]\n"
                                                       "\n"
                                                       "[foo]: /url1\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url1\">foo</a></p>\n");
}

TEST(example_576) {
  const auto result{sourcemeta::core::markdown_to_html("[foo]()\n"
                                                       "\n"
                                                       "[foo]: /url1\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"\">foo</a></p>\n");
}

TEST(example_577) {
  const auto result{sourcemeta::core::markdown_to_html("[foo](not a link)\n"
                                                       "\n"
                                                       "[foo]: /url1\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"/url1\">foo</a>(not a link)</p>\n");
}

TEST(example_578) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][bar][baz]\n"
                                                       "\n"
                                                       "[baz]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo]<a href=\"/url\">bar</a></p>\n");
}

TEST(example_579) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][bar][baz]\n"
                                                       "\n"
                                                       "[baz]: /url1\n"
                                                       "[bar]: /url2\n",
                                                       false)};
  EXPECT_EQ(result,
            "<p><a href=\"/url2\">foo</a><a href=\"/url1\">baz</a></p>\n");
}

TEST(example_580) {
  const auto result{sourcemeta::core::markdown_to_html("[foo][bar][baz]\n"
                                                       "\n"
                                                       "[baz]: /url1\n"
                                                       "[foo]: /url2\n",
                                                       false)};
  EXPECT_EQ(result, "<p>[foo]<a href=\"/url1\">bar</a></p>\n");
}

TEST(example_581) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo](/url \"title\")\n", false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"foo\" title=\"title\" /></p>\n");
}

TEST(example_582) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![foo *bar*]\n"
      "\n"
      "[foo *bar*]: train.jpg \"train & tracks\"\n",
      false)};
  EXPECT_EQ(result, "<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train "
                    "&amp; tracks\" /></p>\n");
}

TEST(example_583) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![foo ![bar](/url)](/url2)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"/url2\" alt=\"foo bar\" /></p>\n");
}

TEST(example_584) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo [bar](/url)](/url2)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"/url2\" alt=\"foo bar\" /></p>\n");
}

TEST(example_585) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![foo *bar*][]\n"
      "\n"
      "[foo *bar*]: train.jpg \"train & tracks\"\n",
      false)};
  EXPECT_EQ(result, "<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train "
                    "&amp; tracks\" /></p>\n");
}

TEST(example_586) {
  const auto result{sourcemeta::core::markdown_to_html(
      "![foo *bar*][foobar]\n"
      "\n"
      "[FOOBAR]: train.jpg \"train & tracks\"\n",
      false)};
  EXPECT_EQ(result, "<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train "
                    "&amp; tracks\" /></p>\n");
}

TEST(example_587) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo](train.jpg)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"train.jpg\" alt=\"foo\" /></p>\n");
}

TEST(example_588) {
  const auto result{sourcemeta::core::markdown_to_html(
      "My ![foo bar](/path/to/train.jpg  \"title\"   )\n", false)};
  EXPECT_EQ(result, "<p>My <img src=\"/path/to/train.jpg\" alt=\"foo bar\" "
                    "title=\"title\" /></p>\n");
}

TEST(example_589) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo](<url>)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"url\" alt=\"foo\" /></p>\n");
}

TEST(example_590) {
  const auto result{sourcemeta::core::markdown_to_html("![](/url)\n", false)};
  EXPECT_EQ(result, "<p><img src=\"/url\" alt=\"\" /></p>\n");
}

TEST(example_591) {
  const auto result{sourcemeta::core::markdown_to_html("![foo][bar]\n"
                                                       "\n"
                                                       "[bar]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><img src=\"/url\" alt=\"foo\" /></p>\n");
}

TEST(example_592) {
  const auto result{sourcemeta::core::markdown_to_html("![foo][bar]\n"
                                                       "\n"
                                                       "[BAR]: /url\n",
                                                       false)};
  EXPECT_EQ(result, "<p><img src=\"/url\" alt=\"foo\" /></p>\n");
}

TEST(example_593) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo][]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"foo\" title=\"title\" /></p>\n");
}

TEST(example_594) {
  const auto result{
      sourcemeta::core::markdown_to_html("![*foo* bar][]\n"
                                         "\n"
                                         "[*foo* bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"foo bar\" title=\"title\" /></p>\n");
}

TEST(example_595) {
  const auto result{
      sourcemeta::core::markdown_to_html("![Foo][]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"Foo\" title=\"title\" /></p>\n");
}

TEST(example_596) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo] \n"
                                         "[]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p><img src=\"/url\" alt=\"foo\" title=\"title\" />\n"
                    "[]</p>\n");
}

TEST(example_597) {
  const auto result{
      sourcemeta::core::markdown_to_html("![foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"foo\" title=\"title\" /></p>\n");
}

TEST(example_598) {
  const auto result{
      sourcemeta::core::markdown_to_html("![*foo* bar]\n"
                                         "\n"
                                         "[*foo* bar]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"foo bar\" title=\"title\" /></p>\n");
}

TEST(example_599) {
  const auto result{
      sourcemeta::core::markdown_to_html("![[foo]]\n"
                                         "\n"
                                         "[[foo]]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>![[foo]]</p>\n"
                    "<p>[[foo]]: /url &quot;title&quot;</p>\n");
}

TEST(example_600) {
  const auto result{
      sourcemeta::core::markdown_to_html("![Foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result,
            "<p><img src=\"/url\" alt=\"Foo\" title=\"title\" /></p>\n");
}

TEST(example_601) {
  const auto result{
      sourcemeta::core::markdown_to_html("!\\[foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>![foo]</p>\n");
}

TEST(example_602) {
  const auto result{
      sourcemeta::core::markdown_to_html("\\![foo]\n"
                                         "\n"
                                         "[foo]: /url \"title\"\n",
                                         false)};
  EXPECT_EQ(result, "<p>!<a href=\"/url\" title=\"title\">foo</a></p>\n");
}

TEST(example_603) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://foo.bar.baz>\n", false)};
  EXPECT_EQ(result,
            "<p><a href=\"http://foo.bar.baz\">http://foo.bar.baz</a></p>\n");
}

TEST(example_604) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<http://foo.bar.baz/test?q=hello&id=22&boolean>\n", false)};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"http://foo.bar.baz/test?q=hello&amp;id=22&amp;boolean\">http://"
      "foo.bar.baz/test?q=hello&amp;id=22&amp;boolean</a></p>\n");
}

TEST(example_605) {
  const auto result{
      sourcemeta::core::markdown_to_html("<irc://foo.bar:2233/baz>\n", false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"irc://foo.bar:2233/baz\">irc://foo.bar:2233/baz</a></p>\n");
}

TEST(example_606) {
  const auto result{
      sourcemeta::core::markdown_to_html("<MAILTO:FOO@BAR.BAZ>\n", false)};
  EXPECT_EQ(result,
            "<p><a href=\"MAILTO:FOO@BAR.BAZ\">MAILTO:FOO@BAR.BAZ</a></p>\n");
}

TEST(example_607) {
  const auto result{sourcemeta::core::markdown_to_html("<a+b+c:d>\n", false)};
  EXPECT_EQ(result, "<p><a href=\"a+b+c:d\">a+b+c:d</a></p>\n");
}

TEST(example_608) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<made-up-scheme://foo,bar>\n", false)};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"made-up-scheme://foo,bar\">made-up-scheme://foo,bar</a></p>\n");
}

TEST(example_609) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://../>\n", false)};
  EXPECT_EQ(result, "<p><a href=\"http://../\">http://../</a></p>\n");
}

TEST(example_610) {
  const auto result{
      sourcemeta::core::markdown_to_html("<localhost:5001/foo>\n", false)};
  EXPECT_EQ(result,
            "<p><a href=\"localhost:5001/foo\">localhost:5001/foo</a></p>\n");
}

TEST(example_611) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://foo.bar/baz bim>\n", false)};
  EXPECT_EQ(result, "<p>&lt;http://foo.bar/baz bim&gt;</p>\n");
}

TEST(example_612) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<http://example.com/\\[\\>\n", false)};
  EXPECT_EQ(result, "<p><a "
                    "href=\"http://example.com/%5C%5B%5C\">http://example.com/"
                    "\\[\\</a></p>\n");
}

TEST(example_613) {
  const auto result{
      sourcemeta::core::markdown_to_html("<foo@bar.example.com>\n", false)};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:foo@bar.example.com\">foo@bar.example.com</a></p>\n");
}

TEST(example_614) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<foo+special@Bar.baz-bar0.com>\n", false)};
  EXPECT_EQ(result, "<p><a "
                    "href=\"mailto:foo+special@Bar.baz-bar0.com\">foo+special@"
                    "Bar.baz-bar0.com</a></p>\n");
}

TEST(example_615) {
  const auto result{
      sourcemeta::core::markdown_to_html("<foo\\+@bar.example.com>\n", false)};
  EXPECT_EQ(result, "<p>&lt;foo+@bar.example.com&gt;</p>\n");
}

TEST(example_616) {
  const auto result{sourcemeta::core::markdown_to_html("<>\n", false)};
  EXPECT_EQ(result, "<p>&lt;&gt;</p>\n");
}

// GFM section 6.9 turns this into an extended autolink, which this example of
// GFM section 6.8 does not account for
TEST(example_617) {
  const auto result{
      sourcemeta::core::markdown_to_html("< http://foo.bar >\n", false)};
  EXPECT_EQ(result,
            "<p>&lt; <a href=\"http://foo.bar\">http://foo.bar</a> &gt;</p>\n");
}

TEST(example_618) {
  const auto result{sourcemeta::core::markdown_to_html("<m:abc>\n", false)};
  EXPECT_EQ(result, "<p>&lt;m:abc&gt;</p>\n");
}

TEST(example_619) {
  const auto result{
      sourcemeta::core::markdown_to_html("<foo.bar.baz>\n", false)};
  EXPECT_EQ(result, "<p>&lt;foo.bar.baz&gt;</p>\n");
}

// GFM section 6.9 turns this into an extended autolink, which this example of
// GFM section 6.8 does not account for
TEST(example_620) {
  const auto result{
      sourcemeta::core::markdown_to_html("http://example.com\n", false)};
  EXPECT_EQ(result,
            "<p><a href=\"http://example.com\">http://example.com</a></p>\n");
}

// GFM section 6.9 turns this into an extended autolink, which this example of
// GFM section 6.8 does not account for
TEST(example_621) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo@bar.example.com\n", false)};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:foo@bar.example.com\">foo@bar.example.com</a></p>\n");
}

TEST(example_622) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.commonmark.org\n", false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"http://www.commonmark.org\">www.commonmark.org</a></p>\n");
}

TEST(example_623) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Visit www.commonmark.org/help for more information.\n", false)};
  EXPECT_EQ(result,
            "<p>Visit <a "
            "href=\"http://www.commonmark.org/help\">www.commonmark.org/help</"
            "a> for more information.</p>\n");
}

TEST(example_624) {
  const auto result{
      sourcemeta::core::markdown_to_html("Visit www.commonmark.org.\n"
                                         "\n"
                                         "Visit www.commonmark.org/a.b.\n",
                                         false)};
  EXPECT_EQ(result,
            "<p>Visit <a "
            "href=\"http://www.commonmark.org\">www.commonmark.org</a>.</p>\n"
            "<p>Visit <a "
            "href=\"http://www.commonmark.org/a.b\">www.commonmark.org/a.b</"
            "a>.</p>\n");
}

TEST(example_625) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=Markup+(business)\n"
      "\n"
      "www.google.com/search?q=Markup+(business)))\n"
      "\n"
      "(www.google.com/search?q=Markup+(business))\n"
      "\n"
      "(www.google.com/search?q=Markup+(business)\n",
      false)};
  EXPECT_EQ(result, "<p><a "
                    "href=\"http://www.google.com/"
                    "search?q=Markup+(business)\">www.google.com/"
                    "search?q=Markup+(business)</a></p>\n"
                    "<p><a "
                    "href=\"http://www.google.com/"
                    "search?q=Markup+(business)\">www.google.com/"
                    "search?q=Markup+(business)</a>))</p>\n"
                    "<p>(<a "
                    "href=\"http://www.google.com/"
                    "search?q=Markup+(business)\">www.google.com/"
                    "search?q=Markup+(business)</a>)</p>\n"
                    "<p>(<a "
                    "href=\"http://www.google.com/"
                    "search?q=Markup+(business)\">www.google.com/"
                    "search?q=Markup+(business)</a></p>\n");
}

TEST(example_626) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=(business))+ok\n", false)};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"http://www.google.com/search?q=(business))+ok\">www.google.com/"
      "search?q=(business))+ok</a></p>\n");
}

TEST(example_627) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=commonmark&hl=en\n"
      "\n"
      "www.google.com/search?q=commonmark&hl;\n",
      false)};
  EXPECT_EQ(result,
            "<p><a "
            "href=\"http://www.google.com/"
            "search?q=commonmark&amp;hl=en\">www.google.com/"
            "search?q=commonmark&amp;hl=en</a></p>\n"
            "<p><a "
            "href=\"http://www.google.com/search?q=commonmark\">www.google.com/"
            "search?q=commonmark</a>&amp;hl;</p>\n");
}

TEST(example_628) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.commonmark.org/he<lp\n", false)};
  EXPECT_EQ(result, "<p><a "
                    "href=\"http://www.commonmark.org/he\">www.commonmark.org/"
                    "he</a>&lt;lp</p>\n");
}

TEST(example_629) {
  const auto result{sourcemeta::core::markdown_to_html(
      "http://commonmark.org\n"
      "\n"
      "(Visit https://encrypted.google.com/search?q=Markup+(business))\n",
      false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"http://commonmark.org\">http://commonmark.org</a></p>\n"
      "<p>(Visit <a "
      "href=\"https://encrypted.google.com/search?q=Markup+(business)\">https:/"
      "/encrypted.google.com/search?q=Markup+(business)</a>)</p>\n");
}

TEST(example_630) {
  const auto result{sourcemeta::core::markdown_to_html("foo@bar.baz\n", false)};
  EXPECT_EQ(result, "<p><a href=\"mailto:foo@bar.baz\">foo@bar.baz</a></p>\n");
}

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_631) {
  const auto result{sourcemeta::core::markdown_to_html(
      "hello@mail+xyz.example isn't valid, but hello+xyz@mail.example is.\n",
      false)};
  EXPECT_EQ(result, "<p>hello@mail+xyz.example isn&#39;t valid, but <a "
                    "href=\"mailto:hello+xyz@mail.example\">hello+xyz@mail."
                    "example</a> is.</p>\n");
}

TEST(example_632) {
  const auto result{sourcemeta::core::markdown_to_html("a.b-c_d@a.b\n"
                                                       "\n"
                                                       "a.b-c_d@a.b.\n"
                                                       "\n"
                                                       "a.b-c_d@a.b-\n"
                                                       "\n"
                                                       "a.b-c_d@a.b_\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a></p>\n"
                    "<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a>.</p>\n"
                    "<p>a.b-c_d@a.b-</p>\n"
                    "<p>a.b-c_d@a.b_</p>\n");
}

TEST(example_633) {
  const auto result{sourcemeta::core::markdown_to_html("mailto:foo@bar.baz\n"
                                                       "\n"
                                                       "mailto:a.b-c_d@a.b\n"
                                                       "\n"
                                                       "mailto:a.b-c_d@a.b.\n"
                                                       "\n"
                                                       "mailto:a.b-c_d@a.b/\n"
                                                       "\n"
                                                       "mailto:a.b-c_d@a.b-\n"
                                                       "\n"
                                                       "mailto:a.b-c_d@a.b_\n"
                                                       "\n"
                                                       "xmpp:foo@bar.baz\n"
                                                       "\n"
                                                       "xmpp:foo@bar.baz.\n",
                                                       false)};
  EXPECT_EQ(result,
            "<p><a href=\"mailto:foo@bar.baz\">mailto:foo@bar.baz</a></p>\n"
            "<p><a href=\"mailto:a.b-c_d@a.b\">mailto:a.b-c_d@a.b</a></p>\n"
            "<p><a href=\"mailto:a.b-c_d@a.b\">mailto:a.b-c_d@a.b</a>.</p>\n"
            "<p><a href=\"mailto:a.b-c_d@a.b\">mailto:a.b-c_d@a.b</a>/</p>\n"
            "<p>mailto:a.b-c_d@a.b-</p>\n"
            "<p>mailto:a.b-c_d@a.b_</p>\n"
            "<p><a href=\"xmpp:foo@bar.baz\">xmpp:foo@bar.baz</a></p>\n"
            "<p><a href=\"xmpp:foo@bar.baz\">xmpp:foo@bar.baz</a>.</p>\n");
}

TEST(example_634) {
  const auto result{
      sourcemeta::core::markdown_to_html("xmpp:foo@bar.baz/txt\n"
                                         "\n"
                                         "xmpp:foo@bar.baz/txt@bin\n"
                                         "\n"
                                         "xmpp:foo@bar.baz/txt@bin.com\n",
                                         false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"xmpp:foo@bar.baz/txt\">xmpp:foo@bar.baz/txt</a></p>\n"
      "<p><a "
      "href=\"xmpp:foo@bar.baz/txt@bin\">xmpp:foo@bar.baz/txt@bin</a></p>\n"
      "<p><a "
      "href=\"xmpp:foo@bar.baz/txt@bin.com\">xmpp:foo@bar.baz/txt@bin.com</a></"
      "p>\n");
}

TEST(example_635) {
  const auto result{
      sourcemeta::core::markdown_to_html("xmpp:foo@bar.baz/txt/bin\n", false)};
  EXPECT_EQ(
      result,
      "<p><a href=\"xmpp:foo@bar.baz/txt\">xmpp:foo@bar.baz/txt</a>/bin</p>\n");
}

TEST(example_636) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a><bab><c2c>\n", false)};
  EXPECT_EQ(result, "<p><a><bab><c2c></p>\n");
}

TEST(example_637) {
  const auto result{sourcemeta::core::markdown_to_html("<a/><b2/>\n", false)};
  EXPECT_EQ(result, "<p><a/><b2/></p>\n");
}

TEST(example_638) {
  const auto result{sourcemeta::core::markdown_to_html("<a  /><b2\n"
                                                       "data=\"foo\" >\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a  /><b2\n"
                    "data=\"foo\" ></p>\n");
}

TEST(example_639) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<a foo=\"bar\" bam = 'baz <em>\"</em>'\n"
      "_boolean zoop:33=zoop:33 />\n",
      false)};
  EXPECT_EQ(result, "<p><a foo=\"bar\" bam = 'baz <em>\"</em>'\n"
                    "_boolean zoop:33=zoop:33 /></p>\n");
}

TEST(example_640) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Foo <responsive-image src=\"foo.jpg\" />\n", false)};
  EXPECT_EQ(result, "<p>Foo <responsive-image src=\"foo.jpg\" /></p>\n");
}

TEST(example_641) {
  const auto result{sourcemeta::core::markdown_to_html("<33> <__>\n", false)};
  EXPECT_EQ(result, "<p>&lt;33&gt; &lt;__&gt;</p>\n");
}

TEST(example_642) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a h*#ref=\"hi\">\n", false)};
  EXPECT_EQ(result, "<p>&lt;a h*#ref=&quot;hi&quot;&gt;</p>\n");
}

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_643) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<a href=\"hi'> <a href=hi'>\n", false)};
  EXPECT_EQ(result,
            "<p>&lt;a href=&quot;hi&#39;&gt; &lt;a href=hi&#39;&gt;</p>\n");
}

TEST(example_644) {
  const auto result{sourcemeta::core::markdown_to_html("< a><\n"
                                                       "foo><bar/ >\n"
                                                       "<foo bar=baz\n"
                                                       "bim!bop />\n",
                                                       false)};
  EXPECT_EQ(result, "<p>&lt; a&gt;&lt;\n"
                    "foo&gt;&lt;bar/ &gt;\n"
                    "&lt;foo bar=baz\n"
                    "bim!bop /&gt;</p>\n");
}

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_645) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a href='bar'title=title>\n", false)};
  EXPECT_EQ(result, "<p>&lt;a href=&#39;bar&#39;title=title&gt;</p>\n");
}

TEST(example_646) {
  const auto result{sourcemeta::core::markdown_to_html("</a></foo >\n", false)};
  EXPECT_EQ(result, "<p></a></foo ></p>\n");
}

TEST(example_647) {
  const auto result{
      sourcemeta::core::markdown_to_html("</a href=\"foo\">\n", false)};
  EXPECT_EQ(result, "<p>&lt;/a href=&quot;foo&quot;&gt;</p>\n");
}

TEST(example_648) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <!-- this is a\n"
                                         "comment - with hyphen -->\n",
                                         false)};
  EXPECT_EQ(result, "<p>foo <!-- this is a\n"
                    "comment - with hyphen --></p>\n");
}

TEST(example_649) {
  const auto result{sourcemeta::core::markdown_to_html(
      "foo <!-- not a comment -- two hyphens -->\n", false)};
  EXPECT_EQ(result, "<p>foo &lt;!-- not a comment -- two hyphens --&gt;</p>\n");
}

TEST(example_650) {
  const auto result{sourcemeta::core::markdown_to_html("foo <!--> foo -->\n"
                                                       "\n"
                                                       "foo <!-- foo--->\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo &lt;!--&gt; foo --&gt;</p>\n"
                    "<p>foo &lt;!-- foo---&gt;</p>\n");
}

TEST(example_651) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <?php echo $a; ?>\n", false)};
  EXPECT_EQ(result, "<p>foo <?php echo $a; ?></p>\n");
}

TEST(example_652) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <!ELEMENT br EMPTY>\n", false)};
  EXPECT_EQ(result, "<p>foo <!ELEMENT br EMPTY></p>\n");
}

TEST(example_653) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <![CDATA[>&<]]>\n", false)};
  EXPECT_EQ(result, "<p>foo <![CDATA[>&<]]></p>\n");
}

TEST(example_654) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <a href=\"&ouml;\">\n", false)};
  EXPECT_EQ(result, "<p>foo <a href=\"&ouml;\"></p>\n");
}

TEST(example_655) {
  const auto result{
      sourcemeta::core::markdown_to_html("foo <a href=\"\\*\">\n", false)};
  EXPECT_EQ(result, "<p>foo <a href=\"\\*\"></p>\n");
}

TEST(example_656) {
  const auto result{
      sourcemeta::core::markdown_to_html("<a href=\"\\\"\">\n", false)};
  EXPECT_EQ(result, "<p>&lt;a href=&quot;&quot;&quot;&gt;</p>\n");
}

TEST(example_657) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<strong> <title> <style> <em>\n"
      "\n"
      "<blockquote>\n"
      "  <xmp> is disallowed.  <XMP> is also disallowed.\n"
      "</blockquote>\n",
      false)};
  EXPECT_EQ(result, "<p><strong> &lt;title> &lt;style> <em></p>\n"
                    "<blockquote>\n"
                    "  &lt;xmp> is disallowed.  &lt;XMP> is also disallowed.\n"
                    "</blockquote>\n");
}

TEST(example_658) {
  const auto result{sourcemeta::core::markdown_to_html("foo  \n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "baz</p>\n");
}

TEST(example_659) {
  const auto result{sourcemeta::core::markdown_to_html("foo\\\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "baz</p>\n");
}

TEST(example_660) {
  const auto result{sourcemeta::core::markdown_to_html("foo       \n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "baz</p>\n");
}

TEST(example_661) {
  const auto result{sourcemeta::core::markdown_to_html("foo  \n"
                                                       "     bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "bar</p>\n");
}

TEST(example_662) {
  const auto result{sourcemeta::core::markdown_to_html("foo\\\n"
                                                       "     bar\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo<br />\n"
                    "bar</p>\n");
}

TEST(example_663) {
  const auto result{sourcemeta::core::markdown_to_html("*foo  \n"
                                                       "bar*\n",
                                                       false)};
  EXPECT_EQ(result, "<p><em>foo<br />\n"
                    "bar</em></p>\n");
}

TEST(example_664) {
  const auto result{sourcemeta::core::markdown_to_html("*foo\\\n"
                                                       "bar*\n",
                                                       false)};
  EXPECT_EQ(result, "<p><em>foo<br />\n"
                    "bar</em></p>\n");
}

TEST(example_665) {
  const auto result{sourcemeta::core::markdown_to_html("`code  \n"
                                                       "span`\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>code   span</code></p>\n");
}

TEST(example_666) {
  const auto result{sourcemeta::core::markdown_to_html("`code\\\n"
                                                       "span`\n",
                                                       false)};
  EXPECT_EQ(result, "<p><code>code\\ span</code></p>\n");
}

TEST(example_667) {
  const auto result{sourcemeta::core::markdown_to_html("<a href=\"foo  \n"
                                                       "bar\">\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"foo  \n"
                    "bar\"></p>\n");
}

TEST(example_668) {
  const auto result{sourcemeta::core::markdown_to_html("<a href=\"foo\\\n"
                                                       "bar\">\n",
                                                       false)};
  EXPECT_EQ(result, "<p><a href=\"foo\\\n"
                    "bar\"></p>\n");
}

TEST(example_669) {
  const auto result{sourcemeta::core::markdown_to_html("foo\\\n", false)};
  EXPECT_EQ(result, "<p>foo\\</p>\n");
}

TEST(example_670) {
  const auto result{sourcemeta::core::markdown_to_html("foo  \n", false)};
  EXPECT_EQ(result, "<p>foo</p>\n");
}

TEST(example_671) {
  const auto result{sourcemeta::core::markdown_to_html("### foo\\\n", false)};
  EXPECT_EQ(result, "<h3>foo\\</h3>\n");
}

TEST(example_672) {
  const auto result{sourcemeta::core::markdown_to_html("### foo  \n", false)};
  EXPECT_EQ(result, "<h3>foo</h3>\n");
}

TEST(example_673) {
  const auto result{sourcemeta::core::markdown_to_html("foo\n"
                                                       "baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo\n"
                    "baz</p>\n");
}

TEST(example_674) {
  const auto result{sourcemeta::core::markdown_to_html("foo \n"
                                                       " baz\n",
                                                       false)};
  EXPECT_EQ(result, "<p>foo\n"
                    "baz</p>\n");
}

// The specification example renders the apostrophe as is, which is the same
// HTML
TEST(example_675) {
  const auto result{
      sourcemeta::core::markdown_to_html("hello $.;'there\n", false)};
  EXPECT_EQ(result, "<p>hello $.;&#39;there</p>\n");
}

TEST(example_676) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Foo \xCF\x87\xCF\x81\xE1\xBF\x86\xCE\xBD\n", false)};
  EXPECT_EQ(result, "<p>Foo \xCF\x87\xCF\x81\xE1\xBF\x86\xCE\xBD</p>\n");
}

TEST(example_677) {
  const auto result{
      sourcemeta::core::markdown_to_html("Multiple     spaces\n", false)};
  EXPECT_EQ(result, "<p>Multiple     spaces</p>\n");
}
