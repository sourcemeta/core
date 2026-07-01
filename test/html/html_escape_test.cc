#include <sourcemeta/core/html.h>
#include <sourcemeta/core/test.h>

TEST(empty_string) {
  std::string text = "";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "");
}

TEST(no_escape_needed_letters) {
  std::string text = "hello";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "hello");
}

TEST(no_escape_needed_alphanumeric) {
  std::string text = "test123";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "test123");
}

TEST(no_escape_needed_spaces_only) {
  std::string text = "   ";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "   ");
}

TEST(ampersand_single) {
  std::string text = "&";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;");
}

TEST(ampersand_in_text) {
  std::string text = "Tom & Jerry";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "Tom &amp; Jerry");
}

TEST(ampersand_multiple) {
  std::string text = "A&B&C";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "A&amp;B&amp;C");
}

TEST(ampersand_already_escaped) {
  std::string text = "&amp;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;amp;");
}

TEST(ampersand_business_context) {
  std::string text = "R&D";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "R&amp;D");
}

TEST(less_than_single) {
  std::string text = "<";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;");
}

TEST(less_than_in_comparison) {
  std::string text = "x < y";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "x &lt; y");
}

TEST(less_than_tag_like) {
  std::string text = "<script>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;script&gt;");
}

TEST(less_than_multiple) {
  std::string text = "a<b<c";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "a&lt;b&lt;c");
}

TEST(greater_than_single) {
  std::string text = ">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&gt;");
}

TEST(greater_than_in_comparison) {
  std::string text = "x > y";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "x &gt; y");
}

TEST(greater_than_multiple) {
  std::string text = "a>b>c";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "a&gt;b&gt;c");
}

TEST(double_quote_single) {
  std::string text = "\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;");
}

TEST(double_quote_in_sentence) {
  std::string text = "She said \"Hello\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "She said &quot;Hello&quot;");
}

TEST(double_quote_consecutive) {
  std::string text = "\"\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;&quot;");
}

TEST(double_quote_attribute_like) {
  std::string text = "value=\"test\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "value=&quot;test&quot;");
}

TEST(single_quote_single) {
  std::string text = "'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;");
}

TEST(single_quote_contraction) {
  std::string text = "It's";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "It&#39;s");
}

TEST(single_quote_cant) {
  std::string text = "can't";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "can&#39;t");
}

TEST(single_quote_consecutive) {
  std::string text = "''";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;&#39;");
}

TEST(all_five_entities) {
  std::string text = "&<>'\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;&lt;&gt;&#39;&quot;");
}

TEST(html_tag_with_attributes) {
  std::string text =
      "<tag attr=\"value\" data-test='other'>content & more</tag>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "&lt;tag attr=&quot;value&quot; "
            "data-test=&#39;other&#39;&gt;content &amp; more&lt;/tag&gt;");
}

TEST(mixed_quotes_and_ampersand) {
  std::string text = "Hello & \"World\" <test>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "Hello &amp; &quot;World&quot; &lt;test&gt;");
}

TEST(script_tag_xss) {
  std::string text = "<script>alert('xss')</script>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt;");
}

TEST(img_onerror_xss) {
  std::string text = "<img src='x' onerror='alert(1)'>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;img src=&#39;x&#39; onerror=&#39;alert(1)&#39;&gt;");
}

TEST(svg_onload_xss) {
  std::string text = "<svg onload=alert(1)>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;svg onload=alert(1)&gt;");
}

TEST(javascript_url) {
  std::string text = "javascript:alert('test')";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "javascript:alert(&#39;test&#39;)");
}

TEST(mxss_img_in_attribute) {
  std::string text = "attr='<img src=x onerror=alert(1)>'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "attr=&#39;&lt;img src=x onerror=alert(1)&gt;&#39;");
}

TEST(mxss_svg_in_attribute) {
  std::string text = "data=\"<svg onload=alert('xss')>\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "data=&quot;&lt;svg onload=alert(&#39;xss&#39;)&gt;&quot;");
}

TEST(mxss_iframe_javascript) {
  std::string text = "<iframe src=\"javascript:alert('xss')\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "&lt;iframe src=&quot;javascript:alert(&#39;xss&#39;)&quot;&gt;");
}

TEST(unicode_cafe) {
  std::string text = "café";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "café");
}

TEST(unicode_naive) {
  std::string text = "naïve";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "naïve");
}

TEST(unicode_japanese) {
  std::string text = "東京";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "東京");
}

TEST(unicode_emoji) {
  std::string text = "🚀";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "🚀");
}

TEST(unicode_with_html_entities) {
  std::string text = "café & 東京 < 🚀";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "café &amp; 東京 &lt; 🚀");
}

TEST(newline_only) {
  std::string text = "\n";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\n");
}

TEST(tab_only) {
  std::string text = "\t";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\t");
}

TEST(carriage_return_only) {
  std::string text = "\r";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "\r");
}

TEST(multiline_text) {
  std::string text = "line1\nline2";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "line1\nline2");
}

TEST(whitespace_with_entities) {
  std::string text = "line1\n<tag> & test";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "line1\n&lt;tag&gt; &amp; test");
}

TEST(numeric_entity_39) {
  std::string text = "&#39;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#39;");
}

TEST(numeric_entity_34) {
  std::string text = "&#34;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#34;");
}

TEST(hex_entity) {
  std::string text = "&#x27;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#x27;");
}

TEST(numeric_entity_60) {
  std::string text = "&#60;";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;#60;");
}

TEST(div_with_class) {
  std::string text = "<div class=\"container\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;div class=&quot;container&quot;&gt;");
}

TEST(input_with_mixed_quotes) {
  std::string text = "<input type='text' value=\"test\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;input type=&#39;text&#39; value=&quot;test&quot;&gt;");
}

