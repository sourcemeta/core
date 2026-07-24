#include <sourcemeta/core/oidc_authentication.h>

#include <sourcemeta/core/oauth.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

namespace {

// Whether a space-delimited list carries a token (OpenID Connect Core 1.0
// Section 3.1.2.1, which compares such parameters as space-delimited lists)
auto space_list_contains(const std::string_view list,
                         const std::string_view token) -> bool {
  std::size_t position{0};
  while (position <= list.size()) {
    const auto space{list.find(' ', position)};
    const auto value{space == std::string_view::npos
                         ? list.substr(position)
                         : list.substr(position, space - position)};
    if (value == token) {
      return true;
    }

    if (space == std::string_view::npos) {
      break;
    }

    position = space + 1;
  }

  return false;
}

// OpenID Connect Core 1.0 Section 3.1.2.1: the none prompt value "MUST NOT be
// used with any other value", so it is only valid alone
auto prompt_is_valid(const std::string_view prompt) -> bool {
  if (prompt.empty() || !space_list_contains(prompt, "none")) {
    return true;
  }

  std::size_t token_count{0};
  std::size_t position{0};
  while (position <= prompt.size()) {
    const auto space{prompt.find(' ', position)};
    const auto value{space == std::string_view::npos
                         ? prompt.substr(position)
                         : prompt.substr(position, space - position)};
    if (!value.empty()) {
      token_count += 1;
    }

    if (space == std::string_view::npos) {
      break;
    }

    position = space + 1;
  }

  return token_count == 1;
}

} // namespace

auto oidc_nonce() -> std::array<char, 43> { return oauth_random_token(); }

auto oidc_build_authentication_url(const std::string_view endpoint,
                                   const OIDCAuthenticationRequest &request,
                                   std::string &sink) -> bool {
  // OpenID Connect Core 1.0 Section 3.1.2.1: scope is REQUIRED and must contain
  // the openid value, which is what makes the request an OpenID Connect one
  if (!space_list_contains(request.scope, "openid")) {
    return false;
  }

  if (!prompt_is_valid(request.prompt)) {
    return false;
  }

  std::array<OAuthParameter, 11> extra_storage;
  std::size_t extra_count{0};
  const auto append_extra{[&extra_storage,
                           &extra_count](const std::string_view name,
                                         const std::string_view value) -> void {
    if (!value.empty()) {
      extra_storage[extra_count] = OAuthParameter{.name = name, .value = value};
      extra_count += 1;
    }
  }};

  append_extra("nonce", request.nonce);
  append_extra("display", request.display);
  append_extra("prompt", request.prompt);
  append_extra("max_age", request.max_age);
  append_extra("ui_locales", request.ui_locales);
  append_extra("id_token_hint", request.id_token_hint);
  append_extra("login_hint", request.login_hint);
  append_extra("acr_values", request.acr_values);
  append_extra("claims", request.claims);
  append_extra("request", request.request);
  append_extra("response_mode", request.response_mode);

  OAuthAuthorizationRequest base;
  base.client_id = request.client_id;
  base.redirect_uri = request.redirect_uri;
  base.scope = request.scope;
  base.state = request.state;
  base.code_challenge = request.code_challenge;
  base.code_challenge_method = request.code_challenge_method;
  base.response_type = request.response_type;
  base.request_uri = request.request_uri;
  base.extra =
      std::span<const OAuthParameter>{extra_storage.data(), extra_count};

  oauth_build_authorization_url(endpoint, base, sink);
  return true;
}

auto oidc_authorization_url(const std::string_view authorization_endpoint,
                            const std::string_view client_id,
                            const std::string_view redirect_uri,
                            const std::string_view state,
                            const std::string_view code_challenge,
                            const std::string_view nonce)
    -> std::optional<std::string> {
  OIDCAuthenticationRequest request;
  request.client_id = client_id;
  request.redirect_uri = redirect_uri;
  request.scope = "openid";
  request.response_type = "code";
  request.state = state;
  request.code_challenge = code_challenge;
  request.code_challenge_method = "S256";
  request.nonce = nonce;

  std::string url;
  if (!oidc_build_authentication_url(authorization_endpoint, request, url)) {
    return std::nullopt;
  }

  return url;
}

auto oidc_parse_authentication_request(const std::string_view query,
                                       std::string &storage,
                                       OIDCAuthenticationRequest &result)
    -> bool {
  OAuthAuthorizationRequest base;
  const auto parsed{oauth_parse_authorization_request(
      query, storage, base,
      [&result](const std::string_view name,
                const std::string_view value) -> void {
        if (name == "nonce") {
          result.nonce = value;
        } else if (name == "display") {
          result.display = value;
        } else if (name == "prompt") {
          result.prompt = value;
        } else if (name == "max_age") {
          result.max_age = value;
        } else if (name == "ui_locales") {
          result.ui_locales = value;
        } else if (name == "id_token_hint") {
          result.id_token_hint = value;
        } else if (name == "login_hint") {
          result.login_hint = value;
        } else if (name == "acr_values") {
          result.acr_values = value;
        } else if (name == "claims") {
          result.claims = value;
        } else if (name == "request") {
          result.request = value;
        } else if (name == "response_mode") {
          result.response_mode = value;
        }
      })};
  if (!parsed) {
    return false;
  }

  result.client_id = base.client_id;
  result.redirect_uri = base.redirect_uri;
  result.scope = base.scope;
  result.response_type = base.response_type;
  result.state = base.state;
  result.code_challenge = base.code_challenge;
  result.code_challenge_method = base.code_challenge_method;
  result.request_uri = base.request_uri;
  return true;
}

} // namespace sourcemeta::core
