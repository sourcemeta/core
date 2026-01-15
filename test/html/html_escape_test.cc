#include <gtest/gtest.h>

#include <sourcemeta/core/html.h>

TEST(HTML_escape, empty_string) {
  std::string text = "";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "");
}

TEST(HTML_escape, no_escape_needed_letters) {
  std::string text = "hello";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "hello");
}

TEST(HTML_escape, no_escape_needed_alphanumeric) {
  std::string text = "test123";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "test123");
}

TEST(HTML_escape, no_escape_needed_spaces_only) {
  std::string text = "   ";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "   ");
}

TEST(HTML_escape, ampersand_single) {
  std::string text = "&";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;");
}

TEST(HTML_escape, ampersand_in_text) {
  std::string text = "Tom & Jerry";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "Tom &amp; Jerry");
}

TEST(HTML_escape, ampersand_multiple) {
  std::string text = "A&B&C";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "A&amp;B&amp;C");
}

TEST(HTML_escape, ampersand_already_escaped) {
  std::string text = "&amp;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;amp;");
}

TEST(HTML_escape, ampersand_business_context) {
  std::string text = "R&D";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "R&amp;D");
}

TEST(HTML_escape, less_than_single) {
  std::string text = "<";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;");
}

TEST(HTML_escape, less_than_in_comparison) {
  std::string text = "x < y";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "x &lt; y");
}

TEST(HTML_escape, less_than_tag_like) {
  std::string text = "<script>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;script&gt;");
}

TEST(HTML_escape, less_than_multiple) {
  std::string text = "a<b<c";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "a&lt;b&lt;c");
}

TEST(HTML_escape, greater_than_single) {
  std::string text = ">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&gt;");
}

TEST(HTML_escape, greater_than_in_comparison) {
  std::string text = "x > y";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "x &gt; y");
}

TEST(HTML_escape, greater_than_multiple) {
  std::string text = "a>b>c";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "a&gt;b&gt;c");
}

TEST(HTML_escape, double_quote_single) {
  std::string text = "\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;");
}

TEST(HTML_escape, double_quote_in_sentence) {
  std::string text = "She said \"Hello\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "She said &quot;Hello&quot;");
}

TEST(HTML_escape, double_quote_consecutive) {
  std::string text = "\"\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;&quot;");
}

TEST(HTML_escape, double_quote_attribute_like) {
  std::string text = "value=\"test\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "value=&quot;test&quot;");
}

TEST(HTML_escape, single_quote_single) {
  std::string text = "'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;");
}

TEST(HTML_escape, single_quote_contraction) {
  std::string text = "It's";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "It&#39;s");
}

TEST(HTML_escape, single_quote_cant) {
  std::string text = "can't";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "can&#39;t");
}

TEST(HTML_escape, single_quote_consecutive) {
  std::string text = "''";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;&#39;");
}

TEST(HTML_escape, all_five_entities) {
  std::string text = "&<>'\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;&lt;&gt;&#39;&quot;");
}

TEST(HTML_escape, html_tag_with_attributes) {
  std::string text =
      "<tag attr=\"value\" data-test='other'>content & more</tag>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "&lt;tag attr=&quot;value&quot; "
            "data-test=&#39;other&#39;&gt;content &amp; more&lt;/tag&gt;");
}

TEST(HTML_escape, mixed_quotes_and_ampersand) {
  std::string text = "Hello & \"World\" <test>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "Hello &amp; &quot;World&quot; &lt;test&gt;");
}

TEST(HTML_escape, script_tag_xss) {
  std::string text = "<script>alert('xss')</script>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt;");
}

TEST(HTML_escape, img_onerror_xss) {
  std::string text = "<img src='x' onerror='alert(1)'>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;img src=&#39;x&#39; onerror=&#39;alert(1)&#39;&gt;");
}

TEST(HTML_escape, svg_onload_xss) {
  std::string text = "<svg onload=alert(1)>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;svg onload=alert(1)&gt;");
}

TEST(HTML_escape, javascript_url) {
  std::string text = "javascript:alert('test')";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "javascript:alert(&#39;test&#39;)");
}

TEST(HTML_escape, mxss_img_in_attribute) {
  std::string text = "attr='<img src=x onerror=alert(1)>'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "attr=&#39;&lt;img src=x onerror=alert(1)&gt;&#39;");
}

TEST(HTML_escape, mxss_svg_in_attribute) {
  std::string text = "data=\"<svg onload=alert('xss')>\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "data=&quot;&lt;svg onload=alert(&#39;xss&#39;)&gt;&quot;");
}

TEST(HTML_escape, mxss_iframe_javascript) {
  std::string text = "<iframe src=\"javascript:alert('xss')\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "&lt;iframe src=&quot;javascript:alert(&#39;xss&#39;)&quot;&gt;");
}

TEST(HTML_escape, unicode_cafe) {
  std::string text = "café";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "café");
}

TEST(HTML_escape, unicode_naive) {
  std::string text = "naïve";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "naïve");
}

