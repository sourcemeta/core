#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oidc.h>
#include <sourcemeta/core/test.h>

TEST(discovery_url_appends_the_well_known_suffix) {
  const auto url{sourcemeta::core::oidc_discovery_url("https://example.com")};
  EXPECT_TRUE(url.has_value());
  EXPECT_EQ(url.value(),
            "https://example.com/.well-known/openid-configuration");
}

TEST(discovery_url_retains_the_issuer_path) {
  const auto url{
      sourcemeta::core::oidc_discovery_url("https://example.com/tenant1")};
  EXPECT_TRUE(url.has_value());
  EXPECT_EQ(url.value(),
            "https://example.com/tenant1/.well-known/openid-configuration");
}

TEST(discovery_url_removes_a_trailing_slash) {
  const auto url{sourcemeta::core::oidc_discovery_url("https://example.com/")};
  EXPECT_TRUE(url.has_value());
  EXPECT_EQ(url.value(),
            "https://example.com/.well-known/openid-configuration");
}

TEST(discovery_url_rejects_a_non_https_issuer) {
  EXPECT_FALSE(
      sourcemeta::core::oidc_discovery_url("http://example.com").has_value());
}

TEST(discovery_url_rejects_an_issuer_with_a_fragment) {
  EXPECT_FALSE(sourcemeta::core::oidc_discovery_url("https://example.com#frag")
                   .has_value());
}

TEST(discovery_url_rejects_an_issuer_with_a_query) {
  EXPECT_FALSE(sourcemeta::core::oidc_discovery_url("https://example.com?a=b")
                   .has_value());
}

TEST(webfinger_request_normalizes_an_acct_identifier) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("acct:joe@example.com")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "acct:joe@example.com");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=acct%3Ajoe%40example.com&rel=http%3A%2F%2F"
            "openid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_normalizes_a_bare_user_and_host) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("joe@example.com")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "acct:joe@example.com");
  EXPECT_TRUE(request.value().url.starts_with(
      "https://example.com/.well-known/webfinger?"));
}

TEST(webfinger_request_normalizes_a_url_identifier) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("https://example.com/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://example.com/joe");
  EXPECT_TRUE(request.value().url.starts_with(
      "https://example.com/.well-known/webfinger?"));
}

TEST(webfinger_request_normalizes_a_bare_host) {
  const auto request{sourcemeta::core::oidc_webfinger_request("example.com")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://example.com");
}

TEST(webfinger_request_keeps_an_acct_scheme_with_a_digit_userinfo) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("acct:123@example.com")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "acct:123@example.com");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=acct%3A123%40example.com&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_recognizes_an_uppercase_acct_scheme) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("ACCT:joe@example.com")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "ACCT:joe@example.com");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=ACCT%3Ajoe%40example.com&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_rejects_a_non_https_url_identifier) {
  EXPECT_FALSE(
      sourcemeta::core::oidc_webfinger_request("http://example.com/joe")
          .has_value());
}

TEST(webfinger_request_brackets_an_ipv6_host) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("https://[::1]/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://[::1]/joe");
  EXPECT_EQ(request.value().url,
            "https://[::1]/.well-known/"
            "webfinger?resource=https%3A%2F%2F%5B%3A%3A1%5D%2Fjoe&rel=http%3A%"
            "2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_preserves_a_non_default_port) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("https://example.com:8443/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://example.com:8443/joe");
  EXPECT_TRUE(request.value().url.starts_with(
      "https://example.com:8443/.well-known/webfinger?"));
}

TEST(webfinger_request_preserves_a_port_with_an_ipv6_host) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("https://[::1]:8443/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_TRUE(request.value().url.starts_with(
      "https://[::1]:8443/.well-known/webfinger?"));
}

TEST(webfinger_request_accepts_an_uppercase_https_scheme) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("HTTPS://example.com/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_TRUE(request.value().url.starts_with(
      "https://example.com/.well-known/webfinger?"));
}

