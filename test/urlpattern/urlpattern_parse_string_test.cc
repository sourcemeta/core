#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse_string, simple_pathname_pattern) {
  const auto pattern{sourcemeta::core::URLPattern::parse("/foo/bar")};

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "bar");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, pathname_with_named_group) {
  const auto pattern{sourcemeta::core::URLPattern::parse("/foo/:bar")};

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "bar");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, pathname_with_wildcard) {
  const auto pattern{sourcemeta::core::URLPattern::parse("/foo/*")};

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern.hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern.pathname, 1, URLPatternPartAsterisk);

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, full_url_with_protocol_host_path) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_named_path_segment) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/:foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartName, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_port) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com:8080/foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_username) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://user@example.com/foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "user");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_username_and_password) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://user:pass@example.com/foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "user");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "pass");

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_query_string) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/foo?bar=baz")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "bar=baz");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_named_query) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/foo?:query")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartName, "query");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, url_with_hash) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/foo#section")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse_string, url_with_named_hash) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("https://example.com/foo#:anchor")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartName, "anchor");
}

TEST(URLPattern_parse_string, url_with_query_and_hash) {
  const auto pattern{sourcemeta::core::URLPattern::parse(
      "https://example.com/foo?bar=baz#section")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar, "bar=baz");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse_string, complex_multi_segment_path) {
  const auto pattern{sourcemeta::core::URLPattern::parse(
      "https://example.com/api/v1/:resource/:id")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 4);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartChar, "v1");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 2, URLPatternPartName, "resource");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 3, URLPatternPartName, "id");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, wildcard_protocol) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse("*://example.com/foo")};

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, named_protocol) {
  const auto pattern{
      sourcemeta::core::URLPattern::parse(":protocol://example.com/foo")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartName,
                                "protocol");
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.password, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "foo");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern.hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_string, empty_string) {
  const auto pattern{sourcemeta::core::URLPattern::parse("")};

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

TEST(URLPattern_parse_string, all_components_full_pattern) {
  const auto pattern{sourcemeta::core::URLPattern::parse(
      "https://user:pass@example.com:8080/api/:id?key=value#section")};

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.username, URLPatternPartChar, "user");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.password, URLPatternPartChar, "pass");

  EXPECT_PART_VECTOR_SIZE(pattern.hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern.hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern.hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.port, URLPatternPartChar, "8080");

  EXPECT_PART_VECTOR_SIZE(pattern.pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern.pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern.pathname, 1, URLPatternPartName, "id");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern.search, URLPatternPartChar,
                                "key=value");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern.hash, URLPatternPartChar, "section");
}
