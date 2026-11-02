#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(name_only) {
  const sourcemeta::core::URI uri{"http://localhost"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_with_port) {
  const sourcemeta::core::URI uri{"http://localhost:8000"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_with_path) {
  const sourcemeta::core::URI uri{"http://localhost/callback"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_with_userinfo) {
  const sourcemeta::core::URI uri{"http://user:pass@localhost/secure"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"http://localhost/search?q=test#section"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_https_scheme) {
  const sourcemeta::core::URI uri{"https://localhost"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_no_scheme) {
  const sourcemeta::core::URI uri{"//localhost/path"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_uppercase) {
  const sourcemeta::core::URI uri{"http://LOCALHOST"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_mixed_case) {
  const sourcemeta::core::URI uri{"http://LocalHost:8000"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(name_absolute_form) {
  const sourcemeta::core::URI uri{"http://localhost."};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(subdomain) {
  const sourcemeta::core::URI uri{"http://foo.localhost"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(subdomain_with_port) {
  const sourcemeta::core::URI uri{"http://foo.localhost:8000"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(subdomain_nested) {
  const sourcemeta::core::URI uri{"http://foo.bar.localhost"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(subdomain_uppercase) {
  const sourcemeta::core::URI uri{"http://FOO.LOCALHOST"};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(subdomain_absolute_form) {
  const sourcemeta::core::URI uri{"http://foo.localhost."};
  EXPECT_TRUE(uri.is_localhost());
}

TEST(prefix_of_domain) {
  const sourcemeta::core::URI uri{"http://localhost.evil.com"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(name_with_suffix) {
  const sourcemeta::core::URI uri{"http://localhosts"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(name_with_prefix) {
  const sourcemeta::core::URI uri{"http://notlocalhost"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(name_inside_label) {
  const sourcemeta::core::URI uri{"http://alocalhostb.example.com"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(name_as_leading_label) {
  const sourcemeta::core::URI uri{"http://localhost.example.com"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(ipv4_loopback) {
  const sourcemeta::core::URI uri{"http://127.0.0.1:8000"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(ipv6_loopback) {
  const sourcemeta::core::URI uri{"http://[::1]:8000"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(ipvfuture_ending_in_name) {
  const sourcemeta::core::URI uri{"http://[v1.localhost]"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(ipvfuture_name_only_label) {
  const sourcemeta::core::URI uri{"http://[v1.foo.localhost]"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(domain) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(relative_starting_with_name) {
  const sourcemeta::core::URI uri{"localhost/path"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(file_empty_host) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_FALSE(uri.is_localhost());
}

TEST(empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_localhost());
}
