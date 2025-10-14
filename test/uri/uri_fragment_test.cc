#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_fragment, https_no_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_fragment, https_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/#foo"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "foo");
}

TEST(URI_fragment, https_with_empty_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "");
}

TEST(URI_fragment, https_with_escaped_jsonpointer) {
  const sourcemeta::core::URI uri{"https://example.com/#/c%25d"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "/c%d");
}

TEST(URI_fragment, invalid_fragment_with_pointer) {
  EXPECT_THROW(sourcemeta::core::URI{"#foo#/$defs/bar"},
               sourcemeta::core::URIParseError);
}

TEST(URI_fragment, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_fragment, urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "foo");
}

TEST(URI_fragment, is_fragment_only_true_1) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_TRUE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_true_2) {
  // In this case it is not really a query, but part of the fragment
  const sourcemeta::core::URI uri{"#foo?bar=baz"};
  EXPECT_TRUE(uri.is_fragment_only());
  EXPECT_EQ(uri.fragment().value(), "foo?bar=baz");
}

TEST(URI_fragment, is_fragment_only_true_3) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_TRUE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_1) {
  const sourcemeta::core::URI uri{"foo#bar"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_2) {
  const sourcemeta::core::URI uri{"example.com/#bar"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_3) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, set_uri_absolute_non_empty_with_hash_prefix) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("#foo");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#foo");
}

TEST(URI_fragment, set_uri_absolute_non_empty) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("foo");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#foo");
}

TEST(URI_fragment, set_uri_absolute_empty) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#");
}

TEST(URI_fragment, set_uri_absolute_replace) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com#bar"};
  uri.fragment("foo");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#foo");
}

TEST(URI_fragment, set_urn_non_empty) {
  sourcemeta::core::URI uri{"urn:example:schema"};
  uri.fragment("foo");
  EXPECT_EQ(uri.recompose(), "urn:example:schema#foo");
}

TEST(URI_fragment, set_urn_replace) {
  sourcemeta::core::URI uri{"urn:example:schema#bar"};
  uri.fragment("foo");
  EXPECT_EQ(uri.recompose(), "urn:example:schema#foo");
}

TEST(URI_fragment, set_urn_empty) {
  sourcemeta::core::URI uri{"urn:example:schema"};
  uri.fragment("");
  EXPECT_EQ(uri.recompose(), "urn:example:schema#");
}

TEST(URI_fragment, set_uri_relative_non_empty) {
  sourcemeta::core::URI uri{"foo/bar"};
  uri.fragment("baz");
  EXPECT_EQ(uri.recompose(), "foo/bar#baz");
}

TEST(URI_fragment, set_uri_empty_non_empty) {
  sourcemeta::core::URI uri{""};
  uri.fragment("baz");
  EXPECT_EQ(uri.recompose(), "#baz");
}

TEST(URI_fragment, set_uri_empty_empty) {
  sourcemeta::core::URI uri{""};
  uri.fragment("");
  EXPECT_EQ(uri.recompose(), "#");
}

TEST(URI_fragment, copy_semantics_1) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  const std::string fragment{"foo"};
  uri.fragment(fragment);
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#foo");
}

TEST(URI_fragment, copy_semantics_2) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  const std::string fragment{""};
  uri.fragment(fragment);
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#");
}

TEST(URI_fragment, copy_semantics_3) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  const std::string fragment{"#foo"};
  uri.fragment(fragment);
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#foo");
}

TEST(URI_fragment, set_pointer_at) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/@foo/bar");
  EXPECT_EQ(uri.fragment(), "/@foo/bar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/@foo/bar");
}

TEST(URI_fragment, set_pointer_bracket) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/[foo/bar");
  EXPECT_EQ(uri.fragment(), "/[foo/bar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/%5Bfoo/bar");
}

TEST(URI_fragment, set_pointer_percentage) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo%bar");
  EXPECT_EQ(uri.fragment(), "/foo%bar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%25bar");
}

TEST(URI_fragment, set_percentage_at_end) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo%");
  EXPECT_EQ(uri.fragment(), "/foo%");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%25");
}

TEST(URI_fragment, set_percentage_with_one_hex) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo%2");
  EXPECT_EQ(uri.fragment(), "/foo%2");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%252");
}

TEST(URI_fragment, set_multiple_percentages) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo%%bar");
  EXPECT_EQ(uri.fragment(), "/foo%%bar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%25%25bar");
}

TEST(URI_fragment, set_percentage_followed_by_valid_hex_sequence) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo%2Fbar");
  EXPECT_EQ(uri.fragment(), "/foo%2Fbar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%252Fbar");
}

TEST(URI_fragment, set_space_character) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo bar");
  EXPECT_EQ(uri.fragment(), "/foo bar");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%20bar");
}

TEST(URI_fragment, set_non_ascii_character) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.fragment("/foo\xC3\xA9");
  EXPECT_EQ(uri.fragment(), "/foo\xC3\xA9");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com#/foo%C3%A9");
}