TEST(HTML_escape, unicode_japanese) {
  std::string text = "東京";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "東京");
}

TEST(HTML_escape, unicode_emoji) {
  std::string text = "🚀";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "🚀");
}

TEST(HTML_escape, unicode_with_html_entities) {
  std::string text = "café & 東京 < 🚀";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "café &amp; 東京 &lt; 🚀");
}

TEST(HTML_escape, newline_only) {
  std::string text = "\n";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\n");
}

TEST(HTML_escape, tab_only) {
  std::string text = "\t";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\t");
}

TEST(HTML_escape, carriage_return_only) {
  std::string text = "\r";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\r");
}

TEST(HTML_escape, multiline_text) {
  std::string text = "line1\nline2";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "line1\nline2");
}

TEST(HTML_escape, whitespace_with_entities) {
  std::string text = "line1\n<tag> & test";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "line1\n&lt;tag&gt; &amp; test");
}

TEST(HTML_escape, numeric_entity_39) {
  std::string text = "&#39;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#39;");
}

TEST(HTML_escape, numeric_entity_34) {
  std::string text = "&#34;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#34;");
}

TEST(HTML_escape, hex_entity) {
  std::string text = "&#x27;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#x27;");
}

TEST(HTML_escape, numeric_entity_60) {
  std::string text = "&#60;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#60;");
}

TEST(HTML_escape, div_with_class) {
  std::string text = "<div class=\"container\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;div class=&quot;container&quot;&gt;");
}

TEST(HTML_escape, input_with_mixed_quotes) {
  std::string text = "<input type='text' value=\"test\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;input type=&#39;text&#39; value=&quot;test&quot;&gt;");
}

TEST(HTML_escape, html_document_end) {
  std::string text = "</body></html>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;/body&gt;&lt;/html&gt;");
}

TEST(HTML_escape, meta_charset) {
  std::string text = "<meta charset=\"utf-8\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;meta charset=&quot;utf-8&quot;&gt;");
}

TEST(HTML_escape, basic_html_comment) {
  std::string text = "<!-- comment -->";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;!-- comment --&gt;");
}

TEST(HTML_escape, comment_with_entities) {
  std::string text = "<!-- <script> & 'test' -->";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;!-- &lt;script&gt; &amp; &#39;test&#39; --&gt;");
}

TEST(HTML_escape, css_with_quotes) {
  std::string text = "body { color: 'red'; }";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "body { color: &#39;red&#39;; }");
}

TEST(HTML_escape, css_selector) {
  std::string text = "div > p";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "div &gt; p");
}

TEST(HTML_escape, javascript_condition) {
  std::string text = "if (x < y && z > 'test') { alert(\"hello\"); }";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "if (x &lt; y &amp;&amp; z &gt; &#39;test&#39;) { "
                  "alert(&quot;hello&quot;); }");
}

TEST(HTML_escape, cdata_basic) {
  std::string text = "<![CDATA[content]]>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;![CDATA[content]]&gt;");
}

TEST(HTML_escape, cdata_with_entities) {
  std::string text = "<![CDATA[<tag> & 'test']]>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;![CDATA[&lt;tag&gt; &amp; &#39;test&#39;]]&gt;");
}

TEST(HTML_escape, url_with_query_params) {
  std::string text = "http://example.com?param='value'&other=\"test\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(
      text,
      "http://example.com?param=&#39;value&#39;&amp;other=&quot;test&quot;");
}

TEST(HTML_escape, search_url_with_script) {
  std::string text = "search?q=<script>&format='json'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "search?q=&lt;script&gt;&amp;format=&#39;json&#39;");
}

TEST(HTML_escape, long_string_no_escaping) {
  std::string text = "abcdefghijklmnopqrstuvwxyz0123456789";
  std::string expected = text;
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, expected);
}

TEST(HTML_escape, consecutive_ampersands) {
  std::string text = "&&&";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;&amp;&amp;");
}

TEST(HTML_escape, consecutive_less_than) {
  std::string text = "<<<";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;&lt;&lt;");
}

TEST(HTML_escape, consecutive_greater_than) {
  std::string text = ">>>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&gt;&gt;&gt;");
}

TEST(HTML_escape, consecutive_double_quotes) {
  std::string text = "\"\"\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;&quot;&quot;");
}

TEST(HTML_escape, consecutive_single_quotes) {
  std::string text = "'''";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;&#39;&#39;");
}

TEST(HTML_escape, complex_mixed_content) {
  std::string text = "This is a test & it contains < and > symbols, "
                     "\"quotes\", 'apostrophes', and <tags>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "This is a test &amp; it contains &lt; and &gt; symbols, "
            "&quot;quotes&quot;, &#39;apostrophes&#39;, and &lt;tags&gt;");
}

TEST(HTML_escape, possessive_with_quotes_and_entities) {
  std::string text = "Tom's \"Café\" & Jerry's <adventures>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "Tom&#39;s &quot;Café&quot; &amp; Jerry&#39;s &lt;adventures&gt;");
}
