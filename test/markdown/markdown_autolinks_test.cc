#include <sourcemeta/core/markdown.h>

#include <sourcemeta/core/test.h>

TEST(autolink_url) {
  const auto result{
      sourcemeta::core::markdown_to_html("Visit https://example.com today")};
  EXPECT_EQ(result,
            "<p>Visit <a href=\"https://example.com\">https://example.com</a>"
            " today</p>\n");
}

TEST(autolink_http) {
  const auto result{sourcemeta::core::markdown_to_html("http://example.com")};
  EXPECT_EQ(result,
            "<p><a href=\"http://example.com\">http://example.com</a></p>\n");
}

TEST(autolink_www) {
  const auto result{sourcemeta::core::markdown_to_html("www.example.com")};
  EXPECT_EQ(result,
            "<p><a href=\"http://www.example.com\">www.example.com</a></p>\n");
}

TEST(autolink_email) {
  const auto result{
      sourcemeta::core::markdown_to_html("Contact user@example.com")};
  EXPECT_EQ(result, "<p>Contact <a href=\"mailto:user@example.com\">"
                    "user@example.com</a></p>\n");
}

TEST(angle_bracket_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://example.com>")};
  EXPECT_EQ(result,
            "<p><a href=\"https://example.com\">https://example.com</a></p>\n");
}

TEST(autolink_inside_list) {
  const auto result{sourcemeta::core::markdown_to_html(
      "- visit https://example.com\n- done")};
  EXPECT_EQ(result, "<ul>\n<li>visit <a href=\"https://example.com\">"
                    "https://example.com</a></li>\n<li>done</li>\n</ul>\n");
}

TEST(multiple_autolinks_in_paragraph) {
  const auto result{sourcemeta::core::markdown_to_html(
      "See https://one.com and https://two.com")};
  EXPECT_EQ(result,
            "<p>See <a href=\"https://one.com\">https://one.com</a> and "
            "<a href=\"https://two.com\">https://two.com</a></p>\n");
}

TEST(autolink_with_trailing_punctuation) {
  const auto result{
      sourcemeta::core::markdown_to_html("See https://example.com.")};
  EXPECT_EQ(
      result,
      "<p>See <a href=\"https://example.com\">https://example.com</a>.</p>\n");
}

TEST(autolink_in_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("(see https://example.com)")};
  EXPECT_EQ(
      result,
      "<p>(see <a href=\"https://example.com\">https://example.com</a>)</p>\n");
}

TEST(autolink_takes_precedence_over_code_span) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/`x>`")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%60x\">https://"
                    "sourcemeta.com/`x</a>`</p>\n");
}

TEST(autolink_uri_schemes) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<irc://chat.example>\n\n<x-custom+scheme.1:thing>\n\n<ab:c>")};
  EXPECT_EQ(
      result,
      "<p><a href=\"irc://chat.example\">irc://chat.example</a></p>\n"
      "<p><a href=\"x-custom+scheme.1:thing\">x-custom+scheme.1:thing</a></p>\n"
      "<p><a href=\"ab:c\">ab:c</a></p>\n");
}

TEST(autolink_uri_scheme_too_short_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("<z:thing>")};
  EXPECT_EQ(result, "<p>&lt;z:thing&gt;</p>\n");
}

TEST(autolink_uri_with_space_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/a b>")};
  EXPECT_EQ(result, "<p>&lt;https://sourcemeta.com/a b&gt;</p>\n");
}

TEST(autolink_uri_backslash_is_literal) {
  const auto result{
      sourcemeta::core::markdown_to_html("<https://sourcemeta.com/\\_>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/%5C_\">https://"
                    "sourcemeta.com/\\_</a></p>\n");
}

TEST(autolink_email_in_angle_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("<team@sourcemeta.com>")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a></p>\n");
}

TEST(autolink_email_with_dots_plus_and_hyphen) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<first.last+tag@sub-domain.sourcemeta.com>")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"mailto:first.last+tag@sub-domain.sourcemeta.com\">"
                    "first.last+tag@sub-domain.sourcemeta.com</a></p>\n");
}

