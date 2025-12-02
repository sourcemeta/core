#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, pathname_name_optional_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?"};
  const auto result{pattern.pathname.match("/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar");
}

TEST(URLPattern_match, pathname_name_optional_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_name_optional_with_prefix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version?"};
  const auto result{pattern.pathname.match("/api/v1")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_match, pathname_name_optional_with_prefix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version?"};
  const auto result{pattern.pathname.match("/api")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_name_optional_with_suffix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id?/profile"};
  const auto result{pattern.pathname.match("/123/profile")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
}

TEST(URLPattern_match, pathname_name_optional_with_suffix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id?/profile"};
  const auto result{pattern.pathname.match("/profile")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_multiple_name_optional_all_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?/:bar?"};
  const auto result{pattern.pathname.match("/first/second")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "first");
  EXPECT_EQ(result.value().at(1), "second");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "first");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "second");
}

TEST(URLPattern_match, pathname_multiple_name_optional_all_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo?/:bar?"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_name_optional_mixed_required_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/users/:id/:action?"};
  const auto result{pattern.pathname.match("/users/42/edit")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "42");
  EXPECT_EQ(result.value().at(1), "edit");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "42");
  EXPECT_TRUE(result.value().at("action").has_value());
  EXPECT_EQ(result.value().at("action").value(), "edit");
}

TEST(URLPattern_match, pathname_name_optional_mixed_required_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/users/:id/:action?"};
  const auto result{pattern.pathname.match("/users/42")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "42");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "42");
}