TEST(webfinger_request_uses_https_for_a_user_and_host_with_a_port) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("joe@example.com:8080")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://joe@example.com:8080");
  EXPECT_EQ(request.value().url,
            "https://example.com:8080/.well-known/"
            "webfinger?resource=https%3A%2F%2Fjoe%40example.com%3A8080&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_uses_https_for_a_user_and_host_with_a_path) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("joe@example.com/path")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://joe@example.com/path");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=https%3A%2F%2Fjoe%40example.com%2Fpath&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_strips_a_fragment_from_a_url_identifier) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("https://example.com/joe#top")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://example.com/joe");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=https%3A%2F%2Fexample.com%2Fjoe&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_request_normalizes_a_bare_host_and_path) {
  const auto request{
      sourcemeta::core::oidc_webfinger_request("example.com/joe")};
  EXPECT_TRUE(request.has_value());
  EXPECT_EQ(request.value().resource, "https://example.com/joe");
  EXPECT_EQ(request.value().url,
            "https://example.com/.well-known/"
            "webfinger?resource=https%3A%2F%2Fexample.com%2Fjoe&rel="
            "http%3A%2F%2Fopenid.net%2Fspecs%2Fconnect%2F1.0%2Fissuer");
}

TEST(webfinger_issuer_extracts_the_href) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "subject": "acct:joe@example.com",
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "https://example.com"
      }
    ]
  })JSON")};
  const auto issuer{sourcemeta::core::oidc_webfinger_issuer(descriptor)};
  EXPECT_TRUE(issuer.has_value());
  EXPECT_EQ(issuer.value(), "https://example.com");
}

TEST(webfinger_issuer_accepts_an_uppercase_https_href) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "subject": "acct:joe@example.com",
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "HTTPS://example.com"
      }
    ]
  })JSON")};
  const auto issuer{sourcemeta::core::oidc_webfinger_issuer(descriptor)};
  EXPECT_TRUE(issuer.has_value());
  EXPECT_EQ(issuer.value(), "HTTPS://example.com");
}

TEST(webfinger_issuer_ignores_other_relations) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [
      {
        "rel": "http://webfinger.net/rel/avatar",
        "href": "https://example.com/pic"
      }
    ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_rejects_a_missing_links) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "subject": "acct:joe@example.com"
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_rejects_a_non_https_href) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "http://example.com"
      }
    ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_rejects_an_href_with_a_fragment) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "https://example.com#frag"
      }
    ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_skips_an_invalid_matching_link) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "http://example.com"
      },
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": "https://example.com"
      }
    ]
  })JSON")};
  const auto issuer{sourcemeta::core::oidc_webfinger_issuer(descriptor)};
  EXPECT_TRUE(issuer.has_value());
  EXPECT_EQ(issuer.value(), "https://example.com");
}

TEST(webfinger_request_rejects_empty_scheme_candidate) {
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_request(":8080").has_value());
}

TEST(webfinger_request_rejects_scheme_first_char_non_alpha) {
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_request("1:foo").has_value());
}

TEST(webfinger_request_rejects_invalid_scheme_character) {
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_request("a b:x").has_value());
}

TEST(webfinger_request_rejects_acct_without_at) {
  EXPECT_FALSE(
      sourcemeta::core::oidc_webfinger_request("acct:foo").has_value());
}

TEST(webfinger_request_rejects_acct_with_empty_host) {
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_request("foo@").has_value());
}

TEST(webfinger_issuer_rejects_non_object) {
  EXPECT_FALSE(
      sourcemeta::core::oidc_webfinger_issuer(sourcemeta::core::JSON{"string"})
          .has_value());
}

TEST(webfinger_issuer_rejects_non_array_links) {
  const auto descriptor{
      sourcemeta::core::parse_json(R"JSON({ "links": "x" })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_rejects_non_object_link) {
  const auto descriptor{
      sourcemeta::core::parse_json(R"JSON({ "links": [ 123 ] })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_skips_link_without_rel) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [ { "href": "https://issuer.example.com" } ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_skips_link_with_non_string_rel) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [ { "rel": 123, "href": "https://issuer.example.com" } ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_skips_link_with_other_rel) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [ { "rel": "other", "href": "https://issuer.example.com" } ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}

TEST(webfinger_issuer_rejects_matching_rel_with_non_string_href) {
  const auto descriptor{sourcemeta::core::parse_json(R"JSON({
    "links": [
      {
        "rel": "http://openid.net/specs/connect/1.0/issuer",
        "href": 123
      }
    ]
  })JSON")};
  EXPECT_FALSE(sourcemeta::core::oidc_webfinger_issuer(descriptor).has_value());
}
