#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>

#include <chrono>      // std::chrono
#include <memory>      // std::make_shared
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

using Fetch =
    std::optional<sourcemeta::core::OAuthMetadataProvider::FetchResult>;

constexpr std::string_view SERVER_DOCUMENT{R"JSON({
    "issuer": "https://example.com",
    "response_types_supported": [ "code" ],
    "token_endpoint": "https://example.com/token"
  })JSON"};

} // namespace

TEST(metadata_provider_fetches_on_first_call) {
  int fetch_count{0};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  const auto metadata{provider.metadata()};
  EXPECT_TRUE(metadata != nullptr);
  EXPECT_EQ(metadata->issuer(), "https://example.com");
  EXPECT_EQ(metadata->token_endpoint().value(), "https://example.com/token");
  EXPECT_EQ(fetch_count, 1);
}

TEST(metadata_provider_derives_the_authorization_server_url) {
  std::string seen;
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&seen](std::string_view url) -> Fetch {
        seen = std::string{url};
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  const auto metadata{provider.metadata()};
  EXPECT_TRUE(metadata != nullptr);
  EXPECT_EQ(seen, "https://example.com/.well-known/oauth-authorization-server");
}

TEST(metadata_provider_caches_within_the_ttl) {
  int fetch_count{0};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  const auto first{provider.metadata()};
  const auto second{provider.metadata()};
  EXPECT_TRUE(first != nullptr);
  EXPECT_TRUE(second != nullptr);
  EXPECT_EQ(fetch_count, 1);
}

TEST(metadata_provider_refreshes_after_the_ttl) {
  int fetch_count{0};
  const auto clock_now{std::make_shared<std::chrono::system_clock::time_point>(
      std::chrono::system_clock::time_point{})};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      },
      sourcemeta::core::OAuthMetadataProvider::Options{},
      [clock_now]() { return *clock_now; }};
  const auto first{provider.metadata()};
  EXPECT_EQ(fetch_count, 1);
  *clock_now += std::chrono::hours{2};
  const auto second{provider.metadata()};
  EXPECT_TRUE(second != nullptr);
  EXPECT_EQ(fetch_count, 2);
}

TEST(metadata_provider_refresh_forces_a_refetch) {
  int fetch_count{0};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  const auto first{provider.metadata()};
  EXPECT_EQ(fetch_count, 1);
  const auto refreshed{provider.refresh()};
  EXPECT_TRUE(refreshed != nullptr);
  EXPECT_EQ(fetch_count, 2);
}

TEST(metadata_provider_returns_null_when_the_fetch_fails) {
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [](std::string_view) -> Fetch { return std::nullopt; }};
  EXPECT_TRUE(provider.metadata() == nullptr);
}

TEST(metadata_provider_serves_stale_after_a_failed_refresh) {
  bool succeed{true};
  const auto clock_now{std::make_shared<std::chrono::system_clock::time_point>(
      std::chrono::system_clock::time_point{})};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&succeed](std::string_view) -> Fetch {
        if (!succeed) {
          return std::nullopt;
        }

        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      },
      sourcemeta::core::OAuthMetadataProvider::Options{},
      [clock_now]() { return *clock_now; }};
  const auto first{provider.metadata()};
  EXPECT_TRUE(first != nullptr);
  succeed = false;
  *clock_now += std::chrono::hours{2};
  const auto stale{provider.metadata()};
  EXPECT_TRUE(stale != nullptr);
  EXPECT_EQ(stale->issuer(), "https://example.com");
}

TEST(metadata_provider_returns_null_on_a_validation_failure) {
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [](std::string_view) -> Fetch {
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = R"JSON({
              "issuer": "https://evil.example",
              "response_types_supported": [ "code" ]
            })JSON",
            .max_age = std::nullopt};
      }};
  EXPECT_TRUE(provider.metadata() == nullptr);
}

TEST(metadata_provider_falls_back_to_the_appended_openid_form) {
  std::string appended_url;
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationInserted,
      [&appended_url](std::string_view url) -> Fetch {
        // The inserted form is tried first and fails, so only the appended
        // form yields a document
        if (url ==
            "https://example.com/issuer1/.well-known/openid-configuration") {
          appended_url = std::string{url};
          return sourcemeta::core::OAuthMetadataProvider::FetchResult{
              .body = R"JSON({
                "issuer": "https://example.com/issuer1",
                "response_types_supported": [ "code" ]
              })JSON",
              .max_age = std::nullopt};
        }

        return std::nullopt;
      }};
  const auto metadata{provider.metadata()};
  EXPECT_TRUE(metadata != nullptr);
  EXPECT_EQ(metadata->issuer(), "https://example.com/issuer1");
  EXPECT_EQ(appended_url,
            "https://example.com/issuer1/.well-known/openid-configuration");
}