TEST(autolink_email_with_backslash_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<team\\@sourcemeta.com>")};
  EXPECT_EQ(result, "<p>&lt;team@sourcemeta.com&gt;</p>\n");
}

TEST(autolink_blank_or_trailing_space_angle_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("< >\n\n<https://sourcemeta.com >")};
  EXPECT_EQ(result, "<p>&lt; &gt;</p>\n"
                    "<p>&lt;https://sourcemeta.com &gt;</p>\n");
}

TEST(autolink_without_scheme_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("<sourcemeta.com>")};
  EXPECT_EQ(result, "<p>&lt;sourcemeta.com&gt;</p>\n");
}

TEST(extended_autolink_www_with_path) {
  const auto result{
      sourcemeta::core::markdown_to_html("see www.sourcemeta.com/docs/intro")};
  EXPECT_EQ(result, "<p>see <a "
                    "href=\"http://www.sourcemeta.com/docs/"
                    "intro\">www.sourcemeta.com/docs/intro</a></p>\n");
}

TEST(extended_autolink_trailing_punctuation_excluded) {
  const auto result{
      sourcemeta::core::markdown_to_html("Go to www.sourcemeta.com/x.y!")};
  EXPECT_EQ(result, "<p>Go to <a "
                    "href=\"http://www.sourcemeta.com/x.y\">www.sourcemeta.com/"
                    "x.y</a>!</p>\n");
}

TEST(extended_autolink_balanced_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/wiki/Thing_(disambiguation)")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"https://sourcemeta.com/wiki/Thing_(disambiguation)\">https://"
      "sourcemeta.com/wiki/Thing_(disambiguation)</a></p>\n");
}

TEST(extended_autolink_unbalanced_closing_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("https://sourcemeta.com/a_(b)))")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/a_(b)\">https://"
                    "sourcemeta.com/a_(b)</a>))</p>\n");
}

TEST(extended_autolink_inside_parentheses) {
  const auto result{
      sourcemeta::core::markdown_to_html("(see https://sourcemeta.com/a_(b))")};
  EXPECT_EQ(result, "<p>(see <a "
                    "href=\"https://sourcemeta.com/a_(b)\">https://"
                    "sourcemeta.com/a_(b)</a>)</p>\n");
}

TEST(extended_autolink_entity_like_suffix_excluded) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/?x=1&amp;\n\nhttps://sourcemeta.com/?x=1&y=2")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/?x=1\">https://"
                    "sourcemeta.com/?x=1</a>&amp;</p>\n"
                    "<p><a "
                    "href=\"https://sourcemeta.com/?x=1&amp;y=2\">https://"
                    "sourcemeta.com/?x=1&amp;y=2</a></p>\n");
}

TEST(extended_autolink_ends_at_less_than) {
  const auto result{
      sourcemeta::core::markdown_to_html("https://sourcemeta.com/a<b")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"https://sourcemeta.com/a\">https://sourcemeta.com/"
                    "a</a>&lt;b</p>\n");
}

TEST(extended_autolink_underscore_in_final_domain_segments) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.sourcemeta._com\n\nwww.source_meta.com")};
  EXPECT_EQ(result, "<p>www.sourcemeta._com</p>\n"
                    "<p>www.source_meta.com</p>\n");
}

TEST(extended_autolink_underscore_in_earlier_domain_segment) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.docs_site.sourcemeta.com")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"http://"
      "www.docs_site.sourcemeta.com\">www.docs_site.sourcemeta.com</a></p>\n");
}

TEST(extended_autolink_http_with_query_and_fragment) {
  const auto result{sourcemeta::core::markdown_to_html(
      "https://sourcemeta.com/search?q=json+schema#results")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"https://sourcemeta.com/search?q=json+schema#results\">https://"
      "sourcemeta.com/search?q=json+schema#results</a></p>\n");
}

