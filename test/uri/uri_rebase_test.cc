#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(absolute_to_relative_match) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "/qux/bar/baz");
}

TEST(absolute_to_relative_mismatch) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://another.com/test"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://example.com/foo/bar/baz");
}

TEST(absolute_to_relative_equal) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "/qux");
}

TEST(rvalue_new_base) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, std::move(new_base));
  EXPECT_EQ(uri.recompose(), "/qux/bar/baz");
}

TEST(rvalue_new_base_temporary) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  uri.rebase(base, sourcemeta::core::URI{"/qux"});
  EXPECT_EQ(uri.recompose(), "/qux/bar/baz");
}

TEST(rvalue_new_base_with_scheme_and_authority) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  uri.rebase(base, sourcemeta::core::URI{"https://other.example.com/qux"});
  EXPECT_EQ(uri.recompose(), "https://other.example.com/qux/bar/baz");
}

TEST(iri_flag_propagates_from_new_base) {
  const auto base{sourcemeta::core::URI::from_iri("https://example.com/dir/")};
  const auto new_base{
      sourcemeta::core::URI::from_iri("https://example.com/caf\xC3\xA9/")};
  sourcemeta::core::URI uri{"https://example.com/dir/file"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://example.com/caf\xC3\xA9/file");
  EXPECT_TRUE(uri.is_internationalized());
}

TEST(sibling_of_base_is_left_intact) {
  sourcemeta::core::URI uri{"https://example.com/foo/other"};
  const sourcemeta::core::URI base{"https://example.com/foo/bar"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://example.com/foo/other");
}

TEST(unrelated_directory_is_left_intact) {
  sourcemeta::core::URI uri{"https://example.com/qux/x"};
  const sourcemeta::core::URI base{"https://example.com/foo/bar"};
  const sourcemeta::core::URI new_base{"/new"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://example.com/qux/x");
}

TEST(partial_segment_match_is_left_intact) {
  sourcemeta::core::URI uri{"https://example.com/foobar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://example.com/foobar/baz");
}

TEST(base_without_path_moves_whole_path) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const sourcemeta::core::URI base{"https://example.com"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "/qux/foo/bar");
}

TEST(different_host_is_left_intact) {
  sourcemeta::core::URI uri{"https://other.com/foo/bar"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://other.com/foo/bar");
}
