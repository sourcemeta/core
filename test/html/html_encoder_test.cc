#include <gtest/gtest.h>

#include <sourcemeta/core/html.h>

TEST(HTML_writer, example_1) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.h1("Title");
  document.ul().attribute("class", "my-list");
  for (const auto &item : {"foo", "bar", "baz"}) {
    document.li(item);
  }
  document.close();
  document.p("Footer");
  document.close();

  EXPECT_EQ(document.str(), "<div><h1>Title</h1><ul "
                             "class=\"my-list\"><li>foo</li><li>bar</li>"
                             "<li>baz</li></ul><p>Footer</p></div>");
}

TEST(HTML_writer, escaped_text_content) {
  sourcemeta::core::HTMLWriter document;
  document.p(
      "This contains <dangerous> & \"potentially\" 'harmful' characters");

  EXPECT_EQ(
      document.str(),
      "<p>This contains &lt;dangerous&gt; &amp; &quot;potentially&quot; "
      "&#39;harmful&#39; characters</p>");
}

TEST(HTML_writer, raw_html_basic) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.raw("<strong>Bold text</strong>");
  document.close();

  EXPECT_EQ(document.str(), "<div><strong>Bold text</strong></div>");
}

TEST(HTML_writer, raw_html_with_dangerous_content) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.raw("<script>alert('xss')</script>");
  document.close();

  EXPECT_EQ(document.str(), "<div><script>alert('xss')</script></div>");
}

TEST(HTML_writer, raw_html_mixed_with_escaped) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.text("Safe text: <script>");
  document.raw("<em>raw italic</em>");
  document.text(" & more safe text");
  document.close();

  EXPECT_EQ(document.str(),
            "<div>Safe text: &lt;script&gt;<em>raw italic</em> "
            "&amp; more safe text</div>");
}

TEST(HTML_writer, raw_html_in_list) {
  sourcemeta::core::HTMLWriter document;
  document.ul();
  document.li("Regular item");
  document.li();
  document.raw("<strong>Bold</strong> item");
  document.close();
  document.li("Another & escaped item");
  document.close();

  EXPECT_EQ(document.str(),
            "<ul><li>Regular item</li><li><strong>Bold</strong> "
            "item</li><li>Another &amp; escaped item</li></ul>");
}

TEST(HTML_writer, raw_html_empty_content) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.raw("");
  document.close();

  EXPECT_EQ(document.str(), "<div></div>");
}

TEST(HTML_writer, raw_html_with_entities) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.raw("&lt;already&gt; &amp; &quot;escaped&quot;");
  document.close();

  EXPECT_EQ(document.str(),
            "<div>&lt;already&gt; &amp; &quot;escaped&quot;</div>");
}

