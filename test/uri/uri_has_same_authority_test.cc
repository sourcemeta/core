#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(same_host_same_port) {
  const sourcemeta::core::URI left{"http://example.com:8080/foo"};
  const sourcemeta::core::URI right{"http://example.com:8080/bar"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(different_host) {
  const sourcemeta::core::URI left{"http://example.com/foo"};
  const sourcemeta::core::URI right{"http://other.example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(different_port) {
  const sourcemeta::core::URI left{"http://example.com:8080/foo"};
  const sourcemeta::core::URI right{"http://example.com:9090/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(scheme_is_not_part_of_authority) {
  const sourcemeta::core::URI left{"https://example.com/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(host_case_sensitive_without_canonicalize) {
  const sourcemeta::core::URI left{"http://Example.COM/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(host_case_insensitive_after_canonicalize) {
  sourcemeta::core::URI left{"http://Example.COM/foo"};
  sourcemeta::core::URI right{"http://EXAMPLE.com/bar"};
  left.canonicalize();
  right.canonicalize();
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(same_userinfo) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://alice@example.com/bar"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(different_userinfo) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://bob@example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(userinfo_vs_none) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(both_relative_no_authority) {
  const sourcemeta::core::URI left{"foo/bar"};
  const sourcemeta::core::URI right{"baz/qux"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(urn_vs_urn) {
  const sourcemeta::core::URI left{"urn:example:one"};
  const sourcemeta::core::URI right{"urn:example:two"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(default_port_vs_explicit) {
  const sourcemeta::core::URI left{"http://example.com:80/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(default_port_vs_explicit_after_canonicalize) {
  sourcemeta::core::URI left{"http://example.com:80/foo"};
  sourcemeta::core::URI right{"http://example.com/foo"};
  left.canonicalize();
  right.canonicalize();
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(ipv6_short_form_vs_long_form) {
  const sourcemeta::core::URI left{"http://[::1]/foo"};
  const sourcemeta::core::URI right{"http://[0:0:0:0:0:0:0:1]/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(ipv6_same_literal) {
  const sourcemeta::core::URI left{"http://[::1]/foo"};
  const sourcemeta::core::URI right{"http://[::1]/bar"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(iri_same_unicode_host) {
  const auto left{sourcemeta::core::URI::from_iri(
      "https://\xE4\xBE\x8B\xE3\x81\x88.jp/foo")};
  const auto right{sourcemeta::core::URI::from_iri(
      "https://\xE4\xBE\x8B\xE3\x81\x88.jp/bar")};
  EXPECT_TRUE(left.has_same_authority(right));
}