TEST(html_document_end) {
  std::string text = "</body></html>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;/body&gt;&lt;/html&gt;");
}

TEST(meta_charset) {
  std::string text = "<meta charset=\"utf-8\">";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;meta charset=&quot;utf-8&quot;&gt;");
}

TEST(basic_html_comment) {
  std::string text = "<!-- comment -->";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;!-- comment --&gt;");
}

TEST(comment_with_entities) {
  std::string text = "<!-- <script> & 'test' -->";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;!-- &lt;script&gt; &amp; &#39;test&#39; --&gt;");
}

TEST(css_with_quotes) {
  std::string text = "body { color: 'red'; }";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "body { color: &#39;red&#39;; }");
}

TEST(css_selector) {
  std::string text = "div > p";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "div &gt; p");
}

TEST(javascript_condition) {
  std::string text = "if (x < y && z > 'test') { alert(\"hello\"); }";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "if (x &lt; y &amp;&amp; z &gt; &#39;test&#39;) { "
                  "alert(&quot;hello&quot;); }");
}

TEST(cdata_basic) {
  std::string text = "<![CDATA[content]]>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;![CDATA[content]]&gt;");
}

TEST(cdata_with_entities) {
  std::string text = "<![CDATA[<tag> & 'test']]>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;![CDATA[&lt;tag&gt; &amp; &#39;test&#39;]]&gt;");
}

TEST(url_with_query_params) {
  std::string text = "http://example.com?param='value'&other=\"test\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(
      text,
      "http://example.com?param=&#39;value&#39;&amp;other=&quot;test&quot;");
}

TEST(search_url_with_script) {
  std::string text = "search?q=<script>&format='json'";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "search?q=&lt;script&gt;&amp;format=&#39;json&#39;");
}

TEST(long_string_no_escaping) {
  std::string text = "abcdefghijklmnopqrstuvwxyz0123456789";
  std::string expected = text;
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, expected);
}

TEST(consecutive_ampersands) {
  std::string text = "&&&";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&amp;&amp;&amp;");
}

TEST(consecutive_less_than) {
  std::string text = "<<<";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&lt;&lt;&lt;");
}

TEST(consecutive_greater_than) {
  std::string text = ">>>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&gt;&gt;&gt;");
}

TEST(consecutive_double_quotes) {
  std::string text = "\"\"\"";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&quot;&quot;&quot;");
}

TEST(consecutive_single_quotes) {
  std::string text = "'''";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text, "&#39;&#39;&#39;");
}

TEST(complex_mixed_content) {
  std::string text = "This is a test & it contains < and > symbols, "
                     "\"quotes\", 'apostrophes', and <tags>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "This is a test &amp; it contains &lt; and &gt; symbols, "
            "&quot;quotes&quot;, &#39;apostrophes&#39;, and &lt;tags&gt;");
}

TEST(possessive_with_quotes_and_entities) {
  std::string text = "Tom's \"Café\" & Jerry's <adventures>";
  sourcemeta::core::html_escape(text);
  EXPECT_EQ(text,
            "Tom&#39;s &quot;Café&quot; &amp; Jerry&#39;s &lt;adventures&gt;");
}

TEST(empty) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "");
  EXPECT_EQ(output, "");
}

TEST(no_escaping_needed) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "hello world");
  EXPECT_EQ(output, "hello world");
}

TEST(ampersand) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "a & b");
  EXPECT_EQ(output, "a &amp; b");
}

TEST(less_than) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "a < b");
  EXPECT_EQ(output, "a &lt; b");
}

TEST(greater_than) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "a > b");
  EXPECT_EQ(output, "a &gt; b");
}

TEST(double_quote) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "say \"hello\"");
  EXPECT_EQ(output, "say &quot;hello&quot;");
}

TEST(single_quote) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "it's");
  EXPECT_EQ(output, "it&#39;s");
}

TEST(all_special_characters) {
  std::string output;
  sourcemeta::core::html_escape_append(output, "&<>\"'");
  EXPECT_EQ(output, "&amp;&lt;&gt;&quot;&#39;");
}

TEST(appends_to_existing) {
  std::string output = "prefix:";
  sourcemeta::core::html_escape_append(output, "<tag>");
  EXPECT_EQ(output, "prefix:&lt;tag&gt;");
}

TEST(buffer_empty) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "");
  EXPECT_EQ(buffer.str(), "");
}

TEST(buffer_no_escaping_needed) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "hello world");
  EXPECT_EQ(buffer.str(), "hello world");
}

TEST(buffer_ampersand) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "a & b");
  EXPECT_EQ(buffer.str(), "a &amp; b");
}

TEST(buffer_less_than) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "a < b");
  EXPECT_EQ(buffer.str(), "a &lt; b");
}

TEST(buffer_greater_than) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "a > b");
  EXPECT_EQ(buffer.str(), "a &gt; b");
}

TEST(buffer_double_quote) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "say \"hello\"");
  EXPECT_EQ(buffer.str(), "say &quot;hello&quot;");
}

TEST(buffer_single_quote) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "it's");
  EXPECT_EQ(buffer.str(), "it&#39;s");
}

TEST(buffer_all_special_characters) {
  sourcemeta::core::HTMLBuffer buffer;
  sourcemeta::core::html_escape_append(buffer, "&<>\"'");
  EXPECT_EQ(buffer.str(), "&amp;&lt;&gt;&quot;&#39;");
}

TEST(with_reserve) {
  sourcemeta::core::HTMLBuffer buffer;
  buffer.reserve(1024);
  sourcemeta::core::html_escape_append(buffer, "<script>alert('xss')</script>");
  EXPECT_EQ(buffer.str(), "&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt;");
}
