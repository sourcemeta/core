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
  EXPECT_EQ(result, "<p>&lt;<a "
                    "href=\"https://sourcemeta.com/a\">https://sourcemeta.com/"
                    "a</a> b&gt;</p>\n");
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
  EXPECT_EQ(
      result,
      "<p>&lt;<a "
      "href=\"mailto:team@sourcemeta.com\">team@sourcemeta.com</a>&gt;</p>\n");
}

TEST(autolink_blank_or_trailing_space_angle_brackets) {
  const auto result{
      sourcemeta::core::markdown_to_html("< >\n\n<https://sourcemeta.com >")};
  EXPECT_EQ(
      result,
      "<p>&lt; &gt;</p>\n"
      "<p>&lt;<a href=\"https://sourcemeta.com\">https://sourcemeta.com</a> "
      "&gt;</p>\n");
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
      sourcemeta::core::markdown_to_html("_https://sourcemeta.com_")};
  EXPECT_EQ(
      result,
      "<p><em><a "
      "href=\"https://sourcemeta.com\">https://sourcemeta.com</a></em></p>\n");
}

TEST(extended_autolink_ftp_scheme) {
  const auto result{
      sourcemeta::core::markdown_to_html("ftp://files.sourcemeta.com/archive")};
  EXPECT_EQ(result, "<p><a "
                    "href=\"ftp://files.sourcemeta.com/archive\">ftp://"
                    "files.sourcemeta.com/archive</a></p>\n");
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
