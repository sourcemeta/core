#include <gtest/gtest.h>
#include <sourcemeta/jsontoolkit/uri.h>

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L302

TEST(URI_parse, syntax_error_1) {
  EXPECT_THROW(sourcemeta::jsontoolkit::URI uri{"//[::44.1"},
               sourcemeta::jsontoolkit::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L510-L516

TEST(URI_parse, syntax_error_2) {
  EXPECT_THROW(sourcemeta::jsontoolkit::URI uri{"http://moo:21@moo:21@moo/"},
               sourcemeta::jsontoolkit::URIParseError);
}

TEST(URI_parse, syntax_error_3) {
  EXPECT_THROW(sourcemeta::jsontoolkit::URI uri{"http://moo:21@moo:21@moo:21/"},
               sourcemeta::jsontoolkit::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L2180

TEST(URI_parse, syntax_error_4) {
  EXPECT_THROW(
      sourcemeta::jsontoolkit::URI uri{"http://[vA.123456"}, // missing "]"
      sourcemeta::jsontoolkit::URIParseError);
}

// Inspired from
// https://github.com/uriparser/uriparser/blob/bf0174e83164a4659c51c135399478bec389eafa/test/test.cpp#L315

TEST(URI_parse, success_1) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "//user:pass@[::1]:80/segment/index.html?query#frag"});
}

TEST(URI_parse, success_2) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://[::1]:80/segment/index.html?query#frag"});
}

TEST(URI_parse, success_3) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://user:pass@[::1]/segment/index.html?query#frag"});
}

TEST(URI_parse, success_4) {
  EXPECT_NO_THROW(
      sourcemeta::jsontoolkit::URI uri{"http://user:pass@[::1]:80?query#frag"});
}

TEST(URI_parse, success_5) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://user:pass@[::1]:80/segment/index.html#frag"});
}

TEST(URI_parse, success_6) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://user:pass@[::1]:80/segment/index.html?query"});
}

TEST(URI_parse, success_7) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"ftp://host:21/gnu/"});
}

TEST(URI_parse, relative_1) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"one/two/three"});
}

TEST(URI_parse, relative_2) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"/one/two/three"});
}

TEST(URI_parse, relative_3) {
  EXPECT_NO_THROW(
      sourcemeta::jsontoolkit::URI uri{"//user:pass@localhost/one/two/three"});
}

TEST(URI_parse, real_life_1) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://sourceforge.net/projects/uriparser/"});
}

TEST(URI_parse, real_life_2) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://sourceforge.net/project/platformdownload.php?group_id=182840"});
}

TEST(URI_parse, real_life_3) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"mailto:test@example.com"});
}

TEST(URI_parse, relative_4) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"../../"});
}

TEST(URI_parse, root_path) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"/"});
}

TEST(URI_parse, empty_uri) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{""});
}

TEST(URI_parse, file_uri) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{"file:///bin/bash"});
}

TEST(URI_parse, percent_encoding) {
  EXPECT_NO_THROW(sourcemeta::jsontoolkit::URI uri{
      "http://www.example.com/name%20with%20spaces/"});
}