TEST(extended_autolink_email_local_and_domain_symbols) {
  const auto result{sourcemeta::core::markdown_to_html(
      "team@sourcemeta.com\n\nteam@source+meta.com\n\nteam+tag@sourcemeta."
      "com")};
  EXPECT_EQ(
      result,
      "<p><a href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a></p>\n"
      "<p>team@source+meta.com</p>\n"
      "<p><a "
      "href=\"mailto:team+tag@sourcemeta.com\">team+tag@sourcemeta.com</a></"
      "p>\n");
}

TEST(extended_autolink_email_trailing_symbols) {
  const auto result{sourcemeta::core::markdown_to_html(
      "first.last@sourcemeta.com.\n\nfirst.last@sourcemeta.com-\n\nfirst.last@"
      "sourcemeta.com_")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"mailto:first.last@sourcemeta.com\">first.last@"
                    "sourcemeta.com</a>.</p>\n"
                    "<p>first.last@sourcemeta.com-</p>\n"
                    "<p>first.last@sourcemeta.com_</p>\n");
}

TEST(extended_autolink_mailto_and_xmpp) {
  const auto result{sourcemeta::core::markdown_to_html(
      "mailto:team@sourcemeta.com\n\nxmpp:team@sourcemeta.com/resource")};
  EXPECT_EQ(
      result,
      "<p><a "
      "href=\"mailto:team@sourcemeta.com\">mailto:team@sourcemeta.com</a></p>\n"
      "<p><a "
      "href=\"xmpp:team@sourcemeta.com/resource\">xmpp:team@sourcemeta.com/"
      "resource</a></p>\n");
}

TEST(extended_autolink_not_after_word_character) {
  const auto result{sourcemeta::core::markdown_to_html("xwww.sourcemeta.com")};
  EXPECT_EQ(result, "<p>xwww.sourcemeta.com</p>\n");
}

TEST(extended_autolink_inside_emphasis) {
  const auto result{
      sourcemeta::core::markdown_to_html("*https://sourcemeta.com*")};
  EXPECT_EQ(
      result,
      "<p><em><a "
      "href=\"https://sourcemeta.com\">https://sourcemeta.com</a></em></p>\n");
}

TEST(extended_autolink_ftp_scheme_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("ftp://files.sourcemeta.com/archive")};
  EXPECT_EQ(result, "<p>ftp://files.sourcemeta.com/archive</p>\n");
}

TEST(extended_autolink_inside_link_text) {
  const auto result{
      sourcemeta::core::markdown_to_html("[https://sourcemeta.com](/x)")};
  EXPECT_EQ(result, "<p><a href=\"/x\">https://sourcemeta.com</a></p>\n");
}

TEST(extended_autolink_inside_code_span) {
  const auto result{
      sourcemeta::core::markdown_to_html("`https://sourcemeta.com`")};
  EXPECT_EQ(result, "<p><code>https://sourcemeta.com</code></p>\n");
}

TEST(extended_autolink_email_after_invalid_candidate) {
  const auto result{
      sourcemeta::core::markdown_to_html("a@b.c_@d\n\nmailto:a@b_@c.d")};
  EXPECT_EQ(result, "<p>a@b.c_@d</p>\n"
                    "<p>mailto:a@b_@c.d</p>\n");
}

TEST(extended_autolink_after_underscore_emphasis_delimiter) {
  const auto result{
      sourcemeta::core::markdown_to_html("_https://sourcemeta.com_")};
  EXPECT_EQ(result, "<p><em>https://sourcemeta.com</em></p>\n");
}

TEST(extended_autolink_trailing_periods) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Visit www.commonmark.org.\n\nVisit www.commonmark.org/a.b.")};
  EXPECT_EQ(result, "<p>Visit <a "
                    "href=\"http://www.commonmark.org\">www.commonmark.org</"
                    "a>.</p>\n"
                    "<p>Visit <a "
                    "href=\"http://www.commonmark.org/a.b\">www.commonmark.org/"
                    "a.b</a>.</p>\n");
}

