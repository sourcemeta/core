#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/urlpattern.h>

#include "urlpattern_test_utils.h"

TEST(URLPattern_parse_json, empty_object) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("{}")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern->hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern->hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->port, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern->pathname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_json, pathname_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": "/foo/bar"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->protocol, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->username, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->password, URLPatternPartAsterisk);
  EXPECT_PART_VECTOR_SIZE(pattern->hostname, 1);
  EXPECT_PART_WITHOUT_VALUE(pattern->hostname, 0, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->port, URLPatternPartAsterisk);

  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern->pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern->pathname, 1, URLPatternPartChar, "bar");

  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->search, URLPatternPartAsterisk);
  EXPECT_SINGLE_PART_WITHOUT_VALUE(pattern->hash, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_json, pathname_with_named_group) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": "/foo/:bar"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern->pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITH_VALUE(pattern->pathname, 1, URLPatternPartName, "bar");
}

TEST(URLPattern_parse_json, pathname_with_wildcard) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": "/foo/*"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern->pathname, 0, URLPatternPartChar, "foo");
  EXPECT_PART_WITHOUT_VALUE(pattern->pathname, 1, URLPatternPartAsterisk);
}

TEST(URLPattern_parse_json, protocol_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"protocol": "https"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->protocol, URLPatternPartChar, "https");
}

TEST(URLPattern_parse_json, hostname_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hostname": "example.com"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_PART_VECTOR_SIZE(pattern->hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern->hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern->hostname, 1, URLPatternPartChar, "com");
}

TEST(URLPattern_parse_json, port_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"port": "8080"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->port, URLPatternPartChar, "8080");
}

TEST(URLPattern_parse_json, username_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"username": "admin"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->username, URLPatternPartChar, "admin");
}

TEST(URLPattern_parse_json, password_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"password": "secret"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->password, URLPatternPartChar,
                                "secret");
}

TEST(URLPattern_parse_json, search_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"search": "foo=bar"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->search, URLPatternPartChar, "foo=bar");
}

TEST(URLPattern_parse_json, hash_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hash": "section"})")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse_json, all_components) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json(R"({
    "protocol": "https",
    "username": "user",
    "password": "pass",
    "hostname": "example.com",
    "port": "8080",
    "pathname": "/api/:id",
    "search": "key=value",
    "hash": "section"
  })")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->protocol, URLPatternPartChar, "https");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern->username, URLPatternPartChar, "user");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern->password, URLPatternPartChar, "pass");

  EXPECT_PART_VECTOR_SIZE(pattern->hostname, 2);
  EXPECT_PART_WITH_VALUE(pattern->hostname, 0, URLPatternPartChar, "example");
  EXPECT_PART_WITH_VALUE(pattern->hostname, 1, URLPatternPartChar, "com");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->port, URLPatternPartChar, "8080");

  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 2);
  EXPECT_PART_WITH_VALUE(pattern->pathname, 0, URLPatternPartChar, "api");
  EXPECT_PART_WITH_VALUE(pattern->pathname, 1, URLPatternPartName, "id");

  EXPECT_SINGLE_PART_WITH_VALUE(pattern->search, URLPatternPartChar,
                                "key=value");
  EXPECT_SINGLE_PART_WITH_VALUE(pattern->hash, URLPatternPartChar, "section");
}

TEST(URLPattern_parse_json, ignores_unknown_properties) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json(R"({
    "pathname": "/foo",
    "unknown": "ignored",
    "another": 123
  })")};
  const auto pattern{sourcemeta::core::URLPattern::parse(input)};
  EXPECT_TRUE(pattern.has_value());

  EXPECT_PART_VECTOR_SIZE(pattern->pathname, 1);
  EXPECT_PART_WITH_VALUE(pattern->pathname, 0, URLPatternPartChar, "foo");
}

TEST(URLPattern_parse_json, input_is_string) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("\"foo\"")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, input_is_array) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("[1, 2, 3]")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, input_is_number) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("42")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, input_is_null) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("null")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, input_is_boolean) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("true")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, non_string_pathname) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": 123})")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, non_string_protocol) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"protocol": true})")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, non_string_hostname) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hostname": null})")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, non_string_port) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"port": 8080})")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, invalid_pattern) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": "/foo/:"})")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}

TEST(URLPattern_parse_json, invalid_regex_pattern) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"JSON({"pathname": "/foo/([)"})JSON")};
  EXPECT_FALSE(sourcemeta::core::URLPattern::parse(input).has_value());
}
