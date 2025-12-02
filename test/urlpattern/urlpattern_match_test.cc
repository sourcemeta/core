#include <gtest/gtest.h>

#include <sourcemeta/core/urlpattern.h>

TEST(URLPattern_match, all_wildcard_single_segment_values) {
  const sourcemeta::core::URLPattern pattern;
  const sourcemeta::core::URLPatternInput input{.protocol = "https",
                                                .username = "admin",
                                                .password = "secret",
                                                .hostname = "localhost",
                                                .port = "8080",
                                                .pathname = "/api",
                                                .search = "q=test",
                                                .hash = "section"};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "https");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "admin");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "secret");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "localhost");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "8080");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "api");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "q=test");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "section");
}

TEST(URLPattern_match, protocol_literal_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  const sourcemeta::core::URLPatternInput input{.protocol = "https",
                                                .username = "",
                                                .password = "",
                                                .hostname = "localhost",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 0);

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "localhost");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, protocol_literal_no_match) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https"};
  const sourcemeta::core::URLPatternInput input{.protocol = "http",
                                                .username = "",
                                                .password = "",
                                                .hostname = "localhost",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_FALSE(result.protocol.has_value());
  EXPECT_FALSE(result.username.has_value());
  EXPECT_FALSE(result.password.has_value());
  EXPECT_FALSE(result.hostname.has_value());
  EXPECT_FALSE(result.port.has_value());
  EXPECT_FALSE(result.pathname.has_value());
  EXPECT_FALSE(result.search.has_value());
  EXPECT_FALSE(result.hash.has_value());
}

TEST(URLPattern_match, hostname_literal_match) {
  const sourcemeta::core::URLPattern pattern{.hostname = "example.com"};
  const sourcemeta::core::URLPatternInput input{.protocol = "https",
                                                .username = "",
                                                .password = "",
                                                .hostname = "example.com",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "https");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 0);

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, pathname_with_named_groups) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/api/:version/:id"};
  const sourcemeta::core::URLPatternInput input{.protocol = "",
                                                .username = "",
                                                .password = "",
                                                .hostname = "localhost",
                                                .port = "",
                                                .pathname = "/api/v1/123",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "localhost");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 2);
  EXPECT_EQ(result.pathname->at(0), "v1");
  EXPECT_EQ(result.pathname->at(1), "123");
  EXPECT_EQ(result.pathname->at("version"), "v1");
  EXPECT_EQ(result.pathname->at("id"), "123");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, hostname_with_named_group) {
  const sourcemeta::core::URLPattern pattern{.hostname =
                                                 ":subdomain.example.com"};
  const sourcemeta::core::URLPatternInput input{.protocol = "",
                                                .username = "",
                                                .password = "",
                                                .hostname = "api.example.com",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "api");
  EXPECT_EQ(result.hostname->at("subdomain"), "api");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, all_components_with_literals) {
  const sourcemeta::core::URLPattern pattern{.protocol = "https",
                                             .username = "admin",
                                             .password = "secret",
                                             .hostname = "example.com",
                                             .port = "8080",
                                             .pathname = "/api/v1",
                                             .search = "q=test",
                                             .hash = "section"};
  const sourcemeta::core::URLPatternInput input{.protocol = "https",
                                                .username = "admin",
                                                .password = "secret",
                                                .hostname = "example.com",
                                                .port = "8080",
                                                .pathname = "/api/v1",
                                                .search = "q=test",
                                                .hash = "section"};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 0);

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 0);

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 0);

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 0);

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 0);

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 0);

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 0);

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 0);
}

TEST(URLPattern_match, wildcard_pathname) {
  const sourcemeta::core::URLPattern pattern{.pathname = "/static/*"};
  const sourcemeta::core::URLPatternInput input{.protocol = "",
                                                .username = "",
                                                .password = "",
                                                .hostname = "localhost",
                                                .port = "",
                                                .pathname = "/static/js",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "localhost");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "js");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, wildcard_hostname) {
  const sourcemeta::core::URLPattern pattern{.hostname = "*.example.com"};
  const sourcemeta::core::URLPatternInput input{.protocol = "",
                                                .username = "",
                                                .password = "",
                                                .hostname = "api.example.com",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "api");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}

TEST(URLPattern_match, protocol_name_pattern) {
  const sourcemeta::core::URLPattern pattern{.protocol = ":scheme"};
  const sourcemeta::core::URLPatternInput input{.protocol = "https",
                                                .username = "",
                                                .password = "",
                                                .hostname = "localhost",
                                                .port = "",
                                                .pathname = "/",
                                                .search = "",
                                                .hash = ""};

  const auto result{pattern.match(input)};

  EXPECT_TRUE(result.protocol.has_value());
  EXPECT_EQ(result.protocol->size(), 1);
  EXPECT_EQ(result.protocol->at(0), "https");
  EXPECT_EQ(result.protocol->at("scheme"), "https");

  EXPECT_TRUE(result.username.has_value());
  EXPECT_EQ(result.username->size(), 1);
  EXPECT_EQ(result.username->at(0), "");

  EXPECT_TRUE(result.password.has_value());
  EXPECT_EQ(result.password->size(), 1);
  EXPECT_EQ(result.password->at(0), "");

  EXPECT_TRUE(result.hostname.has_value());
  EXPECT_EQ(result.hostname->size(), 1);
  EXPECT_EQ(result.hostname->at(0), "localhost");

  EXPECT_TRUE(result.port.has_value());
  EXPECT_EQ(result.port->size(), 1);
  EXPECT_EQ(result.port->at(0), "");

  EXPECT_TRUE(result.pathname.has_value());
  EXPECT_EQ(result.pathname->size(), 1);
  EXPECT_EQ(result.pathname->at(0), "");

  EXPECT_TRUE(result.search.has_value());
  EXPECT_EQ(result.search->size(), 1);
  EXPECT_EQ(result.search->at(0), "");

  EXPECT_TRUE(result.hash.has_value());
  EXPECT_EQ(result.hash->size(), 1);
  EXPECT_EQ(result.hash->at(0), "");
}