TEST(extended_autolink_trailing_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=Markup+(business)\n\n"
      "www.google.com/search?q=Markup+(business)))\n\n"
      "(www.google.com/search?q=Markup+(business))\n\n"
      "(www.google.com/search?q=Markup+(business)")};
  EXPECT_EQ(result,
            "<p><a href=\"http://www.google.com/search?q=Markup+(business)\">"
            "www.google.com/search?q=Markup+(business)</a></p>\n"
            "<p><a href=\"http://www.google.com/search?q=Markup+(business)\">"
            "www.google.com/search?q=Markup+(business)</a>))</p>\n"
            "<p>(<a href=\"http://www.google.com/search?q=Markup+(business)\">"
            "www.google.com/search?q=Markup+(business)</a>)</p>\n"
            "<p>(<a href=\"http://www.google.com/search?q=Markup+(business)\">"
            "www.google.com/search?q=Markup+(business)</a></p>\n");
}

TEST(extended_autolink_interior_parentheses) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=(business))+ok")};
  EXPECT_EQ(result,
            "<p><a href=\"http://www.google.com/search?q=(business))+ok\">"
            "www.google.com/search?q=(business))+ok</a></p>\n");
}

TEST(extended_autolink_entity_reference_lookalike) {
  const auto result{sourcemeta::core::markdown_to_html(
      "www.google.com/search?q=commonmark&hl=en\n\n"
      "www.google.com/search?q=commonmark&hl;")};
  EXPECT_EQ(
      result,
      "<p><a href=\"http://www.google.com/search?q=commonmark&amp;hl=en\">"
      "www.google.com/search?q=commonmark&amp;hl=en</a></p>\n"
      "<p><a href=\"http://www.google.com/search?q=commonmark\">"
      "www.google.com/search?q=commonmark</a>&amp;hl;</p>\n");
}

TEST(extended_autolink_entity_reference_lookalike_with_digits) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.sourcemeta.com/a&b2;")};
  EXPECT_EQ(result, "<p><a href=\"http://www.sourcemeta.com/a\">"
                    "www.sourcemeta.com/a</a>&amp;b2;</p>\n");
}

TEST(extended_autolink_trailing_semicolon_is_kept) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.sourcemeta.com/a;")};
  EXPECT_EQ(result, "<p><a href=\"http://www.sourcemeta.com/a;\">"
                    "www.sourcemeta.com/a;</a></p>\n");
}

TEST(extended_autolink_trailing_quote_is_kept) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.sourcemeta.com/a'")};
  EXPECT_EQ(result, "<p><a href=\"http://www.sourcemeta.com/a&#39;\">"
                    "www.sourcemeta.com/a&#39;</a></p>\n");
}

TEST(extended_autolink_less_than_ends_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.commonmark.org/he<lp")};
  EXPECT_EQ(result, "<p><a href=\"http://www.commonmark.org/he\">"
                    "www.commonmark.org/he</a>&lt;lp</p>\n");
}

TEST(extended_url_autolinks) {
  const auto result{sourcemeta::core::markdown_to_html(
      "http://commonmark.org\n\n"
      "(Visit https://encrypted.google.com/search?q=Markup+(business))")};
  EXPECT_EQ(
      result,
      "<p><a href=\"http://commonmark.org\">http://commonmark.org</a>"
      "</p>\n"
      "<p>(Visit <a "
      "href=\"https://encrypted.google.com/search?q=Markup+(business)\">"
      "https://encrypted.google.com/search?q=Markup+(business)</a>)</p>\n");
}

TEST(extended_url_autolink_without_period_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("http://localhost:8080/x")};
  EXPECT_EQ(result, "<p>http://localhost:8080/x</p>\n");
}

TEST(extended_url_autolink_after_other_character_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("x.https://sourcemeta.com")};
  EXPECT_EQ(result, "<p>x.https://sourcemeta.com</p>\n");
}

