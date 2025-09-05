#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <sstream>

TEST(URI_extension, absolute_single_component_path_without_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(URI_extension, absolute_single_component_path_with_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo"};
  uri.extension(".json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(URI_extension, absolute_single_component_path_with_extension) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo.xml"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(URI_extension, absolute_single_component_path_with_double_extension) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo.tar.gz"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.tar.json");
}

TEST(URI_extension, absolute_no_path) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com");
}

TEST(URI_extension, absolute_trailing_slash) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/");
}

TEST(URI_extension, absolute_trailing_slashes) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com////"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com////");
}

TEST(URI_extension, absolute_trailing_period) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo."};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json");
}

TEST(URI_extension, empty_fragment) {
  sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo#"};
  uri.extension(".json");
  EXPECT_EQ(uri.recompose(), "https://www.sourcemeta.com/foo.json#");
}

TEST(URI_extension, fragment_only) {
  sourcemeta::core::URI uri{"#foo"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI_extension, relative_multi_component_1) {
  sourcemeta::core::URI uri{"/foo/bar"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "/foo/bar.json");
}

TEST(URI_extension, relative_multi_component_2) {
  sourcemeta::core::URI uri{"foo/bar"};
  uri.extension("json");
  EXPECT_EQ(uri.recompose(), "foo/bar.json");
}
