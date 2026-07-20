#include <sourcemeta/core/oauth_metadata_provider.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth_metadata.h>

#include <algorithm> // std::max, std::min
#include <chrono>    // std::chrono::seconds, std::chrono::system_clock
#include <memory>    // std::make_shared, std::shared_ptr
#include <mutex>     // std::scoped_lock
#include <string>    // std::string
#include <utility>   // std::move

namespace sourcemeta::core {

namespace {

auto clamp_ttl(const std::optional<std::chrono::seconds> max_age,
               const OAuthMetadataProvider::Options &options)
    -> std::chrono::seconds {
  if (max_age.has_value()) {
    return std::max(options.minimum_ttl,
                    std::min(max_age.value(), options.maximum_ttl));
  }

  return options.fallback_ttl;
}

} // namespace

OAuthMetadataProvider::OAuthMetadataProvider(std::string issuer,
                                             const OAuthWellKnownKind kind,
                                             Fetcher fetcher)
    : OAuthMetadataProvider{std::move(issuer), kind, std::move(fetcher),
                            Options{}} {}

OAuthMetadataProvider::OAuthMetadataProvider(std::string issuer,
                                             const OAuthWellKnownKind kind,
                                             Fetcher fetcher, Options options)
    : OAuthMetadataProvider{std::move(issuer), kind, std::move(fetcher),
                            options, Clock{&std::chrono::system_clock::now}} {}

OAuthMetadataProvider::OAuthMetadataProvider(std::string issuer,
                                             const OAuthWellKnownKind kind,
                                             Fetcher fetcher, Options options,
                                             Clock clock)
    : issuer_{std::move(issuer)}, kind_{kind}, fetcher_{std::move(fetcher)},
      options_{options}, clock_{std::move(clock)} {}

auto OAuthMetadataProvider::install_locked(
    const OAuthWellKnownKind kind,
    const std::chrono::system_clock::time_point now) -> bool {
  std::string url;
  if (!oauth_well_known_url(this->issuer_, kind, url)) {
    return false;
  }

  const auto fetched{this->fetcher_(url)};
  if (!fetched.has_value()) {
    return false;
  }

  auto document{try_parse_json(fetched.value().body)};
  if (!document.has_value()) {
    return false;
  }

  auto parsed{
      OAuthServerMetadata::from(std::move(document).value(), this->issuer_)};
  if (!parsed.has_value()) {
    return false;
  }

  this->metadata_ =
      std::make_shared<const OAuthServerMetadata>(std::move(parsed).value());
  this->next_refresh_ =
      now + clamp_ttl(fetched.value().max_age, this->options_);
  return true;
}

auto OAuthMetadataProvider::fetch_and_install_locked(
    const std::chrono::system_clock::time_point now) -> bool {
  if (this->install_locked(this->kind_, now)) {
    return true;
  }

  // RFC 8414 Section 5: the inserted openid-configuration form is tried first
  // and the legacy appended form only as a fallback
  if (this->kind_ == OAuthWellKnownKind::OpenIDConfigurationInserted) {
    return this->install_locked(OAuthWellKnownKind::OpenIDConfigurationAppended,
                                now);
  }

  return false;
}

auto OAuthMetadataProvider::metadata()
    -> std::shared_ptr<const OAuthServerMetadata> {
  const auto now{this->clock_()};
  const std::scoped_lock lock{this->mutex_};
  if (this->metadata_ == nullptr || now >= this->next_refresh_) {
    // A failed refresh leaves the last good document in place, so a stale
    // snapshot is served rather than none
    if (!this->fetch_and_install_locked(now) && this->metadata_ != nullptr) {
      this->next_refresh_ = now + this->options_.minimum_ttl;
    }
  }

  return this->metadata_;
}

auto OAuthMetadataProvider::refresh()
    -> std::shared_ptr<const OAuthServerMetadata> {
  const auto now{this->clock_()};
  const std::scoped_lock lock{this->mutex_};
  this->fetch_and_install_locked(now);
  return this->metadata_;
}

} // namespace sourcemeta::core