TEST(extended_autolink_underscore_in_final_segments_of_long_domain) {
  const auto result{
      sourcemeta::core::markdown_to_html("www.a.b.c.d.e.f.g.h.i.j.k.l_m")};
  EXPECT_EQ(result, "<p>www.a.b.c.d.e.f.g.h.i.j.k.l_m</p>\n");
}

TEST(extended_autolink_email_with_plus_before_at_sign_only) {
  const auto result{sourcemeta::core::markdown_to_html(
      "hello@mail+xyz.example isn't valid, but hello+xyz@mail.example is.")};
  EXPECT_EQ(result, "<p>hello@mail+xyz.example isn&#39;t valid, but <a "
                    "href=\"mailto:hello+xyz@mail.example\">"
                    "hello+xyz@mail.example</a> is.</p>\n");
}

TEST(extended_autolink_email_ending_characters) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a.b-c_d@a.b\n\na.b-c_d@a.b.\n\na.b-c_d@a.b-\n\na.b-c_d@a.b_")};
  EXPECT_EQ(result, "<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a></p>\n"
                    "<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a>.</p>\n"
                    "<p>a.b-c_d@a.b-</p>\n"
                    "<p>a.b-c_d@a.b_</p>\n");
}

TEST(extended_autolink_email_ending_in_digit) {
  const auto result{sourcemeta::core::markdown_to_html("a@b.c1")};
  EXPECT_EQ(result, "<p><a href=\"mailto:a@b.c1\">a@b.c1</a></p>\n");
}

TEST(extended_autolink_email_followed_by_at_sign) {
  const auto result{sourcemeta::core::markdown_to_html("a@b.c@d.e")};
  EXPECT_EQ(result, "<p><a href=\"mailto:a@b.c\">a@b.c</a>@d.e</p>\n");
}

TEST(extended_autolink_email_after_other_character_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html(
      "x:team@sourcemeta.com\n\n`x`team@sourcemeta.com")};
  EXPECT_EQ(result, "<p>x:team@sourcemeta.com</p>\n"
                    "<p><code>x</code>team@sourcemeta.com</p>\n");
}

TEST(extended_autolink_email_after_parenthesis_and_emphasis) {
  const auto result{sourcemeta::core::markdown_to_html(
      "(team@sourcemeta.com) *x*team@sourcemeta.com")};
  EXPECT_EQ(result, "<p>(<a href=\"mailto:team@sourcemeta.com\">"
                    "team@sourcemeta.com</a>) <em>x</em><a "
                    "href=\"mailto:team@sourcemeta.com\">"
                    "team@sourcemeta.com</a></p>\n");
}

TEST(extended_autolink_protocols) {
  const auto result{sourcemeta::core::markdown_to_html(
      "mailto:foo@bar.baz\n\nmailto:a.b-c_d@a.b\n\nmailto:a.b-c_d@a.b.\n\n"
      "mailto:a.b-c_d@a.b/\n\nmailto:a.b-c_d@a.b-\n\nmailto:a.b-c_d@a.b_\n\n"
      "xmpp:foo@bar.baz\n\nxmpp:foo@bar.baz.")};
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

TEST(extended_autolink_xmpp_resources) {
  const auto result{sourcemeta::core::markdown_to_html(
      "xmpp:foo@bar.baz/txt\n\nxmpp:foo@bar.baz/txt@bin\n\n"
      "xmpp:foo@bar.baz/txt@bin.com")};
  EXPECT_EQ(result,
            "<p><a href=\"xmpp:foo@bar.baz/txt\">xmpp:foo@bar.baz/txt</a></p>\n"
            "<p><a href=\"xmpp:foo@bar.baz/txt@bin\">xmpp:foo@bar.baz/txt@bin"
            "</a></p>\n"
            "<p><a href=\"xmpp:foo@bar.baz/txt@bin.com\">"
            "xmpp:foo@bar.baz/txt@bin.com</a></p>\n");
}

TEST(extended_autolink_xmpp_resource_ends_at_slash) {
  const auto result{
      sourcemeta::core::markdown_to_html("xmpp:foo@bar.baz/txt/bin")};
  EXPECT_EQ(result,
            "<p><a href=\"xmpp:foo@bar.baz/txt\">xmpp:foo@bar.baz/txt</a>/bin"
            "</p>\n");
}

TEST(extended_autolink_www_without_domain_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("Visit www. or www..com")};
  EXPECT_EQ(result, "<p>Visit www. or www..com</p>\n");
}

