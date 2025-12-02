#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/urlpattern.h>

TEST(URLPatternInput_parse_json, empty_object) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("{}")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(result->protocol.empty());
  EXPECT_TRUE(result->username.empty());
  EXPECT_TRUE(result->password.empty());
  EXPECT_TRUE(result->hostname.empty());
  EXPECT_TRUE(result->port.empty());
  EXPECT_TRUE(result->pathname.empty());
  EXPECT_TRUE(result->search.empty());
  EXPECT_TRUE(result->hash.empty());
}

TEST(URLPatternInput_parse_json, protocol_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"protocol": "https"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->protocol, "https");
  EXPECT_TRUE(result->username.empty());
  EXPECT_TRUE(result->password.empty());
  EXPECT_TRUE(result->hostname.empty());
  EXPECT_TRUE(result->port.empty());
  EXPECT_TRUE(result->pathname.empty());
  EXPECT_TRUE(result->search.empty());
  EXPECT_TRUE(result->hash.empty());
}

TEST(URLPatternInput_parse_json, username_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"username": "admin"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->username, "admin");
}

TEST(URLPatternInput_parse_json, password_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"password": "secret"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->password, "secret");
}

TEST(URLPatternInput_parse_json, hostname_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hostname": "example.com"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->hostname, "example.com");
}

TEST(URLPatternInput_parse_json, port_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"port": "8080"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->port, "8080");
}

TEST(URLPatternInput_parse_json, pathname_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": "/foo/bar"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->pathname, "/foo/bar");
}

TEST(URLPatternInput_parse_json, search_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"search": "foo=bar&baz=qux"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->search, "foo=bar&baz=qux");
}

TEST(URLPatternInput_parse_json, hash_only) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hash": "section"})")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->hash, "section");
}

TEST(URLPatternInput_parse_json, all_components) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json(R"({
    "protocol": "https",
    "username": "user",
    "password": "pass",
    "hostname": "example.com",
    "port": "8080",
    "pathname": "/api/v1",
    "search": "key=value",
    "hash": "section"
  })")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->protocol, "https");
  EXPECT_EQ(result->username, "user");
  EXPECT_EQ(result->password, "pass");
  EXPECT_EQ(result->hostname, "example.com");
  EXPECT_EQ(result->port, "8080");
  EXPECT_EQ(result->pathname, "/api/v1");
  EXPECT_EQ(result->search, "key=value");
  EXPECT_EQ(result->hash, "section");
}

TEST(URLPatternInput_parse_json, ignores_unknown_properties) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json(R"({
    "pathname": "/foo",
    "unknown": "ignored",
    "another": 123
  })")};
  const auto result{sourcemeta::core::URLPatternInput::parse(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->pathname, "/foo");
}

TEST(URLPatternInput_parse_json, input_is_string) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("\"foo\"")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, input_is_array) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("[1, 2, 3]")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, input_is_number) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("42")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, input_is_null) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("null")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, input_is_boolean) {
  const sourcemeta::core::JSON input{sourcemeta::core::parse_json("true")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, non_string_pathname) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"pathname": 123})")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, non_string_protocol) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"protocol": true})")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, non_string_hostname) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"hostname": null})")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}

TEST(URLPatternInput_parse_json, non_string_port) {
  const sourcemeta::core::JSON input{
      sourcemeta::core::parse_json(R"({"port": 8080})")};
  EXPECT_FALSE(sourcemeta::core::URLPatternInput::parse(input).has_value());
}
