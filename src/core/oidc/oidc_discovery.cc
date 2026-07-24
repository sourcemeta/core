#include <sourcemeta/core/oidc_discovery.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/uri.h>

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

constexpr std::string_view ISSUER_RELATION{
    "http://openid.net/specs/connect/1.0/issuer"};

// OpenID Connect Discovery 1.0 Section 2.1: derive the host of the normalized
// resource, the authority of a URL resource or the part after the last "@" of
// an acct resource
auto resource_host(const std::string_view resource) -> std::string_view {
  if (resource.starts_with("acct:")) {
    const auto account{resource.substr(5)};
    const auto at{account.rfind('@')};
    if (at == std::string_view::npos) {
      return {};
    }

    return account.substr(at + 1);
  }

  const auto scheme{resource.find("://")};
  if (scheme == std::string_view::npos) {
    return {};
  }

  const auto authority_start{scheme + 3};
  const auto authority_end{resource.find_first_of("/?#", authority_start)};
  return resource.substr(authority_start,
                         authority_end == std::string_view::npos
                             ? std::string_view::npos
                             : authority_end - authority_start);
}

} // namespace

auto oidc_discovery_url(const std::string_view issuer)
    -> std::optional<std::string> {
  // OpenID Connect Discovery 1.0 Section 4.1: the configuration URL is the
  // issuer with "/.well-known/openid-configuration" appended, the appended form
  // rather than the RFC 8414 inserted form
  std::string sink;
  if (!oauth_well_known_url(
          issuer, OAuthWellKnownKind::OpenIDConfigurationAppended, sink)) {
    return std::nullopt;
  }

  return sink;
}

auto oidc_webfinger_request(const std::string_view identifier)
    -> std::optional<OIDCWebFingerRequest> {
  OIDCWebFingerRequest request;

  // OpenID Connect Discovery 1.0 Section 2.1: an acct URI or a URL is kept, a
  // bare user@host becomes an acct URI, and any other input is an https URL
  if (identifier.starts_with("acct:") ||
      identifier.find("://") != std::string_view::npos) {
    request.resource = identifier;
  } else if (identifier.find('@') != std::string_view::npos) {
    request.resource = "acct:";
    request.resource.append(identifier);
  } else {
    request.resource = "https://";
    request.resource.append(identifier);
  }

  const auto host{resource_host(request.resource)};
  if (host.empty()) {
    return std::nullopt;
  }

  request.url = "https://";
  request.url.append(host);
  request.url.append("/.well-known/webfinger?");
  URI::append_query_parameter(request.url, "resource", request.resource);
  URI::append_query_parameter(request.url, "rel", ISSUER_RELATION);
  return request;
}

auto oidc_webfinger_issuer(const JSON &descriptor)
    -> std::optional<std::string_view> {
  if (!descriptor.is_object() || !descriptor.defines("links")) {
    return std::nullopt;
  }

  const auto &links{descriptor.at("links")};
  if (!links.is_array()) {
    return std::nullopt;
  }

  for (const auto &link : links.as_array()) {
    if (link.is_object() && link.defines("rel") && link.at("rel").is_string() &&
        link.at("rel").to_string() == ISSUER_RELATION && link.defines("href") &&
        link.at("href").is_string()) {
      return std::string_view{link.at("href").to_string()};
    }
  }

  return std::nullopt;
}

} // namespace sourcemeta::core