TEST(extended_autolinks_after_form_feed_and_line_tabulation) {
  const auto result{sourcemeta::core::markdown_to_html(
      "a\fwww.commonmark.org a\vfoo@bar.baz")};
  EXPECT_EQ(
      result,
      "<p>a\f<a href=\"http://www.commonmark.org\">www.commonmark.org</a> "
      "a\v<a href=\"mailto:foo@bar.baz\">foo@bar.baz</a></p>\n");
}

TEST(extended_url_autolink_domain_starting_with_underscore) {
  const auto result{sourcemeta::core::markdown_to_html("http://_a.b")};
  EXPECT_EQ(result, "<p>http://_a.b</p>\n");
}

TEST(extended_autolink_inside_brackets_that_are_not_a_link) {
  const auto result{
      sourcemeta::core::markdown_to_html("[see www.sourcemeta.com]")};
  EXPECT_EQ(result, "<p>[see www.sourcemeta.com]</p>\n");
}

// GFM section 6.9 trims a trailing question mark from an extended autolink,
// which is the one character of the eight the suite never reaches
TEST(autolink_trailing_question_mark_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com? ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.example.com\">"
                    "www.example.com</a>? ok</p>\n");
}

// CommonMark section 6.2 requires an entity inside an absolute URI autolink to
// be recognised rather than left as written
TEST(autolink_resolves_an_entity_inside_the_destination) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://example.com/&amp;>")};
  EXPECT_EQ(result, "<p><a href=\"http://example.com/&amp;\">"
                    "http://example.com/&amp;</a></p>\n");
}

// GFM section 6.9 trims trailing punctuation from an extended autolink, and the
// resource part of an xmpp address is where a trailing dot has never been seen
TEST(extended_autolink_xmpp_resource_trailing_dot_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("xmpp:team@sourcemeta.com/resource.")};
  EXPECT_EQ(result, "<p><a href=\"xmpp:team@sourcemeta.com/resource\">"
                    "xmpp:team@sourcemeta.com/resource</a>.</p>\n");
}

// GFM section 6.9: "Trailing punctuation (specifically, ?, !, ., ,, :, *, _,
// and ~) will not be considered part of the autolink"
TEST(autolink_trailing_a_comma_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com, ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.example.com\">"
                    "www.example.com</a>, ok</p>\n");
}

// GFM section 6.9: "Trailing punctuation (specifically, ?, !, ., ,, :, *, _,
// and ~) will not be considered part of the autolink"
TEST(autolink_trailing_a_colon_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com: ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.example.com\">"
                    "www.example.com</a>: ok</p>\n");
}

// GFM section 6.9: "Trailing punctuation (specifically, ?, !, ., ,, :, *, _,
// and ~) will not be considered part of the autolink"
// The same section also admits "no underscores may be present in the last two
// segments of the domain", and that reading comes first, so the candidate is no
// autolink rather than one with its tail trimmed
TEST(autolink_with_a_trailing_underscore_is_no_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com_ ok")};
  EXPECT_EQ(result, "<p>See www.example.com_ ok</p>\n");
}

// GFM section 6.9: "Trailing punctuation (specifically, ?, !, ., ,, :, *, _,
// and ~) will not be considered part of the autolink"
TEST(autolink_trailing_a_tilde_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com~ ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.example.com\">"
                    "www.example.com</a>~ ok</p>\n");
}