TEST(URLPattern_match, pathname_name_multiple_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+"};
  const auto result{pattern.pathname.match("/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar");
}

TEST(URLPattern_match, pathname_name_multiple_two_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+"};
  const auto result{pattern.pathname.match("/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar/baz");
}

TEST(URLPattern_match, pathname_name_multiple_three_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+"};
  const auto result{pattern.pathname.match("/bar/baz/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz/qux");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar/baz/qux");
}

TEST(URLPattern_match, pathname_name_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:path+"};
  const auto result{pattern.pathname.match("/api/v1/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1/users");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "v1/users");
}

TEST(URLPattern_match, pathname_name_multiple_empty_fails) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo+"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_name_multiple_with_required_before) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id/:path+"};
  const auto result{pattern.pathname.match("/123/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "123");
  EXPECT_EQ(result.value().at(1), "a/b/c");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "a/b/c");
}

TEST(URLPattern_match, pathname_name_multiple_mixed_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/files/:path+"};
  const auto result{pattern.pathname.match("/api/v1/files/docs/readme.md")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "docs/readme.md");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "docs/readme.md");
}

TEST(URLPattern_match, pathname_name_asterisk_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_name_asterisk_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*"};
  const auto result{pattern.pathname.match("/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar");
}

TEST(URLPattern_match, pathname_name_asterisk_two_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*"};
  const auto result{pattern.pathname.match("/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar/baz");
}

TEST(URLPattern_match, pathname_name_asterisk_three_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo*"};
  const auto result{pattern.pathname.match("/bar/baz/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz/qux");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar/baz/qux");
}

TEST(URLPattern_match, pathname_name_asterisk_with_prefix_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:path*"};
  const auto result{pattern.pathname.match("/api")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_pathname, match_name_asterisk_with_prefix_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:path*"};
  const auto result{pattern.pathname.match("/api/v1/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1/users");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "v1/users");
}

TEST(URLPattern_pathname,
     match_name_asterisk_with_required_before_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id/:path*"};
  const auto result{pattern.pathname.match("/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
}

TEST(URLPattern_pathname,
     match_name_asterisk_with_required_before_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id/:path*"};
  const auto result{pattern.pathname.match("/123/a/b/c")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "123");
  EXPECT_EQ(result.value().at(1), "a/b/c");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "a/b/c");
}

TEST(URLPattern_match, pathname_name_asterisk_mixed_tokens_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/files/:path*"};
  const auto result{pattern.pathname.match("/api/v1/files")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_pathname, match_name_asterisk_mixed_tokens_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/files/:path*"};
  const auto result{pattern.pathname.match("/api/v1/files/docs/readme.md")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "docs/readme.md");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "docs/readme.md");
}

TEST(URLPattern_match, pathname_name_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:foo(.*)"};
  const auto result{pattern.pathname.match("/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
  EXPECT_TRUE(result.value().at("foo").has_value());
  EXPECT_EQ(result.value().at("foo").value(), "bar");
}

TEST(URLPattern_match, pathname_name_regex_digits_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id(\\d+)"};
  const auto result{pattern.pathname.match("/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
}

TEST(URLPattern_match, pathname_name_regex_digits_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:id(\\d+)"};
  const auto result{pattern.pathname.match("/abc")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_name_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version(v\\d+)"};
  const auto result{pattern.pathname.match("/api/v2")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v2");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v2");
}

TEST(URLPattern_match, pathname_name_regex_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/:category(\\w+)/:id(\\d+)"};
  const auto result{pattern.pathname.match("/products/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "products");
  EXPECT_EQ(result.value().at(1), "123");
  EXPECT_TRUE(result.value().at("category").has_value());
  EXPECT_EQ(result.value().at("category").value(), "products");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
}

TEST(URLPattern_match, pathname_name_regex_mixed_tokens) {
  const sourcemeta::core::URLPattern pattern{
      .pathname = "/api/:version(v\\d+)/users/:id(\\d+)"};
  const auto result{pattern.pathname.match("/api/v1/users/42")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "42");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "42");
}

TEST(URLPattern_match, pathname_regex_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)"};
  const auto result{pattern.pathname.match("/anything")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "anything");
}

TEST(URLPattern_match, pathname_regex_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
}

TEST(URLPattern_match, pathname_regex_digits_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)"};
  const auto result{pattern.pathname.match("/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
}

TEST(URLPattern_match, pathname_regex_digits_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)"};
  const auto result{pattern.pathname.match("/abc")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_complex) {
  const sourcemeta::core::URLPattern pattern{
      .pathname = "/api/:version/(\\d+)/details"};
  const auto result{pattern.pathname.match("/api/v1/456/details")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "456");
}

TEST(URLPattern_match, pathname_regex_fail_pattern_mismatch) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(foo|bar)"};
  const auto result{pattern.pathname.match("/baz")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_optional_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)?"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_regex_optional_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)?"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_regex_optional_with_prefix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/(\\d+)?"};
  const auto result{pattern.pathname.match("/api/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
}

TEST(URLPattern_match, pathname_regex_optional_with_prefix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/(\\d+)?"};
  const auto result{pattern.pathname.match("/api")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_regex_optional_with_suffix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)?/users"};
  const auto result{pattern.pathname.match("/123/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123");
}

TEST(URLPattern_match, pathname_regex_optional_with_suffix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)?/users"};
  const auto result{pattern.pathname.match("/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_regex_optional_mixed_with_required) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/(\\d+)?/users"};
  const auto result{pattern.pathname.match("/api/v1/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_match, pathname_regex_multiple_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)+"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_regex_multiple_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)+"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar/baz");
}

TEST(URLPattern_match, pathname_regex_multiple_fails_empty) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)+"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)+"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
}

TEST(URLPattern_match, pathname_regex_multiple_with_prefix_fails) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)+"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_multiple_pattern_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)+"};
  const auto result{pattern.pathname.match("/123/456")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123/456");
}

TEST(URLPattern_match, pathname_regex_multiple_pattern_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)+"};
  const auto result{pattern.pathname.match("/abc")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_asterisk_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)*"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_regex_asterisk_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)*"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_regex_asterisk_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(.*)*"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar/baz");
}

TEST(URLPattern_match, pathname_regex_asterisk_with_prefix_zero) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)*"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_regex_asterisk_with_prefix_single) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)*"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
}

TEST(URLPattern_match, pathname_regex_asterisk_with_prefix_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/(.*)*"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
}

TEST(URLPattern_match, pathname_regex_asterisk_pattern_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)*"};
  const auto result{pattern.pathname.match("/123/456")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "123/456");
}

TEST(URLPattern_match, pathname_regex_asterisk_pattern_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)*"};
  const auto result{pattern.pathname.match("/abc")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_regex_asterisk_pattern_zero) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/(\\d+)*"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_name_basic) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/:bar"};
  const auto result{pattern.pathname.match("/foo/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "baz");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "baz");
}

TEST(URLPattern_match, pathname_name_too_few_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/:bar"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_multiple_names) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/users/:id"};
  const auto result{pattern.pathname.match("/api/v1/users/123")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "123");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
  EXPECT_TRUE(result.value().at("id").has_value());
  EXPECT_EQ(result.value().at("id").value(), "123");
}

TEST(URLPattern_match, pathname_asterisk_alone) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*"};
  const auto result{pattern.pathname.match("/anything")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "anything");
}

TEST(URLPattern_match, pathname_bare_asterisk_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "*"};
  const auto result{pattern.pathname.match("anything")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "anything");
}

TEST(URLPattern_match, pathname_bare_asterisk_match_with_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "*"};
  const auto result{pattern.pathname.match("foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar");
}

TEST(URLPattern_match, pathname_bare_name_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "foo");
}

TEST(URLPattern_match, pathname_bare_name_match_with_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path"};
  const auto result{pattern.pathname.match("foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar/baz");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "foo/bar/baz");
}

TEST(URLPattern_match, pathname_bare_char_exact_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "foo"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_bare_char_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "foo"};
  const auto result{pattern.pathname.match("bar")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_bare_regex_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "(foo|bar)"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_bare_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "(foo|bar)"};
  const auto result{pattern.pathname.match("baz")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_bare_name_regex_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path(foo|bar)"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
  EXPECT_TRUE(result.value().at("path").has_value());
  EXPECT_EQ(result.value().at("path").value(), "foo");
}

TEST(URLPattern_match, pathname_bare_name_regex_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = ":path(foo|bar)"};
  const auto result{pattern.pathname.match("baz")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_bare_group_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "{foo}"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_bare_group_no_match) {
  const sourcemeta::core::URLPattern pattern{.pathname = "{foo}"};
  const auto result{pattern.pathname.match("bar")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_asterisk_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*"};
  const auto result{pattern.pathname.match("/static/image.png")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "image.png");
}

TEST(URLPattern_match, pathname_asterisk_with_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*/foo"};
  const auto result{pattern.pathname.match("/anything/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "anything");
}

TEST(URLPattern_match, pathname_multiple_asterisks) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*/*"};
  const auto result{pattern.pathname.match("/first/second")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "first");
  EXPECT_EQ(result.value().at(1), "second");
}

TEST(URLPattern_match, pathname_asterisk_optional_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_asterisk_optional_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_asterisk_optional_with_prefix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*?"};
  const auto result{pattern.pathname.match("/static/image.png")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "image.png");
}

TEST(URLPattern_match, pathname_asterisk_optional_with_prefix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*?"};
  const auto result{pattern.pathname.match("/static")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_asterisk_optional_with_suffix_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?/foo"};
  const auto result{pattern.pathname.match("/anything/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "anything");
}

TEST(URLPattern_match, pathname_asterisk_optional_with_suffix_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*?/foo"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_asterisk_optional_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname =
                                                 "/api/:version/*?/users"};
  const auto result{pattern.pathname.match("/api/v1/users")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_match, pathname_asterisk_multiple_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*+"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_asterisk_multiple_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*+"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar/baz");
}

TEST(URLPattern_match, pathname_asterisk_multiple_fails_empty) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*+"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_asterisk_multiple_with_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
}

TEST(URLPattern_match, pathname_asterisk_multiple_with_prefix_fails) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_asterisk_multiple_with_prefix_single) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/*+"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
}

TEST(URLPattern_match, pathname_asterisk_multiple_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version/*+"};
  const auto result{pattern.pathname.match("/api/v1/x/y/z")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "x/y/z");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_match, pathname_asterisk_asterisk_zero_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/**"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_asterisk_asterisk_single_segment) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/**"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo");
}

TEST(URLPattern_match, pathname_asterisk_asterisk_multiple_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/**"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "foo/bar/baz");
}

TEST(URLPattern_match, pathname_asterisk_asterisk_with_prefix_zero) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_asterisk_asterisk_with_prefix_single) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar");
}

TEST(URLPattern_match, pathname_asterisk_asterisk_with_prefix_multiple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/**"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "bar/baz");
}

TEST(URLPattern_match, pathname_asterisk_asterisk_mixed) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version/**"};
  const auto result{pattern.pathname.match("/api/v1/x/y/z")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "v1");
  EXPECT_EQ(result.value().at(1), "x/y/z");
  EXPECT_TRUE(result.value().at("version").has_value());
  EXPECT_EQ(result.value().at("version").value(), "v1");
}

TEST(URLPattern_match, pathname_all_token_types) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/:bar/*"};
  const auto result{pattern.pathname.match("/foo/baz/qux")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "baz");
  EXPECT_EQ(result.value().at(1), "qux");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "baz");
}

TEST(URLPattern_match, pathname_complex_pattern) {
  const sourcemeta::core::URLPattern pattern{
      .pathname = "/api/:version/*/users/:id/profile"};
  const auto result{pattern.pathname.match("/api/v2/public/users/456/profile")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 3);
  EXPECT_EQ(result.value().at(0), "v2");
  EXPECT_EQ(result.value().at(1), "public");
  EXPECT_EQ(result.value().at(2), "456");
}

TEST(URLPattern_match, pathname_root_path) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/"};
  const auto result{pattern.pathname.match("/")};
  EXPECT_TRUE(result.has_value());
}

TEST(URLPattern_match, pathname_simple_char_token) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
}

TEST(URLPattern_match, pathname_multiple_char_tokens) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/bar/baz"};
  const auto result{pattern.pathname.match("/foo/bar/baz")};
  EXPECT_TRUE(result.has_value());
}

