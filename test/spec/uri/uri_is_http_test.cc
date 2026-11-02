#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(lowercase) {
  const sourcemeta::core::URI uri{"http://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_http());
}

TEST(lowercase_with_port_and_path) {
  const sourcemeta::core::URI uri{"http://www.sourcemeta.com:8000/path"};
  EXPECT_TRUE(uri.is_http());
}

TEST(uppercase) {
  const sourcemeta::core::URI uri{"HTTP://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_http());
}

TEST(mixed_case) {
  const sourcemeta::core::URI uri{"Http://www.sourcemeta.com"};
  EXPECT_TRUE(uri.is_http());
}

TEST(localhost) {
  const sourcemeta::core::URI uri{"http://localhost:8000"};
  EXPECT_TRUE(uri.is_http());
}

TEST(ipv4_host) {
  const sourcemeta::core::URI uri{"http://127.0.0.1:8000"};
  EXPECT_TRUE(uri.is_http());
}

TEST(https) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_http());
}

TEST(scheme_with_suffix) {
  const sourcemeta::core::URI uri{"httpx://www.sourcemeta.com"};
  EXPECT_FALSE(uri.is_http());
}

TEST(file) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_FALSE(uri.is_http());
}

TEST(mailto) {
  const sourcemeta::core::URI uri{"mailto:jdoe@mail.com"};
  EXPECT_FALSE(uri.is_http());
}

TEST(urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.is_http());
}

TEST(relative) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_FALSE(uri.is_http());
}

TEST(no_scheme_network_path) {
  const sourcemeta::core::URI uri{"//www.sourcemeta.com/path"};
  EXPECT_FALSE(uri.is_http());
}

TEST(fragment_only) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.is_http());
}

TEST(empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_FALSE(uri.is_http());
}
