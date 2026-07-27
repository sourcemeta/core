#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

#include <sstream>

TEST(absolute_single_component_path_without_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(absolute_single_component_path_with_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo"};
  uri.extension(".json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(absolute_single_component_path_with_extension) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo.xml"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(absolute_single_component_path_with_double_extension) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo.tar.gz"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.tar.json");
}

TEST(absolute_no_path) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com");
}

TEST(absolute_trailing_slash) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/");
}

TEST(absolute_trailing_slashes) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com////"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com////");
}

TEST(absolute_trailing_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo."};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(empty_fragment) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo#"};
  uri.extension(".json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json#");
}

TEST(fragment_only) {
  sourcemeta::core::URI uri{"#foo"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(relative_multi_component_1) {
  sourcemeta::core::URI uri{"/foo/bar"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "/foo/bar.json");
}

TEST(relative_multi_component_2) {
  sourcemeta::core::URI uri{"foo/bar"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "foo/bar.json");
}

TEST(path_with_dot_in_directory) {
  sourcemeta::core::URI uri{"https://example.com/foo.bar/baz"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo.bar/baz.json");
}

TEST(path_with_multiple_dots_in_filename) {
  sourcemeta::core::URI uri{"https://example.com/foo.bar.baz.qux"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo.bar.baz.json");
}

TEST(empty_extension_removes_existing) {
  sourcemeta::core::URI uri{"https://example.com/foo.json"};
  uri.extension("");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(path_with_query_string) {
  sourcemeta::core::URI uri{"https://example.com/foo?query=value"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo.json?query=value");
}

TEST(path_ending_with_dot_in_directory) {
  sourcemeta::core::URI uri{"https://example.com/foo./bar"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo./bar.json");
}

TEST(iri_unicode_path) {
  auto uri{sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9")};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9.json");
}