TEST(URLPattern_match, pathname_empty_segment_pattern) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo//bar"};
  const auto result{pattern.pathname.match("/foo//bar")};
  EXPECT_TRUE(result.has_value());
}

TEST(URLPattern_match, pathname_empty_segment_name) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo//:bar"};
  const auto result{pattern.pathname.match("/foo//baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "baz");
}

TEST(URLPattern_match, pathname_fail_wrong_char) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/bar"};
  const auto result{pattern.pathname.match("/foo/baz")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_fail_too_many_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_fail_too_few_segments) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/bar"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_fail_no_leading_slash) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo"};
  const auto result{pattern.pathname.match("foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_fail_empty_input) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo"};
  const auto result{pattern.pathname.match("")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_name_with_special_chars_in_value) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/:filename"};
  const auto result{pattern.pathname.match("/image-2024.png")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "image-2024.png");
}

TEST(URLPattern_match, pathname_asterisk_with_special_chars) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/*"};
  const auto result{pattern.pathname.match("/path-with_special.chars")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "path-with_special.chars");
}

TEST(URLPattern_match, pathname_trailing_slash_pattern) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/"};
  const auto result{pattern.pathname.match("/foo/")};
  EXPECT_TRUE(result.has_value());
}

TEST(URLPattern_match, pathname_trailing_slash_mismatch) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_input_trailing_slash_mismatch) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo/bar"};
  const auto result{pattern.pathname.match("/foo/bar/")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_group_simple) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_simple_mismatch) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_group_optional_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}?"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_optional_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}?"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_multiple_single) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}+"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_multiple_twice) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}+"};
  const auto result{pattern.pathname.match("/foo/bar/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_multiple_fails_empty) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}+"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_group_asterisk_zero) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}*"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_asterisk_once) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}*"};
  const auto result{pattern.pathname.match("/foo/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_asterisk_twice) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/bar}*"};
  const auto result{pattern.pathname.match("/foo/bar/bar")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_group_with_name) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/:bar}"};
  const auto result{pattern.pathname.match("/foo/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "baz");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "baz");
}

TEST(URLPattern_match, pathname_group_with_name_optional_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/:bar}?"};
  const auto result{pattern.pathname.match("/foo/baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "baz");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "baz");
}

TEST(URLPattern_match, pathname_group_with_name_optional_absent) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{/:bar}?"};
  const auto result{pattern.pathname.match("/foo")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_complex_segment_with_name) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/file-:name.json"};
  const auto result{pattern.pathname.match("/file-test.json")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "test");
  EXPECT_TRUE(result.value().at("name").has_value());
  EXPECT_EQ(result.value().at("name").value(), "test");
}

TEST(URLPattern_match, pathname_complex_segment_mismatch_prefix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/file-:name.json"};
  const auto result{pattern.pathname.match("/doc-test.json")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_complex_segment_mismatch_suffix) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/file-:name.json"};
  const auto result{pattern.pathname.match("/file-test.txt")};
  EXPECT_FALSE(result.has_value());
}

TEST(URLPattern_match, pathname_complex_segment_multiple_names) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/v:major.:minor"};
  const auto result{pattern.pathname.match("/v1.2")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0), "1");
  EXPECT_EQ(result.value().at(1), "2");
  EXPECT_TRUE(result.value().at("major").has_value());
  EXPECT_EQ(result.value().at("major").value(), "1");
  EXPECT_TRUE(result.value().at("minor").has_value());
  EXPECT_EQ(result.value().at("minor").value(), "2");
}

TEST(URLPattern_match, pathname_complex_segment_with_group) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo-:bar{baz}+"};
  const auto result{pattern.pathname.match("/foo-test baz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "test ");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "test ");
}

TEST(URLPattern_match, pathname_complex_segment_group_first) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{:bar}?baz"};
  const auto result{pattern.pathname.match("/foobaz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}

TEST(URLPattern_match, pathname_complex_segment_group_first_present) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo{:bar}?baz"};
  const auto result{pattern.pathname.match("/footestbaz")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 1);
  EXPECT_EQ(result.value().at(0), "test");
  EXPECT_TRUE(result.value().at("bar").has_value());
  EXPECT_EQ(result.value().at("bar").value(), "test");
}

TEST(URLPattern_match, pathname_escaped_colon_at_end) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/foo\\:"};
  const auto result{pattern.pathname.match("/foo:")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 0);
}
