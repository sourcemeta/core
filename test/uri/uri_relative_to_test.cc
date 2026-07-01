#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(absolute_absolute_base_true_1) {
  const sourcemeta::core::URI base{"https://www.example.com"};
  sourcemeta::core::URI uri{"https://www.example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(absolute_absolute_base_true_2) {
  const sourcemeta::core::URI base{"https://www.example.com/foo"};
  sourcemeta::core::URI uri{"https://www.example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "");
}

TEST(absolute_absolute_base_true_3) {
  const sourcemeta::core::URI base{"https://www.example.com/foo"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar?q=1"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "bar?q=1");
}

TEST(absolute_absolute_base_true_4) {
  const sourcemeta::core::URI base{"https://www.example.com/foo"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "bar#baz");
}

TEST(absolute_absolute_base_false_1) {
  const sourcemeta::core::URI base{"https://www.example.com/foo"};
  sourcemeta::core::URI uri{"http://www.example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "http://www.example.com/foo");
}

TEST(absolute_absolute_base_false_2) {
  const sourcemeta::core::URI base{"https://www.example.com/foo"};
  sourcemeta::core::URI uri{"https://www.example.com"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://www.example.com");
}

TEST(absolute_absolute_base_false_3) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://www.example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../foo");
}

TEST(absolute_absolute_base_false_4) {
  const sourcemeta::core::URI base{"https://foo.com"};
  sourcemeta::core::URI uri{"https://bar.com"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://bar.com");
}

TEST(absolute_absolute_base_false_different_ports) {
  const sourcemeta::core::URI base{"http://localhost:8000"};
  sourcemeta::core::URI uri{"http://localhost:9000/schemas/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "http://localhost:9000/schemas/test.json");
}

TEST(absolute_absolute_base_false_different_userinfo) {
  const sourcemeta::core::URI base{"https://alice@example.com/foo"};
  sourcemeta::core::URI uri{"https://bob@example.com/foo/bar"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://bob@example.com/foo/bar");
}

TEST(absolute_absolute_base_false_userinfo_vs_none) {
  const sourcemeta::core::URI base{"https://example.com/foo"};
  sourcemeta::core::URI uri{"https://alice@example.com/foo/bar"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://alice@example.com/foo/bar");
}

TEST(absolute_relative_1) {
  const sourcemeta::core::URI base{"https://www.example.com"};
  sourcemeta::core::URI uri{"foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(absolute_relative_2) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar/baz"};
  sourcemeta::core::URI uri{"/qux"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "/qux");
}

TEST(relative_absolute_1) {
  const sourcemeta::core::URI base{"foo/bar"};
  sourcemeta::core::URI uri{"https://www.example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://www.example.com/foo");
}

TEST(relative_relative_1) {
  const sourcemeta::core::URI base{"foo/bar"};
  sourcemeta::core::URI uri{"foo/bar/baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "foo/bar/baz");
}

TEST(urn_1) {
  const sourcemeta::core::URI base{"schema:"};
  sourcemeta::core::URI uri{"schema:myschema"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "myschema");
}

TEST(absolute_absolute_trailing_slash) {
  const sourcemeta::core::URI base{
      "https://github.com/apis-json/api-json/blob/develop/spec"};
  sourcemeta::core::URI uri{
      "https://github.com/apis-json/api-json/blob/develop/spec/"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "spec/");
}

TEST(target_is_prefix_of_base_parent) {
  const sourcemeta::core::URI base{"https://example.com/foo/bar/baz"};
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../../foo");
}

TEST(target_is_one_level_up_at_root) {
  const sourcemeta::core::URI base{"https://example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../foo");
}

TEST(target_is_root) {
  const sourcemeta::core::URI base{"https://example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://example.com/"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../");
}

TEST(base_ends_with_slash) {
  const sourcemeta::core::URI base{"https://example.com/foo/"};
  sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "bar");
}

TEST(base_root_only) {
  const sourcemeta::core::URI base{"https://example.com/"};
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(sibling_paths_same_directory) {
  const sourcemeta::core::URI base{"https://example.com/schemas/bar.json"};
  sourcemeta::core::URI uri{"https://example.com/schemas/foo.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "foo.json");
}

TEST(double_slash_with_trailing_slash) {
  const sourcemeta::core::URI base{"https://example.com/slash/"};
  sourcemeta::core::URI uri{"https://example.com/slash/file.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "file.json");
}

TEST(different_directories_same_host_needs_dotdot) {
  const sourcemeta::core::URI base{
      "https://example.com/schemas/with-rebase-same-host.json"};
  sourcemeta::core::URI uri{"https://example.com/bundling/single"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../bundling/single");
}

TEST(different_directories_same_host_needs_dotdot_2) {
  const sourcemeta::core::URI base{"https://example.com/foo/bar/baz.json"};
  sourcemeta::core::URI uri{"https://example.com/qux/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../../qux/test.json");
}

TEST(different_directories_same_host_needs_dotdot_3) {
  const sourcemeta::core::URI base{"https://example.com/a/b/c.json"};
  sourcemeta::core::URI uri{"https://example.com/d.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../../d.json");
}

TEST(file_same_directory) {
  const sourcemeta::core::URI base{"file:///home/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///home/user/schemas/other.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "other.json");
}

TEST(file_subdirectory) {
  const sourcemeta::core::URI base{"file:///home/user/schemas"};
  sourcemeta::core::URI uri{"file:///home/user/schemas/sub/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "sub/test.json");
}

TEST(file_parent_directory) {
  const sourcemeta::core::URI base{"file:///home/user/schemas/sub/base.json"};
  sourcemeta::core::URI uri{"file:///home/user/schemas/other.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../other.json");
}

TEST(file_different_root) {
  const sourcemeta::core::URI base{"file:///home/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///var/data/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../../../var/data/test.json");
}

TEST(file_same_file) {
  const sourcemeta::core::URI base{"file:///home/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///home/user/schemas/base.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "");
}

TEST(file_with_fragment) {
  const sourcemeta::core::URI base{"file:///home/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///home/user/schemas/other.json#/defs/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "other.json#/defs/foo");
}

TEST(file_windows_same_directory) {
  const sourcemeta::core::URI base{"file:///C:/Users/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///C:/Users/user/schemas/other.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "other.json");
}

TEST(file_windows_subdirectory) {
  const sourcemeta::core::URI base{"file:///C:/Users/user/schemas"};
  sourcemeta::core::URI uri{"file:///C:/Users/user/schemas/sub/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "sub/test.json");
}

TEST(file_windows_parent_directory) {
  const sourcemeta::core::URI base{
      "file:///C:/Users/user/schemas/sub/base.json"};
  sourcemeta::core::URI uri{"file:///C:/Users/user/schemas/other.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../other.json");
}

TEST(file_windows_different_drive) {
  const sourcemeta::core::URI base{"file:///C:/Users/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///D:/Data/test.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "../../../../D:/Data/test.json");
}

TEST(file_windows_same_file) {
  const sourcemeta::core::URI base{"file:///C:/Users/user/schemas/base.json"};
  sourcemeta::core::URI uri{"file:///C:/Users/user/schemas/base.json"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "");
}

TEST(file_windows_with_fragment) {
  const sourcemeta::core::URI base{"file:///C:/Users/user/schemas/base.json"};
  sourcemeta::core::URI uri{
      "file:///C:/Users/user/schemas/other.json#/defs/foo"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "other.json#/defs/foo");
}

TEST(iri_preserves_ucschar) {
  const auto base{sourcemeta::core::URI::from_iri("https://example.com/dir/")};
  auto uri{
      sourcemeta::core::URI::from_iri("https://example.com/dir/caf\xC3\xA9")};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "caf\xC3\xA9");
}

TEST(same_path_only_query_differs) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar?q=1"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "?q=1");
}

TEST(same_path_only_fragment_differs) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "#baz");
}

TEST(same_path_query_and_fragment_differ) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar?q=1#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "?q=1#baz");
}

TEST(same_path_base_query_target_none) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "bar");
}

TEST(same_path_base_query_target_fragment_only) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "bar#baz");
}

TEST(same_path_base_query_target_other_query) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/bar?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/bar?q=2"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "?q=2");
}

TEST(same_directory_path_base_query_target_none) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "./");
}

TEST(same_directory_path_base_query_target_fragment) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "./#baz");
}

TEST(same_directory_path_target_query) {
  const sourcemeta::core::URI base{"https://www.example.com/foo/?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com/foo/?q=2"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "?q=2");
}

TEST(authority_no_path_only_fragment_differs) {
  const sourcemeta::core::URI base{"https://www.example.com"};
  sourcemeta::core::URI uri{"https://www.example.com#baz"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "#baz");
}

TEST(authority_no_path_only_query_differs) {
  const sourcemeta::core::URI base{"https://www.example.com?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com?q=2"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "?q=2");
}

TEST(authority_no_path_base_query_target_none) {
  const sourcemeta::core::URI base{"https://www.example.com?q=1"};
  sourcemeta::core::URI uri{"https://www.example.com"};
  uri.relative_to(base);
  EXPECT_EQ(uri.recompose(), "https://www.example.com");
}
