#include <sourcemeta/core/oidc_discovery.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/uri.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

constexpr std::string_view ISSUER_RELATION{
    "http://openid.net/specs/connect/1.0/issuer"};

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

  // OpenID Connect Discovery 1.0 Section 2.1: the host is the domain after the
  // last "@" of an acct resource, or the authority of a URL resource. An acct
  // URI carries no authority component, so the URI parser cannot expose its
  // host, but a URL resource is parsed rather than scanned by hand. The host is
  // copied out because the parsed URI does not outlive this scope
  std::string host;
  if (request.resource.starts_with("acct:")) {
    const std::string_view account{
        std::string_view{request.resource}.substr(5)};
    const auto at{account.rfind('@')};
    if (at == std::string_view::npos) {
      return std::nullopt;
    }

    host = account.substr(at + 1);
  } else {
    try {
      const URI resource{request.resource};
      if (!resource.host().has_value() || resource.host().value().empty()) {
        return std::nullopt;
      }

      host = resource.host().value();
    } catch (const URIParseError &) {
      return std::nullopt;
    }
  }

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
