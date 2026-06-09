#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_has_same_authority, same_host_same_port) {
  const sourcemeta::core::URI left{"http://example.com:8080/foo"};
  const sourcemeta::core::URI right{"http://example.com:8080/bar"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, different_host) {
  const sourcemeta::core::URI left{"http://example.com/foo"};
  const sourcemeta::core::URI right{"http://other.example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, different_port) {
  const sourcemeta::core::URI left{"http://example.com:8080/foo"};
  const sourcemeta::core::URI right{"http://example.com:9090/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, scheme_is_not_part_of_authority) {
  const sourcemeta::core::URI left{"https://example.com/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, host_case_sensitive_without_canonicalize) {
  const sourcemeta::core::URI left{"http://Example.COM/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, host_case_insensitive_after_canonicalize) {
  sourcemeta::core::URI left{"http://Example.COM/foo"};
  sourcemeta::core::URI right{"http://EXAMPLE.com/bar"};
  left.canonicalize();
  right.canonicalize();
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, same_userinfo) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://alice@example.com/bar"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, different_userinfo) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://bob@example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, userinfo_vs_none) {
  const sourcemeta::core::URI left{"http://alice@example.com/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, both_relative_no_authority) {
  const sourcemeta::core::URI left{"foo/bar"};
  const sourcemeta::core::URI right{"baz/qux"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, urn_vs_urn) {
  const sourcemeta::core::URI left{"urn:example:one"};
  const sourcemeta::core::URI right{"urn:example:two"};
  EXPECT_TRUE(left.has_same_authority(right));
}

TEST(URI_has_same_authority, default_port_vs_explicit) {
  const sourcemeta::core::URI left{"http://example.com:80/foo"};
  const sourcemeta::core::URI right{"http://example.com/foo"};
  EXPECT_FALSE(left.has_same_authority(right));
}
