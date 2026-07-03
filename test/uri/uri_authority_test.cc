#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(host_only) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com");
}

TEST(host_without_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com");
}

TEST(host_with_non_default_port) {
  const sourcemeta::core::URI uri{"https://example.com:8443/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com:8443");
}

TEST(host_with_http_port) {
  const sourcemeta::core::URI uri{"http://example.com:8000/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com:8000");
}

TEST(explicit_default_port_is_kept_verbatim) {
  const sourcemeta::core::URI uri{"https://example.com:443/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com:443");
}

TEST(canonicalize_drops_the_default_port) {
  sourcemeta::core::URI uri{"https://example.com:443/foo"};
  uri.canonicalize();
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com");
}

TEST(canonicalize_keeps_a_non_default_port) {
  sourcemeta::core::URI uri{"https://example.com:8443/foo"};
  uri.canonicalize();
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "example.com:8443");
}

TEST(ipv6_host_is_bracketed) {
  const sourcemeta::core::URI uri{"https://[::1]/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "[::1]");
}

TEST(ipv6_host_with_port) {
  const sourcemeta::core::URI uri{"https://[2001:db8::1]:8443/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "[2001:db8::1]:8443");
}

TEST(ipv4_host) {
  const sourcemeta::core::URI uri{"http://192.168.1.1:8080/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "192.168.1.1:8080");
}

TEST(userinfo_and_host) {
  const sourcemeta::core::URI uri{"https://user@example.com/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "user@example.com");
}

TEST(userinfo_host_and_port) {
  const sourcemeta::core::URI uri{"https://user@example.com:8443/foo"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.authority().value(), "user@example.com:8443");
}

TEST(relative_reference_has_no_authority) {
  const sourcemeta::core::URI uri{"../foo/bar"};
  EXPECT_FALSE(uri.authority().has_value());
}

TEST(path_only_absolute_has_no_authority) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_FALSE(uri.authority().has_value());
}

TEST(urn_has_no_authority) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.authority().has_value());
}

TEST(mailto_has_no_authority) {
  const sourcemeta::core::URI uri{"mailto:joe@example.com"};
  EXPECT_FALSE(uri.authority().has_value());
}

TEST(fragment_only_has_no_authority) {
  const sourcemeta::core::URI uri{"#section"};
  EXPECT_FALSE(uri.authority().has_value());
}

TEST(matches_recompose_authority_component) {
  const sourcemeta::core::URI uri{"https://user@example.com:8443/foo?x=1#y"};
  EXPECT_TRUE(uri.authority().has_value());
  EXPECT_EQ(uri.recompose(),
            "https://" + uri.authority().value() + "/foo?x=1#y");
}
