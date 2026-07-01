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