TEST(HTML_writer, empty_elements) {
  {
    sourcemeta::core::HTMLWriter document;
    document.div();
    document.close();
    EXPECT_EQ(document.str(), "<div></div>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.p();
    document.close();
    EXPECT_EQ(document.str(), "<p></p>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.span().attribute("class", "empty");
    document.close();
    EXPECT_EQ(document.str(), "<span class=\"empty\"></span>");
  }
}

TEST(HTML_writer, void_elements_self_closing) {
  {
    sourcemeta::core::HTMLWriter document;
    document.br();
    EXPECT_EQ(document.str(), "<br />");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.hr();
    EXPECT_EQ(document.str(), "<hr />");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.img().attribute("src", "test.png").attribute("alt", "Test");
    EXPECT_EQ(document.str(), "<img src=\"test.png\" alt=\"Test\" />");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.input().attribute("type", "text").attribute("name", "test");
    EXPECT_EQ(document.str(), "<input type=\"text\" name=\"test\" />");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.meta().attribute("charset", "utf-8");
    EXPECT_EQ(document.str(), "<meta charset=\"utf-8\" />");
  }
}

TEST(HTML_writer, single_text_node_inline_rendering) {
  {
    sourcemeta::core::HTMLWriter document;
    document.p("Hello");
    EXPECT_EQ(document.str(), "<p>Hello</p>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.h1("Title");
    EXPECT_EQ(document.str(), "<h1>Title</h1>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.span("Text");
    EXPECT_EQ(document.str(), "<span>Text</span>");
  }
}

TEST(HTML_writer, multiple_children_block_rendering) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.text("Text 1");
  document.span("Text 2");
  document.close();

  EXPECT_EQ(document.str(), "<div>Text 1<span>Text 2</span></div>");
}

TEST(HTML_writer, attribute_value_escaping) {
  sourcemeta::core::HTMLWriter document;
  document.div()
      .attribute("title", "Alert: \"Click here\" & 'submit'")
      .attribute("data-value", "<script>alert('xss')</script>");
  document.text("Content");
  document.close();

  EXPECT_EQ(
      document.str(),
      "<div title=\"Alert: &quot;Click here&quot; &amp; &#39;submit&#39;\" "
      "data-value=\"&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt;\">"
      "Content</div>");
}

TEST(HTML_writer, empty_attribute_values) {
  sourcemeta::core::HTMLWriter document;
  document.input()
      .attribute("type", "text")
      .attribute("value", "")
      .attribute("placeholder", "Enter text");

  EXPECT_EQ(document.str(),
            "<input type=\"text\" value=\"\" placeholder=\"Enter text\" />");
}

TEST(HTML_writer, nested_html_elements) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.p("First paragraph");
  document.div().attribute("class", "nested");
  document.span("Nested content");
  document.strong("Important");
  document.close();
  document.p("Last paragraph");
  document.close();

  EXPECT_EQ(document.str(),
            "<div><p>First paragraph</p><div class=\"nested\">"
            "<span>Nested content</span><strong>Important</strong></div>"
            "<p>Last paragraph</p></div>");
}

TEST(HTML_writer, complex_table_structure) {
  sourcemeta::core::HTMLWriter document;
  document.table();
  document.thead();
  document.tr();
  document.th("Name");
  document.th("Age");
  document.th("City");
  document.close();
  document.close();
  document.tbody();
  document.tr();
  document.td("John");
  document.td("25");
  document.td("NYC");
  document.close();
  document.tr();
  document.td("Jane");
  document.td("30");
  document.td("LA");
  document.close();
  document.close();
  document.tfoot();
  document.tr();
  document.td().attribute("colspan", "3");
  document.text("2 rows total");
  document.close();
  document.close();
  document.close();
  document.close();

  std::string expected =
      "<table><thead><tr><th>Name</th><th>Age</th><th>City</th></tr></thead>"
      "<tbody><tr><td>John</td><td>25</td><td>NYC</td></tr>"
      "<tr><td>Jane</td><td>30</td><td>LA</td></tr></tbody>"
      "<tfoot><tr><td colspan=\"3\">2 rows "
      "total</td></tr></tfoot></table>";

  EXPECT_EQ(document.str(), expected);
}

TEST(HTML_writer, form_elements_combination) {
  sourcemeta::core::HTMLWriter document;
  document.form().attribute("action", "/submit").attribute("method", "post");
  document.fieldset();
  document.legend("Personal Info");
  document.label().attribute("for", "name");
  document.text("Name:");
  document.close();
  document.input()
      .attribute("type", "text")
      .attribute("id", "name")
      .attribute("name", "name");
  document.label().attribute("for", "age");
  document.text("Age:");
  document.close();
  document.select().attribute("id", "age").attribute("name", "age");
  document.option().attribute("value", "");
  document.text("Select age");
  document.close();
  document.option().attribute("value", "18-25");
  document.text("18-25");
  document.close();
  document.option().attribute("value", "26-35");
  document.text("26-35");
  document.close();
  document.close();
  document.close();
  document.button().attribute("type", "submit");
  document.text("Submit");
  document.close();
  document.close();

  std::string expected = "<form action=\"/submit\" method=\"post\">"
                         "<fieldset><legend>Personal Info</legend>"
                         "<label for=\"name\">Name:</label>"
                         "<input type=\"text\" id=\"name\" name=\"name\" />"
                         "<label for=\"age\">Age:</label>"
                         "<select id=\"age\" name=\"age\">"
                         "<option value=\"\">Select age</option>"
                         "<option value=\"18-25\">18-25</option>"
                         "<option value=\"26-35\">26-35</option>"
                         "</select></fieldset>"
                         "<button type=\"submit\">Submit</button></form>";

  EXPECT_EQ(document.str(), expected);
}

TEST(HTML_writer, unicode_and_special_characters) {
  sourcemeta::core::HTMLWriter document;
  document.p("Unicode: 你好世界 🌍 ñáéíóú");

  EXPECT_EQ(document.str(), "<p>Unicode: 你好世界 🌍 ñáéíóú</p>");
}

TEST(HTML_writer, whitespace_handling) {
  sourcemeta::core::HTMLWriter document;
  document.pre("  Whitespace\n  should be\n    preserved  ");

  EXPECT_EQ(document.str(),
            "<pre>  Whitespace\n  should be\n    preserved  </pre>");
}

TEST(HTML_writer, mixed_raw_and_escaped_complex) {
  sourcemeta::core::HTMLWriter document;
  document.article();
  document.h2("Article Title & <subtitle>");
  document.p();
  document.text("Normal text with ");
  document.em("emphasis");
  document.text(" and ");
  document.raw("<mark>highlighted</mark>");
  document.text(" parts.");
  document.close();
  document.raw("<!-- Raw HTML comment -->");
  document.p("More content & special chars");
  document.close();

  std::string expected = "<article>"
                         "<h2>Article Title &amp; &lt;subtitle&gt;</h2>"
                         "<p>Normal text with <em>emphasis</em> and "
                         "<mark>highlighted</mark> parts.</p>"
                         "<!-- Raw HTML comment -->"
                         "<p>More content &amp; special chars</p>"
                         "</article>";

  EXPECT_EQ(document.str(), expected);
}

TEST(HTML_writer, semantic_html5_elements) {
  sourcemeta::core::HTMLWriter document;
  document.main().attribute("role", "main");
  document.header();
  document.nav();
  document.ul();
  document.li();
  document.a().attribute("href", "/");
  document.text("Home");
  document.close();
  document.close();
  document.li();
  document.a().attribute("href", "/about");
  document.text("About");
  document.close();
  document.close();
  document.close();
  document.close();
  document.close();
  document.section();
  document.article();
  document.h1("Article Title");
  document.p("Article content");
  document.close();
  document.aside("Sidebar content");
  document.close();
  document.footer("Copyright 2024");
  document.close();

  std::string expected = "<main role=\"main\">"
                         "<header><nav><ul>"
                         "<li><a href=\"/\">Home</a></li>"
                         "<li><a href=\"/about\">About</a></li>"
                         "</ul></nav></header>"
                         "<section><article>"
                         "<h1>Article Title</h1>"
                         "<p>Article content</p>"
                         "</article>"
                         "<aside>Sidebar content</aside></section>"
                         "<footer>Copyright 2024</footer>"
                         "</main>";

  EXPECT_EQ(document.str(), expected);
}

TEST(HTML_writer, attribute_order_consistency) {
  sourcemeta::core::HTMLWriter document;
  document.div()
      .attribute("z-index", "1")
      .attribute("class", "test")
      .attribute("id", "main");
  document.text("Content");
  document.close();

  EXPECT_EQ(document.str(),
            "<div z-index=\"1\" class=\"test\" id=\"main\">Content</div>");
}

TEST(HTML_writer, zero_length_strings) {
  {
    sourcemeta::core::HTMLWriter document;
    document.p("");
    EXPECT_EQ(document.str(), "<p></p>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.span().attribute("class", "");
    document.text("");
    document.close();
    EXPECT_EQ(document.str(), "<span class=\"\"></span>");
  }
}

TEST(HTML_writer, escaped_text_in_container) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.text("Safe: <script>alert('xss')</script>");
  document.close();

  EXPECT_EQ(document.str(),
            "<div>Safe: &lt;script&gt;alert(&#39;xss&#39;)"
            "&lt;/script&gt;</div>");
}

TEST(HTML_writer, template_element_tag_name) {
  {
    sourcemeta::core::HTMLWriter document;
    document.template_("Content");
    EXPECT_EQ(document.str(), "<template>Content</template>");
  }
  {
    sourcemeta::core::HTMLWriter document;
    document.template_().attribute("id", "my-template");
    document.text("Content");
    document.close();
    EXPECT_EQ(document.str(),
              "<template id=\"my-template\">Content</template>");
  }
}

TEST(HTML_writer, chaining) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.span().text("hello").close();
  document.close();

  EXPECT_EQ(document.str(), "<div><span>hello</span></div>");
}

TEST(HTML_writer, write_to_stream) {
  sourcemeta::core::HTMLWriter document;
  document.p("Hello");

  std::ostringstream output;
  document.write(output);

  EXPECT_EQ(output.str(), "<p>Hello</p>");
}

TEST(HTML_writer, reserve_does_not_change_output) {
  sourcemeta::core::HTMLWriter document;
  document.reserve(1024);
  document.div("Content");

  EXPECT_EQ(document.str(), "<div>Content</div>");
}

TEST(HTML_writer, void_element_with_attributes) {
  sourcemeta::core::HTMLWriter document;
  document.div();
  document.img().attribute("src", "photo.png").attribute("alt", "A photo");
  document.close();

  EXPECT_EQ(document.str(),
            "<div><img src=\"photo.png\" alt=\"A photo\" /></div>");
}

TEST(HTML_writer, attribute_chaining) {
  sourcemeta::core::HTMLWriter document;
  document.a().attribute("href", "/test").attribute("class", "link");
  document.text("Click");
  document.close();

  EXPECT_EQ(document.str(),
            "<a href=\"/test\" class=\"link\">Click</a>");
}
