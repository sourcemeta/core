#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

/*
 * RFC 3986 normalization tests
 *
 * We test the normalization through the .recompose() API, as the
 * URI is normalized during construction.
 */

// Inspired from https://cr.openjdk.org/~dfuchs/writeups/updating-uri/

TEST(URI_normalize, rfc3986_1) {
  const sourcemeta::core::URI uri{"s://h/a/../../b"};
  EXPECT_EQ(uri.recompose(), "s://h/b");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1438

TEST(URI_normalize, rfc3986_2) {
  const sourcemeta::core::URI uri{"eXAMPLE://a/./b/../b/%63/%7bfoo%7d"};
  EXPECT_EQ(uri.recompose(), "example://a/b/c/%7Bfoo%7D");
}

TEST(URI_normalize, percent_encoding) {
  const sourcemeta::core::URI uri{"http://examp%4Ce.com/"};
  EXPECT_EQ(uri.recompose(), "http://example.com/");
}

TEST(URI_normalize, dot_segments) {
  const sourcemeta::core::URI uri{"http://example.com/a/b/%2E%2E/"};
  EXPECT_EQ(uri.recompose(), "http://example.com/a/");
}

TEST(URI_normalize, case_normalization) {
  const sourcemeta::core::URI uri{"http://user:pass@SOMEHOST.COM:123"};
  EXPECT_EQ(uri.recompose(), "http://user:pass@somehost.com:123");
}

TEST(URI_normalize, complex_case) {
  const sourcemeta::core::URI uri{"HTTP://a:b@HOST:123/./1/2/../%41?abc#def"};
  EXPECT_EQ(uri.recompose(), "http://a:b@host:123/1/A?abc#def");
}

TEST(URI_normalize, relative_path_1) {
  const sourcemeta::core::URI uri{"../../abc"};
  EXPECT_EQ(uri.recompose(), "../../abc");
}

TEST(URI_normalize, relative_path_2) {
  const sourcemeta::core::URI uri{"../../abc/.."};
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(URI_normalize, relative_path_3) {
  const sourcemeta::core::URI uri{"../../abc/../def"};
  EXPECT_EQ(uri.recompose(), "../../def");
}

TEST(URI_normalize, relative_path_4) {
  const sourcemeta::core::URI uri{"abc/.."};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_normalize, relative_path_5) {
  const sourcemeta::core::URI uri{"abc/../"};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_normalize, relative_path_6) {
  const sourcemeta::core::URI uri{"../../abc/./def"};
  EXPECT_EQ(uri.recompose(), "../../abc/def");
}

TEST(URI_normalize, relative_path_7) {
  const sourcemeta::core::URI uri{"./def"};
  EXPECT_EQ(uri.recompose(), "def");
}

TEST(URI_normalize, relative_path_8) {
  const sourcemeta::core::URI uri{"def/."};
  EXPECT_EQ(uri.recompose(), "def/");
}

TEST(URI_normalize, relative_path_9) {
  const sourcemeta::core::URI uri{"./abc:def"};
  EXPECT_EQ(uri.recompose(), "./abc:def");
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/master/test/test.cpp#L1531

TEST(URI_normalize, path_1) {
  const sourcemeta::core::URI uri{"http://a/b/c/../../.."};
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_normalize, path_2) {
  const sourcemeta::core::URI uri{"http://a/b/../c/../.."};
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_normalize, path_3) {
  const sourcemeta::core::URI uri{"http://a/.."};
  EXPECT_EQ(uri.recompose(), "http://a/");
}

TEST(URI_normalize, path_4) {
  const sourcemeta::core::URI uri{"/.."};
  EXPECT_EQ(uri.recompose(), "/");
}

TEST(URI_normalize, path_5) {
  const sourcemeta::core::URI uri{"http://a/..///"};
  EXPECT_EQ(uri.recompose(), "http://a///");
}

TEST(URI_normalize, path_6) {
  const sourcemeta::core::URI uri{"http://a/..///.."};
  EXPECT_EQ(uri.recompose(), "http://a//");
}

TEST(URI_normalize, path_7) {
  const sourcemeta::core::URI uri{"a/b/c/../../.."};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_normalize, path_8) {
  const sourcemeta::core::URI uri{"a/b/../../c/.."};
  EXPECT_EQ(uri.recompose(), "");
}
