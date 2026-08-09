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

TEST(identical_uri_keeps_its_query) {
  sourcemeta::core::URI uri{"https://example.com/foo?q=1"};
  const sourcemeta::core::URI base{"https://example.com/foo?q=1"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "/qux?q=1");
}

TEST(identical_uri_keeps_its_fragment) {
  sourcemeta::core::URI uri{"https://example.com/foo#frag"};
  const sourcemeta::core::URI base{"https://example.com/foo#frag"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "/qux#frag");
}

TEST(relative_uri_is_left_intact) {
  sourcemeta::core::URI uri{"foo/bar"};
  const sourcemeta::core::URI base{"foo"};
  const sourcemeta::core::URI new_base{"/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "foo/bar");
}

TEST(ipv6_new_base_keeps_its_brackets) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  const sourcemeta::core::URI new_base{"https://[::1]/qux"};
  uri.rebase(base, new_base);
  EXPECT_EQ(uri.recompose(), "https://[::1]/qux/bar");
}

TEST(rebasing_back_to_the_original_base_restores_the_uri) {
  sourcemeta::core::URI uri{"https://a.com/base/x/y"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"https://b.com/new"});
  EXPECT_EQ(uri.recompose(), "https://b.com/new/x/y");
  uri.rebase(sourcemeta::core::URI{"https://b.com/new"},
             sourcemeta::core::URI{"https://a.com/base"});
  EXPECT_EQ(uri.recompose(), "https://a.com/base/x/y");
}

TEST(rebasing_onto_the_same_base_is_a_no_op) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"https://a.com/base"});
  EXPECT_EQ(uri.recompose(), "https://a.com/base/x");
}

TEST(dot_segments_in_the_uri_are_resolved) {
  sourcemeta::core::URI uri{"https://a.com/base/./x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"/new"});
  EXPECT_EQ(uri.recompose(), "/new/x");
}

TEST(a_uri_escaping_the_base_is_left_intact) {
  sourcemeta::core::URI uri{"https://a.com/base/../etc"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"/new"});
  EXPECT_EQ(uri.recompose(), "https://a.com/base/../etc");
}

TEST(dot_segments_in_the_base_are_resolved) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base/."},
             sourcemeta::core::URI{"/new"});
  EXPECT_EQ(uri.recompose(), "/new/x");
}

TEST(a_query_on_the_new_base_is_not_carried_over) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"/new?q=1"});
  EXPECT_EQ(uri.recompose(), "/new/x");
}

TEST(an_empty_new_base_yields_the_suffix_alone) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{""});
  EXPECT_EQ(uri.recompose(), "x");
}

TEST(a_new_base_with_a_trailing_slash_does_not_double_it) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"/new/"});
  EXPECT_EQ(uri.recompose(), "/new/x");
}

TEST(a_uri_equal_to_the_base_keeps_the_new_base_trailing_slash) {
  sourcemeta::core::URI uri{"https://a.com/base"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base"},
             sourcemeta::core::URI{"/new/"});
  EXPECT_EQ(uri.recompose(), "/new/");
}

TEST(a_base_with_a_trailing_slash_matches_the_same_suffix) {
  sourcemeta::core::URI uri{"https://a.com/base/x"};
  uri.rebase(sourcemeta::core::URI{"https://a.com/base/"},
             sourcemeta::core::URI{"/new"});
  EXPECT_EQ(uri.recompose(), "/new/x");
}
