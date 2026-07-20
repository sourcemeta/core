#ifndef SOURCEMETA_CORE_OAUTH_METADATA_PROVIDER_H_
#define SOURCEMETA_CORE_OAUTH_METADATA_PROVIDER_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <sourcemeta/core/oauth_metadata.h>

#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <functional>  // std::function
#include <memory>      // std::shared_ptr
#include <mutex>       // std::mutex
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

/// @ingroup oauth
/// A long-lived, cached resolver of authorization server metadata (RFC 8414).
/// It derives the well-known URL from an issuer, retrieves and validates the
/// document through an injected transport, and caches it with a freshness-aware
/// refresh. It is meant to be constructed once per issuer at startup, since a
/// per-request instance defeats the caching. Reads take a snapshot, so a
/// returned document is immune to a concurrent refresh. The kind must be an
/// authorization server kind, either `AuthorizationServer` or an OpenID Connect
/// configuration form, since a protected resource document is not an
/// authorization server metadata document and would never validate. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <optional>
///
/// sourcemeta::core::OAuthMetadataProvider provider{
///     "https://example.com",
///     sourcemeta::core::OAuthWellKnownKind::AuthorizationServer,
///     [](std::string_view) -> std::optional<
///         sourcemeta::core::OAuthMetadataProvider::FetchResult> {
///       return sourcemeta::core::OAuthMetadataProvider::FetchResult{
///           .body =
///               R"JSON({"issuer":"https://example.com",
///                      "response_types_supported":["code"]})JSON",
///           .max_age = std::nullopt};
///     }};
/// const auto metadata{provider.metadata()};
/// ```
class SOURCEMETA_CORE_OAUTH_EXPORT OAuthMetadataProvider {
public:
  /// The outcome of one metadata retrieval. The freshness hint is kept separate
  /// from the body because only the caller that made the request can observe
  /// the caching response header. An absent hint means no lifetime was
  /// advertised, which is distinct from an advertised lifetime of zero.
  struct FetchResult {
    /// The raw metadata bytes.
    std::string body;
    /// The advertised freshness lifetime, absent when none was given.
    std::optional<std::chrono::seconds> max_age;
  };

  /// A pluggable transport that turns a URL into raw metadata bytes plus an
  /// optional freshness hint. Returns no value on a failed retrieval, such as a
  /// transport error, an unsuccessful response, or an oversized body. Injecting
  /// the transport keeps this module free of any networking dependency and
  /// makes the provider substitutable for testing.
  using Fetcher =
      std::function<std::optional<FetchResult>(std::string_view url)>;

  /// A source of the current time, defaulting to the system clock and existing
  /// as an injection point so the refresh can be driven deterministically under
  /// test.
  using Clock = std::function<std::chrono::system_clock::time_point()>;

  /// Tunables for the caching policy. The `minimum_ttl` must not exceed the
  /// `maximum_ttl`, since an advertised lifetime is clamped low first then
  /// high.
  struct Options {
    /// The lifetime to assume when the transport advertises none.
    std::chrono::seconds fallback_ttl{std::chrono::hours{1}};
    /// The shortest lifetime honored, clamping smaller advertised values up.
    std::chrono::seconds minimum_ttl{std::chrono::minutes{5}};
    /// The longest lifetime honored, clamping larger advertised values down.
    std::chrono::seconds maximum_ttl{std::chrono::hours{24}};
  };

  /// Construct a provider for an issuer with an injected transport, using the
  /// default policy and the system clock.
  OAuthMetadataProvider(std::string issuer, const OAuthWellKnownKind kind,
                        Fetcher fetcher);
  /// Construct a provider overriding the caching policy.
  OAuthMetadataProvider(std::string issuer, const OAuthWellKnownKind kind,
                        Fetcher fetcher, Options options);
  /// Construct a provider overriding the policy and the clock.
  OAuthMetadataProvider(std::string issuer, const OAuthWellKnownKind kind,
                        Fetcher fetcher, Options options, Clock clock);

  /// The provider owns a lock guarding its cache, so it is neither copyable nor
  /// movable. Store it behind a pointer or construct it in place.
  OAuthMetadataProvider(const OAuthMetadataProvider &) = delete;
  auto operator=(const OAuthMetadataProvider &)
      -> OAuthMetadataProvider & = delete;
  OAuthMetadataProvider(OAuthMetadataProvider &&) = delete;
  auto operator=(OAuthMetadataProvider &&) -> OAuthMetadataProvider & = delete;

  /// The cached metadata, retrieving it on the first call and refreshing it
  /// once its freshness lifetime has elapsed. A failed refresh keeps serving
  /// the last good document, and no value is returned only when the document
  /// has never been retrieved successfully. The result is a snapshot immune to
  /// a concurrent refresh.
  [[nodiscard]] auto metadata() -> std::shared_ptr<const OAuthServerMetadata>;

  /// Force an immediate retrieval regardless of freshness, serving the RFC 9728
  /// Section 5.2 recommendation to re-retrieve on a challenge. A failed
  /// retrieval keeps the last good document.
  [[nodiscard]] auto refresh() -> std::shared_ptr<const OAuthServerMetadata>;

private:
  auto install_locked(const OAuthWellKnownKind kind,
                      const std::chrono::system_clock::time_point now) -> bool;
  auto fetch_and_install_locked(const std::chrono::system_clock::time_point now)
      -> bool;

  const std::string issuer_;
  const OAuthWellKnownKind kind_;
  const Fetcher fetcher_;
  const Options options_;
  const Clock clock_;
  std::mutex mutex_;
  std::shared_ptr<const OAuthServerMetadata> metadata_;
  std::chrono::system_clock::time_point next_refresh_{};
};

#if defined(_MSC_VER)
#pragma warning(default : 4251)
#endif

} // namespace sourcemeta::core

#endif