TEST(metadata_provider_clamps_a_tiny_ttl_to_the_minimum) {
  int fetch_count{0};
  const auto clock_now{std::make_shared<std::chrono::system_clock::time_point>(
      std::chrono::system_clock::time_point{})};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT},
            .max_age = std::chrono::seconds{1}};
      },
      sourcemeta::core::OAuthMetadataProvider::Options{},
      [clock_now]() { return *clock_now; }};
  const auto first{provider.metadata()};
  EXPECT_EQ(fetch_count, 1);
  // The advertised one second is clamped up to the five minute minimum, so a
  // read ten seconds later is still cached
  *clock_now += std::chrono::seconds{10};
  const auto second{provider.metadata()};
  EXPECT_TRUE(second != nullptr);
  EXPECT_EQ(fetch_count, 1);
}

TEST(metadata_provider_openid_inserted_success_skips_appended) {
  int fetch_count{0};
  std::string seen;
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationInserted,
      [&fetch_count, &seen](std::string_view url) -> Fetch {
        fetch_count += 1;
        seen = std::string{url};
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = R"JSON({
              "issuer": "https://example.com/issuer1",
              "response_types_supported": [ "code" ]
            })JSON",
            .max_age = std::nullopt};
      }};
  const auto metadata{provider.metadata()};
  EXPECT_TRUE(metadata != nullptr);
  EXPECT_EQ(fetch_count, 1);
  EXPECT_EQ(seen,
            "https://example.com/.well-known/openid-configuration/issuer1");
}

TEST(metadata_provider_openid_both_forms_fail) {
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com/issuer1",
      sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationInserted,
      [](std::string_view) -> Fetch { return std::nullopt; }};
  EXPECT_TRUE(provider.metadata() == nullptr);
}

TEST(metadata_provider_clamps_a_huge_ttl_to_the_maximum) {
  int fetch_count{0};
  const auto clock_now{std::make_shared<std::chrono::system_clock::time_point>(
      std::chrono::system_clock::time_point{})};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT},
            .max_age = std::chrono::hours{24 * 365}};
      },
      sourcemeta::core::OAuthMetadataProvider::Options{},
      [clock_now]() { return *clock_now; }};
  const auto first{provider.metadata()};
  EXPECT_EQ(fetch_count, 1);
  // A one-year advertised lifetime is clamped to the 24 hour maximum, so 25
  // hours later a refresh is due
  *clock_now += std::chrono::hours{25};
  const auto second{provider.metadata()};
  EXPECT_TRUE(second != nullptr);
  EXPECT_EQ(fetch_count, 2);
}

TEST(metadata_provider_rejects_a_malformed_json_body) {
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [](std::string_view) -> Fetch {
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = "{not valid json", .max_age = std::nullopt};
      }};
  EXPECT_TRUE(provider.metadata() == nullptr);
}

TEST(metadata_provider_recovers_after_a_failure) {
  bool succeed{false};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&succeed](std::string_view) -> Fetch {
        if (!succeed) {
          return std::nullopt;
        }

        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  EXPECT_TRUE(provider.metadata() == nullptr);
  succeed = true;
  const auto recovered{provider.refresh()};
  EXPECT_TRUE(recovered != nullptr);
  EXPECT_EQ(recovered->issuer(), "https://example.com");
}

TEST(metadata_provider_returns_the_same_snapshot_within_the_ttl) {
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [](std::string_view) -> Fetch {
        return sourcemeta::core::OAuthMetadataProvider::FetchResult{
            .body = std::string{SERVER_DOCUMENT}, .max_age = std::nullopt};
      }};
  const auto first{provider.metadata()};
  const auto second{provider.metadata()};
  EXPECT_TRUE(first == second);
}

TEST(metadata_provider_backs_off_after_a_failed_fetch) {
  int fetch_count{0};
  const auto clock_now{std::make_shared<std::chrono::system_clock::time_point>(
      std::chrono::system_clock::time_point{})};
  sourcemeta::core::OAuthMetadataProvider provider{
      "https://example.com",
      sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
      [&fetch_count](std::string_view) -> Fetch {
        fetch_count += 1;
        return std::nullopt;
      },
      sourcemeta::core::OAuthMetadataProvider::Options{},
      [clock_now]() { return *clock_now; }};
  EXPECT_TRUE(provider.metadata() == nullptr);
  EXPECT_EQ(fetch_count, 1);
  // A second call within the minimum lifetime does not re-fetch during the
  // outage, even though the document has never been retrieved
  *clock_now += std::chrono::seconds{10};
  EXPECT_TRUE(provider.metadata() == nullptr);
  EXPECT_EQ(fetch_count, 1);
  // Once the minimum lifetime elapses the fetch is retried
  *clock_now += std::chrono::minutes{6};
  EXPECT_TRUE(provider.metadata() == nullptr);
  EXPECT_EQ(fetch_count, 2);
}