// A semicolon is not among the trailing punctuation that section lists. It is
// excluded only where it closes an entity reference lookalike, so one standing
// on its own stays part of the link
TEST(autolink_trailing_semicolon_without_an_entity_is_kept) {
  const auto result{sourcemeta::core::markdown_to_html("See www.a.b; ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.a.b;\">www.a.b;</a> ok</p>\n");
}

// GFM section 6.9 admits an extended autolink at the beginning of a line or
// after whitespace or one of *, _, ~ and (. An address that starts a text node
// has no character of its own to the left, so the boundary is whatever node
// precedes it. Strong emphasis ends on one of those characters, so an address
// behind it is an autolink
TEST(extended_autolink_email_after_strong_emphasis) {
  const auto result{
      sourcemeta::core::markdown_to_html("**bold**user@example.com")};
  EXPECT_EQ(result, "<p><strong>bold</strong>"
                    "<a href=\"mailto:user@example.com\">user@example.com</a>"
                    "</p>\n");
}

TEST(extended_autolink_email_after_strikethrough) {
  const auto result{
      sourcemeta::core::markdown_to_html("~~gone~~user@example.com")};
  EXPECT_EQ(result, "<p><del>gone</del>"
                    "<a href=\"mailto:user@example.com\">user@example.com</a>"
                    "</p>\n");
}

TEST(extended_autolink_email_after_a_soft_break) {
  const auto result{
      sourcemeta::core::markdown_to_html("See\nuser@example.com")};
  EXPECT_EQ(result, "<p>See\n"
                    "<a href=\"mailto:user@example.com\">user@example.com</a>"
                    "</p>\n");
}

TEST(extended_autolink_email_after_a_hard_break) {
  const auto result{
      sourcemeta::core::markdown_to_html("See  \nuser@example.com")};
  EXPECT_EQ(result, "<p>See<br />\n"
                    "<a href=\"mailto:user@example.com\">user@example.com</a>"
                    "</p>\n");
}

// A node that ends on none of those characters leaves the address without the
// boundary the section asks for, so it stays plain text
TEST(extended_autolink_email_after_raw_html_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("<span>user@example.com")};
  EXPECT_EQ(result, "<p><!-- raw HTML omitted -->user@example.com</p>\n");
}

TEST(extended_autolink_email_after_a_link_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("[one](/a)user@example.com")};
  EXPECT_EQ(result, "<p><a href=\"/a\">one</a>user@example.com</p>\n");
}

TEST(extended_autolink_email_after_an_image_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("![one](/a)user@example.com")};
  EXPECT_EQ(result, "<p><img src=\"/a\" alt=\"one\" />user@example.com</p>\n");
}

TEST(extended_autolink_email_after_a_code_span_is_not_autolink) {
  const auto result{
      sourcemeta::core::markdown_to_html("`one`user@example.com")};
  EXPECT_EQ(result, "<p><code>one</code>user@example.com</p>\n");
}

TEST(extended_autolink_email_after_a_footnote_reference_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html(
      "Note[^1]user@example.com\n\n[^1]: Text")};
  EXPECT_EQ(
      result,
      "<p>Note<sup class=\"footnote-ref\"><a href=\"#fn-1\" "
      "id=\"fnref-1\" data-footnote-ref>1</a></sup>user@example.com</p>\n"
      "<section class=\"footnotes\" data-footnotes>\n<ol>\n"
      "<li id=\"fn-1\">\n<p>Text <a href=\"#fnref-1\" "
      "class=\"footnote-backref\" data-footnote-backref "
      "data-footnote-backref-idx=\"1\" aria-label=\"Back to reference 1\">"
      "\xe2\x86\xa9</a></p>\n</li>\n</ol>\n</section>\n");
}

// A backslash escape becomes a text node of its own, so the node before the
// address is text rather than a delimiter node, and the last character it
// carries decides. An escaped asterisk is one of the characters GFM section
// 6.9 admits
TEST(extended_autolink_email_after_an_escaped_asterisk) {
  const auto result{sourcemeta::core::markdown_to_html("\\*user@example.com")};
  EXPECT_EQ(result, "<p>*<a href=\"mailto:user@example.com\">"
                    "user@example.com</a></p>\n");
}

