#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_resolve_from, relative_base) {
  const sourcemeta::core::URI base{"../foo"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "../baz");
}

TEST(URI_resolve_from, absolute_relative_with_slash) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"/baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(URI_resolve_from, example_1) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"../baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/baz");
}

TEST(URI_resolve_from, add_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar"};
  sourcemeta::core::URI relative{"#baz"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#baz");
}

TEST(URI_resolve_from, change_fragment) {
  const sourcemeta::core::URI base{"https://foobar.com/foo/bar#baz"};
  sourcemeta::core::URI relative{"#qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "https://foobar.com/foo/bar#qux");
}

TEST(URI_resolve_from, fragment_on_relative_path) {
  const sourcemeta::core::URI base{"foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo#/bar");
}

TEST(URI_resolve_from, base_relative_path_leading_slash) {
  const sourcemeta::core::URI base{"/foo"};
  sourcemeta::core::URI relative{"#/bar"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "/foo#/bar");
}

TEST(URI_resolve_from, relative_path_from_relative_path) {
  const sourcemeta::core::URI base{"foo/bar/baz"};
  sourcemeta::core::URI relative{"qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "foo/bar/qux");
}

// RFC 3986, inspired from
// https://cr.openjdk.org/~dfuchs/writeups/updating-uri/A Section "Resolutuon"

TEST(URI_resolve_from, rfc3986_resolve_with_relative_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI relative_path{"../../b"};
  relative_path.resolve_from(base);
  EXPECT_EQ(relative_path.recompose(), "s://h/b");
}

TEST(URI_resolve_from, rfc3986_resolve_with_empty) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI empty{""};
  empty.resolve_from(base);
  EXPECT_EQ(empty.recompose(), "s://h/a/c");
}

TEST(URI_resolve_from, rfc3986_resolve_with_query) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI query{"?x=y"};
  query.resolve_from(base);
  EXPECT_EQ(query.recompose(), "s://h/a/c?x=y");
}

TEST(URI_resolve_from, rfc3986_resolve_with_fragment) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI fragment{"#x=y"};
  fragment.resolve_from(base);
  EXPECT_EQ(fragment.recompose(), "s://h/a/c#x=y");
}

TEST(URI_resolve_from, rfc3986_resolve_with_absolute_path) {
  const sourcemeta::core::URI base{"s://h/a/c"};
  sourcemeta::core::URI absolute_path{"/././x"};
  absolute_path.resolve_from(base);
  EXPECT_EQ(absolute_path.recompose(), "s://h/x");
}

TEST(URI_resolve_from, file_1) {
  const sourcemeta::core::URI base{"file:///foo/bar/baz"};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}

TEST(URI_resolve_from, file_from_path) {
  const std::filesystem::path base_path{"/foo/bar/baz"};
  const auto base{sourcemeta::core::URI::from_path(base_path)};
  sourcemeta::core::URI relative{"./qux"};
  relative.resolve_from(base);
  EXPECT_EQ(relative.recompose(), "file:///foo/bar/qux");
}
