#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse, equality_default_constructed) {
  const sourcemeta::core::URLPattern pattern1;
  const sourcemeta::core::URLPattern pattern2;
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 != pattern2);
}

TEST(URLPattern_parse, equality_same_protocol) {
  const sourcemeta::core::URLPattern pattern1{.protocol = "https"};
  const sourcemeta::core::URLPattern pattern2{.protocol = "https"};
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 != pattern2);
}

TEST(URLPattern_parse, equality_different_protocol) {
  const sourcemeta::core::URLPattern pattern1{.protocol = "https"};
  const sourcemeta::core::URLPattern pattern2{.protocol = "http"};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
}

TEST(URLPattern_parse, equality_same_all_components) {
  const sourcemeta::core::URLPattern pattern1{.protocol = "https",
                                              .username = "user",
                                              .password = "pass",
                                              .hostname = "example.com",
                                              .port = "8080",
                                              .pathname = "/api/:id",
                                              .search = "key=value",
                                              .hash = "section"};
  const sourcemeta::core::URLPattern pattern2{.protocol = "https",
                                              .username = "user",
                                              .password = "pass",
                                              .hostname = "example.com",
                                              .port = "8080",
                                              .pathname = "/api/:id",
                                              .search = "key=value",
                                              .hash = "section"};
  EXPECT_TRUE(pattern1 == pattern2);
  EXPECT_FALSE(pattern1 != pattern2);
}

TEST(URLPattern_parse, equality_different_pathname) {
  const sourcemeta::core::URLPattern pattern1{.pathname = "/foo/bar"};
  const sourcemeta::core::URLPattern pattern2{.pathname = "/foo/baz"};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
}

TEST(URLPattern_parse, equality_different_hostname) {
  const sourcemeta::core::URLPattern pattern1{.hostname = "example.com"};
  const sourcemeta::core::URLPattern pattern2{.hostname = "example.org"};
  EXPECT_FALSE(pattern1 == pattern2);
  EXPECT_TRUE(pattern1 != pattern2);
}

TEST(URLPattern_parse, default_constructor_all_asterisks) {
  const sourcemeta::core::URLPattern pattern;
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, single_component_protocol_only) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, single_component_hostname_only) {
  const sourcemeta::core::URLPattern pattern{.hostname = "example.com"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, single_component_pathname_only) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/v1"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, single_component_hash_only) {
  const sourcemeta::core::URLPattern pattern{.hash = "section"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, protocol_and_hostname) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https",
                                             .hostname = "example.com"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, protocol_hostname_and_pathname) {
  const sourcemeta::core::URLPattern pattern{
      .protocol = "https", .hostname = "example.com", .pathname = "/api/v1"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, protocol_hostname_pathname_and_hash) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https",
                                             .hostname = "example.com",
                                             .pathname = "/api/v1",
                                             .hash = "section"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, all_components_specified) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https",
                                             .username = "admin",
                                             .password = "secret",
                                             .hostname = "example.com",
                                             .port = "8080",
                                             .pathname = "/api/v1",
                                             .search = "q=test",
                                             .hash = "section"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "admin");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "secret");
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "q=test");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, pattern_with_names) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":protocol",
                                             .hostname = ":host.example.com",
                                             .pathname = "/api/:version/:id"};
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartName,
                                "protocol");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 3);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartName, "host");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 3);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "version");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "id");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, username_and_password_only) {
  const sourcemeta::core::URLPattern pattern{.username = "admin",
                                             .password = "secret"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "admin");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "secret");
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, port_and_pathname) {
  const sourcemeta::core::URLPattern pattern{.port = "8080",
                                             .pathname = "/api"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, search_and_hash) {
  const sourcemeta::core::URLPattern pattern{.search = "q=test",
                                             .hash = "section"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "q=test");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse, hostname_with_wildcard) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*.example.com"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 3);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 2, URLPatternPartChar, "com");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse, pathname_with_wildcard) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*"};
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "static");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}
