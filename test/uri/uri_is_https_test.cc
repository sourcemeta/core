#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(lowercase) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_https());
}

TEST(lowercase_with_port_and_path) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com:8443/path"};
  EXPECT_TRUE(uri.is_https());
}

TEST(uppercase) {
  const sourcemeta::core::URI uri{"HTTPS://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_https());
}

TEST(mixed_case) {
  const sourcemeta::core::URI uri{"Https://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_https());
}

TEST(localhost) {
  const sourcemeta::core::URI uri{"https://localhost:8443"};
  EXPECT_TRUE(uri.is_https());
}

TEST(ipv4_host) {
  const sourcemeta::core::URI uri{"https://127.0.0.1:8443"};
  EXPECT_TRUE(uri.is_https());
}

TEST(http) {
  const sourcemeta::core::URI uri{"http://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_https());
}

TEST(scheme_with_suffix) {
  const sourcemeta::core::URI uri{"httpss://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_https());
}

TEST(file) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_FALSE(uri.is_https());
}

TEST(mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_https());
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_https());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_https());
}

TEST(no_scheme_network_path) {
  const sourcemeta::core::URI uri{"//www.sourcemeta.com/path"};
  EXPECT_FALSE(uri.is_https());
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.is_https());
}

TEST(empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_https());
}