TEST(extended_autolink_email_after_an_escaped_exclamation_is_not_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("\\!user@example.com")};
  EXPECT_EQ(result, "<p>!user@example.com</p>\n");
}

// GFM section 6.5 keeps an angle bracket out of the URI of an autolink, so the
// one inside closes nothing and the whole run stays text
TEST(angle_bracket_autolink_with_an_angle_bracket_in_the_uri) {
  const auto result{
      sourcemeta::core::markdown_to_html("<http://example.com/a<b>")};
  EXPECT_EQ(result,
            "<p>&lt;http://example.com/a<!-- raw HTML omitted --></p>\n");
}

// An angle bracket cannot be part of an extended autolink, so it ends the one
// that runs into it
TEST(extended_autolink_ends_at_an_angle_bracket) {
  const auto result{
      sourcemeta::core::markdown_to_html("See www.example.com<1 ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.example.com\">"
                    "www.example.com</a>&lt;1 ok</p>\n");
}

// GFM section 6.9 lists the underscore among the trailing punctuation, which a
// protocol autolink reaches since the rule about the last two domain segments
// only governs the ones written without a protocol
TEST(extended_autolink_protocol_with_a_trailing_underscore_is_trimmed) {
  const auto result{
      sourcemeta::core::markdown_to_html("See http://example.com/a_ ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://example.com/a\">"
                    "http://example.com/a</a>_ ok</p>\n");
}

// The semicolon only leaves an autolink together with an entity reference
// lookalike, which needs at least one alphanumeric character after its
// ampersand, so an ampersand right before the semicolon keeps both
TEST(autolink_trailing_semicolon_after_a_bare_ampersand_is_kept) {
  const auto result{sourcemeta::core::markdown_to_html("See www.a.b/&; ok")};
  EXPECT_EQ(result, "<p>See <a href=\"http://www.a.b/&amp;;\">"
                    "www.a.b/&amp;;</a> ok</p>\n");
}

// GFM section 6.9 writes a scheme as "any sequence of 2-32 characters
// beginning with an ASCII letter", so a longer one names no scheme and the
// angle brackets stand for themselves
TEST(autolink_scheme_of_thirty_three_characters_is_not_an_autolink) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:x>")};
  EXPECT_EQ(result, "<p>&lt;aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:x&gt;</p>\n");
}

TEST(autolink_scheme_of_thirty_one_characters_is_an_autolink) {
  const auto result{sourcemeta::core::markdown_to_html(
      "<aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:x>")};
  EXPECT_EQ(result, "<p><a href=\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:x\">"
                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:x</a></p>\n");
}

// GFM section 6.9 has an extended url autolink need a valid domain after the
// scheme, so a scheme standing on its own names nothing to link to
TEST(extended_autolink_scheme_with_nothing_after_it) {
  const auto result{sourcemeta::core::markdown_to_html("see http:// here")};
  EXPECT_EQ(result, "<p>see http:// here</p>\n");
}

TEST(extended_autolink_scheme_followed_by_no_domain_character) {
  const auto result{sourcemeta::core::markdown_to_html("see http://! here")};
  EXPECT_EQ(result, "<p>see http://! here</p>\n");
}

TEST(extended_autolink_scheme_with_a_single_slash) {
  const auto result{sourcemeta::core::markdown_to_html("see http:/x here")};
  EXPECT_EQ(result, "<p>see http:/x here</p>\n");
}

TEST(extended_autolink_bare_scheme_is_not_an_autolink) {
  const auto result{sourcemeta::core::markdown_to_html("http://")};
  EXPECT_EQ(result, "<p>http://</p>\n");
}

// That section has a valid domain carry "at least one period", so a single
// segment is no domain
TEST(extended_autolink_domain_without_a_period) {
  const auto result{sourcemeta::core::markdown_to_html("http://a")};
  EXPECT_EQ(result, "<p>http://a</p>\n");
}
