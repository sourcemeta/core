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
