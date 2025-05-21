#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L302

TEST(URI_parse, syntax_error_1) {
  EXPECT_THROW(sourcemeta::core::URI uri{"//[::44.1"},
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L510-L516

TEST(URI_parse, syntax_error_2) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://moo:21@moo:21@moo/"},
               sourcemeta::core::URIParseError);
}

TEST(URI_parse, syntax_error_3) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://moo:21@moo:21@moo:21/"},
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L2180

TEST(URI_parse, syntax_error_4) {
  EXPECT_THROW(sourcemeta::core::URI uri{"http://[vA.123456"}, // missing "]"
               sourcemeta::core::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L315

TEST(URI_parse, success_1) {
  sourcemeta::core::URI uri{
      "//user:pass@[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "user:pass@[::1]:80/segment/index.html?query#frag");
}

TEST(URI_parse, success_2) {
  sourcemeta::core::URI uri{"http://[::1]:80/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://[::1]:80/segment/index.html?query#frag");
}

TEST(URI_parse, success_3) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]/segment/index.html?query#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]/segment/index.html?query#frag");
}

TEST(URI_parse, success_4) {
  sourcemeta::core::URI uri{"http://user:pass@[::1]:80?query#frag"};
  EXPECT_EQ(uri.recompose(), "http://user:pass@[::1]:80?query#frag");
}

TEST(URI_parse, success_5) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html#frag"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html#frag");
}

TEST(URI_parse, success_6) {
  sourcemeta::core::URI uri{
      "http://user:pass@[::1]:80/segment/index.html?query"};
  EXPECT_EQ(uri.recompose(),
            "http://user:pass@[::1]:80/segment/index.html?query");
}

TEST(URI_parse, success_7) {
  sourcemeta::core::URI uri{"ftp://host:21/gnu/"};
  EXPECT_EQ(uri.recompose(), "ftp://host:21/gnu/");
}

TEST(URI_parse, relative_1) {
  sourcemeta::core::URI uri{"one/two/three"};
  EXPECT_EQ(uri.recompose(), "one/two/three");
}

TEST(URI_parse, relative_2) {
  sourcemeta::core::URI uri{"/one/two/three"};
  EXPECT_EQ(uri.recompose(), "/one/two/three");
}

TEST(URI_parse, relative_3) {
  sourcemeta::core::URI uri{"//user:pass@localhost/one/two/three"};
  EXPECT_EQ(uri.recompose(), "user:pass@localhost/one/two/three");
}

TEST(URI_parse, real_life_1) {
  sourcemeta::core::URI uri{"http://sourceforge.net/projects/uriparser/"};
  EXPECT_EQ(uri.recompose(), "http://sourceforge.net/projects/uriparser/");
}

TEST(URI_parse, real_life_2) {
  sourcemeta::core::URI uri{
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"};
  EXPECT_EQ(
      uri.recompose(),
      "http://sourceforge.net/project/platformdownload.php?group_id=182840");
}

TEST(URI_parse, mailto) {
  sourcemeta::core::URI uri{"mailto:test@example.com"};
  EXPECT_EQ(uri.recompose(), "mailto:test@example.com");
}

TEST(URI_parse, relative_4) {
  sourcemeta::core::URI uri{"../../"};
  EXPECT_EQ(uri.recompose(), "../../");
}

TEST(URI_parse, root_path) {
  sourcemeta::core::URI uri{"/"};
  EXPECT_EQ(uri.recompose(), "/");
}

TEST(URI_parse, empty_uri) {
  sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_parse, file_uri) {
  sourcemeta::core::URI uri{"file:///bin/bash"};
  EXPECT_EQ(uri.recompose(), "file:///bin/bash");
}

TEST(URI_parse, percent_encoding) {
  sourcemeta::core::URI uri{"http://www.example.com/name%20with%20spaces/"};
  EXPECT_EQ(uri.recompose(), "http://www.example.com/name%20with%20spaces/");
}
