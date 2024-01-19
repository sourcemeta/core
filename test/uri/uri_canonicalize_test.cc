#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/uri.h>

TEST(URI_canonicalize, example_1) {
  sourcemeta::jsontoolkit::URI uri{"https://example.com/foo?bar=baz#test"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/foo?bar=baz#test");
}

// The empty fragment is optional
TEST(URI_canonicalize, example_2) {
  sourcemeta::jsontoolkit::URI uri{"http://example.com/foo#"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/foo");
}

// Explicit 80 port for http scheme
TEST(URI_canonicalize, example_3) {
  sourcemeta::jsontoolkit::URI uri{"http://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/default/port");
}

// Explicit 443 port for https scheme
TEST(URI_canonicalize, example_4) {
  sourcemeta::jsontoolkit::URI uri{"https://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com/default/port");
}

TEST(URI_canonicalize, example_5) {
  sourcemeta::jsontoolkit::URI uri{"https://example.com:80/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "https://example.com:80/default/port");
}

TEST(URI_canonicalize, example_6) {
  sourcemeta::jsontoolkit::URI uri{"http://example.com:443/default/port"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com:443/default/port");
}

// Schemes are case insensitive
TEST(URI_canonicalize, example_7) {
  sourcemeta::jsontoolkit::URI uri{
      "hTtP://example.com/case-insensitive-scheme"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-scheme");
}

// Hosts are case insensitive
TEST(URI_canonicalize, example_8) {
  sourcemeta::jsontoolkit::URI uri{"http://exAmpLe.com/case-insensitive-host"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-insensitive-host");
}

// Paths are case sensitive
TEST(URI_canonicalize, example_9) {
  sourcemeta::jsontoolkit::URI uri{"hTtP://exAmpLe.com/case-SENSITIVE-path"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/case-SENSITIVE-path");
}

TEST(URI_canonicalize, example_10) {
  sourcemeta::jsontoolkit::URI uri{"urn:example:schema"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "urn:example:schema");
}

TEST(URI_canonicalize, example_11) {
  sourcemeta::jsontoolkit::URI uri{"../foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "/foo");
}

TEST(URI_canonicalize, example_12) {
  sourcemeta::jsontoolkit::URI uri{"#foo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI_canonicalize, example_13) {
  sourcemeta::jsontoolkit::URI uri{
      "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(),
            "tag:bowtie.report,2023-11:referencing-suite-tag-uris-id");
}

TEST(URI_canonicalize, example_14) {
  sourcemeta::jsontoolkit::URI uri{"http://example.com/escapes/a%c2%b1b"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/escapes/a%C2%B1b");
}

TEST(URI_canonicalize, example_15) {
  sourcemeta::jsontoolkit::URI uri{"http://example.com/unreserved/%7Efoo"};
  uri.canonicalize();
  EXPECT_EQ(uri.recompose(), "http://example.com/unreserved/~foo");
}
